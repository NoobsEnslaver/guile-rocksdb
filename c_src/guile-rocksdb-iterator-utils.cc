#include <libguile.h>
#include "rocksdb/c.h"
#include "rocksdb/iterator.h"

using namespace ROCKSDB_NAMESPACE;

extern "C" {

extern SCM scm_rocksdb_iterator_t;
//extern void display(const char* msg);
extern void* scm_get_ref(SCM obj);
struct rocksdb_iterator_t { Iterator* rep; };

// spec: (rocksdb_iterator_t) -> boolean
SCM grocksdb_iterator_refresh(SCM scm_iterator){
    scm_assert_foreign_object_type(scm_rocksdb_iterator_t, scm_iterator);
    SCM result[2] = {SCM_BOOL_T, SCM_BOOL_F};

    rocksdb_iterator_t* c_iter = (rocksdb_iterator_t *)scm_get_ref(scm_iterator);
    Status s = c_iter->rep->Refresh();
    if(!s.ok()) {
        result[0] = SCM_BOOL_F;
        result[1] = scm_from_utf8_string(s.getState());
    }

    return scm_c_values(result, 2);
}
}
