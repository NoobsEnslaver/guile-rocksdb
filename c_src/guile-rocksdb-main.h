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
    SCM_ASSERT_TYPE(scm_string_p(db_path), db_path, SCM_ARG2, "rocksdb-open-for-read-only", "string");

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

static SCM grocksdb_put(SCM scm_db, SCM scm_key, SCM scm_val, SCM scm_writeopt){
    char *err = NULL;
    scm_writeopt = (SCM_UNBNDP(scm_writeopt)) ? grocksdb_writeoptions_create() : scm_writeopt;
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_key), scm_key, SCM_ARG2, "rocksdb-put", "bytevector");
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_val), scm_val, SCM_ARG3, "rocksdb-put", "bytevector");
    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    scm_assert_foreign_object_type(scm_rocksdb_writeoptions_t, scm_writeopt);

    rocksdb_put(scm_foreign_object_ref(scm_db, 0),
                scm_foreign_object_ref(scm_writeopt, 0),
                SCM_BYTEVECTOR_CONTENTS(scm_key),
                SCM_BYTEVECTOR_LENGTH(scm_key),
                SCM_BYTEVECTOR_CONTENTS(scm_val),
                SCM_BYTEVECTOR_LENGTH(scm_val), &err);
    if(err != NULL) scm_syserror(err);

    return SCM_UNSPECIFIED;
}

static SCM grocksdb_put_cf(SCM scm_db, SCM scm_cf, SCM scm_key, SCM scm_val, SCM scm_writeopt){
    char *err = NULL;
    scm_writeopt = (SCM_UNBNDP(scm_writeopt)) ? grocksdb_writeoptions_create() : scm_writeopt;
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_key), scm_key, SCM_ARG3, "rocksdb_put_cf", "bytevector");
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_val), scm_val, SCM_ARG4, "rocksdb_put_cf", "bytevector");
    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    scm_assert_foreign_object_type(scm_rocksdb_column_family_handle_t, scm_cf);
    scm_assert_foreign_object_type(scm_rocksdb_writeoptions_t, scm_writeopt);

    rocksdb_put_cf(scm_foreign_object_ref(scm_db, 0),
                   scm_foreign_object_ref(scm_writeopt, 0),
                   scm_foreign_object_ref(scm_cf, 0),
                   SCM_BYTEVECTOR_CONTENTS(scm_key),
                   SCM_BYTEVECTOR_LENGTH(scm_key),
                   SCM_BYTEVECTOR_CONTENTS(scm_val),
                   SCM_BYTEVECTOR_LENGTH(scm_val), &err);
    if(err != NULL) scm_syserror(err);

    return SCM_UNSPECIFIED;
}

static void grocksdb_column_family_handle_destroy(SCM cf){
    rocksdb_column_family_handle_destroy(scm_foreign_object_ref(cf, 0));
}

static SCM grocksdb_create_column_family(SCM scm_db, SCM scm_options, SCM scm_cf_name){
    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    scm_assert_foreign_object_type(scm_rocksdb_options_t, scm_options);
    SCM_ASSERT_TYPE(scm_string_p(scm_cf_name), scm_cf_name, SCM_ARG3, "rocksdb-create-column-family", "string");

    char *err = NULL;
    rocksdb_column_family_handle_t* h;
    h = rocksdb_create_column_family(scm_foreign_object_ref(scm_db, 0),
                                     scm_foreign_object_ref(scm_options, 0),
                                     scm_to_utf8_string(scm_cf_name), &err);
    return scm_make_foreign_object_1(scm_rocksdb_column_family_handle_t, h);
}

