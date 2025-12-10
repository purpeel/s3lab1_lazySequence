#include <gtest/gtest.h>
#include "LazySequence.hpp"
#include "ArraySequence.hpp"
#include <iostream>

// Basic Construction Tests
TEST(LazySequenceTest, EmptySequence) {
    auto seq = LazySequence<int>::create();
    EXPECT_TRUE(seq->isEmpty());
    EXPECT_EQ(static_cast<size_t>(seq->getSize()), 0);
}

TEST(LazySequenceTest, SingleValue) {
    auto seq = LazySequence<int>::create(42);
    EXPECT_FALSE(seq->isEmpty());
    EXPECT_EQ(static_cast<size_t>(seq->getSize()), 1);
    EXPECT_EQ(seq->getFirst(), 42);
}

TEST(LazySequenceTest, FromArraySequence) {
    ArraySequence<int> arr;
    arr.append(1);
    arr.append(2);
    arr.append(3);
    auto seq = LazySequence<int>::create(arr);
    EXPECT_EQ(static_cast<size_t>(seq->getSize()), 3);
    EXPECT_EQ(seq->getFirst(), 1);
    EXPECT_EQ(seq->getLast(), 3);
}

// Append Tests
TEST(LazySequenceTest, AppendSingleValue) {
    auto seq = LazySequence<int>::create(1);
    auto newSeq = seq->append(2);
    EXPECT_EQ(static_cast<size_t>(newSeq->getSize()), 2);
    EXPECT_EQ(newSeq->getFirst(), 1);
    EXPECT_EQ(newSeq->getLast(), 2);
}

TEST(LazySequenceTest, AppendMultiple) {
    auto seq = LazySequence<int>::create();
    auto seq2 = seq->append(1)->append(2)->append(3);
    EXPECT_EQ(static_cast<size_t>(seq2->getSize()), 3);
    EXPECT_EQ(seq2->getFirst(), 1);
    EXPECT_EQ(seq2->getLast(), 3);
}

TEST(LazySequenceTest, AppendSequence) {
    auto seq1 = LazySequence<int>::create(1);
    auto seq2 = LazySequence<int>::create(2);
    auto result = seq1->append(*seq2);
    EXPECT_EQ(static_cast<size_t>(result->getSize()), 2);
}

// Prepend Tests
TEST(LazySequenceTest, PrependSingleValue) {
    auto seq = LazySequence<int>::create(2);
    auto newSeq = seq->prepend(1);
    EXPECT_EQ(static_cast<size_t>(newSeq->getSize()), 2);
    EXPECT_EQ(newSeq->getFirst(), 1);
    EXPECT_EQ(newSeq->getLast(), 2);
}

TEST(LazySequenceTest, PrependMultiple) {
    auto seq = LazySequence<int>::create(3);
    auto seq2 = seq->prepend(2)->prepend(1);
    EXPECT_EQ(static_cast<size_t>(seq2->getSize()), 3);
    EXPECT_EQ(seq2->getFirst(), 1);
}

TEST(LazySequenceTest, PrependSequence) {
    auto seq1 = LazySequence<int>::create(2);
    auto seq2 = LazySequence<int>::create(1);
    auto result = seq1->prepend(*seq2);
    EXPECT_EQ(static_cast<size_t>(result->getSize()), 2);
    EXPECT_EQ(result->getFirst(), 1);
}

// Insert Tests
TEST(LazySequenceTest, InsertAtBeginning) {
    auto seq = LazySequence<int>::create(2)->append(3);
    auto newSeq = seq->insertAt(1, Ordinal(0));
    EXPECT_EQ(static_cast<size_t>(newSeq->getSize()), 3);
    EXPECT_EQ(newSeq->getFirst(), 1);
}

TEST(LazySequenceTest, InsertAtMiddle) {
    auto seq = LazySequence<int>::create(1)->append(3);
    auto newSeq = seq->insertAt(2, Ordinal(1));
    EXPECT_EQ(static_cast<size_t>(newSeq->getSize()), 3);
    EXPECT_EQ((*newSeq)[Ordinal(1)], 2);
}

