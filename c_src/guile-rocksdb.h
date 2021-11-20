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
SCM scm_rocksdb_readoptions_t;
SCM scm_rocksdb_column_family_handle_t;
SCM scm_rocksdb_writebatch_t;
SCM scm_rocksdb_env_t;
SCM scm_rocksdb_cache_t;

// ------------------- Define types -------------------

void display(const char* msg){
    scm_display(scm_from_utf8_string(msg), scm_current_output_port());
}

static SCM define_type_wrapper(char* name_c, void* finalizer){
        SCM name = scm_from_utf8_symbol(name_c);
        SCM slots = scm_list_1(scm_from_utf8_symbol("ref"));

        return scm_make_foreign_object_type(name, slots, (scm_t_struct_finalize)finalizer);
}
