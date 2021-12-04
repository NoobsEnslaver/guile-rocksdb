#include <libguile.h>
#include "rocksdb/c.h"
#include "rocksdb/utilities/options_util.h"
#include "rocksdb/convenience.h"
#include "rocksdb/options.h"

using namespace ROCKSDB_NAMESPACE;

using std::vector;
using std::unordered_set;

extern "C" {

extern SCM scm_rocksdb_t;
extern SCM scm_rocksdb_options_t;
extern SCM scm_rocksdb_env_t;
extern SCM scm_rocksdb_cache_t;

extern void display(const char* msg);
extern void* scm_get_ref(SCM obj);

struct rocksdb_t {DB* rep;};
struct rocksdb_options_t { Options rep; };
struct rocksdb_env_t { Env* rep; bool is_default;};
struct rocksdb_cache_t { std::shared_ptr<Cache> rep; };

// spec: (string, rocksdb_cache_t?) -> (rocksdb_options_t, (list (string . rocksdb_options_t))) | (#f, string)
SCM gload_options_from_file(SCM scm_options_file_name, SCM scm_cache){
    ConfigOptions config_options;
    DBOptions *cpp_options = new DBOptions();
    vector<ColumnFamilyDescriptor> *cpp_cf_descs_vec = new vector<ColumnFamilyDescriptor>; // FIXME: purge?
    SCM result[2] = {SCM_BOOL_F, SCM_BOOL_F};

    SCM_ASSERT_TYPE(scm_is_string(scm_options_file_name), scm_options_file_name, SCM_ARG1,
                    "load-options-from-file", "string");
    std::string cpp_options_file_name = std::string(scm_to_utf8_string(scm_options_file_name));

    Status s;
    if (SCM_UNBNDP(scm_cache)){
        s = LoadOptionsFromFile(config_options, cpp_options_file_name,
                                cpp_options, cpp_cf_descs_vec);
    } else {
        scm_assert_foreign_object_type(scm_rocksdb_cache_t, scm_cache);
        scm_foreign_object_set_x(scm_cache, 1, (void *)true);
        rocksdb_cache_t* c_cache = (rocksdb_cache_t*)scm_get_ref(scm_cache);
        std::shared_ptr<Cache> cpp_cache = c_cache->rep;
        s = LoadOptionsFromFile(config_options, cpp_options_file_name,
                                cpp_options, cpp_cf_descs_vec, &cpp_cache);
    };
    if (s.ok()){
        auto op = new Options(*cpp_options, *(new ColumnFamilyOptions()));
        SCM scm_options = scm_make_foreign_object_2(scm_rocksdb_options_t,
                                                    new rocksdb_options_t{*op}, (void*)false);
        SCM scm_list_cf = SCM_EOL;
        for (const auto &cf_desc : *cpp_cf_descs_vec){
            auto cfop = new Options(*(new DBOptions()), cf_desc.options);
            SCM scm_pair = scm_cons(scm_from_utf8_string(cf_desc.name.c_str()),
                                    scm_make_foreign_object_2(scm_rocksdb_options_t,
                                                              new rocksdb_options_t{*cfop}, (void*)false));
            scm_list_cf = scm_cons(scm_pair, scm_list_cf);
        };
        result[0] = scm_options;
        result[1] = scm_list_cf;
    } else {
        result[1] = scm_from_utf8_string(s.getState());
    };
    return scm_c_values(result, 2);
}

// spec: (string, rocksdb_env_t?) -> (string, undefined) | (#f, string)
SCM gget_latest_options_filename(SCM scm_dbpath, SCM scm_env){
    SCM_ASSERT_TYPE(scm_is_string(scm_dbpath), scm_dbpath, SCM_ARG1,
                    "get-latest-options-filename", "string");
    const std::string cpp_dbpath = std::string(scm_to_utf8_string(scm_dbpath));
    rocksdb_env_t* c_env;
    if (SCM_UNBNDP(scm_env)){
        c_env = rocksdb_create_default_env();
    } else {
        scm_assert_foreign_object_type(scm_rocksdb_env_t, scm_env);
        c_env = (rocksdb_env_t*)scm_get_ref(scm_env);
    };
    Env* cpp_env = c_env->rep;
    std::string options_file_name;
    Status s = GetLatestOptionsFileName(cpp_dbpath, cpp_env, &options_file_name);

    SCM result[2] = {SCM_BOOL_F, SCM_UNDEFINED};
    if (s.ok()){
        result[0] = scm_from_utf8_string(options_file_name.c_str());
    } else {
        result[1] = scm_from_utf8_string(s.getState());
    }

    return scm_c_values(result, 2);
}

// spec: (string, rocksdb_cache_t?) -> (rocksdb_options_t, (list (string . rocksdb_options_t)))
//                                   | (#f, string)
SCM gload_latest_options(SCM scm_dbpath, SCM scm_cache){
    ConfigOptions config_options;
    DBOptions *cpp_options = new DBOptions();
    vector<ColumnFamilyDescriptor> *cpp_cf_descs_vec = new vector<ColumnFamilyDescriptor>; //FIXME: purge?
    SCM result[2] = {SCM_BOOL_F, SCM_BOOL_F};

    SCM_ASSERT_TYPE(scm_is_string(scm_dbpath), scm_dbpath, SCM_ARG1,
                    "load-latest-options", "string");
    std::string cpp_dbpath = std::string(scm_to_utf8_string(scm_dbpath));

    Status s;
    if (SCM_UNBNDP(scm_cache)){
        s = LoadLatestOptions(config_options, cpp_dbpath,
                              cpp_options, cpp_cf_descs_vec);
    } else {
        scm_assert_foreign_object_type(scm_rocksdb_cache_t, scm_cache);
        rocksdb_cache_t* c_cache = (rocksdb_cache_t*)scm_get_ref(scm_cache);
        std::shared_ptr<Cache> cpp_cache = c_cache->rep;
        s = LoadLatestOptions(config_options, cpp_dbpath,
                              cpp_options, cpp_cf_descs_vec, &cpp_cache);
    };

    if (s.ok()){
        auto op = new Options(*cpp_options, *(new ColumnFamilyOptions()));
        SCM scm_options = scm_make_foreign_object_2(scm_rocksdb_options_t,
                                                    new rocksdb_options_t{*op}, (void*)false);
        SCM scm_list_cf = SCM_EOL;
        for (const auto &cf_desc : *cpp_cf_descs_vec){
            auto cfop = new Options(*(new DBOptions()), cf_desc.options);
            SCM scm_pair = scm_cons(scm_from_utf8_string(cf_desc.name.c_str()),
                                    scm_make_foreign_object_2(scm_rocksdb_options_t,
                                                              new rocksdb_options_t{*cfop}, (void*)false));
            scm_list_cf = scm_cons(scm_pair, scm_list_cf);
        };
        result[0] = scm_options;
        result[1] = scm_list_cf;
    } else {
        result[1] = scm_from_utf8_string(s.getState());
    };
    return scm_c_values(result, 2);
}

SCM gget_options_from_string(SCM scm_string, SCM scm_base_opts){
    Options *cpp_new_options = new Options(), cpp_base_options;
    rocksdb_options_t *c_base_options;
    SCM result[2] = {SCM_BOOL_F, SCM_BOOL_F};

    SCM_ASSERT_TYPE(scm_is_string(scm_string), scm_string, SCM_ARG1,
                    "string->rocksdb-options", "string");
    std::string cpp_string = std::string(scm_to_utf8_string(scm_string));

    if (SCM_UNBNDP(scm_base_opts)){
        c_base_options = rocksdb_options_create();
    } else {
        scm_assert_foreign_object_type(scm_rocksdb_options_t, scm_base_opts);
        c_base_options = (rocksdb_options_t *)scm_get_ref(scm_base_opts);
    }
    cpp_base_options = c_base_options->rep;
    ConfigOptions config_options;
    config_options.ignore_unknown_options = true; // FIXME: without it opt->string(string->opt) returns error

    Status s = GetOptionsFromString(config_options, cpp_base_options, cpp_string, cpp_new_options);
    if (s.ok()){
        result[0] = scm_make_foreign_object_2(scm_rocksdb_options_t,
                                              new rocksdb_options_t{*cpp_new_options}, (void*)false);
    } else {
        if (SCM_UNBNDP(scm_base_opts)) rocksdb_options_destroy(c_base_options);
        result[1] = scm_from_utf8_string(s.getState());
    };
    return scm_c_values(result, 2);
}

// spec: (rocksdb_options_t, char?) -> (string, undefined) | (#f, string)
SCM gget_string_from_dboptions(SCM scm_db_opts, SCM scm_delim){
    ConfigOptions config_options;
    scm_assert_foreign_object_type(scm_rocksdb_options_t, scm_db_opts);
    if (!SCM_UNBNDP(scm_delim)){
        SCM_ASSERT_TYPE(scm_is_string(scm_delim), scm_delim, SCM_ARG2,
                        "rocksdb-options->string", "string");
        config_options.delimiter = std::string(scm_to_utf8_string(scm_delim));
    }
    SCM result[2] = {SCM_BOOL_F, SCM_BOOL_F};
    std::string cpp_string_db, cpp_string_cf;
    rocksdb_options_t *c_db_opts = (rocksdb_options_t *)scm_get_ref(scm_db_opts);

    Status s1 = GetStringFromDBOptions(config_options, DBOptions(c_db_opts->rep), &cpp_string_db);
    if (!s1.ok()){
        result[1] = scm_from_utf8_string(s1.getState());
        return scm_c_values(result, 2);}

    Status s2 = GetStringFromColumnFamilyOptions(config_options, ColumnFamilyOptions(c_db_opts->rep), &cpp_string_cf);
    if (!s2.ok()){
        result[1] = scm_from_utf8_string(s2.getState());
        return scm_c_values(result, 2);}

    result[0] = scm_from_utf8_string(cpp_string_db.append(cpp_string_cf).c_str());
    return scm_c_values(result, 2);
}

// spec: (string) -> (list (string . string)) | (#f, string)
SCM grocksdb_options_string_to_alist(SCM scm_string_db_opts){
    SCM_ASSERT_TYPE(scm_is_string(scm_string_db_opts), scm_string_db_opts, SCM_ARG1,
                        "rocksdb-options-string->alist", "string");
    std::string opts_str = std::string(scm_to_utf8_string(scm_string_db_opts));
    std::unordered_map<std::string, std::string> opts_map;
    SCM result[2] = {SCM_BOOL_F, SCM_BOOL_F};
    SCM scm_acc = SCM_EOL;

    Status s = StringToMap(opts_str, &opts_map);
    if (s.ok()){
        for (auto const& x : opts_map){
            SCM scm_pair = scm_cons(scm_from_utf8_string(x.first.c_str()),
                                    scm_from_utf8_string(x.second.c_str()));
            scm_acc = scm_cons(scm_pair, scm_acc);
        }
        result[0] = scm_acc;
    } else
        result[1] = scm_from_utf8_string(s.getState());

    return scm_c_values(result, 2);
}

// spec: (rocksdb_t) -> rocksdb_options_t
SCM grocksdb_get_options(SCM scm_db){
    scm_assert_foreign_object_type(scm_rocksdb_t, scm_db);
    rocksdb_t* c_db = (rocksdb_t *)scm_get_ref(scm_db);
    return scm_make_foreign_object_2(scm_rocksdb_options_t,
                                     new rocksdb_options_t{c_db->rep->GetOptions()}, (void*)true);
}
}

