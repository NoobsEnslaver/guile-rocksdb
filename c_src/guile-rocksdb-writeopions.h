// --------------- Wrapers -------------------------
static SCM grocksdb_writeoptions_create(){
    return scm_make_foreign_object_1(scm_rocksdb_writeoptions_t, rocksdb_writeoptions_create());
}

static void grocksdb_writeoptions_destroy(SCM scm_writeoptions){
    rocksdb_writeoptions_destroy(scm_foreign_object_ref(scm_writeoptions, 0));
}

// --------------- Init ----------------------------
static void init_writeoptions() {
    scm_rocksdb_writeoptions_t = define_type_wrapper("rocksdb-writeoptions", grocksdb_writeoptions_destroy);

    DEF("rocksdb-writeoptions-create", 0, &grocksdb_writeoptions_create);
}
