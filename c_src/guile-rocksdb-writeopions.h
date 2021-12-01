static rocksdb_writeoptions_t *default_rocksdb_writeoptions;

// --------------- Wrapers -------------------------
SCM grocksdb_writeoptions_create(){
    return scm_make_foreign_object_1(scm_rocksdb_writeoptions_t, rocksdb_writeoptions_create());
}

void grocksdb_writeoptions_destroy(SCM scm_writeoptions){
    SAFE_DESTROY_WITH(scm_writeoptions, rocksdb_writeoptions_destroy);
}

// --------------- Init ----------------------------
void init_writeoptions() {
    scm_rocksdb_writeoptions_t = define_type_wrapper("rocksdb-writeoptions", grocksdb_writeoptions_destroy);
    default_rocksdb_writeoptions = rocksdb_writeoptions_create();

    DEF("rocksdb-writeoptions-create", 0, grocksdb_writeoptions_create);
}
