#ifndef LAZY_SEQUENCE
#define LAZY_SEQUENCE

#include "Sequence.hpp"
#include "UniquePtr.hpp"
#include "WeakPtr.hpp"
#include "Transfinite.hpp"
#include <functional>


template <typename T>
class LazySequence
{
public:
    virtual ~LazySequence() = default;
public:
    LazySequence();
    LazySequence( UniquePtr<Sequence<T>> data );

    LazySequence( UniquePtr<Sequence<T>> data, std::function<T( UniquePtr<Sequence<T>> )> producingFunc );
    LazySequence( UniquePtr<Sequence<T>> data, T (*producingFunc)( UniquePtr<Sequence<T>>) );

    LazySequence( const LazySequence<T>& other );
    LazySequence<T>& operator=( const LazySequence<T>& other );
public:
    using ord = Variant<size_t, Ordinal>;
    virtual T& getFirst( ord index) = 0;
    virtual const T& getFirst( ord index ) = 0;

    virtual T& getLast( ord index ) = 0;
    virtual const T& getLast( ord index ) = 0;

    virtual T& operator[]( ord index ) = 0;
    virtual const T& operator[]( ord index ) const = 0;
public:
    virtual LazySequence<T>* append( const T& value ) = 0;
    virtual LazySequence<T>* prepend( const T& value ) = 0;
    virtual LazySequence<T>* insertAt( const T& value ) = 0;

    virtual LazySequence<T>* skip( ord index ) = 0;
    virtual LazySequence<T>* skip( ord start, ord end ) = 0;
    virtual LazySequence<T>* skip( LazySequence<ord> indices ) = 0;

    virtual LazySequence<T>* getSubSequence( ord start, ord end ) = 0;
public:
    template <typename T2>
    virtual LazySequence<T2>* map( T2(*func)(T) ) = 0;

    virtual LazySequence<T2>* where( bool(*func)(T) ) = 0;

    template <typename T2>
    virtual T2 foldl( T2(*func)(const T&, const T2&) ) = 0;
    template <typename T2>
    virtual T2 foldl( T2(*func)(const T&, const T2&), const T& base ) = 0;

    template <typename T2>
    virtual T2 foldr( T2(*func)(const T&, const T2&) ) = 0;
    template <typename T2>
    virtual T2 foldr( T2(*func)(const T&, const T2&), const T& base ) = 0;
public:
    Cardinal getSize();
    size_t getMaterializedCount();
    bool isEmpty();
    bool isFinite();
private:
    class IGenerator;

    UniquePtr<Sequence<T>> items;
    UniquePtr<IGenerator> generator;
};


#endif // LAZY_SEQUENCE