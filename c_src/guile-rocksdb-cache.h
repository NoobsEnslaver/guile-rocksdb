static SCM scm_default_sym;
static SCM scm_jemalloc_sym;
static SCM scm_guile_sym;

extern rocksdb_memory_allocator_t* rocksdb_guile_allocator_create();
// --------------- Wrapers -------------------------
SCM grocksdb_cache_create_lru(SCM capacity, SCM allocator){
    SCM_ASSERT_TYPE(scm_is_exact_integer(capacity), capacity, SCM_ARG1, "rocksdb_cache_create_lru", "exact integer");
    size_t cap = scm_to_size_t(capacity);

    if (SCM_UNBNDP(allocator))
        return scm_make_foreign_object_1(scm_rocksdb_cache_t, rocksdb_cache_create_lru(cap));

    rocksdb_memory_allocator_t* alloc;
    rocksdb_lru_cache_options_t* opts = rocksdb_lru_cache_options_create();
    rocksdb_lru_cache_options_set_capacity(opts, cap);

    if(scm_is_eq(allocator, scm_jemalloc_sym)){
        char *err = NULL;
        rocksdb_memory_allocator_t* jemalloc = rocksdb_jemalloc_nodump_allocator_create(&err);
        if(err != NULL) scm_syserror(err);
        rocksdb_lru_cache_options_set_memory_allocator(opts, jemalloc);
    } else if(scm_is_eq(allocator, scm_guile_sym)){
        //TODO: not tested
        rocksdb_lru_cache_options_set_memory_allocator(opts, rocksdb_guile_allocator_create());
    } else if(!scm_is_eq(allocator, scm_default_sym)){
        scm_wrong_type_arg_msg("rocksdb-cache-create-lru", SCM_ARG2, allocator, "allocator name");
    }

    return scm_make_foreign_object_2(scm_rocksdb_cache_t, rocksdb_cache_create_lru_opts(opts), NULL);
};

void grocksdb_cache_destroy(SCM cache){
    MXSAFE_DESTROY_WITH(cache, if(!scm_foreign_object_ref(cache, 1)) rocksdb_cache_destroy);
};

// --------------- Init ----------------------------
void init_cache() {
    scm_rocksdb_cache_t = define_type_wrapper_2("rocksdb-cache", "consumed?", grocksdb_cache_destroy);

    scm_default_sym = scm_permanent_object(scm_from_utf8_symbol("default"));
    scm_jemalloc_sym = scm_permanent_object(scm_from_utf8_symbol("jemalloc"));
    scm_guile_sym = scm_permanent_object(scm_from_utf8_symbol("guile"));

    DEFOPT("rocksdb-cache-create-lru", 1, 1, grocksdb_cache_create_lru);
}
