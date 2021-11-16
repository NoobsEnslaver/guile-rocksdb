SCM scm_rocksdb_t;
SCM scm_rocksdb_options_t;
SCM scm_rocksdb_backup_engine_t;
SCM scm_rocksdb_restore_options_t;
SCM scm_rocksdb_backup_engine_info_t;
SCM scm_rocksdb_checkpoint_t;

// ------------------- Define types -------------------

static SCM define_type_wrapper(char* name_c, void* finalizer){
        SCM name = scm_from_utf8_symbol(name_c);
        SCM slots = scm_list_1(scm_from_utf8_symbol("ref"));

        return scm_make_foreign_object_type(name, slots, (scm_t_struct_finalize)finalizer);
}

// ------------ Structures ----------

/* struct scm_rocksdb_options{ */
/*   rocksdb_options_t* ref; */
/* }; */
