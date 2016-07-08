#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/standard/base64.h"
#include "ext/standard/php_random.h"

#include "ext/argon2/include/argon2.h"
#include "php_argon2.h"

// Zend Argument information
ZEND_BEGIN_ARG_INFO_EX(arginfo_argon2_hash, 0, 0, 3)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, algorithm)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_argon2_verify, 0, 0, 2)
	ZEND_ARG_INFO(0, password)
	ZEND_ARG_INFO(0, hash)
ZEND_END_ARG_INFO()

static int php_password_salt_to64(const char *str, const size_t str_len, const size_t out_len, char *ret) /* {{{ */
{
	size_t pos = 0;
	zend_string *buffer;
	if ((int) str_len < 0) {
		return FAILURE;
	}
	buffer = php_base64_encode((unsigned char*) str, str_len);
	if (ZSTR_LEN(buffer) < out_len) {
		/* Too short of an encoded string generated */
		zend_string_release(buffer);
		return FAILURE;
	}
	for (pos = 0; pos < out_len; pos++) {
		if (ZSTR_VAL(buffer)[pos] == '+') {
			ret[pos] = '.';
		} else if (ZSTR_VAL(buffer)[pos] == '=') {
			zend_string_free(buffer);
			return FAILURE;
		} else {
			ret[pos] = ZSTR_VAL(buffer)[pos];
		}
	}
	zend_string_free(buffer);
	return SUCCESS;
}
/* }}} */

static int php_password_make_salt(size_t length, char *ret) /* {{{ */
{
	size_t raw_length;
	char *buffer;
	char *result;

	if (length > (INT_MAX / 3)) {
		php_error_docref(NULL, E_WARNING, "Length is too large to safely generate");
		return FAILURE;
	}

	raw_length = length * 3 / 4 + 1;

	buffer = (char *) safe_emalloc(raw_length, 1, 1);

	if (FAILURE == php_random_bytes_silent(buffer, raw_length)) {
		php_error_docref(NULL, E_WARNING, "Unable to generate salt");
		efree(buffer);
		return FAILURE;
	}

	result = safe_emalloc(length, 1, 1);
	if (php_password_salt_to64(buffer, raw_length, length, result) == FAILURE) {
		php_error_docref(NULL, E_WARNING, "Generated salt too short");
		efree(buffer);
		efree(result);
		return FAILURE;
	}
	memcpy(ret, result, length);
	efree(result);
	efree(buffer);
	ret[length] = 0;
	return SUCCESS;
}
/* }}} */

