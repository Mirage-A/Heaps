#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/BinomialHeap.h"
#include "../src/Vector.h"
#include <ctime>

using testing::Eq;

class ClassDeclaration : public testing::Test{
public:
    ClassDeclaration(){}
};
TEST_F(ClassDeclaration, binomial_heap_basics){
    BinomialHeap<int> heap;
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

    Vector<BinomialHeap<int>::Pointer> ptrs;
    for (int i = 0; i < 10; ++i) {
        ptrs.PushBack(heap.Insert(i));
    }
    heap.Change(ptrs.Get(7), -1);
    heap.Change(ptrs.Get(4), 2);
    heap.Change(ptrs.Get(8), 11);
    heap.Change(ptrs.Get(6), 5);
    heap.Change(ptrs.Get(1), -2);
    heap.Change(ptrs.Get(0), 0);
    heap.Delete(ptrs.Get(6));
    heap.Delete(ptrs.Get(9));
    ASSERT_ANY_THROW(heap.Delete(ptrs.Get(6)));
    ASSERT_ANY_THROW(heap.Change(ptrs.Get(9), 12345));
    int a[] {-2, -1, 0, 2, 2, 3, 5, 11};
    for (int i = 0; i < 8; ++i) {
        ASSERT_EQ(a[i], heap.ExtractMin());
    }
}

TEST_F(ClassDeclaration, binomial_heap_speed){
    std::cout << "==== BINOMIAL ====" << std::endl;
    int st = clock();
    BinomialHeap<int> heap;
    for (int i = 0; i < 1000000; ++i) {
        heap.Insert(rand());
    }
    int end = clock();
    std::cout << "1kk random inserts: " << end - st  << " ms" << std::endl;
    BinomialHeap<int> other_heap;
    for (int i = 0; i < 1000000; ++i) {
        other_heap.Insert(rand());
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