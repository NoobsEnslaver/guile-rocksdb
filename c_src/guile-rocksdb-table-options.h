static SCM k_block_size;
static SCM k_block_size_deviation;
static SCM k_block_restart_interval;
static SCM k_index_block_restart_interval;
static SCM k_metadata_block_size;
static SCM k_partition_filters;
static SCM k_use_delta_encoding;
static SCM k_filter_policy;
static SCM k_no_block_cache;
static SCM k_block_cache;
static SCM k_block_cache_compressed;
static SCM k_whole_key_filtering;
static SCM k_format_version;
static SCM k_index_type;
static SCM k_data_block_index_type;
static SCM k_data_block_hash_ratio;
static SCM k_hash_index_allow_collision;
static SCM k_cache_index_and_filter_blocks;
static SCM k_cache_index_and_filter_blocks_with_high_priority;
static SCM k_pin_l0_filter_and_index_blocks_in_cache;
static SCM k_pin_top_level_index_and_filter;
static SCM k_block_based_table_factory;

static SCM scm_bloom_symbol;
static SCM scm_bloom_full_symbol;
static SCM scm_ribbon_symbol;
static SCM scm_ribbon_hybrid_symbol;
static SCM scm_binary_search_symbol;
static SCM scm_binary_search_and_hash_symbol;
static SCM scm_hash_search_symbol;
static SCM scm_two_level_index_search_symbol;

