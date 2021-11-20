// --------------- Wrapers -------------------------
static SCM grocksdb_readoptions_create(){
    return scm_make_foreign_object_1(scm_rocksdb_readoptions_t, rocksdb_readoptions_create());
}

static void grocksdb_readoptions_destroy(SCM scm_readoptions){
    rocksdb_readoptions_destroy(scm_foreign_object_ref(scm_readoptions, 0));
}
// --------------- Init ----------------------------
static void init_readoptions() {
    scm_rocksdb_readoptions_t = define_type_wrapper("rocksdb-readoptions", grocksdb_readoptions_destroy);

    DEF("rocksdb-readoptions-create", 0, &grocksdb_readoptions_create);
}
