#ifndef SHARED_PTR
#define SHARED_PTR

#include "util.hpp"

template <class T>
class UniquePtr;
template <class T>
class WeakPtr;

struct refCount
{
public:
    refCount() : hardRefs_( 0 ), weakRefs_( 0 ) {}
    refCount( long hardRefs, long weakRefs ) : hardRefs_( hardRefs ), weakRefs_( weakRefs ) {}
    ~refCount() = default;
public:
    long hardRefs() { return this->hardRefs_; }
    long weakRefs() { return this->weakRefs_; }

    void increaseHardRefs() { this->hardRefs_++; }
    void increaseWeakRefs() { this->weakRefs_++; }
    
    void decreaseHardRefs() { this->hardRefs_--; }
    void decreaseWeakRefs() { this->weakRefs_--; }
private:
    long hardRefs_;
    long weakRefs_;
};

template <class T>
class SharedPtr
{
public:
    SharedPtr() : ptr(*(new UniquePtr<T>)) , controlBlock( new refCount(1, 0) ) {} 
private:
    SharedPtr( T* ptr ) : ptr(*(new UniquePtr<T>(ptr))), controlBlock( new refCount(1, 0) ) {}
    friend class WeakPtr<T>;
public:
    SharedPtr( UniquePtr<T>& other ) : ptr(other), controlBlock( new refCount(1, 0) ) {}
    SharedPtr( UniquePtr<T>& other, refCount* count ) : ptr(other), controlBlock(count) {}
    
    SharedPtr<T>& operator=( const UniquePtr<T>& other );

    SharedPtr( const SharedPtr<T>& other );
    SharedPtr<T>& operator=( const SharedPtr<T>& other );

    SharedPtr( SharedPtr<T>&& other );
    SharedPtr<T>& operator=( SharedPtr<T>&& other );

    ~SharedPtr();
public:
    operator T*() noexcept {
        return this->ptr;
    }
    T* operator->() noexcept {
        return this->ptr;
    }
    const T* operator->() const noexcept {
        return this->ptr;
    }

    T& operator*();
    const T& operator*() const;

    operator bool() const noexcept;
public:
    void reset() noexcept;
    // void reset( T* const& ptr ) noexcept; //? requires usage of this->UniquePtr.reset() which can cause UB

    void swap( SharedPtr<T>& other ) noexcept;
    bool isUnique() const noexcept {
        return this->controlBlock->hardRefs() == 1;
    }

    int getCount() const noexcept {
        return this->controlBlock->hardRefs();
    }
public:
    bool operator==( const SharedPtr<T>& other ) const noexcept;
    bool operator==( T* const& other ) const noexcept;
    bool operator!=( const SharedPtr<T>& other ) const noexcept;
    bool operator!=( T* const& other ) const noexcept;
private:
    UniquePtr<T>& ptr;

    refCount* controlBlock;

    using weakType = WeakPtr<T>;
};

#include "SharedPtr.tpp"

#endif // SHARED_PTR
