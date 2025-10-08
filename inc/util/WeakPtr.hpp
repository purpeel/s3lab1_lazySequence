#ifndef WEAK_PTR
#define WEAK_PTR

#include "util.hpp"
#include "SharedPtr.hpp"

template <typename T>
class WeakPtr 
{
public:
    WeakPtr();
    WeakPtr( T* ptr );

    WeakPtr( const WeakPtr<T>& other );
    WeakPtr& operator=( const WeakPtr<T>& other );

    WeakPtr( WeakPtr<T>&& other );
    WeakPtr& operator=( WeakPtr<T>&& other );

    ~WeakPtr() {}
public:
    void reset() noexcept;

    int getCount() const noexcept;
    SharedPtr<T> lock() const noexcept;

    void swap( WeakPtr<T>& other ) noexcept;

    operator bool() const noexcept;
public:
    bool operator==( const WeakPtr<T>& other ) const noexcept;
    bool operator==( T* const& other ) const noexcept;
    bool operator!=( const WeakPtr<T>& other ) const noexcept;
    bool operator!=( T* const& other ) const noexcept;
private:
    T* ptr;
};


#endif //WEAK_PTR
