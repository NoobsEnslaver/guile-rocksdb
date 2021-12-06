// --------------- Helpers -------------------------
#define DEF(name,arity,funref) scm_c_define_gsubr(name, arity, 0, 0, funref); scm_c_export(name, NULL)
#define DEFOPT(name,arity,opt,funref) scm_c_define_gsubr(name, arity, opt, 0, funref); scm_c_export(name, NULL)
#define DEFREST(name,funref) scm_c_define_gsubr(name, 0, 0, 1, funref); scm_c_export(name, NULL)
#define ASSERT_CONSUME(type, obj) scm_assert_foreign_object_type(type, obj); \
    if(scm_foreign_object_ref(obj, 1)) scm_misc_error(NULL, "using object after it's consumed", SCM_EOL); \
    scm_foreign_object_set_x(obj, 1, (void *)true)
#define BIND_REF_OR_DEFAULT(type, from, to, def) if(SCM_UNBNDP(from)) to = def; \
    else{scm_assert_foreign_object_type(type, from); to = scm_get_ref(from);}
#define SAFE_DESTROY_WITH(obj, fun) if(scm_foreign_object_ref(obj, 0)){\
        fun(scm_foreign_object_ref(obj, 0));scm_foreign_object_set_x(obj, 0, NULL);}
#define SAFE_DESTROY_WITH_2(obj, fun)                                       \
    if (scm_foreign_object_ref(obj, 1) &&                                   \
        scm_foreign_object_ref(scm_foreign_object_ref(obj, 1), 0)){         \
        scm_foreign_object_set_x(obj, 1, NULL); SAFE_DESTROY_WITH(obj, fun);}
#define MX(body) pthread_mutex_lock(&destroy_mutex); body; pthread_mutex_unlock(&destroy_mutex)
#define MXSAFE_DESTROY_WITH(obj, fun) MX(SAFE_DESTROY_WITH(obj, fun))
#define MXSAFE_DESTROY_WITH_2(obj, fun) MX(SAFE_DESTROY_WITH_2(obj, fun))

//unwrap ref with NULL checks
void* scm_get_ref(SCM obj){
    void *ref = scm_foreign_object_ref(obj, 0);
    if(!ref) scm_syserror_msg(NULL, "using object after destruction", scm_list_1(obj), EFAULT);
    return ref;
}

// unwrap ref with NULL checks, check if it used before, if does - create copy, otherwise
// return original ref and mark it as 'used'
void* scm_get_exclusive_ref(SCM obj, void* (*copier)(void*)){
    void* ref = scm_get_ref(obj);
    ref = scm_foreign_object_ref(obj, 1)? copier(ref) : ref;
    scm_foreign_object_set_x(obj, 1, (void *)true);
    return ref;
}

//just 'scm_get_exclusive_ref' with types casting
#define UNWRAP_OPTIONS(obj)\
    (rocksdb_options_t*)scm_get_exclusive_ref(obj, (void* (*)(void*))rocksdb_options_create_copy)

static pthread_mutex_t destroy_mutex;

// ------------------- Types -------------------------
SCM scm_rocksdb_t;
SCM scm_rocksdb_options_t;
SCM scm_rocksdb_backup_engine_t;
SCM scm_rocksdb_restore_options_t;
SCM scm_rocksdb_backup_engine_info_t;
SCM scm_rocksdb_checkpoint_t;
SCM scm_rocksdb_writeoptions_t;
SCM scm_rocksdb_readoptions_t;
SCM scm_rocksdb_column_family_handle_t;
SCM scm_rocksdb_writebatch_t;
SCM scm_rocksdb_env_t;
SCM scm_rocksdb_cache_t;
SCM scm_rocksdb_iterator_t;
SCM scm_rocksdb_snapshot_t;
SCM scm_rocksdb_block_based_options_t;
SCM scm_rocksdb_cuckoo_options_t;
SCM scm_rocksdb_plain_options_t;

// ------------------- Helpers ------------------------
void* scm_copy_u8vector(const void *src, size_t len){
    return scm_take_u8vector(memcpy(scm_malloc(len), src, len), len);
}

void display(const char* msg){
    scm_display(scm_from_utf8_string(msg), scm_current_output_port());
}

SCM define_type_wrapper(char* name_c, void* finalizer){
    SCM name = scm_from_utf8_symbol(name_c);
    SCM slots = scm_list_1(scm_from_utf8_symbol("ref"));

    return scm_make_foreign_object_type(name, slots, (scm_t_struct_finalize)finalizer);
}

SCM define_type_wrapper_2(char* name_c, char* extra_slot_c, void* finalizer){
    SCM name = scm_from_utf8_symbol(name_c);
    SCM slots = scm_list_2(scm_from_utf8_symbol("ref"),
                           scm_from_utf8_symbol(extra_slot_c));

    return scm_make_foreign_object_type(name, slots, (scm_t_struct_finalize)finalizer);
}

SCM define_type_wrapper_3(char* name_c, char* extra_slot_a, char* extra_slot_b, void* finalizer){
    SCM name = scm_from_utf8_symbol(name_c);
    SCM slots = scm_list_3(scm_from_utf8_symbol("ref"),
                           scm_from_utf8_symbol(extra_slot_a),
                           scm_from_utf8_symbol(extra_slot_b));

    return scm_make_foreign_object_type(name, slots, (scm_t_struct_finalize)finalizer);
}

SCM define_type_wrapper_4(char* name_c, char* extra_slot_a, char* extra_slot_b, char* extra_slot_c, void* finalizer){
    SCM name = scm_from_utf8_symbol(name_c);
    SCM slots = scm_list_4(scm_from_utf8_symbol("ref"),
                           scm_from_utf8_symbol(extra_slot_a),
                           scm_from_utf8_symbol(extra_slot_b),
                           scm_from_utf8_symbol(extra_slot_c));

    return scm_make_foreign_object_type(name, slots, (scm_t_struct_finalize)finalizer);
}
