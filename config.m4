dnl config.m4 for extension argon2

PHP_ARG_WITH(argon2, for argon2 support,
[  --with-argon2             Include argon2 support])

if test "$PHP_ARGON2" != "no"; then
  SEARCH_PATH="./ ./ext/argon2/"
  SEARCH_LIB="libargon2.a"
  AC_MSG_CHECKING([for argon2 files])
  for i in $SEARCH_PATH ; do
    if test -r $i/$SEARCH_LIB; then
      ARGON2_DIR=$i
      AC_MSG_RESULT(found in $i)
    fi
  done
  if test -z "$ARGON2_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please ensure the argon2 headers and static library are installed])
  fi

  LIBS="$LIBS -lpthread -lrt -ldl"
  LDFLAGS="$LDFLAGS -pthread -lrt -ldl"
  
  PHP_ADD_INCLUDE($ARGON2_DIR/include)
  PHP_ADD_INCLUDE($ARGON2_DIR)
  PHP_ADD_LIBRARY(pthread, ARGON2_SHARED_DIR)
  
  LIBNAME=argon2
  LIBSYMBOL=argon2_hash

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ARGON2_DIR, ARGON2_SHARED_LIBADD)
    AC_DEFINE(HAVE_ARGON2LIB,1,[ ])
  ],[
    AC_MSG_ERROR([Problem with libargon2.(a|so). Please check config.log for more information.])
  ],[
    -L$ARGON2_DIR -lrt -ldl -lpthread
  ])
  
  PHP_SUBST(ARGON2_SHARED_LIBADD) 
  PHP_NEW_EXTENSION(argon2, argon2.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi