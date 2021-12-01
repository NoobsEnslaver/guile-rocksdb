//TODO: rewrite with c++ to set memory_allocator to scm_gc_malloc?
// --------------- Wrapers -------------------------
SCM grocksdb_cache_create_lru(SCM capacity){
    SCM_ASSERT_TYPE(scm_integer_p(capacity), capacity, SCM_ARG1, "rocksdb_cache_create_lru", "integer");
    scm_make_foreign_object_1(scm_rocksdb_cache_t, rocksdb_cache_create_lru(scm_to_size_t(capacity)));
};

void grocksdb_cache_destroy(SCM cache){
    SAFE_DESTROY_WITH(cache, rocksdb_cache_destroy);
};

// --------------- Init ----------------------------
void init_cache() {
    scm_rocksdb_cache_t = define_type_wrapper("rocksdb-cache", grocksdb_cache_destroy);

    DEF("rocksdb-cache-create-lru", 1, grocksdb_cache_create_lru);
    //DEF("rocksdb-cache-destroy!", 1, grocksdb_cache_destroy);
}
