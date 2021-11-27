// --------------- Wrapers -------------------------

static SCM grocksdb_backup_engine_open(SCM options, SCM path){
    ASSERT_CONSUME_OPTIONS(options);
    SCM_ASSERT_TYPE(scm_string_p(path), path, SCM_ARG2, "rocksdb_backup_engine_open", "string");

    char *err = NULL;
    rocksdb_backup_engine_t* bk;
    bk = rocksdb_backup_engine_open(scm_get_ref(options),
                                    scm_to_utf8_string(path), &err);
    if(err != NULL) scm_syserror(err);
    return scm_make_foreign_object_2(scm_rocksdb_backup_engine_t, bk, (void *)false);
}

static SCM grocksdb_backup_engine_create_new_backup(SCM bk, SCM db){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_t, bk);
    ASSERT_DB(db);

    char *err = NULL;
    rocksdb_backup_engine_create_new_backup(scm_get_ref(bk),
                                            scm_get_ref(db), &err);
    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_backup_engine_create_new_backup_flush(SCM bk, SCM db, SCM flush_before_backup){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_t, bk);
    ASSERT_DB(db);

    char *err = NULL;
    unsigned char flag = scm_is_false(flush_before_backup)? 0 : 1;
    rocksdb_backup_engine_create_new_backup_flush(scm_get_ref(bk),
                                                  scm_get_ref(db), flag, &err);
    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_backup_engine_purge_old_backups(SCM bk, SCM num_backups_to_keep){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_t, bk);
    SCM_ASSERT_TYPE(scm_integer_p(num_backups_to_keep), num_backups_to_keep, SCM_ARG2,
                    "rocksdb_backup_engine_purge_old_backups", "integer");

    char *err = NULL;
    rocksdb_backup_engine_purge_old_backups(scm_get_ref(bk),
                                            scm_to_size_t(num_backups_to_keep), &err);
    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_restore_options_create(){
    return scm_make_foreign_object_2(scm_rocksdb_restore_options_t,
                                     rocksdb_restore_options_create(), (void *)false);
}

static void grocksdb_restore_options_destroy(SCM opt){
    rocksdb_restore_options_t *ref = scm_foreign_object_ref(opt, 0);
    bool consumed = scm_foreign_object_ref(opt, 1);
    if(ref && !consumed) rocksdb_restore_options_destroy(ref);
}

static SCM grocksdb_restore_options_set_keep_log_files(SCM options, SCM n){
    scm_assert_foreign_object_type(scm_rocksdb_restore_options_t, options);
    SCM_ASSERT_TYPE(scm_integer_p(n), n, SCM_ARG2, "rocksdb_restore_options_set_keep_log_files", "integer");

    rocksdb_restore_options_set_keep_log_files(scm_get_ref(options), scm_to_size_t(n));
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_backup_engine_verify_backup(SCM bk, SCM backup_id){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_t, bk);
    SCM_ASSERT_TYPE(scm_integer_p(backup_id), backup_id, SCM_ARG2, "rocksdb_backup_engine_verify_backup", "integer");

    char *err = NULL;
    rocksdb_backup_engine_verify_backup(scm_get_ref(bk), scm_to_size_t(backup_id), &err);
    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_backup_engine_restore_db_from_latest_backup(SCM bk, SCM db_dir, SCM wal_dir, SCM opt){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_t, bk);
    SCM_ASSERT_TYPE(scm_string_p(db_dir), db_dir, SCM_ARG2,
                    "rocksdb_backup_engine_restore_db_from_latest_backup", "string");
    SCM_ASSERT_TYPE(scm_string_p(wal_dir), wal_dir, SCM_ARG3,
                    "rocksdb_backup_engine_restore_db_from_latest_backup", "string");
    ASSERT_CONSUME(scm_rocksdb_restore_options_t, opt);

    char *err = NULL;
    rocksdb_backup_engine_restore_db_from_latest_backup(scm_get_ref(bk),
                                                        scm_to_utf8_string(db_dir),
                                                        scm_to_utf8_string(wal_dir),
                                                        scm_get_ref(opt),
                                                        &err);
    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_backup_engine_get_backup_info(SCM bk){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_t, bk);
    rocksdb_backup_engine_t* ref = scm_get_ref(bk);
    return scm_make_foreign_object_1(scm_rocksdb_backup_engine_info_t,
                                     (void *)rocksdb_backup_engine_get_backup_info(ref));
}

static SCM grocksdb_backup_engine_info_count(SCM info){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_info_t, info);
    return scm_from_size_t(rocksdb_backup_engine_info_count(scm_get_ref(info)));
}