// ---------------- TODO ------------------
/*

Status GetPlainTableOptionsFromMap(
    const ConfigOptions& config_options, const PlainTableOptions& table_options,
    const std::unordered_map<std::string, std::string>& opts_map,
    PlainTableOptions* new_table_options);
Status GetBlockBasedTableOptionsFromMap(
    const ConfigOptions& config_options,
    const BlockBasedTableOptions& table_options,
    const std::unordered_map<std::string, std::string>& opts_map,
    BlockBasedTableOptions* new_table_options);
Status GetDBOptionsFromMap(
    const ConfigOptions& cfg_options, const DBOptions& base_options,
    const std::unordered_map<std::string, std::string>& opts_map,
    DBOptions* new_options);
Status GetColumnFamilyOptionsFromMap(
    const ConfigOptions& config_options,
    const ColumnFamilyOptions& base_options,
    const std::unordered_map<std::string, std::string>& opts_map,
    ColumnFamilyOptions* new_options);

Status GetStringFromDBOptions(const ConfigOptions& config_options,
                              const DBOptions& db_options,
                              std::string* opts_str);
Status GetStringFromColumnFamilyOptions(const ConfigOptions& config_options,
                                        const ColumnFamilyOptions& cf_options,
                                        std::string* opts_str);
Status GetBlockBasedTableOptionsFromString(
    const ConfigOptions& config_options,
    const BlockBasedTableOptions& table_options, const std::string& opts_str,
    BlockBasedTableOptions* new_table_options);
Status GetPlainTableOptionsFromString(const ConfigOptions& config_options,
                                      const PlainTableOptions& table_options,
                                      const std::string& opts_str,
                                      PlainTableOptions* new_table_options);

Status StringToMap(const std::string& opts_str,
                   std::unordered_map<std::string, std::string>* opts_map);

*/
