
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/Heap.h"

using testing::Eq;

class ClassDeclaration : public testing::Test{
public:
    ClassDeclaration(){}
};
TEST_F(ClassDeclaration, binary_heap_basics){
    Heap<int> heap;
    ASSERT_TRUE(heap.IsEmpty());
    ASSERT_EQ(0, heap.GetSize());
    ASSERT_ANY_THROW(heap.GetMin());
    ASSERT_ANY_THROW(heap.ExtractMin());
    heap.Insert(0);
    ASSERT_TRUE(heap.IsValid());
    ASSERT_FALSE(heap.IsEmpty());
    ASSERT_EQ(1, heap.GetSize());
    heap.Insert(1);
    ASSERT_TRUE(heap.IsValid());
    ASSERT_EQ(0, heap.GetMin());
    ASSERT_EQ(0, heap.ExtractMin());
    ASSERT_TRUE(heap.IsValid());
    ASSERT_EQ(1, heap.ExtractMin());
    ASSERT_TRUE(heap.IsEmpty());
    ASSERT_TRUE(heap.IsValid());
    heap.Insert(5);
    ASSERT_EQ(1, heap.GetSize());
    ASSERT_TRUE(heap.IsValid());
    heap.Insert(2);
    ASSERT_TRUE(heap.IsValid());
    heap.Insert(3);
    ASSERT_TRUE(heap.IsValid());
    heap.Insert(4);
    ASSERT_EQ(4, heap.GetSize());
    ASSERT_TRUE(heap.IsValid());
    ASSERT_EQ(2, heap.GetMin());
    ASSERT_EQ(2, heap.ExtractMin());
    ASSERT_EQ(3, heap.GetSize());
    ASSERT_TRUE(heap.IsValid());
    ASSERT_EQ(3, heap.ExtractMin());
    ASSERT_TRUE(heap.IsValid());
    ASSERT_EQ(4, heap.ExtractMin());
    ASSERT_TRUE(heap.IsValid());
    ASSERT_EQ(5, heap.ExtractMin());
    ASSERT_TRUE(heap.IsEmpty());
    ASSERT_TRUE(heap.IsValid());
}
TEST_F(ClassDeclaration, binary_heap_mass_constructor){
    int* arr = new int[5]{2, 3, 5, 1, 4};
    Heap<int> heap(arr+0, arr+5);
    ASSERT_TRUE(heap.IsValid());
}
TEST_F(ClassDeclaration, binary_heap_pointers){
    int* arr = new int[5]{7, 3, 5, 1, 4};
    Heap<int> heap(arr+0, arr+5);
    ASSERT_FALSE(heap.IsOptimized());
    Heap<int>::Pointer ptr0 = heap.Insert(0);
    Heap<int>::Pointer ptr6 = heap.Insert(6);
    Heap<int>::Pointer ptr2 = heap.Insert(2);
    ASSERT_FALSE(heap.IsOptimized());
    ASSERT_TRUE(heap.IsValid());
    ASSERT_EQ(0, heap.ExtractMin());
    ASSERT_ANY_THROW(heap.Change(ptr0, 0));
    heap.Change(ptr6, 0);
    ASSERT_TRUE(heap.IsValid());
    ASSERT_EQ(0, heap.GetMin());
    ASSERT_EQ(0, heap.ExtractMin());
    ASSERT_ANY_THROW(heap.Change(ptr6, 0));
    ASSERT_EQ(1, heap.ExtractMin());
    ASSERT_EQ(2, heap.GetMin());
    heap.Change(ptr2, 4);
    ASSERT_EQ(3, heap.GetMin());
    ASSERT_TRUE(heap.IsValid());
    ASSERT_EQ(3, heap.ExtractMin());
    ASSERT_EQ(4, heap.ExtractMin());
    ASSERT_EQ(4, heap.ExtractMin());
    ASSERT_EQ(5, heap.ExtractMin());
    ASSERT_EQ(7, heap.ExtractMin());
    ASSERT_TRUE(heap.IsEmpty());
    Heap<int>::Pointer ptr1 = heap.Insert(1);
    Heap<int>::Pointer ptr3 = heap.Insert(3);
    Heap<int>::Pointer ptr4 = heap.Insert(4);
    Heap<int>::Pointer ptr5 = heap.Insert(5);
    Heap<int>::Pointer ptr7 = heap.Insert(7);
    ASSERT_TRUE(heap.IsValid());
    heap.Change(ptr3, 0);
    heap.Change(ptr5, 4);
    heap.Change(ptr7, 2);
    ASSERT_TRUE(heap.IsValid());
    heap.Remove(ptr5);
    ASSERT_TRUE(heap.IsValid());
    heap.Remove(ptr3);
    ASSERT_TRUE(heap.IsValid());
    ASSERT_ANY_THROW(heap.Remove(ptr5));
    ASSERT_ANY_THROW(heap.Change(ptr3, 1));
    heap.Change(ptr7, 0);
    ASSERT_TRUE(heap.IsValid());
    ASSERT_EQ(0, heap.GetMin());
    ASSERT_EQ(0, heap.ExtractMin());
    ASSERT_ANY_THROW(heap.Remove(ptr7));
}
TEST_F(ClassDeclaration, binary_heap_optimization){
    Heap<int> heapy;
    heapy.Insert(0);
    ASSERT_ANY_THROW(heapy.Optimize(1, 1));
    heapy.ExtractMin();
    ASSERT_NO_THROW(heapy.Optimize(-1, 1));
    heapy.Optimize(1, 1);
    ASSERT_FALSE(heapy.IsOptimized());
    heapy.Optimize(666, 1);
    ASSERT_TRUE(heapy.IsOptimized());

}

TEST_F(ClassDeclaration, binary_heap_speed){
    std::cout << "==== BINARY ====" << std::endl;
    int st = clock();
    Heap<int> heap;
    for (int i = 0; i < 1000000; ++i) {
        heap.Insert(i*2);
    }
    int end = clock();
    std::cout << "1kk random inserts: " << end - st  << " ms" << std::endl;
    for (int i = 0; i < 1000000; ++i) {
        heap.Insert(1+i*2);
    }
    st = clock();
    for (int i = 0; i < 1000; ++i) {
        heap.ExtractMin();
    }
    end = clock();
    std::cout << "1k extracts from 2kk elements heap: " << end - st << " ms" << std::endl;
    std::cout << "EVERYFIN IS AWESOME!" << std::endl;
    std::cout << std::endl;
}