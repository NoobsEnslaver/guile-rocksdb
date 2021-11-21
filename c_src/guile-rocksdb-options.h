// --------------- Wrapers -------------------------
static SCM grocksdb_options_create(){
    return scm_make_foreign_object_1(scm_rocksdb_options_t, rocksdb_options_create());
}

static void grocksdb_options_destroy(SCM options){
    rocksdb_options_destroy((rocksdb_options_t*)scm_foreign_object_ref(options, 0));
}

static SCM grocksdb_options_increase_parallelism(SCM options, SCM n){
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);
    SCM_ASSERT_TYPE(scm_integer_p(n), n, SCM_ARG2, "rocksdb_options_increase_parallelism", "integer");

    rocksdb_options_increase_parallelism(scm_foreign_object_ref(options, 0), scm_to_size_t(n));
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_options_optimize_level_style_compaction(SCM options, SCM n){
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);
    SCM_ASSERT_TYPE(scm_integer_p(n), n, SCM_ARG2, "rocksdb_options_optimize_level_style_compaction", "integer");

    rocksdb_options_optimize_level_style_compaction(scm_foreign_object_ref(options, 0), scm_to_size_t(n));
    return SCM_UNSPECIFIED;
}

static SCM grocksdb_options_set_create_if_missing(SCM options, SCM create_if_missing){
    scm_assert_foreign_object_type(scm_rocksdb_options_t, options);
    unsigned char flag = scm_is_true(create_if_missing)? 1 : 0;

    rocksdb_options_set_create_if_missing(scm_foreign_object_ref(options, 0), flag);
    return SCM_UNSPECIFIED;
}

// see guile-rocksdb-options-utils.cc
extern SCM gload_options_from_file(SCM scm_options_file_name, SCM scm_cache);
extern SCM gget_latest_options_filename(SCM scm_dbpath, SCM scm_env);

// --------------- Init ----------------------------
static void init_options() {
    scm_rocksdb_options_t = define_type_wrapper("rocksdb_options", grocksdb_options_destroy);

    DEF("rocksdb-options-create", 0, &grocksdb_options_create);
    DEF("rocksdb-options-increase-parallelism!", 2, &grocksdb_options_increase_parallelism);
    DEF("rocksdb-options-optimize-level-style-compaction!", 2, &grocksdb_options_optimize_level_style_compaction);
    DEF("rocksdb-options-set-create-if-missing!", 2, &grocksdb_options_set_create_if_missing);
    DEFOPT("load-options-from-file", 1, 1, &gload_options_from_file);
    DEFOPT("get-latest-options-filename", 1, 1, &gget_latest_options_filename);

}

//----------------- TODO ---------------------------

