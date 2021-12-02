// --------------- Wrapers -------------------------
SCM grocksdb_checkpoint_object_create(SCM db){
    scm_assert_foreign_object_type(scm_rocksdb_t, db);

    char *err = NULL;
    rocksdb_checkpoint_t* checkpoint = rocksdb_checkpoint_object_create(scm_get_ref(db), &err);
    if(err != NULL) scm_syserror(err);
    return scm_make_foreign_object_1(scm_rocksdb_checkpoint_t, checkpoint);
}

SCM grocksdb_checkpoint_create(SCM cp, SCM cp_dir, SCM log_size_for_flush){
    scm_assert_foreign_object_type(scm_rocksdb_checkpoint_t, cp);
    SCM_ASSERT_TYPE(scm_string_p(cp_dir), cp_dir, SCM_ARG2, "rocksdb-checkpoint-create", "string");
    SCM_ASSERT_TYPE(scm_integer_p(log_size_for_flush), log_size_for_flush,
                    SCM_ARG3, "rocksdb-checkpoint-create", "integer");

    char *err = NULL;
    rocksdb_checkpoint_create(scm_get_ref(cp),
                              scm_to_utf8_string(cp_dir),
                              scm_to_uint64(log_size_for_flush), &err);

    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

void grocksdb_checkpoint_object_destroy(SCM cp){
    MXSAFE_DESTROY_WITH(cp, rocksdb_checkpoint_object_destroy);
}

// --------------- Init ----------------------------
void init_checkpoint() {
    scm_rocksdb_checkpoint_t = define_type_wrapper("rocksdb_checkpoint", grocksdb_checkpoint_object_destroy);

    DEF("rocksdb-checkpoint-create", 3, &grocksdb_checkpoint_create);
    DEF("rocksdb-checkpoint-object-create", 1, &grocksdb_checkpoint_object_create);
}
