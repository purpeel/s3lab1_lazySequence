#ifndef UNIQUE_PTR
#define UNIQUE_PTR

#include "util.hpp"
#include "SharedPtr.hpp"
#include <functional>

template <class T>
class UniquePtr
{
public:
    UniquePtr() : ptr(new T()) {}
private:
    UniquePtr( T* ptr ) : ptr(ptr) {}
    friend class SharedPtr<T>;
public:
    UniquePtr( T* (*fabricMethod)() ) : ptr( fabricMethod() ) {}
    UniquePtr( std::function<T*()>& fabricMethod ) : ptr( fabricMethod() ) {}
    // конструкторы копирования возможно не нужны
    UniquePtr( const UniquePtr<T>& other ) : ptr(new T(*other.ptr)) {}
    UniquePtr<T>& operator=( const UniquePtr<T>& other );

    UniquePtr( UniquePtr<T>&& other );
    UniquePtr<T>& operator=( UniquePtr<T>&& other );

    ~UniquePtr() {
        delete this->ptr;
    }
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
    T* release() noexcept;

    void swap( UniquePtr<T>& other ) noexcept;
public:
    bool operator==( T* const& other ) const noexcept;
    bool operator!=( T* const& other ) const noexcept;
private:
    T* ptr;
};

#include "UniquePtr.tpp"

#endif //UNIQUE_PTR