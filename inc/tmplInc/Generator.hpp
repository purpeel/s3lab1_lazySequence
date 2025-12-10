#ifndef GENERATOR_H
#define GENERATOR_H

#include <functional>
#include "Option.hpp"
#include "ArraySequence.hpp"
#include "Ordinal.hpp"
#include "SharedPtr.hpp"

template <typename T>
class LazySequence;

template <typename T>
class IGenerator
{
public:
    virtual ~IGenerator() = default;

    virtual T getNext() = 0;
    virtual T get( const Ordinal& index ) = 0;
    virtual bool hasNext() = 0;
    virtual Option<T> tryGetNext() = 0;
};
    
template <typename T>
class FiniteGenerator : public IGenerator<T>
{
public:
    FiniteGenerator();    

    FiniteGenerator( ArraySequence<T>&& data );
    FiniteGenerator( const T& value );

    FiniteGenerator( const FiniteGenerator& other );
    FiniteGenerator& operator=( const FiniteGenerator& other );

    FiniteGenerator( FiniteGenerator&& other );
    FiniteGenerator& operator=( FiniteGenerator&& other );

    ~FiniteGenerator() = default;
public:
    T getNext() override;
    T get( const Ordinal& index ) override;
    bool hasNext() override;
    Option<T> tryGetNext() override;
private:
    size_t _lastMaterialized;
    ArraySequence<T> _data;
};
    
template <typename T>
class InfiniteGenerator : public IGenerator<T>
{
public:
    InfiniteGenerator( const InfiniteGenerator& other );
    InfiniteGenerator& operator=( const InfiniteGenerator& other );

    InfiniteGenerator( InfiniteGenerator&& other );
    InfiniteGenerator& operator=( InfiniteGenerator&& other );

    InfiniteGenerator( const size_t arity, const std::function<T(ArraySequence<T>&)>& func, const ArraySequence<T>& data );

    ~InfiniteGenerator() = default;
public:
    T getNext() override;
    T get( const Ordinal& index ) override;
    bool hasNext() override;
    Option<T> tryGetNext() override;
private:
    size_t _arity;
    size_t _lastMaterialized;
    ArraySequence<T> _window;
    std::function<T(ArraySequence<T>&)> _producingFunc;
};

template <typename T>
class AppendGenerator : public IGenerator<T>
{
public:
    AppendGenerator( const T& value, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& border );
    AppendGenerator( const LazySequence<T>& value, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& border );
    
    AppendGenerator( const AppendGenerator& other );
    AppendGenerator& operator=( const AppendGenerator& other );
    
    AppendGenerator( AppendGenerator&& other );
    AppendGenerator& operator=( AppendGenerator&& other );
    
    ~AppendGenerator() = default;
public:
    T getNext() override;
    T get( const Ordinal& index ) override;
    bool hasNext() override;
    Option<T> tryGetNext() override;
private:
    size_t _lastMaterialized;
    Option<Ordinal> _border;
    SharedPtr<LazySequence<T>> _initial;
    SharedPtr<LazySequence<T>> _added;
};

template <typename T>
class PrependGenerator : public IGenerator<T>
{
public:
    PrependGenerator( const T& value, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& border );
    PrependGenerator( const LazySequence<T>& value, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& border );
    
    PrependGenerator( const PrependGenerator& other );
    PrependGenerator& operator=( const PrependGenerator& other );
    
    PrependGenerator( PrependGenerator&& other );
    PrependGenerator& operator=( PrependGenerator&& other );
    
    ~PrependGenerator() = default;
public:
    T getNext() override;
    T get( const Ordinal& index ) override;
    bool hasNext() override;
    Option<T> tryGetNext() override;    
private:
    size_t _lastMaterialized;
    Option<Ordinal> _border;
    SharedPtr<LazySequence<T>> _initial;
    SharedPtr<LazySequence<T>> _added;
};

template <typename T>
class InsertGenerator : public IGenerator<T>
{
public:
    InsertGenerator( const T& value, const Ordinal& index, SharedPtr<LazySequence<T>> parent );
    InsertGenerator( const LazySequence<T>& value, const Ordinal& index, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& addedOrdinality );
    
    InsertGenerator( const InsertGenerator& other );
    InsertGenerator& operator=( const InsertGenerator& other );
    
    InsertGenerator( InsertGenerator&& other );
    InsertGenerator& operator=( InsertGenerator&& other );
    
