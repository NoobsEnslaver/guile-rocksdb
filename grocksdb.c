#include <libguile.h>
#include <stdlib.h>
#include "rocksdb/c.h"
#include "grocksdb.h"

// ------------------ Types -------------------------

/* static void display(char* msg){ */
/*     scm_call_1(display_func, scm_from_utf8_string(msg)); */
/* } */

// --------------- Wrapers -------------------------

static SCM grocksdb_options_create(){
    return scm_make_foreign_object_1(scm_rocksdb_options_t, rocksdb_options_create());
}

static SCM grocksdb_options_increase_parallelism(SCM options, SCM n){
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);
    rocksdb_options_t* ref = scm_foreign_object_ref(options, 0);
    rocksdb_options_increase_parallelism(ref, scm_to_size_t(n));
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_options_optimize_level_style_compaction(SCM options, SCM n){
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);
    rocksdb_options_t* ref = scm_foreign_object_ref(options, 0);
    rocksdb_options_optimize_level_style_compaction(ref, scm_to_size_t(n));
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_options_set_create_if_missing(SCM options, SCM n){
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);
    rocksdb_options_t* ref = scm_foreign_object_ref(options, 0);
    rocksdb_options_set_create_if_missing(ref, scm_to_size_t(n));
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_open(SCM options, SCM db_path){
    if(scm_is_false(scm_string_p(db_path))) scm_wrong_type_arg("not a string", 2, db_path);
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);

    char *err = NULL;
    rocksdb_t *db;
    rocksdb_options_t* ref = scm_foreign_object_ref(options, 0);

    db = rocksdb_open(ref, scm_to_utf8_string(db_path), &err);
    if(err != NULL) scm_syserror(err);

    return scm_make_foreign_object_1(scm_rocksdb_t, db);
}

static SCM grocksdb_open_with_ttl(SCM options, SCM db_path, SCM ttl){
    if(scm_is_false(scm_string_p(db_path))) scm_wrong_type_arg("not a string", 2, db_path);
    if(scm_is_false(scm_integer_p(ttl))) scm_wrong_type_arg("not a integer", 3, ttl);
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);

    char *err = NULL;
    rocksdb_t *db;
    rocksdb_options_t* ref = scm_foreign_object_ref(options, 0);

    db = rocksdb_open_with_ttl(ref, scm_to_utf8_string(db_path), scm_to_size_t(ttl), &err);
    if(err != NULL) scm_syserror(err);

    return scm_make_foreign_object_1(scm_rocksdb_t, db);
}

static SCM grocksdb_open_for_read_only(SCM options, SCM db_path, SCM error_if_log_file_exist){
    if(scm_is_false(scm_string_p(db_path))) scm_wrong_type_arg("not a string", 2, db_path);
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);

    unsigned char flag = scm_is_false(error_if_log_file_exist)? 0 : 1;
    char *err = NULL;
    rocksdb_t *db;
    rocksdb_options_t* ref = scm_foreign_object_ref(options, 0);

    db = rocksdb_open_for_read_only(ref, scm_to_utf8_string(db_path), flag, &err);

    if(err != NULL) scm_syserror(err);
    return scm_make_foreign_object_1(scm_rocksdb_t, db);
}

static SCM grocksdb_backup_engine_open(SCM options, SCM path){
    if(scm_is_false(scm_string_p(path))) scm_wrong_type_arg("not a string", 2, path);
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);

    char *err = NULL;
    rocksdb_backup_engine_t* bk;
    rocksdb_options_t* ref = scm_foreign_object_ref(options, 0);

    bk = rocksdb_backup_engine_open(ref, scm_to_utf8_string(path), &err);

    if(err != NULL) scm_syserror(err);
    return scm_make_foreign_object_1(scm_rocksdb_backup_engine_t, bk);
}

