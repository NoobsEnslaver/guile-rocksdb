// --------------- Wrapers -------------------------
static SCM grocksdb_options_create(){
    return scm_make_foreign_object_1(scm_rocksdb_options_t, rocksdb_options_create());
}

static SCM grocksdb_options_increase_parallelism(SCM options, SCM n){
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);
    SCM_ASSERT_TYPE(scm_integer_p(n), n, SCM_ARG2, "rocksdb_options_increase_parallelism", "integer");

    rocksdb_options_increase_parallelism(scm_foreign_object_ref(options, 0), scm_to_size_t(n));
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_options_optimize_level_style_compaction(SCM options, SCM n){
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);
    SCM_ASSERT_TYPE(scm_integer_p(n), n, SCM_ARG2, "rocksdb_options_optimize_level_style_compaction", "integer");

    rocksdb_options_optimize_level_style_compaction(scm_foreign_object_ref(options, 0), scm_to_size_t(n));
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_options_set_create_if_missing(SCM options, SCM create_if_missing){
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);
    unsigned char flag = scm_is_true(create_if_missing)? 1 : 0;

    rocksdb_options_set_create_if_missing(scm_foreign_object_ref(options, 0), flag);
    return SCM_UNSPECIFIED;
}

// --------------- Init ----------------------------
static void init_options() {
    scm_rocksdb_options_t = define_type_wrapper("rocksdb_options", NULL);

    DEF("rocksdb-options-create", 0, &grocksdb_options_create);
    DEF("rocksdb-options-increase-parallelism!", 2, &grocksdb_options_increase_parallelism);
    DEF("rocksdb-options-optimize-level-style-compaction!", 2, &grocksdb_options_optimize_level_style_compaction);
    DEF("rocksdb-options-set-create-if-missing!", 2, &grocksdb_options_set_create_if_missing);
}
