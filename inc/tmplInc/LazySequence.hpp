#ifndef LAZY_SEQUENCE_H
#define LAZY_SEQUENCE_H

#include "ArraySequence.hpp"
#include "UniquePtr.hpp"
#include "WeakPtr.hpp"
#include "Option.hpp"
#include "Ordinal.hpp"
#include "Cardinal.hpp"
#include <functional>

template <typename T>
class LazySequence
{
private:
    class IGenerator;
    class FiniteGenerator;
    class StandardGenerator;
    class AppendGenerator;
private:
    LazySequence();

    LazySequence( const T& value );
    LazySequence( const ArraySequence<T>& data );
    LazySequence( const size_t arity, const std::function<T(ArraySequence<T>&)>& func, const ArraySequence<T>& data );
    LazySequence( UniquePtr<IGenerator>&& generator, const Cardinal& size, const Option<Ordinal>& ordinality );
public:
    LazySequence( const LazySequence<T>& other );
    LazySequence<T>& operator=( const LazySequence<T>& other );

    LazySequence( LazySequence<T>&& other );
    LazySequence<T>& operator=( LazySequence<T>&& other );

    ~LazySequence() = default;
public:
    static SharedPtr<LazySequence<T>> create();
    static SharedPtr<LazySequence<T>> create( const T& value );
    static SharedPtr<LazySequence<T>> create( const ArraySequence<T>& data );
    static SharedPtr<LazySequence<T>> create( const size_t arity, const std::function<T(ArraySequence<T>&)>& producingFunc, const ArraySequence<T>& data );
    static SharedPtr<LazySequence<T>> create( UniquePtr<IGenerator>&& generator, const Cardinal& size, const Option<Ordinal>& ordinality );
public:
    const T& getFirst();

    const T& getLast();

    const T& operator[]( const Ordinal& index );
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
    T2 foldl( const std::function<T2(T)>& func, const T2& base );
    template <typename T2>
    T2 foldr( const std::function<T2(T)>& func, const T2& base );
public:
    Cardinal getSize();
    size_t getMaterializedCount();
    bool isEmpty() const;
    bool isFinite() const;
private:
    Cardinal _size;
    Ordinal _offset;
    Option<Ordinal> _ordinality; // maximum possible index (might be unable to resolve)
 
    SharedPtr<IGenerator> _generator;
    UniquePtr<ArraySequence<T>> _items;
public:
    class IGenerator
    {
    public:
        virtual ~IGenerator() = default;
    
        virtual T getNext() = 0;
        virtual T get( const Ordinal& index ) = 0;
        virtual bool hasNext() = 0;
        Option<T> tryGetNext() = 0;
    };
private:
    class FiniteGenerator : IGenerator
    {
    public:
        FiniteGenerator();    

        FiniteGenerator( const ArraySequence<T>& data );
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
        Ordinal _lastMaterialized;
        ArraySequence<T> _data;
    };
private:
    class InfiniteGenerator : public IGenerator
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
        Ordinal _lastMaterialized;
        ArraySequence<T> _window;
        std::function<T(ArraySequence<T>&)> _producingFunc;
    };
private:
    class AppendGenerator : public IGenerator
    {
    public:
        AppendGenerator( const T& value, SharedPtr<IGenerator> parent, const Ordinal& border );
        AppendGenerator( const LazySequence<T>& value, SharedPtr<IGenerator> parent, const Ordinal& border );
        
        AppendGenerator( const AppendGenerator& other );
        AppendGenerator& operator=( const AppendGenerator& other );
        
        AppendGenerator( AppendGenerator&& other );
        AppendGenerator& operator=( AppendGenerator&& other );
        
        ~AppendGenerator() = default;
    public:
        T getNext() override;
        T get( const Ordinal& index ) override;
        bool hasNext();
        Option<T> tryGetNext();
    private:
        Ordinal _lastMaterialized;
        Ordinal _border;
        SharedPtr<IGenerator> _initial;
        SharedPtr<IGenerator> _added;
    };
private:
    class PrependGenerator : public IGenerator
    {
    public:
        PrependGenerator( const T& value, SharedPtr<IGenerator> parent, const Ordinal& border );
        PrependGenerator( const LazySequence<T>& value, SharedPtr<IGenerator> parent, const Ordinal& border );
        
        PrependGenerator( const PrependGenerator& other );
        PrependGenerator& operator=( const PrependGenerator& other );
        
        PrependGenerator( PrependGenerator&& other );
        PrependGenerator& operator=( PrependGenerator&& other );
        
        ~PrependGenerator() = default;
    public:
        T getNext() override;
        T get( const Ordinal& index ) override;
        bool hasNext();
        Option<T> tryGetNext();    
    private:
        Ordinal _lastMaterialized;
        Ordinal _border;
        SharedPtr<IGenerator> _initial;
        SharedPtr<IGenerator> _added;
    };
private:
    class InsertGenerator : public IGenerator
    {
    public:
        InsertGenerator( const T& value, const Ordinal& index, SharedPtr<IGenerator> parent, const Ordinal& addedOrdinality );
        InsertGenerator( const LazySequence<T>& value, const Ordinal& index, SharedPtr<IGenerator> parent, const Ordinal& addedOrdinality );
        
        InsertGenerator( const InsertGenerator& other );
        InsertGenerator& operator=( const InsertGenerator& other );
        
        InsertGenerator( InsertGenerator&& other );
        InsertGenerator& operator=( InsertGenerator&& other );
        
        ~InsertGenerator() = default;
    public:
        T getNext() override;
        T get( const Ordinal& index ) override;
        bool hasNext();
        Option<T> tryGetNext();
    private:
        Ordinal _lastMaterialized;
        Ordinal _targetIndex;
        Ordinal _border;
        SharedPtr<IGenerator> _initial;
        SharedPtr<IGenerator> _added;
    };