static SCM grocksdb_delete(SCM scm_db, SCM scm_key, SCM scm_writeopt){
    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    scm_writeopt = (SCM_UNBNDP(scm_writeopt)) ? grocksdb_writeoptions_create() : scm_writeopt;
    scm_assert_foreign_object_type(scm_rocksdb_writeoptions_t, scm_writeopt);
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_key), scm_key, SCM_ARG2, "rocksdb-delete", "bytevector");

    char *err = NULL;
    rocksdb_delete(scm_foreign_object_ref(scm_db, 0),
                   scm_foreign_object_ref(scm_writeopt, 0),
                   SCM_BYTEVECTOR_CONTENTS(scm_key),
                   SCM_BYTEVECTOR_LENGTH(scm_key), &err);
    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_delete_cf(SCM scm_db, SCM scm_cf, SCM scm_key, SCM scm_writeopt){
    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    scm_writeopt = (SCM_UNBNDP(scm_writeopt)) ? grocksdb_writeoptions_create() : scm_writeopt;
    scm_assert_foreign_object_type(scm_rocksdb_column_family_handle_t, scm_cf);
    scm_assert_foreign_object_type(scm_rocksdb_writeoptions_t, scm_writeopt);
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_key), scm_key, SCM_ARG3, "rocksdb-delete-cf", "bytevector");

    char *err = NULL;
    rocksdb_delete_cf(scm_foreign_object_ref(scm_db, 0),
                      scm_foreign_object_ref(scm_writeopt, 0),
                      scm_foreign_object_ref(scm_cf, 0),
                      SCM_BYTEVECTOR_CONTENTS(scm_key),
                      SCM_BYTEVECTOR_LENGTH(scm_key), &err);
    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_delete_range_cf(SCM scm_db, SCM scm_cf, SCM scm_start_key, SCM scm_end_key, SCM scm_writeopt){
    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    scm_writeopt = (SCM_UNBNDP(scm_writeopt)) ? grocksdb_writeoptions_create() : scm_writeopt;
    scm_assert_foreign_object_type(scm_rocksdb_column_family_handle_t, scm_cf);
    scm_assert_foreign_object_type(scm_rocksdb_writeoptions_t, scm_writeopt);
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_start_key), scm_start_key, SCM_ARG3,
                    "rocksdb-delete-range-cf", "bytevector");
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_end_key), scm_end_key, SCM_ARG4,
                    "rocksdb-delete-range-cf", "bytevector");

    char *err = NULL;
    rocksdb_delete_range_cf(scm_foreign_object_ref(scm_db, 0),
                            scm_foreign_object_ref(scm_writeopt, 0),
                            scm_foreign_object_ref(scm_cf, 0),
                            SCM_BYTEVECTOR_CONTENTS(scm_start_key),
                            SCM_BYTEVECTOR_LENGTH(scm_start_key),
                            SCM_BYTEVECTOR_CONTENTS(scm_end_key),
                            SCM_BYTEVECTOR_LENGTH(scm_end_key), &err);
    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_write(SCM scm_db, SCM scm_writebatch, SCM scm_writeopt){
    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    scm_assert_foreign_object_type(scm_rocksdb_writebatch_t, scm_writebatch);
    scm_writeopt = (SCM_UNBNDP(scm_writeopt)) ? grocksdb_writeoptions_create() : scm_writeopt;
    scm_assert_foreign_object_type(scm_rocksdb_writeoptions_t, scm_writeopt);

    char *err = NULL;
    rocksdb_write(scm_foreign_object_ref(scm_db, 0),
                  scm_foreign_object_ref(scm_writeopt, 0),
                  scm_foreign_object_ref(scm_writebatch, 0), &err);
    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_get(SCM scm_db, SCM scm_key, SCM scm_readopt){
    scm_readopt = (SCM_UNBNDP(scm_readopt)) ? grocksdb_readoptions_create() : scm_readopt;
    SCM_ASSERT_TYPE(scm_bytevector_p(scm_key), scm_key, SCM_ARG2, "rocksdb-get", "bytevector");
    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    scm_assert_foreign_object_type(scm_rocksdb_readoptions_t, scm_readopt);

    size_t vallen;
    char *err = NULL;
    char *ret = rocksdb_get(scm_foreign_object_ref(scm_db, 0),
                            scm_foreign_object_ref(scm_readopt, 0),
                            SCM_BYTEVECTOR_CONTENTS(scm_key),
                            SCM_BYTEVECTOR_LENGTH(scm_key),
                            &vallen, &err);
    if(err != NULL) scm_syserror(err);

    //ATTENTION: it's SRFI-4 u8 uniform numeric vector, not 'bytevector', fortunately it's compatible under hood
    return ret == NULL? SCM_BOOL_F : scm_take_u8vector((uint8_t *) ret, vallen);
}

extern ROCKSDB_LIBRARY_API char* rocksdb_get(
    rocksdb_t* db, const rocksdb_readoptions_t* options, const char* key,
    size_t keylen, size_t* vallen, char** errptr);

// --------------- Init ----------------------------
static void init_main() {
    scm_rocksdb_t = define_type_wrapper("rocksdb", grocksdb_close);
    scm_rocksdb_column_family_handle_t =
        define_type_wrapper("rocksdb-column-family-handle", grocksdb_column_family_handle_destroy);

    DEF("rocksdb-open", 2, &grocksdb_open);
    DEF("rocksdb-open-with-ttl", 3, &grocksdb_open_with_ttl);
    DEF("rocksdb-open-for-read-only", 3, &grocksdb_open_for_read_only);
    DEF("rocksdb-close!", 1, &grocksdb_close);
    DEF("rocksdb-create-column-family", 3, &grocksdb_create_column_family);
    DEFOPT("rocksdb-put", 3, 1, &grocksdb_put);
    DEFOPT("rocksdb-put-cf", 4, 1, &grocksdb_put_cf);
    DEFOPT("rocksdb-delete", 2, 1, &grocksdb_delete);
    DEFOPT("rocksdb-delete-cf", 3, 1, &grocksdb_delete_cf);
    DEFOPT("rocksdb-delete-range-cf", 4, 1, &grocksdb_delete_range_cf);
    DEFOPT("rocksdb-write", 2, 1, &grocksdb_write);
    DEFOPT("rocksdb-get", 2, 1, &grocksdb_get);

}
