#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

/**
 * @usage: argon2_hash
 */
PHP_FUNCTION(argon2_hash)
{
}

/**
 * @usage: argon2_verify
 */
PHP_FUNCTION(argon2_verify)
{
}


// Zend Argument information
ZEND_BEGIN_ARG_INFO_EX(arginfo_argon2_hash, 0, 0, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_argon2_verify, 0, 0, 1)
ZEND_END_ARG_INFO()

const zend_function_entry argon2_functions[] = {
	PHP_FE(argon2_hash, arginfo_argon2_hash)
	PHP_FE(argon2_verify, arginfo_argon2_verify)
	PHP_FE_END
};

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
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(argon2),
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_ARGON2
ZEND_GET_MODULE(argon2)
#endif
