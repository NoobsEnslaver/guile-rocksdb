static SCM scm_sync_k;
static SCM scm_disable_wal_k;
static SCM scm_ignore_missing_column_families_k;
static SCM scm_no_slowdown_k;
static SCM scm_low_pri_k;
static SCM scm_memtable_insert_hint_per_batch_k;

static SCM scm_sync_s;
static SCM scm_disable_wal_s;
static SCM scm_ignore_missing_column_families_s;
static SCM scm_no_slowdown_s;
static SCM scm_low_pri_s;
static SCM scm_memtable_insert_hint_per_batch_s;

static rocksdb_writeoptions_t *default_rocksdb_writeoptions;

// --------------- Wrapers -------------------------
SCM grocksdb_writeoptions_create(SCM rest){
    rocksdb_writeoptions_t *opts = rocksdb_writeoptions_create();
    SCM sync = SCM_UNDEFINED;
    SCM disable_wal = SCM_UNDEFINED;
    SCM ignore_missing_column_families = SCM_UNDEFINED;
    SCM no_slowdown = SCM_UNDEFINED;
    SCM low_pri = SCM_UNDEFINED;
    SCM memtable_insert_hint_per_batch = SCM_UNDEFINED;

    scm_c_bind_keyword_arguments("rocksdb-writeoptions-create", rest, 0,
                                 scm_sync_k, &sync,
                                 scm_disable_wal_k, &disable_wal,
                                 scm_ignore_missing_column_families_k, &ignore_missing_column_families,
                                 scm_no_slowdown_k, &no_slowdown,
                                 scm_low_pri_k, &low_pri,
                                 scm_memtable_insert_hint_per_batch_k, &memtable_insert_hint_per_batch,
                                 SCM_UNDEFINED);

    if (!SCM_UNBNDP(sync))
        rocksdb_writeoptions_set_sync(opts, scm_is_true(sync));
    if (!SCM_UNBNDP(disable_wal))
        rocksdb_writeoptions_disable_WAL(opts, scm_is_true(disable_wal));
    if (!SCM_UNBNDP(ignore_missing_column_families))
        rocksdb_writeoptions_set_ignore_missing_column_families
            (opts, scm_is_true(ignore_missing_column_families));
    if (!SCM_UNBNDP(no_slowdown))
        rocksdb_writeoptions_set_no_slowdown(opts, scm_is_true(no_slowdown));
    if (!SCM_UNBNDP(low_pri))
        rocksdb_writeoptions_set_low_pri(opts, scm_is_true(low_pri));
    if (!SCM_UNBNDP(memtable_insert_hint_per_batch))
        rocksdb_writeoptions_set_memtable_insert_hint_per_batch
            (opts, scm_is_true(memtable_insert_hint_per_batch));

    return scm_make_foreign_object_1(scm_rocksdb_writeoptions_t, opts);
}

SCM grocksdb_writeoptions_to_alist(SCM scm_wopts){
    scm_assert_foreign_object_type(scm_rocksdb_writeoptions_t, scm_wopts);
    rocksdb_writeoptions_t *opts = scm_foreign_object_ref(scm_wopts, 0);
    return scm_list_n(scm_cons(scm_sync_s,
                               scm_from_bool(rocksdb_writeoptions_get_sync(opts))),
                      scm_cons(scm_disable_wal_s,
                               scm_from_bool(rocksdb_writeoptions_get_disable_WAL(opts))),
                      scm_cons(scm_ignore_missing_column_families_s,
                               scm_from_bool(rocksdb_writeoptions_get_ignore_missing_column_families(opts))),
                      scm_cons(scm_no_slowdown_s,
                               scm_from_bool(rocksdb_writeoptions_get_no_slowdown(opts))),
                      scm_cons(scm_low_pri_s,
                               scm_from_bool(rocksdb_writeoptions_get_low_pri(opts))),
                      scm_cons(scm_memtable_insert_hint_per_batch_s,
                               scm_from_bool(rocksdb_writeoptions_get_memtable_insert_hint_per_batch(opts))),
                      SCM_UNDEFINED);
}


void grocksdb_writeoptions_destroy(SCM scm_writeoptions){
    SAFE_DESTROY_WITH(scm_writeoptions, rocksdb_writeoptions_destroy);
}

// --------------- Init ----------------------------
void init_writeoptions() {
    scm_sync_k = scm_from_utf8_keyword("sync");
    scm_disable_wal_k = scm_from_utf8_keyword("disable-wal");
    scm_ignore_missing_column_families_k = scm_from_utf8_keyword("ignore-missing-column-families");
    scm_no_slowdown_k = scm_from_utf8_keyword("no-slowdown");
    scm_low_pri_k = scm_from_utf8_keyword("low-pri");
    scm_memtable_insert_hint_per_batch_k = scm_from_utf8_keyword("memtable-insert-hint-per-batch");

    scm_sync_s = scm_from_utf8_symbol("sync");
    scm_disable_wal_s = scm_from_utf8_symbol("disable-wal");
    scm_ignore_missing_column_families_s = scm_from_utf8_symbol("ignore-missing-column-families");
    scm_no_slowdown_s = scm_from_utf8_symbol("no-slowdown");
    scm_low_pri_s = scm_from_utf8_symbol("low-pri");
    scm_memtable_insert_hint_per_batch_s = scm_from_utf8_symbol("memtable-insert-hint-per-batch");

    scm_rocksdb_writeoptions_t = define_type_wrapper("rocksdb-writeoptions", grocksdb_writeoptions_destroy);
    default_rocksdb_writeoptions = rocksdb_writeoptions_create();

    DEFREST("rocksdb-writeoptions-create", grocksdb_writeoptions_create);
    DEF("rocksdb-writeoptions->alist", 1, grocksdb_writeoptions_to_alist);
}