TEST(LazySequenceTest, InsertAtEnd) {
    auto seq = LazySequence<int>::create(1)->append(2);
    auto newSeq = seq->insertAt(3, Ordinal(2));
    EXPECT_EQ(static_cast<size_t>(newSeq->getSize()), 3);
    EXPECT_EQ(newSeq->getLast(), 3);
}

// Skip Tests
TEST(LazySequenceTest, SkipFirst) {
    auto seq = LazySequence<int>::create(1)->append(2)->append(3);
    auto newSeq = seq->skip(Ordinal(0));
    EXPECT_EQ(static_cast<size_t>(newSeq->getSize()), 2);
    EXPECT_EQ(newSeq->getFirst(), 2);
}

TEST(LazySequenceTest, SkipRange) {
    auto seq = LazySequence<int>::create(1)->append(2)->append(3)->append(4);
    auto newSeq = seq->skip(Ordinal(1), Ordinal(3));
    EXPECT_EQ(static_cast<size_t>(newSeq->getSize()), 2);
    EXPECT_EQ(newSeq->getFirst(), 1);
    EXPECT_EQ(newSeq->getLast(), 4);
}

// SubSequence Tests
TEST(LazySequenceTest, GetSubSequence) {
    auto seq = LazySequence<int>::create(1)->append(2)->append(3)->append(4)->append(5);
    auto subSeq = seq->getSubSequence(Ordinal(1), Ordinal(4));
    EXPECT_EQ(static_cast<size_t>(subSeq->getSize()), 3);
    EXPECT_EQ(subSeq->getFirst(), 2);
    EXPECT_EQ(subSeq->getLast(), 4);
}

// Concat Tests
TEST(LazySequenceTest, ConcatSequences) {
    auto seq1 = LazySequence<int>::create(1)->append(2);
    auto seq2 = LazySequence<int>::create(3)->append(4);
    auto result = seq1->concat(*seq2);
    EXPECT_EQ(static_cast<size_t>(result->getSize()), 4);
    EXPECT_EQ(result->getFirst(), 1);
    EXPECT_EQ(result->getLast(), 4);
}

// Map Tests
TEST(LazySequenceTest, MapFunction) {
    auto seq = LazySequence<int>::create(1)->append(2)->append(3);
    auto mapped = seq->map<int>([](int x) { return x * 2; });
    EXPECT_EQ(static_cast<size_t>(mapped->getSize()), 3);
    EXPECT_EQ(mapped->getFirst(), 2);
    EXPECT_EQ((*mapped)[Ordinal(1)], 4);
    EXPECT_EQ(mapped->getLast(), 6);
}

TEST(LazySequenceTest, MapTypeConversion) {
    auto seq = LazySequence<int>::create(1)->append(2)->append(3);
    auto mapped = seq->map<double>([](int x) { return x * 1.5; });
    EXPECT_EQ(static_cast<size_t>(mapped->getSize()), 3);
    EXPECT_DOUBLE_EQ(mapped->getFirst(), 1.5);
}

// Where Tests
TEST(LazySequenceTest, WhereFilter) {
    auto seq = LazySequence<int>::create(1)->append(2)->append(3)->append(4)->append(5);
    auto filtered = seq->where([](int x) { return x % 2 == 0; });
    EXPECT_EQ(filtered->getFirst(), 2);
    EXPECT_EQ((*filtered)[Ordinal(1)], 4);
}

// Fold Tests
TEST(LazySequenceTest, FoldLeft) {
    auto seq = LazySequence<int>::create(1)->append(2)->append(3)->append(4);
    int sum = seq->foldl<int>([](int x, int y) { return x + y; }, 0);
    EXPECT_EQ(sum, 10);
}

