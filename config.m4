PHP_ARG_ENABLE(argon2, Whether to enable the "argon2" extension,
	[  --enable-argon2       Enable "php-argon2" extension support])

if test $PHP_ARGON2 != "no"; then
	PHP_NEW_EXTENSION(argon2, argon2.c, $ext_shared)
fi
