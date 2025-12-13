#ifndef LAZY_SEQUENCE_H
#define LAZY_SEQUENCE_H

#include "ArraySequence.hpp"
#include "UniquePtr.hpp"
#include "WeakPtr.hpp"
#include "Option.hpp"
#include "Ordinal.hpp"
#include "Cardinal.hpp"
#include "SharedFromThis.hpp"
#include "Generator.hpp"
#include <functional>

template <typename T>
class LazySequence : public EnableSharedFromThis<LazySequence<T>>
{
public:
    LazySequence();

    LazySequence( const T& value );
    LazySequence( ArraySequence<T>& data );
    LazySequence( const size_t arity, const std::function<T(ArraySequence<T>&)>& func, ArraySequence<T>& data );
    LazySequence( UniquePtr<IGenerator<T>>&& generator, const Cardinal& size, const Option<Ordinal>& ordinality );
public:
    LazySequence( const LazySequence<T>& other );
    LazySequence<T>& operator=( const LazySequence<T>& other );

    LazySequence( LazySequence<T>&& other );
    LazySequence<T>& operator=( LazySequence<T>&& other );

    ~LazySequence() = default;
public:
    static SharedPtr<LazySequence<T>> create();
    static SharedPtr<LazySequence<T>> create( const T& value );
    static SharedPtr<LazySequence<T>> create( ArraySequence<T>& data );
    static SharedPtr<LazySequence<T>> create( const size_t arity, const std::function<T(ArraySequence<T>&)>& producingFunc, ArraySequence<T>& data );
    template <typename T2>
    static SharedPtr<LazySequence<T2>> create( UniquePtr<IGenerator<T2>>&& generator, const Cardinal& size, const Option<Ordinal>& ordinality );
public:
    T getFirst();
    T getLast();
    T operator[]( const Ordinal& index );
public:
    SharedPtr<LazySequence<T>> append( const T& value );
    SharedPtr<LazySequence<T>> append( const LazySequence<T>& value );
    
    SharedPtr<LazySequence<T>> prepend( const T& value );
    SharedPtr<LazySequence<T>> prepend( const LazySequence<T>& value );

    SharedPtr<LazySequence<T>> insertAt( const T& value, const Ordinal& index );
    SharedPtr<LazySequence<T>> insertAt( const LazySequence<T>& items, const Ordinal& index );

    SharedPtr<LazySequence<T>> skip( const Ordinal& index );
    SharedPtr<LazySequence<T>> skip( const Ordinal& start, const Ordinal& end );

    SharedPtr<LazySequence<T>> getSubSequence( const Ordinal& start, const Ordinal& end );
    SharedPtr<LazySequence<T>> concat( const LazySequence<T>& other );
public:
    template <typename T2>
    SharedPtr<LazySequence<T2>> map( const std::function<T2(T)>& func );

    SharedPtr<LazySequence<T>> where( const std::function<bool(T)>& func );

    template <typename T2>
    T2 foldl( const std::function<T2(T2, T)>& func, const T2& base );
    template <typename T2>
    T2 foldr( const std::function<T2(T, T2)>& func, const T2& base );
public:
    Cardinal getSize() const;
    size_t getMaterializedCount() const;
    bool isEmpty() const;
    bool isFinite() const;
private:
    Cardinal _size;
    size_t _offset;
    Option<Ordinal> _ordinality; // maximum possible index starting with 1 (might be unable to resolve for where)
 
    SharedPtr<IGenerator<T>> _generator;
    UniquePtr<ArraySequence<T>> _items;
    static const size_t CACHE_MAX_SIZE = 2'000;
    static const size_t CACHE_DECREASE_SIZE = 1'000;
    static const size_t CACHE_OFFSET_INCREASE = 1'000;
    void trimCache();
public: // methods independent of indices which support correct memoization process
    const T& memoiseNext();
    T get( const Ordinal& index ); // also supports correct memoization because doesn't memoise anything
    bool canMemoiseNext();
    Option<T> tryMemoiseNext();
private:
    class LazySequenceIterator
    {
    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type = T;
        using reference  = T;
        using pointer    = void;
    public:
        LazySequenceIterator();

        LazySequenceIterator( SharedPtr<LazySequence<T>> parent, const Ordinal& pos );

        LazySequenceIterator( const LazySequenceIterator& other );
        LazySequenceIterator& operator=( const LazySequenceIterator& other );

        LazySequenceIterator( LazySequenceIterator&& other );
        LazySequenceIterator& operator=( LazySequenceIterator&& other );
    public:
        LazySequenceIterator& operator++();
        LazySequenceIterator operator++(int);

        T operator*();

        bool operator==( const LazySequenceIterator& other ) const noexcept;
        bool operator!=( const LazySequenceIterator& other ) const noexcept;
    private:
        SharedPtr<LazySequence<T>> _observed;
        Ordinal _pos;
    };
public: 
    LazySequenceIterator begin();
    LazySequenceIterator end();
};

#include "LazySequence.tpp"
#include "LazySequenceIterator.tpp"

#endif // LAZY_SEQUENCE_H