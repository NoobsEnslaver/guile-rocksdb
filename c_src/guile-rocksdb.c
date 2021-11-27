#include <libguile.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include "rocksdb/c.h"
#include "guile-rocksdb.h"
#include "guile-rocksdb-options.h"
#include "guile-rocksdb-writeopions.h"
#include "guile-rocksdb-readopions.h"
#include "guile-rocksdb-writebatch.h"
#include "guile-rocksdb-main.h"
#include "guile-rocksdb-backup.h"
#include "guile-rocksdb-checkpoint.h"
#include "guile-rocksdb-cache.h"
#include "guile-rocksdb-env.h"
#include "guile-rocksdb-iterator.h"

void* init_modules() {
    scm_c_define_module("rocksdb", init_main, NULL);
    scm_c_define_module("rocksdb backup", init_backup, NULL);
    scm_c_define_module("rocksdb options", init_options, NULL);
    scm_c_define_module("rocksdb checkpoint", init_checkpoint, NULL);
    scm_c_define_module("rocksdb writeoptions", init_writeoptions, NULL);
    scm_c_define_module("rocksdb readoptions", init_readoptions, NULL);
    scm_c_define_module("rocksdb writebatch", init_writebatch, NULL);
    scm_c_define_module("rocksdb env", init_env, NULL);
    scm_c_define_module("rocksdb cache", init_cache, NULL);
    scm_c_define_module("rocksdb iterator", init_iterator, NULL);
}

void init() {
    scm_with_guile(init_modules, NULL);

    /* DEBUG */
    display("Guile RocksDB driver loaded\n");

    /* scm_shell (argc, argv); */
}