static SCM grocksdb_backup_engine_create_new_backup(SCM bk, SCM db){
    scm_assert_foreign_object_type(scm_rocksdb_t, db);
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_t, bk);

    char *err = NULL;
    rocksdb_t* db_ref = scm_foreign_object_ref(db, 0);
    rocksdb_backup_engine_t* bk_ref = scm_foreign_object_ref(bk, 0);

    rocksdb_backup_engine_create_new_backup(bk_ref, db_ref, &err);

    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_backup_engine_create_new_backup_flush(SCM bk, SCM db, SCM flush_before_backup){
    scm_assert_foreign_object_type(scm_rocksdb_t, db);
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_t, bk);

    char *err = NULL;
    rocksdb_t* db_ref = scm_foreign_object_ref(db, 0);
    rocksdb_backup_engine_t* bk_ref = scm_foreign_object_ref(bk, 0);
    unsigned char flag = scm_is_false(flush_before_backup)? 0 : 1;

    rocksdb_backup_engine_create_new_backup_flush(bk_ref, db_ref, flag, &err);

    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_backup_engine_purge_old_backups(SCM bk, SCM num_backups_to_keep){
    if(scm_is_false(scm_integer_p(num_backups_to_keep)))
        scm_wrong_type_arg("not a integer", 2, num_backups_to_keep);
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_t, bk);

    char *err = NULL;
    rocksdb_backup_engine_t* bk_ref = scm_foreign_object_ref(bk, 0);

    rocksdb_backup_engine_purge_old_backups(bk_ref, scm_to_size_t(num_backups_to_keep), &err);

    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_restore_options_create(){
    return scm_make_foreign_object_1(scm_rocksdb_restore_options_t, rocksdb_restore_options_create());
}

static SCM grocksdb_restore_options_destroy(SCM opt){
    scm_assert_foreign_object_type(scm_rocksdb_restore_options_t, opt);
    rocksdb_restore_options_t* ref = scm_foreign_object_ref(opt, 0);
    rocksdb_restore_options_destroy(ref);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_restore_options_set_keep_log_files(SCM options, SCM n){
    scm_assert_foreign_object_type(scm_rocksdb_restore_options_t, options);
    rocksdb_restore_options_t* ref = scm_foreign_object_ref(options, 0);
    rocksdb_restore_options_set_keep_log_files(ref, scm_to_size_t(n));
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_backup_engine_verify_backup(SCM bk, SCM backup_id){
    if(scm_is_false(scm_integer_p(backup_id))) scm_wrong_type_arg("not a integer", 2, backup_id);
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_t, bk);

    char *err = NULL;
    rocksdb_backup_engine_t* bk_ref = scm_foreign_object_ref(bk, 0);

    rocksdb_backup_engine_verify_backup(bk_ref, scm_to_size_t(backup_id), &err);

    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_backup_engine_restore_db_from_latest_backup(SCM bk, SCM db_dir, SCM wal_dir, SCM opt){
    if(scm_is_false(scm_string_p(db_dir)))  scm_wrong_type_arg("not a string", 2, db_dir);
    if(scm_is_false(scm_string_p(wal_dir))) scm_wrong_type_arg("not a string", 3, wal_dir);
    scm_assert_foreign_object_type(scm_rocksdb_restore_options_t, opt);
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_t, bk);

    char *err = NULL;
    rocksdb_backup_engine_t* bk_ref = scm_foreign_object_ref(bk, 0);
    rocksdb_restore_options_t* opt_ref = scm_foreign_object_ref(opt, 0);

    rocksdb_backup_engine_restore_db_from_latest_backup(bk_ref,
                                                        scm_to_utf8_string(db_dir),
                                                        scm_to_utf8_string(wal_dir),
                                                        opt_ref,
                                                        &err);
    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_backup_engine_get_backup_info(SCM bk){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_t, bk);

    rocksdb_backup_engine_t* bk_ref = scm_foreign_object_ref(bk, 0);
    rocksdb_backup_engine_info_t* info = rocksdb_backup_engine_get_backup_info(bk_ref);

    return scm_make_foreign_object_1(scm_rocksdb_backup_engine_info_t, info);
}

static SCM grocksdb_backup_engine_info_count(SCM info){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_info_t, info);

    rocksdb_backup_engine_info_t* ref = scm_foreign_object_ref(info, 0);
    int result = rocksdb_backup_engine_info_count(ref);

    return scm_from_size_t(result);
}

