#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_argon2.h"

/**
 * Converts a salt to a base64 encoded value
 * @param char str
 * @param size_t str_len
 * @param size_t out_len
 * @param char ret
 * @license: Copyright (c) 1997-2016 The PHP Group
 * @see https://github.com/php/php-src/blob/1c295d4a9ac78fcc2f77d6695987598bb7abcb83/LICENSE  
 * @return integer
 */
static int salt_to_base64(const char *str, const size_t str_len, const size_t out_len, char *ret)
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

/**
 * Generates a salt value
 * @param size_t length
 * @param char *ret
 * @license: Copyright (c) 1997-2016 The PHP Group
 * @see https://github.com/php/php-src/blob/1c295d4a9ac78fcc2f77d6695987598bb7abcb83/LICENSE    
 * @return integer
 */
static int generate_salt(size_t length, char *ret)
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
	if (salt_to_base64(buffer, raw_length, length, result) == FAILURE) {
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

/**
 * Hashes a password with Argon2
 * @param string password
 * @param integer (constant) algorithm
 * 	@default: PASSWORD_ARGON2_I
 * @param array options
 *  @default: [
 *  	         m_cost: 3,
 *				 t_cost: 1<<16,
 *				 threads: 1,
 *				 lanes: 1
 *            ]
 * @usage: argon2_hash(string $password [, PASSWORD_ARGON2_D|PASSWORD_ARGON2_I ] [, array $options])
 */
PHP_FUNCTION(argon2_hash)
{
	// Argon2 Options
	uint32_t t_cost = 2; 			// 3 
    uint32_t m_cost = (1<<16);	 	// 64 MiB
	uint32_t lanes = 1;
	uint32_t threads = 1;
	uint32_t out_len = 32;
	argon2_type type = Argon2_i; 	// Default to Argon2_i

	size_t salt_len = 16;		    // 16 Byte salt
	size_t password_len;
	size_t encoded_len;

	char* out = malloc(out_len + 1);
	char *salt = malloc(salt_len + 1);
	char *password;
	char *encoded;

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

	// Determine the t_cost if it was passed via options
	if (options && (option_buffer = zend_hash_str_find(options, "t_cost", sizeof("t_cost")-1)) != NULL) {
		t_cost = zval_get_long(option_buffer);
	}
	
	// Determine the lanes if it was passed via options
	if (options && (option_buffer = zend_hash_str_find(options, "lanes", sizeof("lanes")-1)) != NULL) {
		lanes = zval_get_long(option_buffer);
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

	// Generate a salt using the same algorithm used by password_hash()
	if (generate_salt(salt_len, salt) == FAILURE) {
		efree(salt);
		// Return false if a hash can't be generated
		RETURN_FALSE;
	}

	// Determine the encoded length
	encoded_len = argon2_encodedlen(
		t_cost,
		m_cost,
		lanes,
		(uint32_t)salt_len,
		out_len
	);

	// Allocate the size of encoded
	encoded = malloc(encoded_len + 1);

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

	// If the hash wasn't generated, throw an exception
	if (result != ARGON2_OK) {
		zend_throw_exception(spl_ce_RuntimeException, argon2_error_message(result), 0 TSRMLS_CC);
		RETURN_FALSE;
	}

	// Return the generated encoded string
	RETURN_STRINGL(encoded, encoded_len);
}

/**
 * Determines if a given password matches a given hash
 * @param string password
 * @param string hash
 * @usage: argon2_verify(string $password, string $hash)
 */
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

const zend_function_entry argon2_functions[] = {
	PHP_FE(argon2_hash, arginfo_argon2_hash)
	PHP_FE(argon2_verify, arginfo_argon2_verify)
	PHP_FE_END
};

// PHP init
PHP_MINIT_FUNCTION(argon2)
{
	// Create contants for ARGON2
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2D", Argon2_d, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PASSWORD_ARGON2I", Argon2_i, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}

// PHP info
PHP_MINFO_FUNCTION(argon2)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Argon2 support", "enabled");
	php_info_print_table_end();
}

zend_module_entry argon2_module_entry = {
	STANDARD_MODULE_HEADER,
	"argon2",
	argon2_functions,
	PHP_MINIT(argon2),
	NULL,
	NULL,
	NULL,
	PHP_MINFO(argon2),
	"1.0",
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ARGON2
ZEND_GET_MODULE(argon2)
#endif
