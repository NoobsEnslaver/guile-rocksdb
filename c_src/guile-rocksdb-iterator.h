#define ASSERT_ITER(i) scm_assert_foreign_object_type(scm_rocksdb_iterator_t, i); \
    if (!rocksdb_iter_valid(scm_get_ref(scm_iterator))) return SCM_BOOL_F

extern SCM grocksdb_iterator_refresh(SCM scm_iterator);

SCM grocksdb_create_iterator(SCM scm_db, SCM scm_readopts){
    rocksdb_readoptions_t* readopts;
    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    BIND_REF_OR_DEFAULT(scm_rocksdb_readoptions_t, scm_readopts, readopts, default_rocksdb_readoptions);

    rocksdb_iterator_t* iter = rocksdb_create_iterator(scm_get_ref(scm_db), readopts);
    void* slots[3] = {iter, scm_db, SCM_UNBNDP(scm_readopts) ? NULL : scm_readopts};
    return scm_make_foreign_object_n(scm_rocksdb_iterator_t, 3, slots);
}

SCM grocksdb_create_iterator_cf(SCM scm_db, SCM scm_cf, SCM scm_readopts){
    rocksdb_readoptions_t* readopts;
    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    scm_assert_foreign_object_type(scm_rocksdb_column_family_handle_t, scm_cf);
    BIND_REF_OR_DEFAULT(scm_rocksdb_readoptions_t, scm_readopts, readopts, default_rocksdb_readoptions);

    rocksdb_iterator_t* iter = rocksdb_create_iterator_cf(scm_get_ref(scm_db),
                                                          readopts,
                                                          scm_get_ref(scm_cf));

    void* slots[3] = {iter, scm_db, SCM_UNBNDP(scm_readopts) ? NULL : scm_readopts};
    return scm_make_foreign_object_n(scm_rocksdb_iterator_t, 3, slots);
}

void grocksdb_iter_destroy(SCM scm_iterator){
    MXSAFE_DESTROY_WITH_2(scm_iterator, rocksdb_iter_destroy);
}

SCM grocksdb_iter_seek_to_first(SCM scm_iterator){
    scm_assert_foreign_object_type(scm_rocksdb_iterator_t, scm_iterator);
    rocksdb_iter_seek_to_first(scm_foreign_object_ref(scm_iterator, 0));
    return SCM_BOOL_T;
}

SCM grocksdb_iter_seek_to_last(SCM scm_iterator){
    scm_assert_foreign_object_type(scm_rocksdb_iterator_t, scm_iterator);
    rocksdb_iter_seek_to_first(scm_foreign_object_ref(scm_iterator, 0));
    return SCM_BOOL_T;
}

SCM grocksdb_iter_next(SCM scm_iterator){
    ASSERT_ITER(scm_iterator);
    rocksdb_iter_next(scm_foreign_object_ref(scm_iterator, 0));
    return SCM_BOOL_T;
}

SCM grocksdb_iter_prev(SCM scm_iterator){
    ASSERT_ITER(scm_iterator);
    rocksdb_iter_prev(scm_foreign_object_ref(scm_iterator, 0));
    return SCM_BOOL_T;
}

SCM grocksdb_iter_seek(SCM scm_iterator, SCM key){
    scm_assert_foreign_object_type(scm_rocksdb_iterator_t, scm_iterator);
    SCM_ASSERT_TYPE(scm_is_bytevector(key), key, SCM_ARG2, "rocksdb-iter-seek!", "bytevector");
    rocksdb_iter_seek(scm_foreign_object_ref(scm_iterator, 0),
                      SCM_BYTEVECTOR_CONTENTS(key),
                      SCM_BYTEVECTOR_LENGTH(key));
    return SCM_BOOL_T;
}

SCM grocksdb_iter_seek_for_prev(SCM scm_iterator, SCM key){
    scm_assert_foreign_object_type(scm_rocksdb_iterator_t, scm_iterator);
    SCM_ASSERT_TYPE(scm_is_bytevector(key), key, SCM_ARG2, "rocksdb-iter-seek-for-prev!", "bytevector");
    rocksdb_iter_seek_for_prev(scm_foreign_object_ref(scm_iterator, 0),
                               SCM_BYTEVECTOR_CONTENTS(key),
                               SCM_BYTEVECTOR_LENGTH(key));
    return SCM_BOOL_T;
}

