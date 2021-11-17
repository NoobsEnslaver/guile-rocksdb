// --------------- Wrapers -------------------------
static SCM grocksdb_open(SCM options, SCM db_path){
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);
    SCM_ASSERT_TYPE(scm_string_p(db_path), db_path, SCM_ARG2, "rocksdb_open", "string");

    char *err = NULL;
    rocksdb_t *db = rocksdb_open(scm_foreign_object_ref(options, 0),
                                 scm_to_utf8_string(db_path), &err);
    if(err != NULL) scm_syserror(err);
    return scm_make_foreign_object_1(scm_rocksdb_t, db);
}

static SCM grocksdb_open_with_ttl(SCM options, SCM db_path, SCM ttl){
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);
    SCM_ASSERT_TYPE(scm_string_p(db_path), db_path, SCM_ARG2, "rocksdb_open_with_ttl", "string");
    SCM_ASSERT_TYPE(scm_integer_p(ttl), ttl, SCM_ARG3, "rocksdb_open_with_ttl", "integer");

    char *err = NULL;
    rocksdb_t *db = rocksdb_open_with_ttl(scm_foreign_object_ref(options, 0),
                                          scm_to_utf8_string(db_path),
                                          scm_to_size_t(ttl), &err);
    if(err != NULL) scm_syserror(err);
    return scm_make_foreign_object_1(scm_rocksdb_t, db);
}

static SCM grocksdb_open_for_read_only(SCM options, SCM db_path, SCM error_if_log_file_exist){
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);
    SCM_ASSERT_TYPE(scm_string_p(db_path), db_path, SCM_ARG2, "rocksdb_open_for_read_only", "string");

    unsigned char flag = scm_is_true(error_if_log_file_exist)? 1 : 0;
    char *err = NULL;
    rocksdb_t *db = rocksdb_open_for_read_only(scm_foreign_object_ref(options, 0),
                                               scm_to_utf8_string(db_path), flag, &err);
    if(err != NULL) scm_syserror(err);
    return scm_make_foreign_object_1(scm_rocksdb_t, db);
}

static SCM grocksdb_close(SCM db){
    scm_assert_foreign_object_type(scm_rocksdb_t, db);
    rocksdb_close(scm_foreign_object_ref(db, 0));
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_writeoptions_create(){
    return scm_make_foreign_object_1(scm_rocksdb_writeoptions_t, rocksdb_writeoptions_create());
}

static void grocksdb_writeoptions_destroy(SCM opt){
    rocksdb_writeoptions_destroy(scm_foreign_object_ref(opt, 0));
}

static SCM grocksdb_put(SCM scm_db, SCM scm_key, SCM scm_val, SCM scm_writeopt){
    rocksdb_writeoptions_t* writeopt;
    char *err = NULL;
    scm_writeopt = (SCM_UNBNDP(scm_writeopt)) ? grocksdb_writeoptions_create() : scm_writeopt;
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_key), scm_key, SCM_ARG2, "rocksdb_put", "bytevector");
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_val), scm_val, SCM_ARG3, "rocksdb_put", "bytevector");
    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    scm_assert_foreign_object_type(scm_rocksdb_writeoptions_t, scm_writeopt);

    rocksdb_put(scm_foreign_object_ref(scm_db, 0),
                scm_foreign_object_ref(scm_writeopt, 0),
                SCM_BYTEVECTOR_CONTENTS(scm_key),
                scm_c_bytevector_length(scm_key),
                SCM_BYTEVECTOR_CONTENTS(scm_val),
                scm_c_bytevector_length(scm_val), &err);
    if(err != NULL) scm_syserror(err);

    return SCM_UNSPECIFIED;
}

// --------------- Init ----------------------------
static void init_main() {
    scm_rocksdb_t = define_type_wrapper("rocksdb", grocksdb_close);
    scm_rocksdb_writeoptions_t = define_type_wrapper("rocksdb_writeoptions", grocksdb_writeoptions_destroy);

    DEF("rocksdb-open", 2, &grocksdb_open);
    DEF("rocksdb-open-with-ttl", 3, &grocksdb_open_with_ttl);
    DEF("rocksdb-open-for-read-only", 3, &grocksdb_open_for_read_only);
    DEF("rocksdb-close!", 1, &grocksdb_close);

    DEF("rocksdb-writeoptions-create", 0, &grocksdb_writeoptions_create);
    DEFOPT("rocksdb-put", 3, 1, &grocksdb_put);
}