private:
    class SkipGenerator : public IGenerator
    {
    public:
        SkipGenerator( const Ordinal& index, SharedPtr<IGenerator> parent );
        SkipGenerator( const Ordinal& start, const Ordinal& end, SharedPtr<IGenerator> parent );
        
        SkipGenerator( const SkipGenerator& other );
        SkipGenerator& operator=( const SkipGenerator& other );
        
        SkipGenerator( SkipGenerator&& other );
        SkipGenerator& operator=( SkipGenerator&& other );

        ~SkipGenerator() = default;
    public:
        T getNext() override;
        T get( const Ordinal& index ) override;
        bool hasNext();
        Option<T> tryGetNext();        
    private:
        Ordinal _lastMaterialized;
        Ordinal _from;
        Ordinal _to;
        SharedPtr<IGenerator> _parent;
    };
private:
    class SubSequenceGenerator : public IGenerator
    {
    public:
        SubSequenceGenerator( const Ordinal& start, const Ordinal& end, SharedPtr<IGenerator> parent );

        SubSequenceGenerator( const SubSequenceGenerator& other );
        SubSequenceGenerator& operator=( const SubSequenceGenerator& other );
        
        SubSequenceGenerator( SubSequenceGenerator&& other );
        SubSequenceGenerator& operator=( SubSequenceGenerator&& other );
        
        ~SubSequenceGenerator() = default;
    public:
        T getNext() override;
        T get( const Ordinal& index ) override;
        bool hasNext();
        Option<T> tryGetNext();
    private:
        Ordinal _lastMaterialized;
        Ordinal _from;
        Ordinal _to;
        SharedPtr<IGenerator> _parent;
    };
private:
    class ConcatGenerator : public IGenerator
    {
    public:
        ConcatGenerator( SharedPtr<IGenerator> first, SharedPtr<IGenerator> second, const Ordinal& border );
        
        ConcatGenerator( const ConcatGenerator& other );
        ConcatGenerator& operator=( const ConcatGenerator& other );
        
        ConcatGenerator( ConcatGenerator&& other );
        ConcatGenerator& operator=( ConcatGenerator&& other );

        ~ConcatGenerator();
    public:
        T getNext() override;
        T get( const Ordinal& index ) override;
        bool hasNext();
        Option<T> tryGetNext();   
    private:
        Ordinal _border;
        SharedPtr<IGenerator> _first;
        SharedPtr<IGenerator> _second;
    };
private:
    template <typename T2>
    class MapGenerator : public IGenerator 
    {
    public:
        MapGenerator( const std::function<T2(T)> func, SharedPtr<IGenerator> parent );
        
        MapGenerator( const MapGenerator& other );
        MapGenerator& operator=( const MapGenerator& other );
        
        MapGenerator( MapGenerator&& other );
        MapGenerator& operator=( MapGenerator&& other );

        ~MapGenerator();
    public:
        T2 getNext() override;
        T2 get( const Ordinal& index ) override;
        bool hasNext();
        Option<T> tryGetNext();   
    private:
        std::function<T2(T)> func;
        SharedPtr<IGenerator> _parent;
    };
private:
    class WhereGenerator : IGenerator 
    {
    public:
        WhereGenerator( const std::function<bool(T)> func, SharedPtr<IGenerator> parent );
        
        WhereGenerator( const WhereGenerator& other );
        WhereGenerator& operator=( const WhereGenerator& other );
        
        WhereGenerator( WhereGenerator&& other );
        WhereGenerator& operator=( WhereGenerator&& other );

        ~WhereGenerator();
    public:
        T getNext() override;
        T get( const Ordinal& index ) override;
        bool hasNext();
        Option<T> tryGetNext();   
    private:
        std::function<bool(T)> func;
        SharedPtr<IGenerator> _parent;
    }
private:
//     class LazySequenceIterator
//     {
//     public:
//         using iterator_category = std::forward_iterator_tag;
//         using difference_type   = std::ptrdiff_t;
//         using value_type = T;
//         using pointer    = T*;
//         using reference  = T&;
//     public:
//         LazySequenceIterator();

//         LazySequenceIterator( SharedPtr<IGenerator> generator, const Ordinal& pos );

//         LazySequenceIterator( const LazySequenceIterator& other );
//         LazySequenceIterator& operator=( const LazySequenceIterator& other );

//         LazySequenceIterator( LazySequenceIterator&& other );
//         LazySequenceIterator& operator=( LazySequenceIterator&& other );
//     public:
//         LazySequenceIterator& operator++();
//         LazySequenceIterator operator++(int);

//         T& operator*();
//         const T& operator*() const;
        
//         T* operator->();
//         const T* operator->() const;

//         bool operator==( const LazySequenceIterator& other ) const noexcept;
//         bool operator!=( const LazySequenceIterator& other ) const noexcept;
//     private:
//         SharedPtr<IGenerator> _observed;
//         Ordinal _pos;
//     };
// public: 
//     LazySequenceIterator begin();
//     LazySequenceIterator end();

//     const LazySequenceIterator begin() const;
//     const LazySequenceIterator end() const;
};

#include "LazySequence.tpp"
#include "Generator.tpp"
#include "LazySequenceIterator.tpp"

#endif // LAZY_SEQUENCE_H