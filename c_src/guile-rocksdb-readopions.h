static SCM scm_verify_checksums_symbol;
static SCM scm_fill_cache_symbol;
static SCM scm_snapshot_symbol;
static SCM scm_iterate_upper_bound_symbol;
static SCM scm_iterate_lower_bound_symbol;
static SCM scm_read_tier_symbol;
static SCM scm_tailing_symbol;
static SCM scm_readahead_size_symbol;
static SCM scm_prefix_same_as_start_symbol;
static SCM scm_pin_data_symbol;
static SCM scm_total_order_seek_symbol;
static SCM scm_max_skippable_internal_keys_symbol;
static SCM scm_background_purge_on_iterator_cleanup_symbol;
static SCM scm_ignore_range_deletions_symbol;

static rocksdb_readoptions_t* default_rocksdb_readoptions;
// --------------- Wrapers -------------------------

SCM grocksdb_readoptions_create(SCM alist){
    rocksdb_readoptions_t *opts = rocksdb_readoptions_create();
    if (!SCM_UNBNDP(alist)){
        SCM_ASSERT_TYPE(scm_list_p(alist), alist, SCM_ARG1, "rocksdb-readoptions-create", "alist");
        SCM pair, key, val;
        while(!scm_null_p(alist)){
            pair = scm_car(alist);
            alist = scm_cdr(alist);
            key = scm_car(pair);
            val = scm_cdr(pair);
            if(scm_eq_p(key, scm_verify_checksums_symbol))
                rocksdb_readoptions_set_verify_checksums(opts, scm_is_true(val));
            else if (scm_eq_p(key, scm_fill_cache_symbol))
                rocksdb_readoptions_set_fill_cache(opts, scm_is_true(val));
            else if (scm_eq_p(key, scm_snapshot_symbol)){
                scm_assert_foreign_object_type(scm_rocksdb_snapshot_t, val);
                rocksdb_readoptions_set_snapshot(opts, scm_foreign_object_ref(val, 0));}
            else if (scm_eq_p(key, scm_iterate_upper_bound_symbol)){
                SCM_ASSERT_TYPE(scm_string_p(val), val, SCM_ARG1,
                                "rocksdb_readoptions.iterate_upper_bound", "string");
                size_t len;
                char *str = scm_to_utf8_stringn(val, &len);
                rocksdb_readoptions_set_iterate_upper_bound(opts, str, len);}
            else if (scm_eq_p(key, scm_iterate_lower_bound_symbol)){
                SCM_ASSERT_TYPE(scm_string_p(val), val, SCM_ARG1,
                                "rocksdb_readoptions.iterate_lower_bound", "string");
                size_t len;
                char *str = scm_to_utf8_stringn(val, &len);
                rocksdb_readoptions_set_iterate_lower_bound(opts, str, len);}
            else if (scm_eq_p(key, scm_read_tier_symbol)){
                SCM_ASSERT_TYPE(scm_integer_p(val), val, SCM_ARG1,
                                "rocksdb_readoptions.read_tier", "integer");
                rocksdb_readoptions_set_read_tier(opts, scm_to_int(val));}
            else if (scm_eq_p(key, scm_tailing_symbol))
                rocksdb_readoptions_set_tailing(opts, scm_is_true(val));
            else if (scm_eq_p(key, scm_readahead_size_symbol)){
                SCM_ASSERT_TYPE(scm_integer_p(val), val, SCM_ARG1,
                                "rocksdb_readoptions.readahead_size", "integer");
                rocksdb_readoptions_set_readahead_size(opts, scm_to_int(val));}
            else if (scm_eq_p(key, scm_prefix_same_as_start_symbol))
                rocksdb_readoptions_set_prefix_same_as_start(opts, scm_is_true(val));
            else if (scm_eq_p(key, scm_pin_data_symbol))
                rocksdb_readoptions_set_pin_data(opts, scm_is_true(val));
            else if (scm_eq_p(key, scm_total_order_seek_symbol))
                rocksdb_readoptions_set_total_order_seek(opts, scm_is_true(val));
            else if (scm_eq_p(key, scm_max_skippable_internal_keys_symbol)){
                SCM_ASSERT_TYPE(scm_integer_p(val), val, SCM_ARG1,
                                "rocksdb_readoptions.max_skippable_internal_keys", "integer");
                rocksdb_readoptions_set_max_skippable_internal_keys(opts, scm_to_int(val));}
            else if (scm_eq_p(key, scm_background_purge_on_iterator_cleanup_symbol))
                rocksdb_readoptions_set_background_purge_on_iterator_cleanup(opts, scm_is_true(val));
            else if (scm_eq_p(key, scm_ignore_range_deletions_symbol))
                rocksdb_readoptions_set_ignore_range_deletions(opts, scm_is_true(val));
            else
                scm_wrong_type_arg_msg("rocksdb-readoptions-create", SCM_ARG1, key, "unknown key");
        }
    }
    return scm_make_foreign_object_1(scm_rocksdb_readoptions_t, opts);
}

