#ifndef LAZY_SEQUENCE_H
#define LAZY_SEQUENCE_H

#include "Sequence.hpp"
#include "UniquePtr.hpp"
#include "WeakPtr.hpp"
#include "Ordinal.hpp"
#include "Option.hpp"
#include <functional>
#include <tuple>


template <typename T>
class LazySequence
{
protected:
    LazySequence( SharedPtr<IGenerator> generator );
public:
    LazySequence();
    LazySequence( const Sequence<T>& data );

    LazySequence( const Sequence<T>& data, std::function<T(Sequence<T>)> producingFunc );
    LazySequence( const Sequence<T>& data, T (*producingFunc)(Sequence<T>) );

    LazySequence( const LazySequence<T>& other );
    LazySequence<T>& operator=( const LazySequence<T>& other );

    virtual ~LazySequence() = default;
public:
    using ordinal = Variant<size_t, Ordinal>;
    T& getFirst();
    const T& getFirst();

    T& getLast();
    const T& getLast();

    T& operator[]( ordinal index );
    const T& operator[]( ordinal index ) const;
public:
    LazySequence<T>* append( const T& value );
    LazySequence<T>* prepend( const T& value );
    LazySequence<T>* insertAt( const T& value, ordinal index );
    LazySequence<T>* insertAt( const LazySequence<T>& items, ordinal index );

    LazySequence<T>* skip( ordinal index );
    LazySequence<T>* skip( ordinal start, ordinal end );
    LazySequence<T>* skip( const LazySequence<ordinal>& indices );

    LazySequence<T>* getSubSequence( ordinal start, ordinal end );
    LazySequence<T>* concat( const LazySequence<T>& other );
public:
    template <typename T2>
    LazySequence<T2>* map( T2(*func)(T) );

    LazySequence<T2>* where( bool(*func)(T) );

    template <typename T2>
    T2 foldl( T2(*func)(const T&, const T2&) );
    template <typename T2>
    T2 foldl( T2(*func)(const T&, const T2&), const T& base );

    template <typename T2>
    T2 foldr( T2(*func)(const T&, const T2&) );
    template <typename T2>
    T2 foldr( T2(*func)(const T&, const T2&), const T& base );
public:
    Cardinal getSize();
    size_t getMaterializedCount();
    bool isEmpty();
    bool isFinite();
private:
    class IGenerator {
    public:
        virtual T getNext() = 0;
        virtual bool hasNext() = 0;
        virtual Option<T> tryGetNext() = 0;
    public:
        virtual IGenerator* append( const T& value ) = 0;
        virtual IGenerator* prepend( const T& value ) = 0;
        virtual IGenerator* insertAt( const T& value, ordinal index ) = 0;

        virtual IGenerator* skip( ordinal index ) = 0;
        virtual IGenerator* skip( ordinal start, ordinal end ) = 0;
        virtual IGenerator* skip( const LazySequence<T>& indices ) = 0;

        virtual IGenerator* getSubSequence( ordinal start, ordinal end ) = 0;
        virtual IGenerator* concat( const LazySequence<T>& other ) = 0;
    };

    UniquePtr<Sequence<T>> _items;
    UniquePtr<IGenerator> _generator;

    Cardinal _size;
    Option<unsigned> generatorArity;
private:
    class StandardGenerator : IGenerator
    {
    public:
        StandardGenerator();

        ~StandardGenerator();
    public:
        T getNext() override;
        bool hasNext() override;
        Option<T> tryGetNext() override;
    public:
        IGenerator* append( const T& value ) override;
        IGenerator* prepend( const T& value ) override;
        IGenerator* insertAt( const T& value, ordinal index ) override;

        IGenerator* skip( ordinal index ) override;
        IGenerator* skip( ordinal start, ordinal end ) override;
        IGenerator* skip( const LazySequence<T>& indices ) override;

        IGenerator* getSubSequence( ordinal start, ordinal end ) override;
        IGenerator* concat( const LazySequence<T>& other ) override;
    private:
        WeakPtr<LazySequence<T>> _parent;
        SharedPtr<Sequence<T>> _parentItems;
    };
private:
    class AppendGenerator : IGenerator 
    {
    public:
        AppendGenerator() = delete;
        
        AppendGenerator( const T& value, SharedPtr<LazySequence<T>> parent );
        
        ~AppendGenerator();
    public:
        T getNext() override;
        bool hasNext() override;
        Option<T> tryGetNext() override;
    public:
        IGenerator* append( const T& value ) override;
        IGenerator* prepend( const T& value ) override;
        IGenerator* insertAt( const T& value, ordinal index ) override;

        IGenerator* skip( ordinal index ) override;
        IGenerator* skip( ordinal start, ordinal end ) override;
        IGenerator* skip( const LazySequence<T>& indices ) override;

        IGenerator* getSubSequence( ordinal start, ordinal end ) override;
        IGenerator* concat( const LazySequence<T>& other ) override;
    private:
        WeakPtr<LazySequence<T>> _parent;
        SharedPtr<Sequence<T>> _parentItems;
        T _value;
        ordinal _currentIndex;
        ordinal _targetIndex;
    };
private:
    class PrependGenerator : IGenerator
    {
    public:
        PrependGenerator() = delete;
        
        PrependGenerator( const T& value, SharedPtr<LazySequence<T>> parent );
        
