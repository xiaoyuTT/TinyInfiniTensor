#pragma once
#include "core/common.h"
#include "core/ref.h"

namespace infini {

class RuntimeObj;
using Runtime = Ref<RuntimeObj>;

//ptr指向的是一个存储tensor数据的内存块，每个tensor都有一个blobobj对象，用于存储数据
class BlobObj
{
  Runtime runtime;
  void *ptr;

public:
  BlobObj(Runtime runtime, void *ptr) : runtime(runtime), ptr(ptr) {}
  BlobObj(BlobObj &other) = delete;
  BlobObj &operator=(BlobObj const &) = delete;
  ~BlobObj() {};

  template <typename T>
  T getPtr() const { return reinterpret_cast<T>(ptr); }
};

} // namespace infini