static SCM grocksdb_backup_engine_info_timestamp(SCM info, SCM index){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_info_t, info);

    rocksdb_backup_engine_info_t* ref = scm_foreign_object_ref(info, 0);
    int64_t result = rocksdb_backup_engine_info_timestamp(ref, scm_to_size_t(index));

    return scm_from_int64(result);
}

static SCM grocksdb_backup_engine_info_backup_id(SCM info, SCM index){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_info_t, info);

    rocksdb_backup_engine_info_t* ref = scm_foreign_object_ref(info, 0);
    uint32_t result = rocksdb_backup_engine_info_backup_id(ref, scm_to_size_t(index));

    return scm_from_uint32(result);
}

static SCM grocksdb_backup_engine_info_size(SCM info, SCM index){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_info_t, info);

    rocksdb_backup_engine_info_t* ref = scm_foreign_object_ref(info, 0);
    uint64_t result = rocksdb_backup_engine_info_size(ref, scm_to_size_t(index));

    return scm_from_uint64(result);
}

static SCM grocksdb_backup_engine_info_number_files(SCM info, SCM index){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_info_t, info);

    rocksdb_backup_engine_info_t* ref = scm_foreign_object_ref(info, 0);
    uint32_t result = rocksdb_backup_engine_info_number_files(ref, scm_to_size_t(index));

    return scm_from_uint32(result);
}

