AC_INIT([guile-rocksdb], [0.1], [vorontsov.nstu@yandex.ru])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
LT_INIT

PKG_CHECK_MODULES([GUILE], [guile-3.0])

AC_CHECK_HEADERS([rocksdb/c.h], [],
  [AC_MSG_ERROR([*** Can't find rocksdb include files])])

AC_SEARCH_LIBS(rocksdb_open, [rocksdb], [],
  [AC_MSG_ERROR([*** Can't find rocksdb.so file])])

AC_CONFIG_HEADERS([config.h])
AC_OUTPUT([Makefile c_src/Makefile])
