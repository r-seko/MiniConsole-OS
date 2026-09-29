#include "core/memory/fixed_block_allocator.hpp"
#include <algorithm>
#include <cassert>

namespace miniconsole::core::memory {

FixedBlockAllocator::FixedBlockAllocator(std::size_t blockSize, std::size_t blockCount)
    : block_size_(std::max(blockSize, sizeof(Node))), block_count_(blockCount) {
    if (block_count_ == 0)
        return;

    raw_pool_ = new std::uint8_t[block_size_ * block_count_];

    free_list_ = reinterpret_cast<Node *>(raw_pool_);
    Node *current = free_list_;
    for (std::size_t i = 0; i < block_count_ - 1; ++i) {
        std::uint8_t *nextPtr = reinterpret_cast<std::uint8_t *>(current) + block_size_;
        current->next = reinterpret_cast<Node *>(nextPtr);
        current = current->next;
    }
    current->next = nullptr;
}

FixedBlockAllocator::~FixedBlockAllocator() {
    delete[] raw_pool_;
}

void *FixedBlockAllocator::allocate() {
    if (!free_list_) {
        return nullptr;
    }

    Node *node = free_list_;
    free_list_ = free_list_->next;

    active_allocations_.fetch_add(1, std::memory_order_relaxed);
    total_allocations_.fetch_add(1, std::memory_order_relaxed);

    return reinterpret_cast<void *>(node);
}

void FixedBlockAllocator::deallocate(void *ptr) {
    if (!ptr)
        return;

    Node *node = reinterpret_cast<Node *>(ptr);
    node->next = free_list_;
    free_list_ = node;

    active_allocations_.fetch_sub(1, std::memory_order_relaxed);
}

} // namespace miniconsole::core::memory