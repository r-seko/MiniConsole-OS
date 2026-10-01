#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <optional>
#include <utility>

namespace miniconsole::core::container {

template <typename T, std::size_t Capacity> class LockFreeRingBuffer {
    static_assert(Capacity > 0, "Capacity must be greater than 0");

  public:
    LockFreeRingBuffer() : head_(0), tail_(0) {}
    ~LockFreeRingBuffer() = default;

    LockFreeRingBuffer(const LockFreeRingBuffer &) = delete;
    LockFreeRingBuffer &operator=(const LockFreeRingBuffer &) = delete;

    bool push(const T &item) {
        const auto current_head = head_.load(std::memory_order_relaxed);
        const auto current_tail = tail_.load(std::memory_order_acquire);

        if (is_full(current_head, current_tail)) {
            return false;
        }

        buffer_[current_head] = item;
        head_.store(next_index(current_head), std::memory_order_release);
        return true;
    }

    bool push(T &&item) {
        const auto current_head = head_.load(std::memory_order_relaxed);
        const auto current_tail = tail_.load(std::memory_order_acquire);

        if (is_full(current_head, current_tail)) {
            return false;
        }

        buffer_[current_head] = std::move(item);
        head_.store(next_index(current_head), std::memory_order_release);
        return true;
    }

    std::optional<T> pop() {
        const auto current_tail = tail_.load(std::memory_order_relaxed);
        const auto current_head = head_.load(std::memory_order_acquire);

        if (is_empty(current_head, current_tail)) {
            return std::nullopt;
        }

        T item = std::move(buffer_[current_tail]);
        tail_.store(next_index(current_tail), std::memory_order_release);
        return item;
    }

    [[nodiscard]] bool empty() const noexcept {
        return is_empty(head_.load(std::memory_order_relaxed),
                        tail_.load(std::memory_order_relaxed));
    }

    [[nodiscard]] std::size_t capacity() const noexcept { return Capacity; }

  private:
    [[nodiscard]] static constexpr std::size_t next_index(std::size_t current) noexcept {
        return (current + 1) % (Capacity + 1);
    }

    [[nodiscard]] static bool is_full(std::size_t head, std::size_t tail) noexcept {
        return next_index(head) == tail;
    }

    [[nodiscard]] static bool is_empty(std::size_t head, std::size_t tail) noexcept {
        return head == tail;
    }

    std::array<T, Capacity + 1> buffer_{};

    alignas(64) std::atomic<std::size_t> head_{0};

    alignas(64) std::atomic<std::size_t> tail_{0};
};

} // namespace miniconsole::core::container