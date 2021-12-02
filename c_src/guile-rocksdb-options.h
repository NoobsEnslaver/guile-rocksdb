static SCM k_rocksdb_options_create_if_missing;
static SCM k_rocksdb_options_info_log_level;
static SCM k_rocksdb_options_write_buffer_size;
static SCM k_rocksdb_options_num_levels;
static SCM k_rocksdb_options_max_bytes_for_level_base;
static SCM k_rocksdb_options_max_bytes_for_level_multiplier;
static SCM k_rocksdb_options_level_compaction_dynamic_level_bytes;
static SCM k_rocksdb_options_max_subcompactions;
static SCM k_rocksdb_options_max_background_jobs;

// --------------- Wrapers -------------------------
SCM grocksdb_options_create(SCM rest){
    SCM create_if_missing = SCM_UNDEFINED;
    SCM info_log_level = SCM_UNDEFINED;
    SCM write_buffer_size = SCM_UNDEFINED;
    SCM num_levels = SCM_UNDEFINED;
    SCM max_bytes_for_level_base = SCM_UNDEFINED;
    SCM max_bytes_for_level_multiplier = SCM_UNDEFINED;
    SCM level_compaction_dynamic_level_bytes = SCM_UNDEFINED;
    SCM max_subcompactions = SCM_UNDEFINED;
    SCM max_background_jobs = SCM_UNDEFINED;
    rocksdb_options_t *opts = rocksdb_options_create();

    scm_c_bind_keyword_arguments("rocksdb-options-create", rest, 0,
                                 k_rocksdb_options_create_if_missing, &create_if_missing,
                                 k_rocksdb_options_info_log_level, &info_log_level,
                                 k_rocksdb_options_write_buffer_size, &write_buffer_size,
                                 k_rocksdb_options_num_levels, &num_levels,
                                 k_rocksdb_options_max_bytes_for_level_base, &max_bytes_for_level_base,
                                 k_rocksdb_options_level_compaction_dynamic_level_bytes, &level_compaction_dynamic_level_bytes,
                                 k_rocksdb_options_max_subcompactions, &max_subcompactions,
                                 k_rocksdb_options_max_background_jobs, &max_background_jobs,
                                 k_rocksdb_options_max_bytes_for_level_multiplier, &max_bytes_for_level_multiplier,
                                 SCM_UNDEFINED);

    if (!SCM_UNBNDP(create_if_missing))
        rocksdb_options_set_create_if_missing(opts, scm_is_true(create_if_missing));

    if (!SCM_UNBNDP(info_log_level)){
        SCM_ASSERT_TYPE(scm_integer_p(info_log_level), info_log_level, SCM_ARG1,
                        "rocksdb-options-create:info-log-level", "integer");
        rocksdb_options_set_info_log_level(opts, scm_to_size_t(info_log_level));}

    if (!SCM_UNBNDP(write_buffer_size)){
        SCM_ASSERT_TYPE(scm_integer_p(write_buffer_size), write_buffer_size, SCM_ARG1,
                        "rocksdb-options-create:write-buffer-size", "integer");
        rocksdb_options_set_write_buffer_size(opts, scm_to_size_t(write_buffer_size));}

    if (!SCM_UNBNDP(num_levels)){
        SCM_ASSERT_TYPE(scm_integer_p(num_levels), num_levels, SCM_ARG1,
                        "rocksdb-options-create:num-levels", "integer");
        rocksdb_options_set_num_levels(opts, scm_to_size_t(num_levels));}

    if (!SCM_UNBNDP(max_bytes_for_level_base)){
        SCM_ASSERT_TYPE(scm_integer_p(max_bytes_for_level_base), max_bytes_for_level_base, SCM_ARG1,
                        "rocksdb-options-create:max-bytes-for-level-base", "integer");
        rocksdb_options_set_max_bytes_for_level_base(opts, scm_to_size_t(max_bytes_for_level_base));}

    if (!SCM_UNBNDP(max_bytes_for_level_multiplier)){
        SCM_ASSERT_TYPE(scm_real_p(max_bytes_for_level_multiplier), max_bytes_for_level_multiplier, SCM_ARG1,
                        "rocksdb-options-create:max-bytes-for-level-multiplier", "real");
        rocksdb_options_set_max_bytes_for_level_multiplier(opts, scm_to_double(max_bytes_for_level_multiplier));}

    if (!SCM_UNBNDP(level_compaction_dynamic_level_bytes))
        rocksdb_options_set_level_compaction_dynamic_level_bytes
            (opts, scm_is_true(level_compaction_dynamic_level_bytes));

    if (!SCM_UNBNDP(max_subcompactions)){
        SCM_ASSERT_TYPE(scm_integer_p(max_subcompactions), max_subcompactions, SCM_ARG1,
                        "rocksdb-options-create:max-subcompactions", "integer");
        rocksdb_options_set_max_subcompactions(opts, scm_to_size_t(max_subcompactions));}

    if (!SCM_UNBNDP(max_background_jobs)){
        SCM_ASSERT_TYPE(scm_integer_p(max_background_jobs), max_background_jobs, SCM_ARG1,
                        "rocksdb-options-create:max_background-jobs", "integer");
        rocksdb_options_set_max_background_jobs(opts, scm_to_size_t(max_background_jobs));}

    return scm_make_foreign_object_2(scm_rocksdb_options_t, opts, false);
}

