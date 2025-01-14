#include "core/allocator.h"
#include <utility>

namespace infini
{
    Allocator::Allocator(Runtime runtime) : runtime(runtime)
    {
        used = 0;
        peak = 0;
        ptr = nullptr;

        // 'alignment' defaults to sizeof(uint64_t), because it is the length of
        // the longest data type currently supported by the DataType field of
        // the tensor
        alignment = sizeof(uint64_t);
    }

    Allocator::~Allocator()
    {
        if (this->ptr != nullptr)
        {
            runtime->dealloc(this->ptr);
        }
    }

    size_t Allocator::alloc(size_t size)
    {
        IT_ASSERT(this->ptr == nullptr);
        // pad the size to the multiple of alignment
        size = this->getAlignedSize(size);

        // =================================== 作业 ===================================
        // TODO: 设计一个算法来分配内存，返回起始地址偏移量
        // =================================== 作业 ===================================
        //注意,free_blocks只是一个map,并不是真正的内存块，我们用它来管理内存块，而不是分配内存块，而在这里就要实现管理。
        //至于什么时候分配内存块，是在runtime->alloc()中实现的，而对于什么时候调用runtime->alloc()，是在graph.cc中实现的。

        for(auto it : free_blocks){
            if(it.second >= size){
                size_t offset = it.first;
                free_blocks.erase(it.first);
                if(it.second > size){
                    free_blocks[offset+size] = it.second - size;
                }
                this->used += size;
                if (this->used > this->peak) this->peak = this->used;

                std::cout << "this->size:" << size << std::endl;
                std::cout << "this->used:" << this->used << std::endl;
                std::cout << "this->peak:" << this->peak << std::endl;

                return offset;
            }
        }
        throw std::runtime_error("No enough memory");
        return 0;
    }

    void Allocator::free(size_t addr, size_t size)
    {
        IT_ASSERT(this->ptr == nullptr);
        size = getAlignedSize(size);

        // =================================== 作业 ===================================
        // TODO: 设计一个算法来回收内存
        // =================================== 作业 ===================================
        
        this->used -= size;
        bool pre = false,next=false;
        std::pair<size_t,size_t> block;
        
        if(free_blocks.find(addr+size) != free_blocks.end()) next=true;
        for(auto it : free_blocks){
            if(it.first + it.second == addr){
                pre = true;
                block=it;
                break;
            }
        }

        if(pre && !next) free_blocks[block.first] += size;
        else if(!pre && next)
        {
            free_blocks[addr] = size + free_blocks[addr+size];
            free_blocks.erase(addr+size);
        }
        else if(pre && next)
        {
            free_blocks[block.first] += size + free_blocks[addr+size];
            free_blocks.erase(addr+size);
        }
        else free_blocks[addr] = size;
    }

    void *Allocator::getPtr()
    {
        if (this->ptr == nullptr)
        {
            this->ptr = runtime->alloc(this->peak);
            printf("Allocator really alloc: %p %lu bytes\n", this->ptr, peak);
        }
        return this->ptr;
    }

    size_t Allocator::getAlignedSize(size_t size)
    {
        return ((size - 1) / this->alignment + 1) * this->alignment;
    }

    void Allocator::info()
    {
        std::cout << "Used memory: " << this->used
                  << ", peak memory: " << this->peak << std::endl;
    }
}
