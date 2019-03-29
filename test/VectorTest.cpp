
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/Vector.h"

using testing::Eq;

class ClassDeclaration : public testing::Test {
public:
    ClassDeclaration(){}
};

TEST_F(ClassDeclaration, vector_basics) {
    Vector<int> v;
    ASSERT_TRUE(v.IsEmpty());
    ASSERT_EQ(0, v.GetSize());
    v.PushBack(1);
    ASSERT_FALSE(v.IsEmpty());
    ASSERT_EQ(1, v.GetSize());
    ASSERT_EQ(1, v.Get(0));
    v.PushBack(2);
    ASSERT_EQ(2, v.GetSize());
    ASSERT_EQ(2, v.Get(1));
    ASSERT_EQ(1, v.Get(0));
    v.Swap(0, 1);
    ASSERT_EQ(2, v.Get(0));
    ASSERT_EQ(1, v.Get(1));
    v.Swap(1, 0);
    v.Set(4, 0);
    ASSERT_EQ(4, v.Get(0));
    ASSERT_EQ(2, v.GetSize());
}

TEST_F(ClassDeclaration, vector_templating) {
    Vector<Vector<int>> v;
    ASSERT_EQ(0, v.GetSize());
    Vector<int> v0;
    v.PushBack(v0);
    v0.PushBack(123);
    ASSERT_EQ(1, v0.GetSize());
    ASSERT_EQ(0, v.Get(0).GetSize());
}

TEST_F(ClassDeclaration, vector_pointers) {
    Vector<int*> v;
    v.PushBack(new int(123));
    v.PushBack(new int(321));
    ASSERT_EQ(123, *v.Get(0));
    ASSERT_EQ(321, *v.Get(1));
}

TEST_F(ClassDeclaration, vector_throwing) {
    Vector<int> v;
    ASSERT_ANY_THROW(v.Set(0, 0));
    v.PushBack(1);
    ASSERT_ANY_THROW(v.Get(1));
    ASSERT_ANY_THROW(v.Set(1, 1));
    ASSERT_ANY_THROW(v.IncreaseCapacityTo(0));
}

TEST_F(ClassDeclaration, vector_capacity) {
    Vector<int> v;
    v.IncreaseCapacity();
    v.IncreaseCapacityFor(1);
    ASSERT_ANY_THROW(v.IncreaseCapacityFor(-1));
    v.DecreaseCapacityTo(1);
    ASSERT_EQ(1, v.GetCapacity());
    v.DecreaseCapacityTo(0);
    ASSERT_EQ(0, v.GetCapacity());
    v.ShrinkToFit();
    ASSERT_EQ(0, v.GetCapacity());
    ASSERT_ANY_THROW(v.DecreaseCapacityTo(1));
    ASSERT_EQ(0, v.GetCapacity());
    v.PushBack(0);
    v.PushBack(1);
    v.PushBack(2);
    v.PushBack(3);
    ASSERT_TRUE(v.GetCapacity() >= v.GetSize());
    v.ShrinkToFit();
    ASSERT_TRUE(v.GetCapacity() == v.GetSize());
    v.IncreaseCapacity();
    ASSERT_TRUE(v.GetCapacity() >= v.GetSize());
    v.IncreaseCapacityTo(1000);
    ASSERT_EQ(1000, v.GetCapacity());
    ASSERT_EQ(4, v.GetSize());
    ASSERT_EQ(3, v.PopBack());
    ASSERT_EQ(3, v.GetSize());
    ASSERT_EQ(1000, v.GetCapacity());
    v.ShrinkToFit();
    ASSERT_EQ(3, v.GetCapacity());
}

TEST_F(ClassDeclaration, vector_memory) {
    Vector<int> v;
    for (int i = 0; i < 10; i++) { //Make more iterations if u want to, 10^5 works fine
        v.IncreaseCapacityFor(1);
    }
}