// --------------- Wrapers -------------------------
SCM grocksdb_block_based_options_create(SCM rest){
    SCM block_size = SCM_UNDEFINED;
    SCM block_size_deviation = SCM_UNDEFINED;
    SCM block_restart_interval = SCM_UNDEFINED;
    SCM index_block_restart_interval = SCM_UNDEFINED;
    SCM metadata_block_size = SCM_UNDEFINED;
    SCM partition_filters = SCM_UNDEFINED;
    SCM use_delta_encoding = SCM_UNDEFINED;
    SCM filter_policy = SCM_UNDEFINED;
    SCM no_block_cache = SCM_UNDEFINED;
    SCM block_cache = SCM_UNDEFINED;
    SCM block_cache_compressed = SCM_UNDEFINED;
    SCM whole_key_filtering = SCM_UNDEFINED;
    SCM format_version = SCM_UNDEFINED;
    SCM index_type = SCM_UNDEFINED;
    SCM data_block_index_type = SCM_UNDEFINED;
    SCM data_block_hash_ratio = SCM_UNDEFINED;
    SCM hash_index_allow_collision = SCM_UNDEFINED;
    SCM cache_index_and_filter_blocks = SCM_UNDEFINED;
    SCM cache_index_and_filter_blocks_with_high_priority = SCM_UNDEFINED;
    SCM pin_l0_filter_and_index_blocks_in_cache = SCM_UNDEFINED;
    SCM pin_top_level_index_and_filter = SCM_UNDEFINED;

    rocksdb_block_based_table_options_t* opts = rocksdb_block_based_options_create();

    scm_c_bind_keyword_arguments("rocksdb-block-based-options-create", rest, 0,
                                 k_block_size, &block_size,
                                 k_block_size_deviation, &block_size_deviation,
                                 k_block_restart_interval, &block_restart_interval,
                                 k_index_block_restart_interval, &index_block_restart_interval,
                                 k_metadata_block_size, &metadata_block_size,
                                 k_partition_filters, &partition_filters,
                                 k_use_delta_encoding, &use_delta_encoding,
                                 k_filter_policy, &filter_policy,
                                 k_no_block_cache, &no_block_cache,
                                 k_block_cache, &block_cache,
                                 k_block_cache_compressed, &block_cache_compressed,
                                 k_whole_key_filtering, &whole_key_filtering,
                                 k_format_version, &format_version,
                                 k_index_type, &index_type,
                                 k_data_block_index_type, &data_block_index_type,
                                 k_data_block_hash_ratio, &data_block_hash_ratio,
                                 k_hash_index_allow_collision, &hash_index_allow_collision,
                                 k_cache_index_and_filter_blocks, &cache_index_and_filter_blocks,
                                 k_cache_index_and_filter_blocks_with_high_priority, &cache_index_and_filter_blocks_with_high_priority,
                                 k_pin_l0_filter_and_index_blocks_in_cache, &pin_l0_filter_and_index_blocks_in_cache,
                                 k_pin_top_level_index_and_filter, &pin_top_level_index_and_filter,
                                 SCM_UNDEFINED);

    if(!SCM_UNBNDP(block_size)){
        SCM_ASSERT_TYPE(scm_is_exact_integer(block_size), block_size, SCM_ARG1,
                        "rocksdb-block-based-options-create:block-size", "integer");
        rocksdb_block_based_options_set_block_size(opts, scm_to_size_t(block_size));}
    if(!SCM_UNBNDP(block_size_deviation)){
        SCM_ASSERT_TYPE(scm_is_exact_integer(block_size_deviation), block_size_deviation, SCM_ARG1,
                        "rocksdb-block-based-options-create:block-size-deviation", "integer");
        rocksdb_block_based_options_set_block_size_deviation(opts, scm_to_int(block_size_deviation));}
    if(!SCM_UNBNDP(block_restart_interval)){
        SCM_ASSERT_TYPE(scm_is_exact_integer(block_restart_interval), block_restart_interval, SCM_ARG1,
                        "rocksdb-block-based-options-create:block_restart_interval", "integer");
        rocksdb_block_based_options_set_block_restart_interval(opts, scm_to_int(block_restart_interval));}
    if(!SCM_UNBNDP(index_block_restart_interval)){
        SCM_ASSERT_TYPE(scm_is_exact_integer(index_block_restart_interval), index_block_restart_interval, SCM_ARG1,
                        "rocksdb-block-based-options-create:index_block_restart_interval", "integer");
        rocksdb_block_based_options_set_index_block_restart_interval(opts, scm_to_int(index_block_restart_interval));}
    if(!SCM_UNBNDP(metadata_block_size)){
        SCM_ASSERT_TYPE(scm_is_exact_integer(metadata_block_size), metadata_block_size, SCM_ARG1,
                        "rocksdb-block-based-options-create:metadata_block_size", "integer");
        rocksdb_block_based_options_set_metadata_block_size(opts, scm_to_size_t(metadata_block_size));}
    if(!SCM_UNBNDP(partition_filters))
        rocksdb_block_based_options_set_partition_filters(opts, scm_is_true(partition_filters));
    if(!SCM_UNBNDP(use_delta_encoding))
        rocksdb_block_based_options_set_use_delta_encoding(opts, scm_is_true(use_delta_encoding));
    if(!SCM_UNBNDP(no_block_cache))
        rocksdb_block_based_options_set_no_block_cache(opts, scm_is_true(no_block_cache));
    if(!SCM_UNBNDP(block_cache)){
        scm_assert_foreign_object_type(scm_rocksdb_cache_t, block_cache);
        scm_foreign_object_set_x(block_cache, 1, (void *)true);
        rocksdb_block_based_options_set_block_cache(opts, scm_get_ref(block_cache));}
    if(!SCM_UNBNDP(block_cache_compressed)){
        scm_assert_foreign_object_type(scm_rocksdb_cache_t, block_cache_compressed);
        scm_foreign_object_set_x(block_cache_compressed, 1, (void *)true);
        rocksdb_block_based_options_set_block_cache_compressed(opts, scm_get_ref(block_cache));}
    if(!SCM_UNBNDP(whole_key_filtering))
        rocksdb_block_based_options_set_whole_key_filtering(opts, scm_is_true(whole_key_filtering));
    if(!SCM_UNBNDP(format_version)){
        SCM_ASSERT_TYPE(scm_is_exact_integer(format_version), format_version, SCM_ARG1,
                        "rocksdb-block-based-options-create:format-version", "integer");
        rocksdb_block_based_options_set_format_version(opts, scm_to_int(format_version));}
    if(!SCM_UNBNDP(index_type)){
        int i = scm_is_eq(index_type, scm_binary_search_symbol)? 0:
            scm_is_eq(index_type, scm_hash_search_symbol)? 1:
            scm_is_eq(index_type, scm_two_level_index_search_symbol)? 2 : -1;
        if(i<0)scm_wrong_type_arg_msg("rocksdb-block-based-options-create:index-type", SCM_ARG1,
                                      index_type, "binary-search | hash-search | two-level-index-search");
        rocksdb_block_based_options_set_index_type(opts, i);}
    if(!SCM_UNBNDP(data_block_index_type)){
        int i = scm_is_eq(data_block_index_type, scm_binary_search_symbol)? 0:
            scm_is_eq(data_block_index_type, scm_binary_search_and_hash_symbol)? 1: -1;
        if(i<0)scm_wrong_type_arg_msg("rocksdb-block-based-options-create:data-block-index-type", SCM_ARG1,
                                      data_block_index_type, "binary-search | binary-search-and-hash");
        rocksdb_block_based_options_set_data_block_index_type(opts, i);}
    if(!SCM_UNBNDP(data_block_hash_ratio)){
        SCM_ASSERT_TYPE(scm_is_real(data_block_hash_ratio), data_block_hash_ratio, SCM_ARG1,
                        "rocksdb-block-based-options-create:data-block-hash-ratio", "real");
        rocksdb_block_based_options_set_data_block_hash_ratio(opts, scm_to_double(data_block_hash_ratio));}
    if(!SCM_UNBNDP(hash_index_allow_collision))
        rocksdb_block_based_options_set_hash_index_allow_collision(opts, scm_is_true(hash_index_allow_collision));
    if(!SCM_UNBNDP(cache_index_and_filter_blocks))
        rocksdb_block_based_options_set_cache_index_and_filter_blocks(opts, scm_is_true(cache_index_and_filter_blocks));
    if(!SCM_UNBNDP(pin_l0_filter_and_index_blocks_in_cache))
        rocksdb_block_based_options_set_pin_l0_filter_and_index_blocks_in_cache(opts, scm_is_true(pin_l0_filter_and_index_blocks_in_cache));
    if(!SCM_UNBNDP(pin_top_level_index_and_filter))
        rocksdb_block_based_options_set_pin_top_level_index_and_filter(opts, scm_is_true(pin_top_level_index_and_filter));
    if(!SCM_UNBNDP(filter_policy)){
        SCM_ASSERT_TYPE(scm_is_pair(filter_policy), filter_policy, SCM_ARG1,
                        "rocksdb-block-based-options-create:filter-policy", "pair");
        rocksdb_filterpolicy_t* filter_policy_obj;
        SCM key = scm_car(filter_policy);
        SCM val = scm_car(scm_cdr(filter_policy));
        if(scm_is_eq(key, scm_bloom_symbol))
            filter_policy_obj = rocksdb_filterpolicy_create_bloom(scm_to_double(val));
        else if(scm_is_eq(key, scm_bloom_full_symbol))
            filter_policy_obj = rocksdb_filterpolicy_create_bloom_full(scm_to_double(val));
        else if(scm_is_eq(key, scm_ribbon_symbol))
            filter_policy_obj = rocksdb_filterpolicy_create_ribbon(scm_to_double(val));
        else if(scm_is_eq(key, scm_ribbon_hybrid_symbol))
            filter_policy_obj = rocksdb_filterpolicy_create_ribbon_hybrid(scm_to_double(val),
                                    scm_to_double(scm_list_ref(filter_policy, scm_from_int(2))));
        else
            scm_wrong_type_arg_msg("rocksdb-block-based-options-create:filter-policy",
                                   SCM_ARG1, filter_policy, "bloom | bloom-full | ribbon | ribbon-hybrid");

        rocksdb_block_based_options_set_filter_policy(opts, filter_policy_obj);}

    return scm_make_foreign_object_2(scm_rocksdb_block_based_options_t, opts, false);
}