void grocksdb_options_destroy(SCM options){
    MXSAFE_DESTROY_WITH(options, if(!scm_foreign_object_ref(options, 1)) rocksdb_options_destroy);
}

SCM grocksdb_options_set_create_if_missing(SCM options, SCM create_if_missing){
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);
    rocksdb_options_set_create_if_missing(scm_get_ref(options), scm_is_true(create_if_missing));
    return SCM_UNSPECIFIED;
}

SCM grocksdb_set_options(SCM scm_db, SCM scm_options_alist){
    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    SCM_ASSERT_TYPE(scm_list_p(scm_options_alist), scm_options_alist, SCM_ARG2,
                    "rocksdb-set-options", "alist (string . string)");

    char* err = NULL;
    int count = scm_to_int(scm_length(scm_options_alist));
    const char* keys[count];
    const char* values[count];
    SCM pair,k,v;
    for (int i=0; i < count; i++){
        pair = scm_car(scm_options_alist);
        SCM_ASSERT_TYPE(scm_pair_p(pair), pair, SCM_ARG2, "rocksdb-set-options", "alist (string . string)");

        k = scm_car(pair);
        SCM_ASSERT_TYPE(scm_string_p(k), k, SCM_ARG2, "rocksdb-set-options", "alist (string . string)");

        v = scm_cdr(pair);
        SCM_ASSERT_TYPE(scm_string_p(v), v, SCM_ARG2, "rocksdb-set-options", "alist (string . string)");

        keys[i] = scm_to_utf8_string(k);
        values[i] = scm_to_utf8_string(v);
        scm_options_alist = scm_cdr(scm_options_alist);
    }
    rocksdb_set_options(scm_get_ref(scm_db), count, keys, values, &err);
    if(err != NULL) scm_syserror(err);

    return SCM_UNSPECIFIED;
}

SCM grocksdb_options_p(SCM opts){
    return scm_from_bool(SCM_IS_A_P(opts, scm_rocksdb_options_t));
}

// see guile-rocksdb-options-utils.cc
extern SCM gload_options_from_file(SCM scm_options_file_name, SCM scm_cache);
extern SCM gget_latest_options_filename(SCM scm_dbpath, SCM scm_env);
extern SCM gload_latest_options(SCM scm_dbpath, SCM scm_cache);
extern SCM gget_options_from_string(SCM scm_string, SCM scm_base_opts);
extern SCM gget_string_from_dboptions(SCM scm_db_opts, SCM scm_delim);
extern SCM grocksdb_options_string_to_alist(SCM scm_string_db_opts);
extern SCM grocksdb_get_options(SCM scm_db);

// for tests
SCM grocksdb_options_get_info_log_level(SCM scm_options){
    scm_assert_foreign_object_type(scm_rocksdb_options_t, scm_options);
    return scm_from_int(rocksdb_options_get_info_log_level(scm_foreign_object_ref(scm_options, 0)));
}