static SCM grocksdb_backup_engine_info_timestamp(SCM info, SCM index){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_info_t, info);
    SCM_ASSERT_TYPE(scm_integer_p(index), index, SCM_ARG2, "rocksdb_backup_engine_info_timestamp", "integer");

    int64_t result = rocksdb_backup_engine_info_timestamp(scm_get_ref(info),
                                                          scm_to_size_t(index));
    return scm_from_int64(result);
}

static SCM grocksdb_backup_engine_info_backup_id(SCM info, SCM index){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_info_t, info);
    SCM_ASSERT_TYPE(scm_integer_p(index), index, SCM_ARG2, "rocksdb_backup_engine_info_backup_id", "integer");

    uint32_t result = rocksdb_backup_engine_info_backup_id(scm_get_ref(info),
                                                           scm_to_size_t(index));
    return scm_from_uint32(result);
}

static SCM grocksdb_backup_engine_info_size(SCM info, SCM index){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_info_t, info);
    SCM_ASSERT_TYPE(scm_integer_p(index), index, SCM_ARG2, "rocksdb_backup_engine_info_size", "integer");

    uint64_t result = rocksdb_backup_engine_info_size(scm_get_ref(info),
                                                      scm_to_size_t(index));
    return scm_from_uint64(result);
}

static SCM grocksdb_backup_engine_info_number_files(SCM info, SCM index){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_info_t, info);
    SCM_ASSERT_TYPE(scm_integer_p(index), index, SCM_ARG2, "rocksdb_backup_engine_info_number_files", "integer");

    uint32_t result = rocksdb_backup_engine_info_number_files(scm_get_ref(info),
                                                              scm_to_size_t(index));
    return scm_from_uint32(result);
}

static void grocksdb_backup_engine_info_destroy(SCM info){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_info_t, info);
    rocksdb_backup_engine_info_t *ref = scm_foreign_object_ref(info, 0);
    if(ref) rocksdb_backup_engine_info_destroy(ref);
}

static SCM grocksdb_backup_engine_close(SCM bk){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_t, bk);
    rocksdb_backup_engine_t *ref = scm_foreign_object_ref(bk, 0);
    bool closed = scm_foreign_object_ref(bk, 1);
    if (ref && !closed) rocksdb_backup_engine_close(ref);
    return SCM_UNSPECIFIED;
}

// --------------- Init -------------------------
static void init_backup() {
    scm_rocksdb_restore_options_t = define_type_wrapper_2("rocksdb-restore-options", "already-consumed?",
                                                        grocksdb_restore_options_destroy);
    scm_rocksdb_backup_engine_info_t = define_type_wrapper("rocksdb-backup-engine-info",
                                                           grocksdb_backup_engine_info_destroy); // const?
    scm_rocksdb_backup_engine_t = define_type_wrapper_2("rocksdb-backup-engine", "already-closed?",
                                                        grocksdb_backup_engine_close);

    DEF("rocksdb-backup-engine-open", 2, &grocksdb_backup_engine_open);
    DEF("rocksdb-backup-engine-create-new-backup", 2, &grocksdb_backup_engine_create_new_backup);
    DEF("rocksdb-backup-engine-purge-old-backups", 2, &grocksdb_backup_engine_purge_old_backups);
    DEF("rocksdb-restore-options-set-keep-log-files!", 2, &grocksdb_restore_options_set_keep_log_files);
    DEF("rocksdb-backup-engine-verify-backup", 2, &grocksdb_backup_engine_verify_backup);
    DEF("rocksdb-backup-engine-restore-db-from-latest-backup", 4, &grocksdb_backup_engine_restore_db_from_latest_backup);
    DEF("rocksdb-backup-engine-get-backup-info", 1, &grocksdb_backup_engine_get_backup_info);
    DEF("rocksdb-backup-engine-info-count", 1, &grocksdb_backup_engine_info_count);
    DEF("rocksdb-backup-engine-info-timestamp", 2, &grocksdb_backup_engine_info_timestamp);
    DEF("rocksdb-backup-engine-info-backup-id", 2, &grocksdb_backup_engine_info_backup_id);
    DEF("rocksdb-backup-engine-info-size", 2, &grocksdb_backup_engine_info_size);
    DEF("rocksdb-backup-engine-info-number-files", 2, &grocksdb_backup_engine_info_number_files);
    DEF("rocksdb-backup-engine-close!", 1, &grocksdb_backup_engine_close);
}
