#ifndef SHARED_PTR
#define SHARED_PTR

#include "util.hpp"
#include "WeakPtr.hpp"
#include "UniquePtr.hpp"

template <typename T>
class SharedPtr
{
public:
    SharedPtr();
private:
    SharedPtr( T* ptr );
    friend class WeakPtr<T>;
public:
    SharedPtr( SharedPtr<T>& other );
    SharedPtr<T>& operator=( SharedPtr<T>& other );

    SharedPtr( SharedPtr<T>&& other );
    SharedPtr<T>& operator=( SharedPtr<T>&& other );

    ~SharedPtr();
public:
    operator T*() noexcept;
    T* operator->() noexcept;

    T& operator*();
    const T& operator*() const;

    operator bool() const noexcept;
public:
    void reset() noexcept;
    // void reset( T* const& ptr ) noexcept; //? requires usage of this->UniquePtr.reset() which can cause UB

    void swap( SharedPtr<T>& other ) noexcept;
    bool isUnique() const noexcept;

    int getCount() const noexcept;
public:
    bool operator==( const SharedPtr<T>& other ) const noexcept;
    bool operator==( T* const& other ) const noexcept;
    bool operator!=( const SharedPtr<T>& other ) const noexcept;
    bool operator!=( T* const& other ) const noexcept;
private:
    UniquePtr<T>& ptr;

    long *refCount;

    using weakType = WeakPtr<T>;
};

#include "SharedPtr.tpp"

#endif // SHARED_PTR
