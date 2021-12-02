// --------------- Wrapers -------------------------
SCM grocksdb_open(SCM options, SCM db_path){
    ASSERT_CONSUME_OPTIONS(options);
    SCM_ASSERT_TYPE(scm_string_p(db_path), db_path, SCM_ARG2, "rocksdb_open", "string");

    char *err = NULL;
    rocksdb_t *db = rocksdb_open(scm_foreign_object_ref(options, 0),
                                 scm_to_utf8_string(db_path), &err);
    if(err != NULL) scm_syserror(err);

    return scm_make_foreign_object_1(scm_rocksdb_t, db);
}

SCM grocksdb_open_with_ttl(SCM options, SCM db_path, SCM ttl){
    ASSERT_CONSUME_OPTIONS(options);
    SCM_ASSERT_TYPE(scm_string_p(db_path), db_path, SCM_ARG2, "rocksdb_open_with_ttl", "string");
    SCM_ASSERT_TYPE(scm_integer_p(ttl), ttl, SCM_ARG3, "rocksdb_open_with_ttl", "integer");

    char *err = NULL;
    rocksdb_t *db = rocksdb_open_with_ttl(scm_foreign_object_ref(options, 0),
                                          scm_to_utf8_string(db_path),
                                          scm_to_size_t(ttl), &err);
    if(err != NULL) scm_syserror(err);
    return scm_make_foreign_object_1(scm_rocksdb_t, db);
}

SCM grocksdb_open_for_read_only(SCM options, SCM db_path, SCM error_if_log_file_exist){
    ASSERT_CONSUME_OPTIONS(options);
    SCM_ASSERT_TYPE(scm_string_p(db_path), db_path, SCM_ARG2, "rocksdb-open-for-read-only", "string");

    char *err = NULL;
    rocksdb_t *db = rocksdb_open_for_read_only(scm_foreign_object_ref(options, 0),
                                               scm_to_utf8_string(db_path),
                                               scm_is_true(error_if_log_file_exist), &err);
    if(err != NULL) scm_syserror(err);
    return scm_make_foreign_object_1(scm_rocksdb_t, db);
}

SCM grocksdb_close(SCM db){
    MXSAFE_DESTROY_WITH(db, rocksdb_close);
    return SCM_UNSPECIFIED;
}

//TODO: provide unsafe, but fast version, without types validation
SCM grocksdb_put(SCM scm_db, SCM scm_key, SCM scm_val, SCM scm_writeopt){
    char *err = NULL;
    rocksdb_writeoptions_t *wopts;

    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_key), scm_key, SCM_ARG2, "rocksdb-put", "bytevector");
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_val), scm_val, SCM_ARG3, "rocksdb-put", "bytevector");
    BIND_REF_OR_DEFAULT(scm_rocksdb_writeoptions_t, scm_writeopt, wopts, default_rocksdb_writeoptions);

    rocksdb_put(scm_get_ref(scm_db),
                wopts,
                SCM_BYTEVECTOR_CONTENTS(scm_key),
                SCM_BYTEVECTOR_LENGTH(scm_key),
                SCM_BYTEVECTOR_CONTENTS(scm_val),
                SCM_BYTEVECTOR_LENGTH(scm_val), &err);
    if(err != NULL) scm_syserror(err);

    return SCM_UNSPECIFIED;
}

//TODO: provide unsafe, but fast version, without types validation
SCM grocksdb_put_cf(SCM scm_db, SCM scm_cf, SCM scm_key, SCM scm_val, SCM scm_writeopt){
    char *err = NULL;
    rocksdb_writeoptions_t *wopts;

    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_key), scm_key, SCM_ARG3, "rocksdb_put_cf", "bytevector");
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_val), scm_val, SCM_ARG4, "rocksdb_put_cf", "bytevector");
    scm_assert_foreign_object_type(scm_rocksdb_column_family_handle_t, scm_cf);
    BIND_REF_OR_DEFAULT(scm_rocksdb_writeoptions_t, scm_writeopt, wopts, default_rocksdb_writeoptions);

    rocksdb_put_cf(scm_get_ref(scm_db),
                   wopts,
                   scm_get_ref(scm_cf),
                   SCM_BYTEVECTOR_CONTENTS(scm_key),
                   SCM_BYTEVECTOR_LENGTH(scm_key),
                   SCM_BYTEVECTOR_CONTENTS(scm_val),
                   SCM_BYTEVECTOR_LENGTH(scm_val), &err);
    if(err != NULL) scm_syserror(err);

    return SCM_UNSPECIFIED;
}

SCM grocksdb_column_family_handle_destroy(SCM cf){
    MXSAFE_DESTROY_WITH(cf, rocksdb_column_family_handle_destroy);
    return SCM_UNSPECIFIED;
}

SCM grocksdb_create_column_family(SCM scm_db, SCM scm_options, SCM scm_cf_name){
    char *err = NULL;
    rocksdb_column_family_handle_t* h;

    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    ASSERT_CONSUME_OPTIONS(scm_options);
    SCM_ASSERT_TYPE(scm_string_p(scm_cf_name), scm_cf_name, SCM_ARG3, "rocksdb-create-column-family", "string");

    h = rocksdb_create_column_family(scm_get_ref(scm_db),
                                     scm_get_ref(scm_options),
                                     scm_to_utf8_string(scm_cf_name), &err);
    return scm_make_foreign_object_1(scm_rocksdb_column_family_handle_t, h);
}

