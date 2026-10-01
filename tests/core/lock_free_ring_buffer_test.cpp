#include "core/container/lock_free_ring_buffer.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <memory>
#include <thread>
#include <vector>

using namespace miniconsole::core::container;

TEST(LockFreeRingBufferTest, BasicPushPop) {
    LockFreeRingBuffer<int, 3> buffer;

    EXPECT_TRUE(buffer.empty());
    EXPECT_EQ(buffer.capacity(), 3U);

    EXPECT_TRUE(buffer.push(10));
    EXPECT_TRUE(buffer.push(20));
    EXPECT_TRUE(buffer.push(30));

    EXPECT_FALSE(buffer.push(40));

    auto val1 = buffer.pop();
    ASSERT_TRUE(val1.has_value());
    EXPECT_EQ(val1.value(), 10);

    auto val2 = buffer.pop();
    ASSERT_TRUE(val2.has_value());
    EXPECT_EQ(val2.value(), 20);

    EXPECT_TRUE(buffer.push(40));

    auto val3 = buffer.pop();
    ASSERT_TRUE(val3.has_value());
    EXPECT_EQ(val3.value(), 30);

    auto val4 = buffer.pop();
    ASSERT_TRUE(val4.has_value());
    EXPECT_EQ(val4.value(), 40);

    EXPECT_FALSE(buffer.pop().has_value());
    EXPECT_TRUE(buffer.empty());
}

TEST(LockFreeRingBufferTest, MoveOnlyType) {
    LockFreeRingBuffer<std::unique_ptr<int>, 2> buffer;

    EXPECT_TRUE(buffer.push(std::make_unique<int>(100)));
    EXPECT_TRUE(buffer.push(std::make_unique<int>(200)));

    auto item = buffer.pop();
    ASSERT_TRUE(item.has_value());
    EXPECT_EQ(*item.value(), 100);
}

TEST(LockFreeRingBufferTest, ConcurrentProducerConsumer) {
    constexpr std::size_t BufferCapacity = 1024;
    constexpr std::size_t ItemCount = 100000;

    LockFreeRingBuffer<std::size_t, BufferCapacity> buffer;

    std::thread producer([&]() {
        for (std::size_t i = 1; i <= ItemCount; ++i) {
            while (!buffer.push(i)) {
                std::this_thread::yield();
            }
        }
    });

    std::vector<std::size_t> received;
    received.reserve(ItemCount);

    std::thread consumer([&]() {
        while (received.size() < ItemCount) {
            auto val = buffer.pop();
            if (val.has_value()) {
                received.push_back(val.value());
            } else {
                std::this_thread::yield();
            }
        }
    });

    producer.join();
    consumer.join();

    ASSERT_EQ(received.size(), ItemCount);
    for (std::size_t i = 0; i < ItemCount; ++i) {
        EXPECT_EQ(received[i], i + 1U);
    }
}