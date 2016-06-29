#include "php.h"

#include "ext/standard/info.h"
#include "zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/standard/base64.h"
#include "ext/standard/php_random.h"
#include "ext/standard/php_rand.h"

#include "ext/argon2/include/argon2.h"

extern zend_module_entry argon2_module_entry;

#define argon2_module_ptr &argon2_module_entry
#define phpext_argon2_ptr argon2_module_ptr
