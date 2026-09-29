#include "core/memory/fixed_block_allocator.hpp"
#include <gtest/gtest.h>

using namespace miniconsole::core::memory;

TEST(FixedBlockAllocatorTest, BasicAllocationAndDeallocation) {
    FixedBlockAllocator allocator(32, 10);

    EXPECT_EQ(allocator.get_active_allocation_count(), 0);

    void *p1 = allocator.allocate();
    void *p2 = allocator.allocate();

    EXPECT_NE(p1, nullptr);
    EXPECT_NE(p2, nullptr);
    EXPECT_NE(p1, p2);
    EXPECT_EQ(allocator.get_active_allocation_count(), 2);
    EXPECT_EQ(allocator.get_total_allocation_count(), 2);

    allocator.deallocate(p1);
    EXPECT_EQ(allocator.get_active_allocation_count(), 1);

    allocator.deallocate(p2);
    EXPECT_EQ(allocator.get_active_allocation_count(), 0);
}

TEST(FixedBlockAllocatorTest, PoolExhaustion) {
    FixedBlockAllocator allocator(16, 2);

    void *p1 = allocator.allocate();
    void *p2 = allocator.allocate();
    void *p3 = allocator.allocate();

    EXPECT_NE(p1, nullptr);
    EXPECT_NE(p2, nullptr);
    EXPECT_EQ(p3, nullptr);

    allocator.deallocate(p1);
    void *p4 = allocator.allocate();
    EXPECT_NE(p4, nullptr);
}