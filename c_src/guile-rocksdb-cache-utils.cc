#include <memory>
#include <libguile.h>
#include "rocksdb/c.h"
#include "rocksdb/iterator.h"
#include "rocksdb/memory_allocator.h"

using namespace ROCKSDB_NAMESPACE;

extern void display(const char* msg);

struct rocksdb_memory_allocator_t {
  std::shared_ptr<MemoryAllocator> rep;
};

class GuileAllocator : public MemoryAllocator {
 public:
    GuileAllocator(){};
    ~GuileAllocator(){};
    const char* Name() const override {
        return "scm_malloc";
    }

    void* Allocate(size_t size) override {
        display("alloc\n");
        return scm_malloc(size);
    };

    void Deallocate(void* p) override {
        display("dealloc\n");
        return free(p);
    };
};

extern "C" {

rocksdb_memory_allocator_t* rocksdb_guile_allocator_create(){
    rocksdb_memory_allocator_t* allocator = new rocksdb_memory_allocator_t;
    allocator->rep = std::shared_ptr<GuileAllocator>(new GuileAllocator());
    return allocator;
};

}
