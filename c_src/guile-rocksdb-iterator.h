#define ASSERT_ITER(i) scm_assert_foreign_object_type(scm_rocksdb_iterator_t, i); \
    if (!rocksdb_iter_valid(scm_foreign_object_ref(scm_iterator, 0))) return SCM_BOOL_F

static SCM grocksdb_create_iterator(SCM scm_db, SCM scm_readopts){
    ASSERT_DB(scm_db);
    rocksdb_readoptions_t* readopts;

    if (SCM_UNBNDP(scm_readopts)){
        readopts = rocksdb_readoptions_create();
    } else {
        scm_assert_foreign_object_type(scm_rocksdb_readoptions_t, scm_readopts);
        readopts = scm_foreign_object_ref(scm_readopts, 0);
    }

    rocksdb_iterator_t* iter = rocksdb_create_iterator(scm_foreign_object_ref(scm_db, 0),
                                                       readopts);

    return scm_make_foreign_object_1(scm_rocksdb_iterator_t, iter);
}

static SCM grocksdb_create_iterator_cf(SCM scm_db, SCM scm_cf, SCM scm_readopts){
    ASSERT_DB(scm_db);
    scm_assert_foreign_object_type(scm_rocksdb_column_family_handle_t, scm_cf);
    rocksdb_readoptions_t* readopts;

    if (SCM_UNBNDP(scm_readopts)){
        readopts = rocksdb_readoptions_create();
    } else {
        scm_assert_foreign_object_type(scm_rocksdb_readoptions_t, scm_readopts);
        readopts = scm_foreign_object_ref(scm_readopts, 0);
    }

    rocksdb_iterator_t* iter = rocksdb_create_iterator_cf(scm_foreign_object_ref(scm_db, 0),
                                                          readopts,
                                                          scm_foreign_object_ref(scm_cf, 0));

    return scm_make_foreign_object_1(scm_rocksdb_iterator_t, iter);
}

static void grocksdb_iter_destroy(SCM scm_iterator){
    rocksdb_iter_destroy(scm_foreign_object_ref(scm_iterator, 0));
}

static SCM grocksdb_iter_seek_to_first(SCM scm_iterator){
    scm_assert_foreign_object_type(scm_rocksdb_iterator_t, scm_iterator);
    rocksdb_iter_seek_to_first(scm_foreign_object_ref(scm_iterator, 0));
    return SCM_BOOL_T;
}

static SCM grocksdb_iter_seek_to_last(SCM scm_iterator){
    scm_assert_foreign_object_type(scm_rocksdb_iterator_t, scm_iterator);
    rocksdb_iter_seek_to_first(scm_foreign_object_ref(scm_iterator, 0));
    return SCM_BOOL_T;
}

static SCM grocksdb_iter_next(SCM scm_iterator){
    ASSERT_ITER(scm_iterator);
    rocksdb_iter_next(scm_foreign_object_ref(scm_iterator, 0));
    return SCM_BOOL_T;
}

static SCM grocksdb_iter_prev(SCM scm_iterator){
    ASSERT_ITER(scm_iterator);
    rocksdb_iter_prev(scm_foreign_object_ref(scm_iterator, 0));
    return SCM_BOOL_T;
}

static SCM grocksdb_iter_seek(SCM scm_iterator, SCM key){
    scm_assert_foreign_object_type(scm_rocksdb_iterator_t, scm_iterator);
    SCM_ASSERT_TYPE(scm_bytevector_p(key), key, SCM_ARG2, "rocksdb-iter-seek!", "bytevector");
    rocksdb_iter_seek(scm_foreign_object_ref(scm_iterator, 0),
                      SCM_BYTEVECTOR_CONTENTS(key),
                      SCM_BYTEVECTOR_LENGTH(key));
    return SCM_BOOL_T;
}

static SCM grocksdb_iter_seek_for_prev(SCM scm_iterator, SCM key){
    scm_assert_foreign_object_type(scm_rocksdb_iterator_t, scm_iterator);
    SCM_ASSERT_TYPE(scm_bytevector_p(key), key, SCM_ARG2, "rocksdb-iter-seek-for-prev!", "bytevector");
    rocksdb_iter_seek_for_prev(scm_foreign_object_ref(scm_iterator, 0),
                               SCM_BYTEVECTOR_CONTENTS(key),
                               SCM_BYTEVECTOR_LENGTH(key));
    return SCM_BOOL_T;
}

static SCM grocksdb_iter_key(SCM scm_iterator){
    ASSERT_ITER(scm_iterator);
    size_t len;
    const uint8_t *ret = rocksdb_iter_key(scm_foreign_object_ref(scm_iterator, 0), &len);
    return ret == NULL? SCM_BOOL_F : scm_copy_u8vector(ret, len);
}

static SCM grocksdb_iter_value(SCM scm_iterator){
    ASSERT_ITER(scm_iterator);
    size_t len;
    const uint8_t* ret = rocksdb_iter_value(scm_foreign_object_ref(scm_iterator, 0), &len);
    return ret == NULL? SCM_BOOL_F : scm_copy_u8vector(ret, len);
}

static SCM grocksdb_iter_get_error(SCM scm_iterator){
    scm_assert_foreign_object_type(scm_rocksdb_iterator_t, scm_iterator);
    char* err = NULL;
    rocksdb_iter_get_error(scm_foreign_object_ref(scm_iterator, 0), &err);
    return err ? scm_from_utf8_string(err) : SCM_BOOL_F;
}

// ------------------ utils --------------------
static SCM grocksdb_iterator_p(SCM obj){
    return scm_from_bool(SCM_IS_A_P(obj, scm_rocksdb_iterator_t));
}

void init_iterator(){
    scm_rocksdb_iterator_t = define_type_wrapper("rocksdb-iterator", grocksdb_iter_destroy);

    DEFOPT("rocksdb-create-iterator", 1, 1, grocksdb_create_iterator);
    DEFOPT("rocksdb-create-iterator-cf", 2, 1, grocksdb_create_iterator_cf);
    DEF("rocksdb-iter-seek-to-first!", 1, grocksdb_iter_seek_to_first);
    DEF("rocksdb-iter-seek-to-last!", 1, grocksdb_iter_seek_to_last);
    DEF("rocksdb-iter-prev!", 1, grocksdb_iter_prev);
    DEF("rocksdb-iter-next!", 1, grocksdb_iter_next);
    DEF("rocksdb-iter-seek!", 2, grocksdb_iter_seek);
    DEF("rocksdb-iter-seek-for-prev!", 2, grocksdb_iter_seek_for_prev);
    // DEF("rocksdb-iter-valid", 1, grocksdb_iter_valid); //to prevent explicit usage
    DEF("rocksdb-iter-get-error", 1, grocksdb_iter_get_error);
    DEF("rocksdb-iter-key", 1, grocksdb_iter_key);
    DEF("rocksdb-iter-value", 1, grocksdb_iter_value);
    DEF("rocksdb-iterator?", 1, grocksdb_iterator_p);
}

/*TODO

extern ROCKSDB_LIBRARY_API void rocksdb_create_iterators(
    rocksdb_t *db, rocksdb_readoptions_t* opts,
    rocksdb_column_family_handle_t** column_families,
    rocksdb_iterator_t** iterators, size_t size, char** errptr);

*/
