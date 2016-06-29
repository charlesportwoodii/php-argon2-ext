dnl config.m4 for extension argon2

PHP_ARG_ENABLE(argon2, Whether to enable the "argon2" extension,
	[  --enable-argon2       Enable "php-argon2" extension support])

if test $PHP_ARGON2 != "no"; then

	AC_MSG_CHECKING(for Argon2 library)
	if test -r $PHP_ARGON2/ext/argon2/libargon2.a; then
   		ARGON2_DIR=$PHP_ARGON2/ext/argon2/
	fi

	if test -z "$ARGON2_DIR"; then
		AC_MSG_RESULT(Argon2 not found)
		AC_MSG_ERROR(Compile Argon2 before proceeding)
	fi

	PHP_ADD_INCLUDE($ARGON2_DIR/include)
	PHP_REQUIRE_CXX()

	PHP_ADD_LIBRARY(stdc++, 1, ARGON2_SHARED_LIBADD)
	PHP_ADD_LIBRARY_WITH_PATH(argon2, $ARGON2_DIR, ARGON2_SHARED_LIBADD)
	PHP_SUBST(ARGON2_SHARED_LIBADD)

	PHP_NEW_EXTENSION(argon2, argon2.c, $ext_shared,,,1)
	PHP_ADD_EXTENSION_DEP(argon2, spl)	
fi
