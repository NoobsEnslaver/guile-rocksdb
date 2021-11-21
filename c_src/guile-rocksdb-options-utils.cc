#include <libguile.h>
#include "rocksdb/c.h"
#include "rocksdb/utilities/options_util.h"

using namespace ROCKSDB_NAMESPACE;

using std::vector;
using std::unordered_set;

extern "C" {
extern SCM scm_rocksdb_options_t;
extern SCM scm_rocksdb_env_t;
extern SCM scm_rocksdb_cache_t;
//extern void display(const char* msg);

struct rocksdb_options_t { Options rep; };
struct rocksdb_env_t { Env* rep; bool is_default;};
struct rocksdb_cache_t { std::shared_ptr<Cache> rep; };

SCM gload_options_from_file(SCM scm_options_file_name, SCM scm_cache){
    ConfigOptions config_options;
    DBOptions cpp_options;
    std::vector<ColumnFamilyDescriptor> cpp_cf_descs_vec;
    SCM result[2] = {SCM_BOOL_F, SCM_BOOL_F};

    SCM_ASSERT_TYPE(scm_string_p(scm_options_file_name), scm_options_file_name, SCM_ARG1,
                    "load-options-from-file", "string");
    std::string cpp_options_file_name = std::string(scm_to_utf8_string(scm_options_file_name));

    Status s;
    if (SCM_UNBNDP(scm_cache)){
        s = LoadOptionsFromFile(config_options, cpp_options_file_name,
                                &cpp_options, &cpp_cf_descs_vec);
    } else {
        scm_assert_foreign_object_type(scm_rocksdb_cache_t, scm_cache);
        rocksdb_cache_t* c_cache = (rocksdb_cache_t*)scm_foreign_object_ref(scm_cache, 0);
        std::shared_ptr<Cache> cpp_cache = c_cache->rep;
        s = LoadOptionsFromFile(config_options, cpp_options_file_name,
                                &cpp_options, &cpp_cf_descs_vec, &cpp_cache);
    };

    if (s.ok()){
        rocksdb_options_t c_options = {Options(cpp_options, ColumnFamilyOptions())};
        SCM scm_options = scm_make_foreign_object_1(scm_rocksdb_options_t, &c_options);
        SCM scm_list_cf = SCM_EOL;
        for (const auto &cf_desc : cpp_cf_descs_vec){
            auto cf_wrapped = Options(DBOptions(), cf_desc.options);
            SCM scm_pair = scm_cons(scm_from_utf8_string(cf_desc.name.c_str()),
                                    scm_make_foreign_object_1(scm_rocksdb_options_t, &cf_wrapped));
            scm_list_cf = scm_cons(scm_pair, scm_list_cf);
        };
        result[0] = scm_options;
        result[1] = scm_list_cf;
    } else {
        result[1] = scm_from_utf8_string(s.getState());
    };
    return scm_c_values(result, 2);
}

SCM gget_latest_options_filename(SCM scm_dbpath, SCM scm_env){
    SCM_ASSERT_TYPE(scm_string_p(scm_dbpath), scm_dbpath, SCM_ARG1,
                    "get-latest-options-filename", "string");
    const std::string cpp_dbpath = std::string(scm_to_utf8_string(scm_dbpath));
    rocksdb_env_t* c_env;
    if (SCM_UNBNDP(scm_env)){
        c_env = rocksdb_create_default_env();
    } else {
        scm_assert_foreign_object_type(scm_rocksdb_env_t, scm_env);
        c_env = (rocksdb_env_t*)scm_foreign_object_ref(scm_env, 0);
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

SCM gload_latest_options(SCM scm_dbpath, SCM scm_cache){
    ConfigOptions config_options;
    DBOptions cpp_options;
    std::vector<ColumnFamilyDescriptor> cpp_cf_descs_vec;
    SCM result[2] = {SCM_BOOL_F, SCM_BOOL_F};

    SCM_ASSERT_TYPE(scm_string_p(scm_dbpath), scm_dbpath, SCM_ARG1,
                    "load-latest-options", "string");
    std::string cpp_dbpath = std::string(scm_to_utf8_string(scm_dbpath));

    Status s;
    if (SCM_UNBNDP(scm_cache)){
        s = LoadLatestOptions(config_options, cpp_dbpath,
                              &cpp_options, &cpp_cf_descs_vec);
    } else {
        scm_assert_foreign_object_type(scm_rocksdb_cache_t, scm_cache);
        rocksdb_cache_t* c_cache = (rocksdb_cache_t*)scm_foreign_object_ref(scm_cache, 0);
        std::shared_ptr<Cache> cpp_cache = c_cache->rep;
        s = LoadLatestOptions(config_options, cpp_dbpath,
                              &cpp_options, &cpp_cf_descs_vec, &cpp_cache);
    };

    if (s.ok()){
        rocksdb_options_t c_options = {Options(cpp_options, ColumnFamilyOptions())};
        SCM scm_options = scm_make_foreign_object_1(scm_rocksdb_options_t, &c_options);
        SCM scm_list_cf = SCM_EOL;
        for (const auto &cf_desc : cpp_cf_descs_vec){
            auto cf_wrapped = Options(DBOptions(), cf_desc.options);
            SCM scm_pair = scm_cons(scm_from_utf8_string(cf_desc.name.c_str()),
                                    scm_make_foreign_object_1(scm_rocksdb_options_t, &cf_wrapped));
            scm_list_cf = scm_cons(scm_pair, scm_list_cf);
        };
        result[0] = scm_options;
        result[1] = scm_list_cf;
    } else {
        result[1] = scm_from_utf8_string(s.getState());
    };
    return scm_c_values(result, 2);
}
}

// Status CheckOptionsCompatibility(
//     const ConfigOptions& config_options, const std::string& dbpath,
//     const DBOptions& db_options,
//     const std::vector<ColumnFamilyDescriptor>& cf_descs);
