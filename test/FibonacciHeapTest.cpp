#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/FibonacciHeap.h"
#include <ctime>

using testing::Eq;

class ClassDeclaration : public testing::Test {
public:
    ClassDeclaration() {}
};

TEST_F(ClassDeclaration, fibonacci_heap_basics) {
    FibonacciHeap<int> heap;
    ASSERT_ANY_THROW(heap.GetMin());
    ASSERT_ANY_THROW(heap.ExtractMin());
    ASSERT_TRUE(heap.IsEmpty());
    heap.Insert(5);
    ASSERT_FALSE(heap.IsEmpty());
    ASSERT_EQ(5, heap.GetMin());
    ASSERT_EQ(5, heap.ExtractMin());
    ASSERT_TRUE(heap.IsEmpty());
    heap.Insert(5);
    heap.Insert(6);
    ASSERT_EQ(5, heap.GetMin());
    heap.Insert(2);
    ASSERT_EQ(2, heap.GetMin());
    heap.Insert(4);
    ASSERT_EQ(2, heap.GetMin());
    ASSERT_EQ(2, heap.ExtractMin());
    ASSERT_EQ(4, heap.ExtractMin());
    ASSERT_EQ(5, heap.ExtractMin());
    ASSERT_EQ(6, heap.ExtractMin());
    ASSERT_ANY_THROW(heap.ExtractMin());
}

TEST_F(ClassDeclaration, fibonacci_heap_pointers) {
    FibonacciHeap<int> heap;
    Vector<FibonacciHeap<int>::Pointer> ptrs;
    for (int i = 0; i < 10; ++i) {
        ptrs.PushBack(heap.Insert(i));
    }
    heap.Decrease(ptrs.Get(7), -1);
    heap.Decrease(ptrs.Get(4), 2);
    ASSERT_ANY_THROW(heap.Decrease(ptrs.Get(9), 11));
    heap.Decrease(ptrs.Get(6), 5);
    heap.Decrease(ptrs.Get(1), -2);
    heap.Decrease(ptrs.Get(0), 0);
    int a[]{-2, -1, 0, 2, 2, 3, 5, 5, 8, 9};
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(a[i], heap.ExtractMin());
    }
}

TEST_F(ClassDeclaration, fibonacci_heap_merge) {
    FibonacciHeap<int> heap;
    FibonacciHeap<int> other_heap;

    heap.Insert(2);
    heap.Insert(-1);
    heap.Insert(11);
    heap.Insert(3);

    other_heap.Insert(0);
    other_heap.Insert(-2);
    other_heap.Insert(5);
    other_heap.Insert(2);

    heap.Merge(other_heap);
    int a[] {-2, -1, 0, 2, 2, 3, 5, 11};
    for (int i = 0; i < 8; ++i) {
        ASSERT_EQ(a[i], heap.ExtractMin());
    }
}

TEST_F(ClassDeclaration, fibonacci_heap_speed) {
    std::cout << "==== FIBONACCI ====" << std::endl;
    int st = clock();
    FibonacciHeap<int> heap;
    for (int i = 0; i < 1000000; ++i) {
        heap.Insert(i*2);
    }
    int end = clock();
    std::cout << "1kk random inserts: " << end - st  << " ms" << std::endl;
    FibonacciHeap<int> other_heap;
    for (int i = 0; i < 1000000; ++i) {
        other_heap.Insert(1+i*2);
    }
    st = clock();
    heap.Merge(other_heap);
    end = clock();
    std::cout << "merge of 1kk x 2 elements: " << end - st  << " ms" << std::endl;
    st = clock();
    for (int i = 0; i < 1000; ++i) {
        heap.ExtractMin();
    }
    end = clock();
    std::cout << "1k extracts from 2kk elements heap: " << end - st  << " ms" << std::endl;
    std::cout << "EVERYFIN IS AWESOME!" << std::endl;
    std::cout << std::endl;
}