SCM grocksdb_iter_key(SCM scm_iterator){
    ASSERT_ITER(scm_iterator);
    size_t len;
    const uint8_t *ret = rocksdb_iter_key(scm_foreign_object_ref(scm_iterator, 0), &len);
    return ret == NULL? SCM_BOOL_F : scm_copy_u8vector(ret, len);
}

//same, but without typechecking and copying data
SCM grocksdb_iter_key_unsafe(SCM scm_iterator){
    size_t len;
    rocksdb_iterator_t *iter = (rocksdb_iterator_t *)scm_foreign_object_ref(scm_iterator, 0);
    if (!rocksdb_iter_valid(iter)) return SCM_BOOL_F;
    const uint8_t *ret = rocksdb_iter_key(iter, &len);
    //uint8_t * casting is hack for skip warning about ignoring 'const', we 'take' and realy may to change it
    //FIXME: we need to protect key for garbage collection
    return ret == NULL? SCM_BOOL_F : scm_take_u8vector((uint8_t *)ret, len);
}

SCM grocksdb_iter_value(SCM scm_iterator){
    ASSERT_ITER(scm_iterator);
    size_t len;
    const uint8_t* ret = rocksdb_iter_value(scm_foreign_object_ref(scm_iterator, 0), &len);
    return ret == NULL? SCM_BOOL_F : scm_copy_u8vector(ret, len);
}

//same, but without typechecking and copying data
SCM grocksdb_iter_value_unsafe(SCM scm_iterator){
    size_t len;
    rocksdb_iterator_t *iter = (rocksdb_iterator_t *)scm_foreign_object_ref(scm_iterator, 0);
    const uint8_t *ret = rocksdb_iter_value(iter, &len);
    //uint8_t * casting is hack for skip warning about ignoring 'const', we 'take' and realy may to change it
    //FIXME: we need to protect key for garbage collection
    return ret == NULL? SCM_BOOL_F : scm_take_u8vector((uint8_t *)ret, len);
}

SCM grocksdb_iter_get_error(SCM scm_iterator){
    scm_assert_foreign_object_type(scm_rocksdb_iterator_t, scm_iterator);
    char* err = NULL;
    rocksdb_iter_get_error(scm_foreign_object_ref(scm_iterator, 0), &err);
    return err ? scm_from_utf8_string(err) : SCM_BOOL_F;
}

// ------------------ utils --------------------
SCM grocksdb_iterator_p(SCM obj){
    return scm_from_bool(SCM_IS_A_P(obj, scm_rocksdb_iterator_t));
}

void init_iterator(){
    scm_rocksdb_iterator_t = define_type_wrapper_3("rocksdb-iterator", "db", "opts", grocksdb_iter_destroy);

    /* DEF("rocksdb-iter-destroy!", 1, grocksdb_iter_destroy); */
    DEFOPT("rocksdb-create-iterator", 1, 1, grocksdb_create_iterator);
    DEFOPT("rocksdb-create-iterator-cf", 2, 1, grocksdb_create_iterator_cf);
    DEF("rocksdb-iter-seek-to-first!", 1, grocksdb_iter_seek_to_first);
    DEF("rocksdb-iter-seek-to-last!", 1, grocksdb_iter_seek_to_last);
    DEF("rocksdb-iter-prev!", 1, grocksdb_iter_prev);
    DEF("rocksdb-iter-next!", 1, grocksdb_iter_next);
    DEF("rocksdb-iter-seek!", 2, grocksdb_iter_seek);
    DEF("rocksdb-iter-seek-for-prev!", 2, grocksdb_iter_seek_for_prev);
    DEF("rocksdb-iter-get-error", 1, grocksdb_iter_get_error);
    DEF("rocksdb-iter-key", 1, grocksdb_iter_key);
    DEF("rocksdb-iter-value", 1, grocksdb_iter_value);
    DEF("rocksdb-iter-key-unsafe", 1, grocksdb_iter_key_unsafe);
    DEF("rocksdb-iter-value-unsafe", 1, grocksdb_iter_value_unsafe);
    DEF("rocksdb-iterator?", 1, grocksdb_iterator_p);
    DEF("rocksdb-iter-refresh!", 1, grocksdb_iterator_refresh);
}

/*TODO

extern ROCKSDB_LIBRARY_API void rocksdb_create_iterators(
    rocksdb_t *db, rocksdb_readoptions_t* opts,
    rocksdb_column_family_handle_t** column_families,
    rocksdb_iterator_t** iterators, size_t size, char** errptr);

*/