    ~InsertGenerator() = default;
public:
    T getNext() override;
    T get( const Ordinal& index ) override;
    bool hasNext() override;
    Option<T> tryGetNext() override;
private:
    size_t _lastMaterialized;
    Ordinal _targetIndex;
    Option<Ordinal> _border;
    SharedPtr<LazySequence<T>> _initial;
    SharedPtr<LazySequence<T>> _added;
};

template <typename T>
class SkipGenerator : public IGenerator<T>
{
public:
    SkipGenerator( const Ordinal& index, SharedPtr<LazySequence<T>> parent );
    SkipGenerator( const Ordinal& start, const Ordinal& end, SharedPtr<LazySequence<T>> parent );
    
    SkipGenerator( const SkipGenerator& other );
    SkipGenerator& operator=( const SkipGenerator& other );
    
    SkipGenerator( SkipGenerator&& other );
    SkipGenerator& operator=( SkipGenerator&& other );

    ~SkipGenerator() = default;
public:
    T getNext() override;
    T get( const Ordinal& index ) override;
    bool hasNext() override;
    Option<T> tryGetNext() override; 
private:
    size_t _lastMaterialized;
    Ordinal _from;
    Ordinal _to;
    SharedPtr<LazySequence<T>> _parent;
};

template <typename T>
class SubSequenceGenerator : public IGenerator<T>
{
public:
    SubSequenceGenerator( const Ordinal& start, const Ordinal& end, SharedPtr<LazySequence<T>> parent );

    SubSequenceGenerator( const SubSequenceGenerator& other );
    SubSequenceGenerator& operator=( const SubSequenceGenerator& other );
    
    SubSequenceGenerator( SubSequenceGenerator&& other );
    SubSequenceGenerator& operator=( SubSequenceGenerator&& other );
    
    ~SubSequenceGenerator() = default;
public:
    T getNext() override;
    T get( const Ordinal& index ) override;
    bool hasNext() override;
    Option<T> tryGetNext() override;
private:
    size_t _lastMaterialized;
    Ordinal _from;
    Ordinal _to;
    SharedPtr<LazySequence<T>> _parent;
};

template <typename T>
class ConcatGenerator : public IGenerator<T>
{
public:
    ConcatGenerator( SharedPtr<LazySequence<T>> first, SharedPtr<LazySequence<T>> second, const Option<Ordinal>& border );
    
    ConcatGenerator( const ConcatGenerator& other );
    ConcatGenerator& operator=( const ConcatGenerator& other );
    
    ConcatGenerator( ConcatGenerator&& other );
    ConcatGenerator& operator=( ConcatGenerator&& other );

    ~ConcatGenerator() = default;
public:
    T getNext() override;
    T get( const Ordinal& index ) override;
    bool hasNext() override;
    Option<T> tryGetNext() override;   
private:
    Option<Ordinal> _border;
    SharedPtr<LazySequence<T>> _first;
    SharedPtr<LazySequence<T>> _second;
};

template <typename TIn, typename TOut>
class MapGenerator : public IGenerator<TOut>
{
public:
    MapGenerator( const std::function<TOut(TIn)>& func, SharedPtr<LazySequence<TIn>> parent );
    
    MapGenerator( const MapGenerator<TIn, TOut>& other );
    MapGenerator<TIn, TOut>& operator=( const MapGenerator<TIn, TOut>& other );
    
    MapGenerator( MapGenerator<TIn, TOut>&& other );
    MapGenerator<TIn, TOut>& operator=( MapGenerator<TIn, TOut>&& other );

    ~MapGenerator() = default;
public:
    TOut getNext() override;
    TOut get( const Ordinal& index ) override;
    bool hasNext() override;
    Option<TOut> tryGetNext() override;   
private:
    std::function<TOut(TIn)> _func;
    SharedPtr<LazySequence<TIn>> _parent;
};

template <typename T>
class WhereGenerator : public IGenerator<T>
{
public:
    WhereGenerator( const std::function<bool(T)>& func, SharedPtr<LazySequence<T>> parent );
    
    WhereGenerator( const WhereGenerator& other );
    WhereGenerator& operator=( const WhereGenerator& other );
    
    WhereGenerator( WhereGenerator&& other );
    WhereGenerator& operator=( WhereGenerator&& other );

    ~WhereGenerator() = default;
public:
    T getNext() override;
    T get( const Ordinal& index ) override;
    bool hasNext() override;
    Option<T> tryGetNext() override;   
private:
    std::function<bool(T)> _predicate;
    SharedPtr<LazySequence<T>> _parent;
    Option<T> _memoized; // need to remember an element since both hasNext() and getNext() are changing the state of parent generator and valid elements can become lost
    bool _isFinished;
};

#include "Generator.tpp"

#endif  // GENERATOR_H