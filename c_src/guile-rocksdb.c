#include <libguile.h>
#include <stdlib.h>
#include "rocksdb/c.h"
#include "guile-rocksdb.h"
#include "guile-rocksdb-main.h"
#include "guile-rocksdb-options.h"
#include "guile-rocksdb-writeopions.h"
#include "guile-rocksdb-backup.h"
#include "guile-rocksdb-checkpoint.h"

void init_modules() {
    scm_c_define_module("rocksdb", init_main, NULL);
    scm_c_define_module("rocksdb backup", init_backup, NULL);
    scm_c_define_module("rocksdb options", init_options, NULL);
    scm_c_define_module("rocksdb checkpoint", init_checkpoint, NULL);
}

void init() {
    scm_with_guile((void*)init_modules, NULL);

    /* DEBUG */
    display_func = scm_c_lookup("display");
    display("starting\n");

    /* scm_shell (argc, argv); */
}
