#ifndef LAZY_SEQUENCE
#define LAZY_SEQUENCE

#include "Sequence.hpp"
#include "SharedPtr.hpp"
#include "UniquePtr.hpp"
#include "WeakPtr.hpp"

template <typename T>
class LazySequence
{
public:
    LazySequence();

    LazySequence( Sequence<T>* other );
    LazySequence( T(*)( Sequence<T>* ), Sequence<T>* other );

    LazySequence( const LazySequence<T>& other );
    LazySequence<T>& operator=( const LazySequence<T>& other );

    LazySequence( LazySequence<T>&& other );
    LazySequence<T>& operator=( LazySequence<T>&& other );
    
    virtual ~LazySequence();
public:
    T& getFirst();
    const T& getFirst() const;

    T& getLast();
    const T& getLast() const;

    T& get( size_t index );
    const T& get( size_t index ) const;
    
};


#endif // LAZY_SEQUENCE