SCM grocksdb_readoptions_to_alist(SCM scm_readoptions){
    scm_assert_foreign_object_type(scm_rocksdb_readoptions_t, scm_readoptions);
    rocksdb_readoptions_t *opt = scm_foreign_object_ref(scm_readoptions, 0);
    return scm_list_n(scm_cons(scm_verify_checksums_symbol,
                               scm_from_bool(rocksdb_readoptions_get_verify_checksums(opt))),
                      scm_cons(scm_fill_cache_symbol,
                               scm_from_bool(rocksdb_readoptions_get_fill_cache(opt))),
                      scm_cons(scm_read_tier_symbol,
                               scm_from_bool(rocksdb_readoptions_get_read_tier(opt))),
                      scm_cons(scm_tailing_symbol,
                               scm_from_bool(rocksdb_readoptions_get_tailing(opt))),
                      scm_cons(scm_readahead_size_symbol,
                               scm_from_bool(rocksdb_readoptions_get_readahead_size(opt))),
                      scm_cons(scm_prefix_same_as_start_symbol,
                               scm_from_bool(rocksdb_readoptions_get_prefix_same_as_start(opt))),
                      scm_cons(scm_pin_data_symbol,
                               scm_from_bool(rocksdb_readoptions_get_pin_data(opt))),
                      scm_cons(scm_total_order_seek_symbol,
                               scm_from_bool(rocksdb_readoptions_get_total_order_seek(opt))),
                      scm_cons(scm_max_skippable_internal_keys_symbol,
                               scm_from_int(rocksdb_readoptions_get_max_skippable_internal_keys(opt))),
                      scm_cons(scm_background_purge_on_iterator_cleanup_symbol,
                               scm_from_bool(rocksdb_readoptions_get_background_purge_on_iterator_cleanup(opt))),
                      scm_cons(scm_ignore_range_deletions_symbol,
                               scm_from_bool(rocksdb_readoptions_get_ignore_range_deletions(opt))),
                      SCM_UNDEFINED);
}

void grocksdb_readoptions_destroy(SCM scm_readoptions){
    MXSAFE_DESTROY_WITH(scm_readoptions, rocksdb_readoptions_destroy);
}
// --------------- Init ----------------------------
void init_readoptions() {
    scm_rocksdb_readoptions_t = define_type_wrapper("rocksdb-readoptions", grocksdb_readoptions_destroy);

    scm_verify_checksums_symbol = scm_permanent_object(scm_from_locale_symbol("verify-checksums"));
    scm_fill_cache_symbol = scm_permanent_object(scm_from_locale_symbol("fill-cache"));
    scm_snapshot_symbol = scm_permanent_object(scm_from_locale_symbol("snapshot"));
    scm_iterate_upper_bound_symbol = scm_permanent_object(scm_from_locale_symbol("iterate-upper-bound"));
    scm_iterate_lower_bound_symbol = scm_permanent_object(scm_from_locale_symbol("iterate-lower-bound"));
    scm_read_tier_symbol = scm_permanent_object(scm_from_locale_symbol("read-tier"));
    scm_tailing_symbol = scm_permanent_object(scm_from_locale_symbol("tailing"));
    scm_readahead_size_symbol = scm_permanent_object(scm_from_locale_symbol("readahead-size"));
    scm_prefix_same_as_start_symbol = scm_permanent_object(scm_from_locale_symbol("prefix-same-as-start"));
    scm_pin_data_symbol = scm_permanent_object(scm_from_locale_symbol("pin_data"));
    scm_total_order_seek_symbol = scm_permanent_object(scm_from_locale_symbol("total-order-seek"));
    scm_max_skippable_internal_keys_symbol = scm_permanent_object(scm_from_locale_symbol("max-skippable-internal-keys"));
    scm_background_purge_on_iterator_cleanup_symbol = scm_permanent_object(scm_from_locale_symbol("background-purge-on-iterator-cleanup"));
    scm_ignore_range_deletions_symbol = scm_permanent_object(scm_from_locale_symbol("ignore-range-deletions"));

    default_rocksdb_readoptions = rocksdb_readoptions_create();

    DEFOPT("rocksdb-readoptions-create", 0, 1, &grocksdb_readoptions_create);
    DEF("rocksdb-readoptions->alist", 1, grocksdb_readoptions_to_alist);
}
