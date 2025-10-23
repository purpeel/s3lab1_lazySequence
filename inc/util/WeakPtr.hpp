#ifndef WEAK_PTR
#define WEAK_PTR

#include "util.hpp"

template <class T>
class SharedPtr;
template <class T>
class UniquePtr;
struct refCount; // check SharedPtr.hpp

template <class T>
class WeakPtr 
{
public:
    WeakPtr();
    WeakPtr( T* (*FabricMethod)() );

    WeakPtr( const WeakPtr<T>& other );
    WeakPtr& operator=( const WeakPtr<T>& other );

    WeakPtr( const SharedPtr<T>& other );
    WeakPtr<T>& operator=( const SharedPtr<T>& other );

    WeakPtr( WeakPtr<T>&& other );
    WeakPtr& operator=( WeakPtr<T>&& other );

    ~WeakPtr();
public:
    void reset() noexcept;

    long getSharedCount() const noexcept {
        return this->controlBlock->hardRefs();
    }
    long getWeakCount() const noexcept {
        return this->controlBlock->weakRefs();
    }
    SharedPtr<T> lock() const noexcept;

    void swap( WeakPtr<T>& other ) noexcept;

    operator bool() const noexcept;
public:
    bool operator==( const WeakPtr<T>& other ) const noexcept;
    bool operator==( const SharedPtr<T>& other ) const noexcept;
    bool operator==( T* const& other ) const noexcept;
    bool operator!=( const WeakPtr<T>& other ) const noexcept;
    bool operator!=( const SharedPtr<T>& other ) const noexcept;
    bool operator!=( T* const& other ) const noexcept;
private:
    UniquePtr<T>& ptr;

    refCount* controlBlock;
};

#include "WeakPtr.tpp"

#endif //WEAK_PTR
