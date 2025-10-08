#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include "util.hpp"

template <typename T> 
class DynamicArray 
{
public:
    DynamicArray();
    DynamicArray( const int capacity );

    DynamicArray( const DynamicArray<T>& src );
    DynamicArray<T>& operator=( const DynamicArray<T>& src );
    
    DynamicArray( DynamicArray<T>&& src);
    DynamicArray<T>& operator=( DynamicArray<T>&& src );
    
    virtual ~DynamicArray();
    virtual void clear();
public:
    void append( const T& value );
    void prepend( const T& value );
    void setAt( const T& value, const int pos );
    void insertAt( const T& value, int pos );
    void removeAt( const int pos );
    void swap( const int pos1, const int pos2 );
    DynamicArray<T>* subArray( const int startIndex, const int endIndex ) const;
    DynamicArray<T>* concat( const DynamicArray<T>& other );
private:
    void extend( const int sizeDiff );
    void shrink( const int sizeDiff );
public:
    T& operator[]( const int pos );
    const T& operator[]( const int pos ) const;
public:
    int getSize() const;
    bool isEmpty() const;
public:
    DynamicArray<T>* appendImmutable( const T& value ) const;
    DynamicArray<T>* prependImmutable( const T& value ) const;
    DynamicArray<T>* insertAtImmutable( const T& value, const int pos ) const;
    DynamicArray<T>* removeAtImmutable( const int pos ) const;
    DynamicArray<T>* setAtImmutable( const T& value, const int pos ) const;
    DynamicArray<T>* swapImmutable( const int pos1, const int pos2 ) const;
    DynamicArray<T>* concatImmutable( const DynamicArray<T>& other ) const;
public:
    const std::string print() const;
private:
    int getCapacity() const;
    T* allocBegin;
    T* data;
    T* allocEnd;
    int size;
    int capacity;
};


#include "../../src/tmpl/DynamicArray.tpp"
#endif // DYNAMIC_ARRAY_H