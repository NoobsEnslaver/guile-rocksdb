// --------------- Wrapers -------------------------
static SCM grocksdb_writebatch_create(){
    return scm_make_foreign_object_1(scm_rocksdb_writebatch_t, rocksdb_writebatch_create());
}

static void grocksdb_writebatch_destroy(SCM batch){
    rocksdb_writebatch_destroy(scm_foreign_object_ref(scm_rocksdb_writebatch_t, 0));
}

// --------------- Init ----------------------------
static void init_writebatch() {
    scm_rocksdb_writebatch_t = define_type_wrapper("rocksdb-writebatch", grocksdb_writebatch_destroy);

    DEF("rocksdb-writebatch-create", 0, &grocksdb_writebatch_create);
}
