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
extern void display(const char* msg);

struct rocksdb_options_t { Options rep; };
struct rocksdb_column_family_handle_t  { ColumnFamilyHandle* rep; };
struct rocksdb_env_t { Env* rep; bool is_default;};
struct rocksdb_cache_t { std::shared_ptr<Cache> rep; };

SCM gload_options_from_file(SCM scm_options_file_name,
     /* optional: */        SCM scm_ignore_unknown_options, SCM scm_cache, SCM scm_env){

    display("-1\n");
    ConfigOptions config_options;
    DBOptions cpp_options;
    // ColumnFamilyDescriptor cpp_cf_desc;
    std::vector<ColumnFamilyDescriptor> cpp_cf_descs_vec;
    rocksdb_cache_t* c_cache;
    rocksdb_env_t* c_env;
    SCM result[4] = {SCM_BOOL_F, SCM_BOOL_F, SCM_BOOL_F, SCM_BOOL_F};

    SCM_ASSERT_TYPE(scm_string_p(scm_options_file_name), scm_options_file_name, SCM_ARG1,
                    "load-options-from-file", "string");
    std::string cpp_options_file_name = std::string(scm_to_utf8_string(scm_options_file_name));

    scm_ignore_unknown_options = SCM_UNBNDP(scm_ignore_unknown_options)? SCM_BOOL_T : scm_ignore_unknown_options;
    bool ignore_unknown_options = scm_is_true(scm_ignore_unknown_options);

    if (SCM_UNBNDP(scm_cache)){
        c_cache = rocksdb_cache_create_lru(1 * 1024 * 1024 * 1024); // 1 GB
        scm_cache = scm_make_foreign_object_1(scm_rocksdb_cache_t, c_cache);
    } else {
        scm_assert_foreign_object_type(scm_rocksdb_cache_t, scm_cache);
        c_cache = (rocksdb_cache_t*)scm_foreign_object_ref(scm_cache, 0);
    };

    if (SCM_UNBNDP(scm_env)){
        c_env = rocksdb_create_default_env();
        scm_env = scm_make_foreign_object_1(scm_rocksdb_env_t, c_env);
    } else {
        scm_assert_foreign_object_type(scm_rocksdb_env_t, scm_env);
        c_env = (rocksdb_env_t*)scm_foreign_object_ref(scm_env, 0);
    };

    std::shared_ptr<Cache> cpp_cache = c_cache->rep;
    // Env* cpp_env = c_env->rep;

    display("0\n");
    // Status s = LoadOptionsFromFile(cpp_options_file_name, cpp_env, &cpp_options, &cpp_cf_descs_vec,
    //                                ignore_unknown_options, &cpp_cache);
    Status s = LoadOptionsFromFile(config_options, cpp_options_file_name, &cpp_options, &cpp_cf_descs_vec,
                                   &cpp_cache);

    display("1\n");
    //ret: rocksdb_options_t* options
    // (list (char* column_family_names . rocksdb_options_t* column_family_options))
    if (s.ok()){
        display("1\n");
        // display("1\n");
        rocksdb_options_t c_options = {Options(cpp_options, ColumnFamilyOptions())};
        SCM scm_options = scm_make_foreign_object_1(scm_rocksdb_options_t, &c_options);

        display("2\n");
        SCM scm_list_cf = scm_list_n(SCM_UNDEFINED);
        display("3\n");
        for (const auto &cf_desc : cpp_cf_descs_vec){
            auto cf_wrapped = Options(DBOptions(), cf_desc.options);
            SCM scm_pair = scm_cons(scm_from_utf8_string(cf_desc.name.c_str()),
                                    scm_make_foreign_object_1(scm_rocksdb_options_t, &cf_wrapped));
            scm_list_cf = scm_cons(scm_pair, scm_list_cf);
        };
        display("4\n");
        result[0] = scm_options;
        result[1] = scm_list_cf;
        result[2] = scm_cache;
        result[4] = scm_env;
    } else {
        result[1] = scm_from_utf8_string(s.getState());
    };

    // SCM ret[] = {16, 2, 77, 40};
    // SCM_BOOL_F;
    return scm_c_values(result, 4);
}

}

// Status LoadOptionsFromFile(const ConfigOptions& config_options,
//                            const std::string& options_file_name,
//                            DBOptions* db_options,
//                            std::vector<ColumnFamilyDescriptor>* cf_descs,
//                            std::shared_ptr<Cache>* cache = {});

// Status CheckOptionsCompatibility(
//     const std::string& dbpath, Env* env, const DBOptions& db_options,
//     const std::vector<ColumnFamilyDescriptor>& cf_descs,
//     bool ignore_unknown_options = false);
