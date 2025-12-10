#include <gtest/gtest.h>
#include "UniquePtr.hpp"
#include "SharedPtr.hpp"
#include "WeakPtr.hpp"
#include "Option.hpp"
#include "Variant.hpp"

struct TestObj {
    int value;
    TestObj(int v = 0) : value(v) {}
};

// UniquePtr Tests
TEST(UniquePtrTest, DefaultConstruction) {
    auto ptr = makeUnique<TestObj>(42);
    EXPECT_EQ(ptr->value, 42);
}

TEST(UniquePtrTest, MoveSemantics) {
    auto ptr1 = makeUnique<TestObj>(10);
    auto ptr2 = std::move(ptr1);
    EXPECT_EQ(ptr2->value, 10);
}

TEST(UniquePtrTest, Reset) {
    auto ptr = makeUnique<TestObj>(5);
    ptr.reset();
    EXPECT_FALSE(ptr);
}

TEST(UniquePtrTest, Release) {
    auto ptr = makeUnique<TestObj>(7);
    TestObj* raw = ptr.release();
    EXPECT_FALSE(ptr);
    EXPECT_EQ(raw->value, 7);
    delete raw;
}

TEST(UniquePtrTest, Dereference) {
    auto ptr = makeUnique<TestObj>(15);
    EXPECT_EQ((*ptr).value, 15);
}

// SharedPtr Tests
TEST(SharedPtrTest, DefaultConstruction) {
    auto ptr = makeShared<TestObj>(42);
    EXPECT_EQ(ptr->value, 42);
    EXPECT_EQ(ptr.getCount(), 1);
}

TEST(SharedPtrTest, CopySemantics) {
    auto ptr1 = makeShared<TestObj>(10);
    auto ptr2 = ptr1;
    EXPECT_EQ(ptr1.getCount(), 2);
    EXPECT_EQ(ptr2.getCount(), 2);
    EXPECT_EQ(ptr1->value, ptr2->value);
}

TEST(SharedPtrTest, MoveSemantics) {
    auto ptr1 = makeShared<TestObj>(20);
    auto ptr2 = std::move(ptr1);
    EXPECT_EQ(ptr2->value, 20);
    EXPECT_EQ(ptr2.getCount(), 1);
}

TEST(SharedPtrTest, Reset) {
    auto ptr = makeShared<TestObj>(5);
    EXPECT_EQ(ptr.getCount(), 1);
    ptr.reset();
    EXPECT_FALSE(ptr);
}

TEST(SharedPtrTest, IsUnique) {
    auto ptr1 = makeShared<TestObj>(30);
    EXPECT_TRUE(ptr1.isUnique());
    auto ptr2 = ptr1;
    EXPECT_FALSE(ptr1.isUnique());
}

TEST(SharedPtrTest, Equality) {
    auto ptr1 = makeShared<TestObj>(40);
    auto ptr2 = ptr1;
    EXPECT_TRUE(ptr1 == ptr2);
    auto ptr3 = makeShared<TestObj>(40);
    EXPECT_FALSE(ptr1 == ptr3);
}

TEST(SharedPtrTest, FromUniquePtr) {
    auto uptr = makeUnique<TestObj>(50);
    SharedPtr<TestObj> sptr = std::move(uptr);
    EXPECT_EQ(sptr->value, 50);
    EXPECT_EQ(sptr.getCount(), 1);
}

// WeakPtr Tests
TEST(WeakPtrTest, ConstructFromShared) {
    auto sptr = makeShared<TestObj>(60);
    WeakPtr<TestObj> wptr(sptr);
    EXPECT_EQ(wptr.getSharedCount(), 1);
    EXPECT_FALSE(wptr.isExpired());
}

TEST(WeakPtrTest, Lock) {
    auto sptr = makeShared<TestObj>(70);
    WeakPtr<TestObj> wptr(sptr);
    auto locked = wptr.lock();
    EXPECT_EQ(locked->value, 70);
    EXPECT_EQ(locked.getCount(), 2);
}

TEST(WeakPtrTest, Expiration) {
    WeakPtr<TestObj> wptr;
    {
        auto sptr = makeShared<TestObj>(80);
        wptr = sptr;
        EXPECT_FALSE(wptr.isExpired());
    }
    EXPECT_TRUE(wptr.isExpired());
}

TEST(WeakPtrTest, Reset) {
    auto sptr = makeShared<TestObj>(90);
    WeakPtr<TestObj> wptr(sptr);
    wptr.reset();
    EXPECT_TRUE(wptr.isExpired());
}

// Option Tests
TEST(OptionTest, EmptyOption) {
    Option<int> opt;
    EXPECT_FALSE(opt.hasValue());
    EXPECT_THROW(opt.get(), Exception);
}

TEST(OptionTest, ValueOption) {
    Option<int> opt(42);
    EXPECT_TRUE(opt.hasValue());
    EXPECT_EQ(opt.get(), 42);
}

TEST(OptionTest, CopySemantics) {
    Option<int> opt1(10);
    Option<int> opt2 = opt1;
    EXPECT_EQ(opt2.get(), 10);
}

TEST(OptionTest, MoveSemantics) {
    Option<TestObj> opt1(TestObj(20));
    Option<TestObj> opt2 = std::move(opt1);
    EXPECT_EQ(opt2.get().value, 20);
}

TEST(OptionTest, Assignment) {
    Option<int> opt;
    opt = 30;
    EXPECT_TRUE(opt.hasValue());
    EXPECT_EQ(opt.get(), 30);
}

TEST(OptionTest, BoolConversion) {
    Option<int> opt1;
    Option<int> opt2(5);
    EXPECT_FALSE(opt1);
    EXPECT_TRUE(opt2);
}

// Variant Tests
TEST(VariantTest, FirstType) {
    Variant<int, double> var(42);
    EXPECT_TRUE(var.isInstanceOfT1());
    EXPECT_FALSE(var.isInstanceOfT2());
    EXPECT_EQ(var.getT1(), 42);
}

TEST(VariantTest, SecondType) {
    Variant<int, double> var(3.14);
    EXPECT_FALSE(var.isInstanceOfT1());
    EXPECT_TRUE(var.isInstanceOfT2());
    EXPECT_DOUBLE_EQ(var.getT2(), 3.14);
}

TEST(VariantTest, Assignment) {
    Variant<int, double> var(10);
    var = 2.71;
    EXPECT_TRUE(var.isInstanceOfT2());
    EXPECT_DOUBLE_EQ(var.getT2(), 2.71);
}

TEST(VariantTest, CopySemantics) {
    Variant<int, double> var1(100);
    Variant<int, double> var2 = var1;
    EXPECT_EQ(var2.getT1(), 100);
}

TEST(VariantTest, InvalidTypeAccess) {
    Variant<int, double> var(42);
    EXPECT_THROW(var.getT2(), Exception);
}

TEST(VariantTest, TypeConversion) {
    Variant<int, double> var(42);
    int value = static_cast<int>(var);
    EXPECT_EQ(value, 42);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
