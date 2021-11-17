// --------------- Wrapers -------------------------
static SCM grocksdb_checkpoint_object_create(SCM db){
    scm_assert_foreign_object_type(scm_rocksdb_t, db);

    char *err = NULL;
    rocksdb_t* db_ref = scm_foreign_object_ref(db, 0);
    rocksdb_checkpoint_t* checkpoint = rocksdb_checkpoint_object_create(db_ref, &err);

    if(err != NULL) scm_syserror(err);
    return scm_make_foreign_object_1(scm_rocksdb_checkpoint_t, checkpoint);
}

static SCM grocksdb_checkpoint_create(SCM cp, SCM cp_dir, SCM log_size_for_flush){
    if(scm_is_false(scm_string_p(cp_dir))) scm_wrong_type_arg("not a string", 2, cp_dir);
    if(scm_is_false(scm_integer_p(log_size_for_flush)))
        scm_wrong_type_arg("not a integer", 3, log_size_for_flush);
    scm_assert_foreign_object_type(scm_rocksdb_checkpoint_t, cp);

    char *err = NULL;
    rocksdb_checkpoint_t* cp_ref = scm_foreign_object_ref(cp, 0);
    rocksdb_checkpoint_create(cp_ref, scm_to_utf8_string(cp_dir), scm_to_uint64(log_size_for_flush), &err);

    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_checkpoint_object_destroy(SCM cp){
    scm_assert_foreign_object_type(scm_rocksdb_checkpoint_t, cp);
    rocksdb_checkpoint_t* cp_ref = scm_foreign_object_ref(cp, 0);
    rocksdb_checkpoint_object_destroy(cp_ref);
    return SCM_UNSPECIFIED;
}

// --------------- Init ----------------------------
static void init_checkpoint() {
    scm_rocksdb_checkpoint_t = define_type_wrapper("rocksdb_checkpoint", grocksdb_checkpoint_object_destroy);

    DEF("rocksdb-checkpoint-create", 3, &grocksdb_checkpoint_create);
    DEF("rocksdb-checkpoint-object-create", 1, &grocksdb_checkpoint_object_create);
}
