// --------------- Wrapers -------------------------
static SCM grocksdb_cache_create_lru(SCM capacity){
    SCM_ASSERT_TYPE(scm_integer_p(capacity), capacity, SCM_ARG1, "rocksdb_cache_create_lru", "integer");
    scm_make_foreign_object_1(scm_rocksdb_cache_t, rocksdb_cache_create_lru(scm_to_size_t(capacity)));
};

static void grocksdb_cache_destroy(SCM cache){
    rocksdb_cache_destroy(scm_foreign_object_ref(cache, 0));
};

// --------------- Init ----------------------------
static void init_cache() {
    scm_rocksdb_cache_t = define_type_wrapper("rocksdb-cache", NULL /* grocksdb_cache_destroy */);

    DEF("rocksdb-cache-create-lru", 1, &grocksdb_cache_create_lru);
}