TEST(LazySequenceTest, FoldRight) {
    auto seq = LazySequence<int>::create(1)->append(2)->append(3)->append(4);
    int sum = seq->foldr<int>([](int x, int y) { return x + y; }, 0);
    EXPECT_EQ(sum, 10);
}

// Index Access Tests
TEST(LazySequenceTest, IndexOperator) {
    auto seq = LazySequence<int>::create(10)->append(20)->append(30);
    EXPECT_EQ((*seq)[Ordinal(0)], 10);
    EXPECT_EQ((*seq)[Ordinal(1)], 20);
    EXPECT_EQ((*seq)[Ordinal(2)], 30);
}

// Infinite Sequence Tests
TEST(LazySequenceTest, InfiniteSequence) {
    ArraySequence<int> initial;
    initial.append(1);
    auto seq = LazySequence<int>::create(1, [](ArraySequence<int>& window) {
        return window[0] + 1;
    }, initial);
    
    EXPECT_FALSE(seq->isFinite());
    EXPECT_EQ((*seq)[Ordinal(0)], 1);
    EXPECT_EQ((*seq)[Ordinal(1)], 2);
    EXPECT_EQ((*seq)[Ordinal(10)], 11);
}

TEST(LazySequenceTest, FibonacciSequence) {
    ArraySequence<int> initial;
    initial.append(0);
    initial.append(1);
    auto fib = LazySequence<int>::create(2, [](ArraySequence<int>& window) {
        return window[0] + window[1];
    }, initial);
    
    EXPECT_EQ((*fib)[Ordinal(0)], 0);
    EXPECT_EQ((*fib)[Ordinal(1)], 1);
    EXPECT_EQ((*fib)[Ordinal(2)], 1);
    EXPECT_EQ((*fib)[Ordinal(3)], 2);
    EXPECT_EQ((*fib)[Ordinal(4)], 3);
    EXPECT_EQ((*fib)[Ordinal(5)], 5);
    EXPECT_EQ((*fib)[Ordinal(6)], 8);
}

// Size and State Tests
TEST(LazySequenceTest, GetSize) {
    auto seq = LazySequence<int>::create(1)->append(2)->append(3);
    EXPECT_EQ(static_cast<size_t>(seq->getSize()), 3);
}

TEST(LazySequenceTest, IsEmpty) {
    auto empty = LazySequence<int>::create();
    auto notEmpty = LazySequence<int>::create(1);
    EXPECT_TRUE(empty->isEmpty());
    EXPECT_FALSE(notEmpty->isEmpty());
}

TEST(LazySequenceTest, IsFinite) {
    auto finite = LazySequence<int>::create(1)->append(2);
    EXPECT_TRUE(finite->isFinite());
}

// Memoization Tests
TEST(LazySequenceTest, Memoization) {
    int callCount = 0;
    ArraySequence<int> initial;
    initial.append(1);
    auto seq = LazySequence<int>::create(1, [&callCount](ArraySequence<int>& window) {
        callCount++;
        return window[0] + 1;
    }, initial);
    
    (*seq)[Ordinal(5)];
    int firstCallCount = callCount;
    (*seq)[Ordinal(3)];
    EXPECT_EQ(callCount, firstCallCount);
}

// Complex Operations Tests
TEST(LazySequenceTest, ChainedOperations) {
    auto seq = LazySequence<int>::create(1)
        ->append(2)->append(3)->append(4)->append(5)
        ->map<int>([](int x) { return x * 2; })
        ->where([](int x) { return x > 4; });
    
    EXPECT_EQ(seq->getFirst(), 6);
}

TEST(LazySequenceTest, MultipleAppends) {
    auto seq = LazySequence<int>::create();
    for (int i = 0; i < 10; i++) {
        seq = seq->append(i);
    }
    EXPECT_EQ(static_cast<size_t>(seq->getSize()), 10);
    EXPECT_EQ(seq->getFirst(), 0);
    EXPECT_EQ(seq->getLast(), 9);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