/* {{{ proto string argon2_hash(string password, int algorithm, array options)
Generates an argon2 hash */
PHP_FUNCTION(argon2_hash)
{
	// Argon2 Options
	uint32_t t_cost = 3; 
	uint32_t m_cost = (1<<16);	 	// 64 MiB
	uint32_t lanes = 1;
	uint32_t threads = 1;
	uint32_t out_len = 32;
	argon2_type type = Argon2_i; 	// Default to Argon2_i

	size_t salt_len = 16;		    // 16 Byte salt
	size_t password_len;
	size_t encoded_len;

	char *out;
	char *salt;
	char *password;
	char *encoded;
	char *encoded_result;

	int result;
	long argon2_type = -1;

	zval *option_buffer;
	HashTable *options = 0;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STRING(password, password_len)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(argon2_type)
		Z_PARAM_ARRAY_HT(options);
	ZEND_PARSE_PARAMETERS_END();
	
	// Determine the m_cost if it was passed via options
	if (options && (option_buffer = zend_hash_str_find(options, "m_cost", sizeof("m_cost")-1)) != NULL) {
		m_cost = zval_get_long(option_buffer);
	}

	if (m_cost > ARGON2_MAX_MEMORY) {
		zend_throw_exception(spl_ce_InvalidArgumentException, "`m_cost` exceeds maximum memory", 0 TSRMLS_CC);
	}

	// Determine the t_cost if it was passed via options
	if (options && (option_buffer = zend_hash_str_find(options, "t_cost", sizeof("t_cost")-1)) != NULL) {
		t_cost = zval_get_long(option_buffer);
	}

	if (t_cost > ARGON2_MAX_TIME) {
		zend_throw_exception(spl_ce_InvalidArgumentException, "`t_cost` exceeds maximum time", 0 TSRMLS_CC);
	}
	
	// Determine the lanes if it was passed via options
	if (options && (option_buffer = zend_hash_str_find(options, "lanes", sizeof("lanes")-1)) != NULL) {
		lanes = zval_get_long(option_buffer);
	}

	if (lanes > ARGON2_MAX_LANES || lanes == 0) {
		zend_throw_exception(spl_ce_InvalidArgumentException, "Invalid numeric input for `lanes`", 0 TSRMLS_CC);
	}

	// Determine the threads if it was passed via options
	if (options && (option_buffer = zend_hash_str_find(options, "threads", sizeof("threads")-1)) != NULL) {
		threads = zval_get_long(option_buffer);
	}

	// Sanity check the password for non-zero length
	if (password_len == 0) {
		zend_throw_exception(spl_ce_InvalidArgumentException, "Password must be provided", 0 TSRMLS_CC);
	}

	// Determine the Algorithm type
	if (argon2_type == Argon2_i || argon2_type == -1) {
		type = Argon2_i;
	} else if (argon2_type == Argon2_d) {
		type = Argon2_d;
	} else {
		zend_throw_exception(spl_ce_InvalidArgumentException, "Algorithm must be one of `PASSWORD_ARGON2_D, PASSWORD_ARGON2_I`", 0 TSRMLS_CC);
	}

	salt = emalloc(salt_len + 1);

	// Generate a salt using the same algorithm used by password_hash()
	if (php_password_make_salt(salt_len, salt) == FAILURE) {
		efree(salt);
		zend_throw_exception(spl_ce_RuntimeException, "Failed to securely generate a salt", 0 TSRMLS_CC);
	}

	// Determine the encoded length
	encoded_len = argon2_encodedlen(
		t_cost,
		m_cost,
		lanes,
		(uint32_t)salt_len,
		out_len
	);

	// Allocate the size of encoded, and out
	encoded = emalloc(encoded_len + 1);
	out = emalloc(out_len + 1);

	// Generate the argon2_hash
	result = argon2_hash(
		t_cost,
		m_cost,
		threads,
		password,
		password_len,
		salt,
		salt_len,
		out,
		out_len,
		encoded,
		encoded_len,
		type,
		ARGON2_VERSION_NUMBER
	);

	// Convert encoded to zend_string for memory dealloc
	zend_string *ret = zend_string_init(encoded, encoded_len, 0);

	// Free allocated memory
	efree(out);
	efree(salt);
	efree(encoded);

	// If the hash wasn't generated, throw an exception
	if (result != ARGON2_OK) {
		zend_throw_exception(spl_ce_RuntimeException, argon2_error_message(result), 0 TSRMLS_CC);
	}
		
	// Return the generated encoded string
	RETURN_STR(ret);
}
/* }}} */

/**
/* {{{ proto string argon2_verify(string password, string hash)
Generates an argon2 hash */
PHP_FUNCTION(argon2_verify)
{
	// Argon2 Options
	argon2_type type = Argon2_i; 	// Default to Argon2_i

	size_t password_len;
	size_t encoded_len;

	char *password;
	char *encoded;

	int result;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(password, password_len)
		Z_PARAM_STRING(encoded, encoded_len)
	ZEND_PARSE_PARAMETERS_END();
 
	// Determine which algorithm is used
	if (strstr(encoded, "argon2d")) {
		type = Argon2_d;
	} else if (strstr(encoded, "argon2i")) {
		type = Argon2_i;
	} else {
		zend_throw_exception(spl_ce_InvalidArgumentException, "Invalid Argon2 hash", 0 TSRMLS_CC);
	}

	result = argon2_verify(encoded, password, password_len, type);

	// If verification failed just return false
	if (result != ARGON2_OK) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ argon2_functions[]
 */
const zend_function_entry argon2_functions[] = {
	PHP_FE(argon2_hash, arginfo_argon2_hash)
	PHP_FE(argon2_verify, arginfo_argon2_verify)
	PHP_FE_END
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(argon2)
{
	// Create contants for ARGON2
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2D", Argon2_d, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2I", Argon2_i, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(argon2)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Argon2 support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(argon2)
{
#if defined(COMPILE_DL_ARGON2) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* {{{ argon2_module_entry
 */
zend_module_entry argon2_module_entry = {
	STANDARD_MODULE_HEADER,
	"argon2",
	argon2_functions,
	PHP_MINIT(argon2),
	NULL,
	PHP_RINIT(argon2),
	NULL,
	PHP_MINFO(argon2),
	PHP_ARGON2_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ARGON2
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(argon2)
#endif
