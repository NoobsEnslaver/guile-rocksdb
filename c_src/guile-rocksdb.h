#define DEF(name,arity,funref) scm_c_define_gsubr(name, arity, 0, 0, funref); scm_c_export(name, NULL)
#define DEFOPT(name,arity,opt,funref) scm_c_define_gsubr(name, arity, opt, 0, funref); scm_c_export(name, NULL)

// ------------------- Types -------------------------

SCM scm_rocksdb_t;
SCM scm_rocksdb_options_t;
SCM scm_rocksdb_backup_engine_t;
SCM scm_rocksdb_restore_options_t;
SCM scm_rocksdb_backup_engine_info_t;
SCM scm_rocksdb_checkpoint_t;
SCM scm_rocksdb_writeoptions_t;

// ------------------- Define types -------------------

SCM display_func;//debug

static void display(char* msg){
    scm_call_1(scm_variable_ref(display_func), scm_from_utf8_string(msg));
}

static SCM define_type_wrapper(char* name_c, void* finalizer){
        SCM name = scm_from_utf8_symbol(name_c);
        SCM slots = scm_list_1(scm_from_utf8_symbol("ref"));

        return scm_make_foreign_object_type(name, slots, (scm_t_struct_finalize)finalizer);
}