// --------------- Init ----------------------------
void init_options() {
    k_rocksdb_options_create_if_missing = scm_from_utf8_keyword("create-if-missing");
    k_rocksdb_options_info_log_level = scm_from_utf8_keyword("info-log-level");
    k_rocksdb_options_write_buffer_size = scm_from_utf8_keyword("write-buffer-size");
    k_rocksdb_options_num_levels = scm_from_utf8_keyword("num-levels");
    k_rocksdb_options_max_bytes_for_level_base = scm_from_utf8_keyword("max-bytes-for-level-base");
    k_rocksdb_options_max_bytes_for_level_multiplier = scm_from_utf8_keyword("max-bytes-for-level-multiplier");
    k_rocksdb_options_level_compaction_dynamic_level_bytes = scm_from_utf8_keyword("level-compaction-dynamic-level-bytes");
    k_rocksdb_options_max_subcompactions = scm_from_utf8_keyword("max-subcompactions");
    k_rocksdb_options_max_background_jobs = scm_from_utf8_keyword("max-background-jobs");

    scm_rocksdb_options_t = define_type_wrapper_2("rocksdb-options", "already-consumed?", grocksdb_options_destroy);

    DEFREST("rocksdb-options-create", grocksdb_options_create);
    DEF("rocksdb-options-set-create-if-missing!", 2, grocksdb_options_set_create_if_missing);
    DEFOPT("load-options-from-file", 1, 1, gload_options_from_file);
    DEFOPT("get-latest-options-filename", 1, 1, gget_latest_options_filename);
    DEFOPT("load-latest-options", 1, 1, gload_latest_options);
    DEF("rocksdb-set-options", 2, grocksdb_set_options);

    DEFOPT("string->rocksdb-options", 1, 1, gget_options_from_string);
    DEFOPT("rocksdb-options->string", 1, 1, gget_string_from_dboptions);
    DEF("rocksdb-options-string->alist", 1, grocksdb_options_string_to_alist);
    DEF("rocksdb->rocksdb-options", 1, grocksdb_get_options);
    DEF("rocksdb-options?", 1, grocksdb_options_p);

//  DEF("rocksdb-options-set-info-log-level", 2, grocksdb_options_set_info_log_level);
    DEF("rocksdb-options-get-info-log-level", 1, grocksdb_options_get_info_log_level);
//  DEF("rocksdb-options-increase-parallelism!", 2, grocksdb_options_increase_parallelism);
//  DEF("rocksdb-options-optimize-level-style-compaction!", 2, grocksdb_options_optimize_level_style_compaction);
}

// ----------------------------------------------------------------------------------
// Stop working here with getters-setters because functions 'load_options_from_file'
// and 'rocksdb_set_options' do all setters job and getters don't really needs,
// only few, like 'rocksdb_options_statistics_get_string', another options you may see
// in OPTIONS-XXXX file. All function here will be realized on demand.
// ----------------------------------------------------------------------------------
// SCM grocksdb_options_set_info_log_level(SCM scm_options, SCM n){
//     scm_assert_foreign_object_type(scm_rocksdb_options_t, scm_options);
//     SCM_ASSERT_TYPE(scm_integer_p(n), n, SCM_ARG2, "rocksdb-options-set-info-log-level", "integer");

//     rocksdb_options_set_info_log_level(scm_foreign_object_ref(scm_options, 0), scm_to_size_t(n));
//     return SCM_UNSPECIFIED;
// }

//SCM grocksdb_options_increase_parallelism(SCM options, SCM n){
//    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);
//    SCM_ASSERT_TYPE(scm_integer_p(n), n, SCM_ARG2, "rocksdb_options_increase_parallelism", "integer");
//
//    rocksdb_options_increase_parallelism(scm_foreign_object_ref(options, 0), scm_to_size_t(n));
//    return SCM_UNSPECIFIED;
//}

//SCM grocksdb_options_optimize_level_style_compaction(SCM options, SCM n){
//    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);
//    SCM_ASSERT_TYPE(scm_integer_p(n), n, SCM_ARG2, "rocksdb_options_optimize_level_style_compaction", "integer");
//
//    rocksdb_options_optimize_level_style_compaction(scm_foreign_object_ref(options, 0), scm_to_size_t(n));
//    return SCM_UNSPECIFIED;
//}

//----------------- TODO ---------------------------

// extern ROCKSDB_LIBRARY_API void rocksdb_options_enable_statistics(
//     rocksdb_options_t*);

// returns a pointer to a malloc()-ed, null terminated string
// extern ROCKSDB_LIBRARY_API char* rocksdb_options_statistics_get_string(
//     rocksdb_options_t* opt);

// extern ROCKSDB_LIBRARY_API void rocksdb_set_options_cf(
//     rocksdb_t* db, rocksdb_column_family_handle_t* handle, int count, const char* const keys[], const char* const values[], char** errptr);

// extern ROCKSDB_LIBRARY_API rocksdb_options_t* rocksdb_options_create_copy(
//     rocksdb_options_t*);