/*

extern ROCKSDB_LIBRARY_API void rocksdb_set_options(
    rocksdb_t* db, int count, const char* const keys[], const char* const values[], char** errptr);

extern ROCKSDB_LIBRARY_API void rocksdb_set_options_cf(
    rocksdb_t* db, rocksdb_column_family_handle_t* handle, int count, const char* const keys[], const char* const values[], char** errptr);

extern ROCKSDB_LIBRARY_API rocksdb_options_t* rocksdb_options_create_copy(
    rocksdb_options_t*);
extern ROCKSDB_LIBRARY_API void rocksdb_options_increase_parallelism(
    rocksdb_options_t* opt, int total_threads);
extern ROCKSDB_LIBRARY_API void rocksdb_options_optimize_for_point_lookup(
    rocksdb_options_t* opt, uint64_t block_cache_size_mb);
extern ROCKSDB_LIBRARY_API void rocksdb_options_optimize_level_style_compaction(
    rocksdb_options_t* opt, uint64_t memtable_memory_budget);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_optimize_universal_style_compaction(
    rocksdb_options_t* opt, uint64_t memtable_memory_budget);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_allow_ingest_behind(rocksdb_options_t*,
                                                   unsigned char);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_compaction_filter(
    rocksdb_options_t*, rocksdb_compactionfilter_t*);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_compaction_filter_factory(
    rocksdb_options_t*, rocksdb_compactionfilterfactory_t*);
extern ROCKSDB_LIBRARY_API void rocksdb_options_compaction_readahead_size(
    rocksdb_options_t*, size_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_comparator(
    rocksdb_options_t*, rocksdb_comparator_t*);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_merge_operator(
    rocksdb_options_t*, rocksdb_mergeoperator_t*);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_uint64add_merge_operator(
    rocksdb_options_t*);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_compression_per_level(
    rocksdb_options_t* opt, int* level_values, size_t num_levels);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_create_if_missing(
    rocksdb_options_t*, unsigned char);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_create_missing_column_families(rocksdb_options_t*,
                                                   unsigned char);

extern ROCKSDB_LIBRARY_API void rocksdb_options_set_error_if_exists(
    rocksdb_options_t*, unsigned char);

extern ROCKSDB_LIBRARY_API void rocksdb_options_set_paranoid_checks(
    rocksdb_options_t*, unsigned char);

extern ROCKSDB_LIBRARY_API void rocksdb_options_set_db_paths(rocksdb_options_t*,
                                                             const rocksdb_dbpath_t** path_values,
                                                             size_t num_paths);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_env(rocksdb_options_t*,
                                                        rocksdb_env_t*);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_info_log(rocksdb_options_t*,
                                                             rocksdb_logger_t*);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_info_log_level(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_write_buffer_size(
    rocksdb_options_t*, size_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_db_write_buffer_size(
    rocksdb_options_t*, size_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_max_open_files(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_max_file_opening_threads(
    rocksdb_options_t*, int);

extern ROCKSDB_LIBRARY_API void rocksdb_options_set_max_total_wal_size(
    rocksdb_options_t* opt, uint64_t n);

extern ROCKSDB_LIBRARY_API void rocksdb_options_set_compression_options(
    rocksdb_options_t*, int, int, int, int);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_compression_options_zstd_max_train_bytes(rocksdb_options_t*,
                                                             int);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_bottommost_compression_options(rocksdb_options_t*, int, int,
                                                   int, int, unsigned char);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_bottommost_compression_options_zstd_max_train_bytes(
    rocksdb_options_t*, int, unsigned char);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_prefix_extractor(
    rocksdb_options_t*, rocksdb_slicetransform_t*);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_num_levels(
    rocksdb_options_t*, int);

extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_level0_file_num_compaction_trigger(rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_level0_slowdown_writes_trigger(rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_level0_stop_writes_trigger(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_max_mem_compaction_level(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_target_file_size_base(
    rocksdb_options_t*, uint64_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_target_file_size_multiplier(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_max_bytes_for_level_base(
    rocksdb_options_t*, uint64_t);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_level_compaction_dynamic_level_bytes(rocksdb_options_t*,
                                                         unsigned char);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_max_bytes_for_level_multiplier(rocksdb_options_t*, double);

extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_max_bytes_for_level_multiplier_additional(
    rocksdb_options_t*, int* level_values, size_t num_levels);
extern ROCKSDB_LIBRARY_API void rocksdb_options_enable_statistics(
    rocksdb_options_t*);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_skip_stats_update_on_db_open(rocksdb_options_t* opt,
                                                 unsigned char val);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_skip_checking_sst_file_sizes_on_db_open(
    rocksdb_options_t* opt, unsigned char val);

// returns a pointer to a malloc()-ed, null terminated string
extern ROCKSDB_LIBRARY_API char* rocksdb_options_statistics_get_string(
    rocksdb_options_t* opt);

extern ROCKSDB_LIBRARY_API void rocksdb_options_set_max_write_buffer_number(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_min_write_buffer_number_to_merge(rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_max_write_buffer_number_to_maintain(rocksdb_options_t*,
                                                        int);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_max_write_buffer_size_to_maintain(rocksdb_options_t*,
                                                      int64_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_enable_pipelined_write(
    rocksdb_options_t*, unsigned char);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_unordered_write(
    rocksdb_options_t*, unsigned char);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_max_subcompactions(
    rocksdb_options_t*, uint32_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_max_background_jobs(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_max_background_compactions(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_base_background_compactions(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_max_background_flushes(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_max_log_file_size(
    rocksdb_options_t*, size_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_log_file_time_to_roll(
    rocksdb_options_t*, size_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_keep_log_file_num(
    rocksdb_options_t*, size_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_recycle_log_file_num(
    rocksdb_options_t*, size_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_soft_rate_limit(
    rocksdb_options_t*, double);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_hard_rate_limit(
    rocksdb_options_t*, double);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_soft_pending_compaction_bytes_limit(
    rocksdb_options_t* opt, size_t v);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_hard_pending_compaction_bytes_limit(
    rocksdb_options_t* opt, size_t v);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_rate_limit_delay_max_milliseconds(rocksdb_options_t*,
                                                      unsigned int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_max_manifest_file_size(
    rocksdb_options_t*, size_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_table_cache_numshardbits(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_table_cache_remove_scan_count_limit(rocksdb_options_t*,
                                                        int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_arena_block_size(
    rocksdb_options_t*, size_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_use_fsync(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_db_log_dir(
    rocksdb_options_t*, const char*);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_wal_dir(rocksdb_options_t*,
                                                            const char*);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_WAL_ttl_seconds(
    rocksdb_options_t*, uint64_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_WAL_size_limit_MB(
    rocksdb_options_t*, uint64_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_manifest_preallocation_size(
    rocksdb_options_t*, size_t);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_purge_redundant_kvs_while_flush(rocksdb_options_t*,
                                                    unsigned char);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_allow_mmap_reads(
    rocksdb_options_t*, unsigned char);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_allow_mmap_writes(
    rocksdb_options_t*, unsigned char);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_use_direct_reads(
    rocksdb_options_t*, unsigned char);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_use_direct_io_for_flush_and_compaction(rocksdb_options_t*,
                                                           unsigned char);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_is_fd_close_on_exec(
    rocksdb_options_t*, unsigned char);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_skip_log_error_on_recovery(
    rocksdb_options_t*, unsigned char);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_stats_dump_period_sec(
    rocksdb_options_t*, unsigned int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_stats_persist_period_sec(
    rocksdb_options_t*, unsigned int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_advise_random_on_open(
    rocksdb_options_t*, unsigned char);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_access_hint_on_compaction_start(rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_use_adaptive_mutex(
    rocksdb_options_t*, unsigned char);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_bytes_per_sync(
    rocksdb_options_t*, uint64_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_wal_bytes_per_sync(
        rocksdb_options_t*, uint64_t);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_writable_file_max_buffer_size(rocksdb_options_t*, uint64_t);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_allow_concurrent_memtable_write(rocksdb_options_t*,
                                                    unsigned char);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_enable_write_thread_adaptive_yield(rocksdb_options_t*,
                                                       unsigned char);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_max_sequential_skip_in_iterations(rocksdb_options_t*,
                                                      uint64_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_disable_auto_compactions(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_optimize_filters_for_hits(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_delete_obsolete_files_period_micros(rocksdb_options_t*,
                                                        uint64_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_prepare_for_bulk_load(
    rocksdb_options_t*);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_memtable_vector_rep(
    rocksdb_options_t*);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_memtable_prefix_bloom_size_ratio(
    rocksdb_options_t*, double);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_max_compaction_bytes(
    rocksdb_options_t*, uint64_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_hash_skip_list_rep(
    rocksdb_options_t*, size_t, int32_t, int32_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_hash_link_list_rep(
    rocksdb_options_t*, size_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_plain_table_factory(
    rocksdb_options_t*, uint32_t, int, double, size_t);

extern ROCKSDB_LIBRARY_API void rocksdb_options_set_min_level_to_compress(
    rocksdb_options_t* opt, int level);

extern ROCKSDB_LIBRARY_API void rocksdb_options_set_memtable_huge_page_size(
    rocksdb_options_t*, size_t);

extern ROCKSDB_LIBRARY_API void rocksdb_options_set_max_successive_merges(
    rocksdb_options_t*, size_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_bloom_locality(
    rocksdb_options_t*, uint32_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_inplace_update_support(
    rocksdb_options_t*, unsigned char);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_inplace_update_num_locks(
    rocksdb_options_t*, size_t);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_report_bg_io_stats(
    rocksdb_options_t*, int);

enum {
  rocksdb_tolerate_corrupted_tail_records_recovery = 0,
  rocksdb_absolute_consistency_recovery = 1,
  rocksdb_point_in_time_recovery = 2,
  rocksdb_skip_any_corrupted_records_recovery = 3
};
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_wal_recovery_mode(
    rocksdb_options_t*, int);

enum {
  rocksdb_no_compression = 0,
  rocksdb_snappy_compression = 1,
  rocksdb_zlib_compression = 2,
  rocksdb_bz2_compression = 3,
  rocksdb_lz4_compression = 4,
  rocksdb_lz4hc_compression = 5,
  rocksdb_xpress_compression = 6,
  rocksdb_zstd_compression = 7
};
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_compression(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_bottommost_compression(
    rocksdb_options_t*, int);

enum {
  rocksdb_level_compaction = 0,
  rocksdb_universal_compaction = 1,
  rocksdb_fifo_compaction = 2
};
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_compaction_style(
    rocksdb_options_t*, int);
extern ROCKSDB_LIBRARY_API void
rocksdb_options_set_universal_compaction_options(
    rocksdb_options_t*, rocksdb_universal_compaction_options_t*);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_fifo_compaction_options(
    rocksdb_options_t* opt, rocksdb_fifo_compaction_options_t* fifo);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_ratelimiter(
    rocksdb_options_t* opt, rocksdb_ratelimiter_t* limiter);
extern ROCKSDB_LIBRARY_API void rocksdb_options_set_atomic_flush(
    rocksdb_options_t* opt, unsigned char);

extern ROCKSDB_LIBRARY_API void rocksdb_options_set_row_cache(
    rocksdb_options_t* opt, rocksdb_cache_t* cache
);

*/
