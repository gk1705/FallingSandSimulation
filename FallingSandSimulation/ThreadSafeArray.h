#pragma once

#include <algorithm>
#include <atomic>
#include <array>

template<typename T, size_t N>
class ThreadSafeArray
{
public:
    ThreadSafeArray() : index(0) {}

    bool add(const T& element)
    {
        lockArray();
        if (index.load(std::memory_order_relaxed) >= array.size())
        {
            unlockArray(); // release lock if the array is full
            return false;
        }

        array[index.fetch_add(1, std::memory_order_relaxed)] = element;
        unlockArray();
        return true;
    }

    T get(int i)
    {
        return array[i];
    }

    T operator[](int i)
	{
		return array[i];
	}

    void sort(bool(*compare)(const T&, const T&))
	{
		lockArray();
		std::sort(array.begin(), array.begin() + index.load(std::memory_order_relaxed), compare);
		unlockArray();
	}

    int getCurrentIndex() const
    {
        return index.load(std::memory_order_relaxed);
    }

    void resetCurrentIndex()
	{
		index.store(0, std::memory_order_relaxed);
	}

private:
    std::array<T, N> array;
    std::atomic<int> index;
    std::atomic_flag lock = ATOMIC_FLAG_INIT;

    void lockArray()
    {
        while (lock.test_and_set(std::memory_order_acquire))
        {
            // spin-wait without blocking
        }
    }

    void unlockArray()
    {
        lock.clear(std::memory_order_release);
    }
};