static SCM grocksdb_backup_engine_info_destroy(SCM info){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_info_t, info);
    rocksdb_backup_engine_info_t* ref = scm_foreign_object_ref(info, 0);
    rocksdb_backup_engine_info_destroy(ref);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_backup_engine_close(SCM bk){
    scm_assert_foreign_object_type(scm_rocksdb_backup_engine_t, bk);
    rocksdb_backup_engine_t* ref = scm_foreign_object_ref(bk, 0);
    rocksdb_backup_engine_close(ref);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_checkpoint_object_create(SCM db){
    scm_assert_foreign_object_type(scm_rocksdb_t, db);

    char *err = NULL;
    rocksdb_t* db_ref = scm_foreign_object_ref(db, 0);
    rocksdb_checkpoint_t* checkpoint = rocksdb_checkpoint_object_create(db_ref, err);

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
    rocksdb_checkpoint_create(cp_ref, scm_to_utf8_string(cp_dir), scm_to_uint64(log_size_for_flush), err);

    if(err != NULL) scm_syserror(err);
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_checkpoint_object_destroy(SCM cp){
    scm_assert_foreign_object_type(scm_rocksdb_checkpoint_t, cp);
    rocksdb_checkpoint_t* cp_ref = scm_foreign_object_ref(cp, 0);
    rocksdb_checkpoint_object_destroy(cp_ref);
    return SCM_UNSPECIFIED;
}

/* --- waiting for registration */


// ------------------------------

static SCM grocksdb_close(SCM db){
    scm_assert_foreign_object_type(scm_rocksdb_t, db);
    rocksdb_t* ref = scm_foreign_object_ref(db, 0);
    rocksdb_close(ref);
    return SCM_UNSPECIFIED;
}

//-------------------------------------------------------------------------------------------

static void init_types(){
    scm_rocksdb_t = define_type_wrapper("rocksdb", grocksdb_close);
    scm_rocksdb_options_t = define_type_wrapper("rocksdb_options", NULL);
    scm_rocksdb_backup_engine_t = define_type_wrapper("rocksdb_backup_engine", grocksdb_backup_engine_close);
    scm_rocksdb_restore_options_t = define_type_wrapper("rocksdb_restore_options", grocksdb_restore_options_destroy);
    scm_rocksdb_backup_engine_info_t = define_type_wrapper("rocksdb_backup_engine_info", grocksdb_backup_engine_info_destroy);

    scm_rocksdb_checkpoint_t = define_type_wrapper("rocksdb_checkpoint", grocksdb_checkpoint_object_destroy);
}

static void register_functions (void* data) {
    scm_c_define_gsubr("rocksdb-open", 2, 0, 0, &grocksdb_open);
    scm_c_define_gsubr("rocksdb-open-with-ttl", 3, 0, 0, &grocksdb_open_with_ttl);
    scm_c_define_gsubr("rocksdb-open-for-read-only", 3, 0, 0, &grocksdb_open_for_read_only);
    scm_c_define_gsubr("rocksdb-backup-engine-open", 2, 0, 0, &grocksdb_backup_engine_open);
    scm_c_define_gsubr("rocksdb-backup-engine-create-new-backup", 2, 0, 0, &grocksdb_backup_engine_create_new_backup);
    scm_c_define_gsubr("rocksdb-backup-engine-purge-old-backups", 2, 0, 0, &grocksdb_backup_engine_purge_old_backups);
    scm_c_define_gsubr("rocksdb-restore-options-set-keep-log-files!", 2, 0, 0, &grocksdb_restore_options_set_keep_log_files);
    scm_c_define_gsubr("rocksdb-backup-engine-verify-backup", 2, 0, 0, &grocksdb_backup_engine_verify_backup);
    scm_c_define_gsubr("rocksdb-backup-engine-restore-db-from-latest-backup", 4, 0, 0, &grocksdb_backup_engine_restore_db_from_latest_backup);
    scm_c_define_gsubr("rocksdb-backup-engine-get-backup-info", 1, 0, 0, &grocksdb_backup_engine_get_backup_info);
    scm_c_define_gsubr("rocksdb-backup-engine-info-count", 1, 0, 0, &grocksdb_backup_engine_info_count);
    scm_c_define_gsubr("rocksdb-backup-engine-info-timestamp", 2, 0, 0, &grocksdb_backup_engine_info_timestamp);
    scm_c_define_gsubr("rocksdb-backup-engine-info-backup-id", 2, 0, 0, &grocksdb_backup_engine_info_backup_id);
    scm_c_define_gsubr("rocksdb-backup-engine-info-size", 2, 0, 0, &grocksdb_backup_engine_info_size);
    scm_c_define_gsubr("rocksdb-backup-engine-info-number-files", 2, 0, 0, &grocksdb_backup_engine_info_number_files);
    scm_c_define_gsubr("rocksdb-backup-engine-close!", 1, 0, 0, &grocksdb_backup_engine_close);
    scm_c_define_gsubr("rocksdb-checkpoint-object-create", 1, 0, 0, &grocksdb_checkpoint_object_create);
    scm_c_define_gsubr("rocksdb-checkpoint-create", 3, 0, 0, &grocksdb_checkpoint_create);
    //scm_c_define_gsubr("", 2, 0, 0, &);


    scm_c_define_gsubr("rocksdb-close!", 1, 0, 0, &grocksdb_close);

    scm_c_define_gsubr("rocksdb-options-create", 0, 0, 0, &grocksdb_options_create);
    scm_c_define_gsubr("rocksdb-options-increase-parallelism!", 2, 0, 0, &grocksdb_options_increase_parallelism);
    scm_c_define_gsubr("rocksdb-options-optimize-level-style-compaction!", 2, 0, 0, &grocksdb_options_optimize_level_style_compaction);
    scm_c_define_gsubr("rocksdb-options-set-create-if-missing!", 2, 0, 0, &grocksdb_options_set_create_if_missing);


    return NULL;
}

/* INIT */
void init() {
    scm_with_guile(&register_functions, NULL);
    init_types();

    /* display_func = scm_variable_ref(scm_c_lookup("display")); //debug */

    /* scm_shell (argc, argv); */
}
