// --------------- Wrapers -------------------------
static SCM grocksdb_create_default_env(){
    scm_make_foreign_object_1(scm_rocksdb_env_t, rocksdb_create_default_env());
};

static SCM grocksdb_create_mem_env(){
    scm_make_foreign_object_1(scm_rocksdb_env_t, grocksdb_create_mem_env());
};

static void grocksdb_env_destroy(SCM env){
    rocksdb_env_destroy(scm_foreign_object_ref(env, 0));
};

// --------------- Init ----------------------------
static void init_env() {
    scm_rocksdb_env_t = define_type_wrapper("rocksdb-env", NULL /* grocksdb_env_destroy */);

    DEF("rocksdb-create-default-env", 0, &grocksdb_create_default_env);
    DEF("rocksdb-create-mem-env", 0, &grocksdb_create_mem_env);
}