void grocksdb_block_based_options_destroy(SCM bbopts){
    MXSAFE_DESTROY_WITH(bbopts, if(!scm_foreign_object_ref(bbopts, 1)) rocksdb_options_destroy);
}

void init_table_options() {
    k_block_size = scm_from_utf8_keyword("block-size");
    k_block_size_deviation = scm_from_utf8_keyword("block-size-deviation");
    k_block_restart_interval = scm_from_utf8_keyword("block-restart-interval");
    k_index_block_restart_interval = scm_from_utf8_keyword("index-block-restart-interval");
    k_metadata_block_size = scm_from_utf8_keyword("metadata-block-size");
    k_partition_filters = scm_from_utf8_keyword("partition-filters");
    k_use_delta_encoding = scm_from_utf8_keyword("use-delta-encoding");
    k_filter_policy = scm_from_utf8_keyword("filter-policy");
    k_no_block_cache = scm_from_utf8_keyword("no-block-cache");
    k_block_cache = scm_from_utf8_keyword("block-cache");
    k_block_cache_compressed = scm_from_utf8_keyword("cache-compressed");
    k_whole_key_filtering = scm_from_utf8_keyword("whole-key-filtering");
    k_format_version = scm_from_utf8_keyword("format-version");
    k_index_type = scm_from_utf8_keyword("index-type");
    k_data_block_index_type = scm_from_utf8_keyword("data-block-index-type");
    k_data_block_hash_ratio = scm_from_utf8_keyword("data-block-hash-ratio");
    k_hash_index_allow_collision = scm_from_utf8_keyword("hash-index-allow-collision");
    k_cache_index_and_filter_blocks = scm_from_utf8_keyword("cache-index-and-filter-blocks");
    k_cache_index_and_filter_blocks_with_high_priority =
        scm_from_utf8_keyword("cache-index-and-filter-blocks-with-high-priority");
    k_pin_l0_filter_and_index_blocks_in_cache =
        scm_from_utf8_keyword("pin-l0-filter-and-index-blocks-in-cache");
    k_pin_top_level_index_and_filter = scm_from_utf8_keyword("pin-top-level-index-and-filter");

    scm_bloom_symbol = scm_permanent_object(scm_from_utf8_symbol("bloom"));
    scm_bloom_full_symbol = scm_permanent_object(scm_from_utf8_symbol("bloom-full"));
    scm_ribbon_symbol = scm_permanent_object(scm_from_utf8_symbol("ribbon"));
    scm_ribbon_hybrid_symbol = scm_permanent_object(scm_from_utf8_symbol("ribbon-hybrid"));
    scm_binary_search_symbol = scm_permanent_object(scm_from_utf8_symbol("binary-search"));
    scm_binary_search_and_hash_symbol = scm_permanent_object(scm_from_utf8_symbol("binary-search-and-hash"));
    scm_hash_search_symbol = scm_permanent_object(scm_from_utf8_symbol("hash-search"));
    scm_two_level_index_search_symbol = scm_permanent_object(scm_from_utf8_symbol("two-level-index-search"));

    scm_rocksdb_block_based_options_t =
        define_type_wrapper_2("rocksdb-block-based-options", "consumed?", grocksdb_block_based_options_destroy);

    DEFREST("rocksdb-block-based-options-create", grocksdb_block_based_options_create);
}
