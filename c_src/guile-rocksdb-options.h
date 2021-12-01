// --------------- Wrapers -------------------------
SCM grocksdb_options_create(){
    return scm_make_foreign_object_2(scm_rocksdb_options_t, rocksdb_options_create(), false);
}

void grocksdb_options_destroy(SCM options){
    if(!scm_foreign_object_ref(options, 1))
        SAFE_DESTROY_WITH(options, rocksdb_options_destroy);
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
    scm_rocksdb_options_t = define_type_wrapper_2("rocksdb-options", "already-consumed?", grocksdb_options_destroy);

    DEFOPT("rocksdb-options-create", 0, 1, grocksdb_options_create);
    DEF("rocksdb-options-set-create-if-missing!", 2, grocksdb_options_set_create_if_missing);
    DEFOPT("load-options-from-file", 1, 1, gload_options_from_file);
    DEFOPT("get-latest-options-filename", 1, 1, gget_latest_options_filename);
    DEFOPT("load-latest-options", 1, 1, gload_latest_options);
    DEF("rocksdb-set-options", 2, grocksdb_set_options);

    DEFOPT("string->rocksdb-options", 1, 1, gget_options_from_string);
    DEFOPT("rocksdb-options->string", 1, 1, gget_string_from_dboptions);
    DEF("rocksdb-options-string->alist", 1, grocksdb_options_string_to_alist);
    DEF("rocksdb->rocksdb-options", 1, grocksdb_get_options);

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