        ~PrependGenerator();
    public:
        T getNext() override;
        bool hasNext() override;
        Option<T> tryGetNext() override;
    public:
        IGenerator* append( const T& value ) override;
        IGenerator* prepend( const T& value ) override;
        IGenerator* insertAt( const T& value, ordinal index ) override;

        IGenerator* skip( ordinal index ) override;
        IGenerator* skip( ordinal start, ordinal end ) override;
        IGenerator* skip( SharedPtr<LazySequence<T>> indices ) override;

        IGenerator* getSubSequence( ordinal start, ordinal end ) override;
        IGenerator* concat( SharedPtr<LazySequence<T>> other ) override;
    private:
        WeakPtr<LazySequence<T>> _parent;
        SharedPtr<Sequence<T>> _parentItems;
        T _value;
        ordinal _currentIndex;
        ordinal _targetIndex;
    };
private:
    class InsertGenerator : IGenerator 
    {
    public:
        InsertGenerator() = delete;

        InsertGenerator( const T& value, ordinal index, SharedPtr<LazySequence<T>> parent );
        InsertGenerator( const LazySequence<T>& items, ordinal index, SharedPtr<LazySequence<T>> parent );

        ~PrependGenerator();
    public:
        IGenerator* append( const T& value ) override;
        IGenerator* prepend( const T& value ) override;
        IGenerator* insertAt( const T& value, ordinal index ) override;

        IGenerator* skip( ordinal index ) override;
        IGenerator* skip( ordinal start, ordinal end ) override;
        IGenerator* skip( SharedPtr<LazySequence<T>> indices ) override;

        IGenerator* getSubSequence( ordinal start, ordinal end ) override;
        IGenerator* concat( SharedPtr<LazySequence<T>> other ) override;
    private:
        WeakPtr<LazySequence<T>> _parent;
        SharedPtr<Sequence<T>> _parentItems;

        Option<T> _value;
        Option<SharedPtr<LazySequence<T>>> _items;

        ordinal _currentIndex;
        ordinal _targetIndex;
    };
private:
    class SkipGenerator : IGenerator 
    {
    public:
        SkipGenerator() = delete;

        SkipGenerator( ordinal index, SharedPtr<LazySequence<T>> parent );
        SkipGenerator( ordinal start, ordinal end, SharedPtr<LazySequence<T>> parent );
        SkipGenerator( const LazySequence<T>& indices, SharedPtr<LazySequence<T>> parent );

        ~SkipGenerator();
    public:
        IGenerator* append( const T& value ) override;
        IGenerator* prepend( const T& value ) override;
        IGenerator* insertAt( const T& value, ordinal index ) override;

        IGenerator* skip( ordinal index ) override;
        IGenerator* skip( ordinal start, ordinal end ) override;
        IGenerator* skip( SharedPtr<LazySequence<T>> indices ) override;

        IGenerator* getSubSequence( ordinal start, ordinal end ) override;
        IGenerator* concat( SharedPtr<LazySequence<T>> other ) override;
    private:
        WeakPtr<LazySequence<T>> _parent;
        SharedPtr<Sequence<T>> _parentItems;

        Option<ordinal> _from;
        Option<ordinal> _to;
        Option<SharedPtr<LazySequence<T>>> _indices;
    };
private:
    class SubSequenceGenerator : IGenerator 
    {
    public:
        SubSequenceGenerator() = delete;

        SubSequenceGenerator( ordinal index, SharedPtr<LazySequence<T>> parent );
        SubSequenceGenerator( ordinal start, ordinal end, SharedPtr<LazySequence<T>> parent );
        SubSequenceGenerator( const LazySequence<T>& indices, SharedPtr<LazySequence<T>> parent );
        
        ~SubSequenceGenerator();
    public:
        IGenerator* append( const T& value ) override;
        IGenerator* prepend( const T& value ) override;
        IGenerator* insertAt( const T& value, ordinal index ) override;

        IGenerator* skip( ordinal index ) override;
        IGenerator* skip( ordinal start, ordinal end ) override;
        IGenerator* skip( SharedPtr<LazySequence<T>> indices ) override;

        IGenerator* getSubSequence( ordinal start, ordinal end ) override;
        IGenerator* concat( SharedPtr<LazySequence<T>> other ) override;
    private:
        WeakPtr<LazySequence<T>> _parent;
        SharedPtr<Sequence<T>> _parentItems;

        Option<ordinal> _from;
        Option<ordinal> _to;
        Option<SharedPtr<LazySequence<T>>> _indices;
    };
private:
    class ConcatGenerator : IGenerator 
    {
    public:
        ConcatGenerator() = delete;

        ConcatGenerator( const LazySequence<T>& items, SharedPtr<LazySequence<T>> parent );
        
        ~ConcatGenerator();
    public:
        IGenerator* append( const T& value ) override;
        IGenerator* prepend( const T& value ) override;
        IGenerator* insertAt( const T& value, ordinal index ) override;

        IGenerator* skip( ordinal index ) override;
        IGenerator* skip( ordinal start, ordinal end ) override;
        IGenerator* skip( SharedPtr<LazySequence<T>> indices ) override;

        IGenerator* getSubSequence( ordinal start, ordinal end ) override;
        IGenerator* concat( SharedPtr<LazySequence<T>> other ) override;
    private:
        WeakPtr<LazySequence<T>> _parent;
        SharedPtr<Sequence<T>> _parentItems;

        Option<SharedPtr<LazySequence<T>>> _items;
    };
};

#include "LazySequence.tpp"

#endif // LAZY_SEQUENCE_H