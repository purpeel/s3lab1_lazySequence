#ifndef UNIQUE_PTR
#define UNIQUE_PTR

#include "util.hpp"
#include "SharedPtr.hpp"

template <typename T>
class UniquePtr
{
public:
    UniquePtr();
private:
    UniquePtr( T* ptr );
    friend class SharedPtr<T>;
public:
    UniquePtr( const UniquePtr<T>& other );
    UniquePtr<T>& operator=( const UniquePtr<T>& other );

    UniquePtr( UniquePtr<T>&& other );
    UniquePtr<T>& operator=( UniquePtr<T>&& other );

    ~UniquePtr();
public:
    operator T*() noexcept;
    T* operator->() noexcept;
    
    T& operator*();
    const T& operator*() const;

    operator bool() const noexcept;
public:
    T* release() noexcept;

    // void reset( T* const& ptr ) noexcept; //! usage can cause UB so implementation's postponed until the end of strong consideration process

    void swap( UniquePtr<T>& other ) noexcept;
public:
    bool operator==( T* const& other ) const noexcept;
    bool operator!=( T* const& other ) const noexcept;
private:
    T* ptr;
};

#include "UniquePtr.tpp"

#endif //UNIQUE_PTR