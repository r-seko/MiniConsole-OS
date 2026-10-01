#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>

namespace miniconsole::core::memory {

class FixedBlockAllocator {
  public:
    FixedBlockAllocator(std::size_t blockSize, std::size_t blockCount);
    ~FixedBlockAllocator();

    FixedBlockAllocator(const FixedBlockAllocator &) = delete;
    FixedBlockAllocator &operator=(const FixedBlockAllocator &) = delete;

    [[nodiscard]] void *allocate();

    void deallocate(void *ptr);

    [[nodiscard]] std::size_t get_active_allocation_count() const noexcept {
        return active_allocations_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] std::size_t get_total_allocation_count() const noexcept {
        return total_allocations_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] std::size_t get_capacity() const noexcept { return block_count_; }
    [[nodiscard]] std::size_t get_block_size() const noexcept { return block_size_; }

  private:
    struct Node {
        Node *next;
    };

    std::size_t block_size_;
    std::size_t block_count_;
    std::uint8_t *raw_pool_{nullptr};
    Node *free_list_{nullptr};

    mutable std::atomic<std::size_t> active_allocations_{0};
    mutable std::atomic<std::size_t> total_allocations_{0};
};

} // namespace miniconsole::core::memory