lib_LTLIBRARIES = libguile-rocksdb.la
libguile_rocksdb_la_SOURCES = guile-rocksdb.c guile-rocksdb-options-utils.cc
libguile_rocksdb_la_CFLAGS = ${GUILE_CFLAGS}
libguile_rocksdb_la_CXXFLAGS = ${GUILE_CFLAGS}
libguile_rocksdb_la_LIBADD = ${GUILE_LTLIBS} ${LIBS}
libguile_rocksdb_la_LDFLAGS = ${GUILE_LDFLAGS}

# BUILT_SOURCES = guile-rocksdb.x

# snarfcppopts = ${GUILE_CFLAGS} ${GUILE_LTLIBS} ${LIBS}
# .SUFFIXES = .x
# .c.x:
# 	guile-snarf -o $@ $< $(snarfcppopts)