SCM grocksdb_delete(SCM scm_db, SCM scm_key, SCM scm_writeopt){
    char *err = NULL;
    rocksdb_writeoptions_t *wopts;

    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_key), scm_key, SCM_ARG2, "rocksdb-delete", "bytevector");
    BIND_REF_OR_DEFAULT(scm_rocksdb_writeoptions_t, scm_writeopt, wopts, default_rocksdb_writeoptions);

    rocksdb_delete(scm_get_ref(scm_db),
                   wopts,
                   SCM_BYTEVECTOR_CONTENTS(scm_key),
                   SCM_BYTEVECTOR_LENGTH(scm_key), &err);
    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

SCM grocksdb_delete_cf(SCM scm_db, SCM scm_cf, SCM scm_key, SCM scm_writeopt){
    rocksdb_writeoptions_t *wopts;
    char *err = NULL;

    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    scm_assert_foreign_object_type(scm_rocksdb_column_family_handle_t, scm_cf);
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_key), scm_key, SCM_ARG3, "rocksdb-delete-cf", "bytevector");
    BIND_REF_OR_DEFAULT(scm_rocksdb_writeoptions_t, scm_writeopt, wopts, default_rocksdb_writeoptions);

    rocksdb_delete_cf(scm_get_ref(scm_db),
                      wopts,
                      scm_get_ref(scm_cf),
                      SCM_BYTEVECTOR_CONTENTS(scm_key),
                      SCM_BYTEVECTOR_LENGTH(scm_key), &err);
    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

SCM grocksdb_delete_range_cf(SCM scm_db, SCM scm_cf, SCM scm_start_key, SCM scm_end_key, SCM scm_writeopt){
    rocksdb_writeoptions_t *wopts;
    char *err = NULL;

    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    scm_assert_foreign_object_type(scm_rocksdb_column_family_handle_t, scm_cf);
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_start_key), scm_start_key, SCM_ARG3,
                    "rocksdb-delete-range-cf", "bytevector");
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_end_key), scm_end_key, SCM_ARG4,
                    "rocksdb-delete-range-cf", "bytevector");
    BIND_REF_OR_DEFAULT(scm_rocksdb_writeoptions_t, scm_writeopt, wopts, default_rocksdb_writeoptions);

    rocksdb_delete_range_cf(scm_get_ref(scm_db),
                            wopts,
                            scm_get_ref(scm_cf),
                            SCM_BYTEVECTOR_CONTENTS(scm_start_key),
                            SCM_BYTEVECTOR_LENGTH(scm_start_key),
                            SCM_BYTEVECTOR_CONTENTS(scm_end_key),
                            SCM_BYTEVECTOR_LENGTH(scm_end_key), &err);
    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

SCM grocksdb_write(SCM scm_db, SCM scm_writebatch, SCM scm_writeopt){
    char *err = NULL;
    rocksdb_writeoptions_t *wopts;

    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    scm_assert_foreign_object_type(scm_rocksdb_writebatch_t, scm_writebatch);
    BIND_REF_OR_DEFAULT(scm_rocksdb_writeoptions_t, scm_writeopt, wopts, default_rocksdb_writeoptions);

    rocksdb_write(scm_get_ref(scm_db), wopts, scm_get_ref(scm_writebatch), &err);
    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

SCM grocksdb_get(SCM scm_db, SCM scm_key, SCM scm_readopt){
    char *err = NULL;
    size_t vallen;
    rocksdb_readoptions_t *ropts;

    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_key), scm_key, SCM_ARG2, "rocksdb-get", "bytevector");
    BIND_REF_OR_DEFAULT(scm_rocksdb_readoptions_t, scm_readopt, ropts, default_rocksdb_readoptions);

    char *ret = rocksdb_get(scm_get_ref(scm_db),
                            ropts,
                            SCM_BYTEVECTOR_CONTENTS(scm_key),
                            SCM_BYTEVECTOR_LENGTH(scm_key),
                            &vallen, &err);
    if(err != NULL) scm_syserror(err);

    //ATTENTION: it's SRFI-4 u8 uniform numeric vector, not 'bytevector', fortunately it's compatible under hood
    return ret == NULL? SCM_BOOL_F : scm_take_u8vector((uint8_t *) ret, vallen);
}

SCM grocksdb_closed_p(SCM scm_db){
    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    return scm_from_bool(scm_foreign_object_ref(scm_db, 0) == NULL);
}

// --------------- Init ----------------------------
void init_main() {
    scm_rocksdb_t = define_type_wrapper("rocksdb", grocksdb_close);
    scm_rocksdb_column_family_handle_t =
        define_type_wrapper("rocksdb-column-family-handle", grocksdb_column_family_handle_destroy);

    DEF("rocksdb-open", 2, grocksdb_open);
    DEF("rocksdb-open-with-ttl", 3, grocksdb_open_with_ttl);
    DEF("rocksdb-open-for-read-only", 3, grocksdb_open_for_read_only);
    DEF("rocksdb-close!", 1, grocksdb_close);
    DEF("rocksdb-create-column-family", 3, grocksdb_create_column_family);
    DEFOPT("rocksdb-put", 3, 1, grocksdb_put);
    DEFOPT("rocksdb-put-cf", 4, 1, grocksdb_put_cf);
    DEFOPT("rocksdb-delete", 2, 1, grocksdb_delete);
    DEFOPT("rocksdb-delete-cf", 3, 1, grocksdb_delete_cf);
    DEFOPT("rocksdb-delete-range-cf", 4, 1, grocksdb_delete_range_cf);
    DEFOPT("rocksdb-write", 2, 1, grocksdb_write);
    DEFOPT("rocksdb-get", 2, 1, grocksdb_get);

    DEF("rocksdb-closed?", 1, grocksdb_closed_p);
}
