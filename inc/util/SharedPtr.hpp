#ifndef SHARED_PTR
#define SHARED_PTR

#include "util.hpp"

template <typename T>
class UniquePtr;
template <typename T>
class WeakPtr;

struct RefCount
{
public:
    RefCount() : _hardRefs( 0 ), _weakRefs( 0 ) {}
    RefCount( const long& hardRefs, const long& weakRefs ) 
    : _hardRefs( hardRefs ), _weakRefs( weakRefs ) {}
    ~RefCount() {}
public:
    long hardRefs() { return this->_hardRefs; }
    long weakRefs() { return this->_weakRefs; }

    bool hasHardRefs() { return this->_hardRefs != 0; }
    bool hasWeakRefs() { return this->_weakRefs != 0; }

    void increaseHardRefs() { this->_hardRefs++; }
    void increaseWeakRefs() { this->_weakRefs++; }
    
    void decreaseHardRefs() { this->_hardRefs--; }
    void decreaseWeakRefs() { this->_weakRefs--; }
private:
    long _hardRefs;
    long _weakRefs;
};

template <class T>
class SharedPtr
{
private:
    SharedPtr( T* ptr ) : _ptr( ptr ), _controlBlock( new RefCount(1, 0) ) {}
    SharedPtr( T* ptr, RefCount* count ) : _ptr( ptr ), _controlBlock( count ) {
        this->_controlBlock->increaseHardRefs();
    }

    template<typename T2> requires (std::is_base_of_v<T,T2>)
    SharedPtr( T2* ptr ) : _ptr( ptr ) {}

    SharedPtr( UniquePtr<T>& other ) : _ptr(other._ptr), _controlBlock( new RefCount(1, 0) ) {}
    SharedPtr( UniquePtr<T>& other, RefCount* count ) : _ptr(other._ptr), _controlBlock( count ) { 
        this->_controlBlock->increaseHardRefs();
    }

    template<typename T2> requires (std::is_base_of_v<T,T2>)
    SharedPtr( UniquePtr<T2>& other ) : _ptr(other._ptr), _controlBlock( new RefCount(1, 0) ) {}
    template<typename T2> requires (std::is_base_of_v<T,T2>)
    SharedPtr( UniquePtr<T2>& other, RefCount* count ) : _ptr(other._ptr), _controlBlock( count ) { 
        this->_controlBlock->increaseHardRefs();
    }
    
    friend class WeakPtr<T>;
    template <typename T2> 
    friend class SharedPtr;
    template <typename T2>
    friend class WeakPtr;
public:
    SharedPtr() requires(!std::is_abstract_v<T>) : _ptr(new T()) , _controlBlock( new RefCount(1, 0) ) {} 
    
    SharedPtr() requires(std::is_abstract_v<T>)  = delete;

    template <typename ... Ts>
    SharedPtr<T> makeShared(Ts&& ... args ) requires(!std::is_abstract_v<T>) {
        return SharedPtr<T>( new T( std::forward(args)...));
    }

    template <typename ... Ts>
    SharedPtr<T> makeShared(Ts&& ... args ) requires(std::is_abstract_v<T>) = delete;

    SharedPtr( UniquePtr<T>&& other, RefCount* count ) : _ptr(other.release()), _controlBlock( count ) {}
    SharedPtr( UniquePtr<T>&& other ) : _ptr(other.release()), _controlBlock( new RefCount(1, 0)) {}
    SharedPtr<T>& operator=( UniquePtr<T>&& other );

    SharedPtr( const SharedPtr<T>& other );
    SharedPtr<T>& operator=( const SharedPtr<T>& other );

    SharedPtr( SharedPtr<T>&& other );
    SharedPtr<T>& operator=( SharedPtr<T>&& other );

    ~SharedPtr();
public:
    template <typename T2> requires (std::is_base_of_v<T,T2>)
    SharedPtr( UniquePtr<T2>&& other, RefCount* count ) : _ptr(other.release()), _controlBlock( count ) {}

    template <typename T2> requires (std::is_base_of_v<T,T2>)
    SharedPtr( UniquePtr<T2>&& other ) : _ptr(other.release()), _controlBlock( new RefCount(1, 0)) {}
    template <typename T2> requires (std::is_base_of_v<T,T2>)
    SharedPtr<T>& operator=( UniquePtr<T2>&& other );

    template <typename T2> requires (std::is_base_of_v<T,T2>)
    SharedPtr( const SharedPtr<T2>& other ) : _ptr( other._ptr ), _controlBlock(other._controlBlock) {}
    template <typename T2> requires (std::is_base_of_v<T,T2>)
    SharedPtr<T>& operator=( const SharedPtr<T2>& other );

    template <typename T2> requires (std::is_base_of_v<T,T2>)
    SharedPtr( SharedPtr<T2>&& other );
    template <typename T2> requires (std::is_base_of_v<T,T2>)
    SharedPtr<T>& operator=( SharedPtr<T2>&& other );
public:
    operator T*() noexcept {
        return this->_ptr;
    }
    T* operator->() noexcept {
        return this->_ptr;
    }
    const T* operator->() const noexcept {
        return this->_ptr;
    }

    T& operator*();
    const T& operator*() const;

    operator bool() const noexcept;
    bool isUnique() {
        return this->_controlBlock->hardRefs() == 1;
    }
public:
    void reset() noexcept;

    void swap( SharedPtr<T>& other ) noexcept;
    bool isUnique() const noexcept {
        return this->_controlBlock->hardRefs() == 1;
    }

    int getCount() const noexcept {
        return this->_controlBlock->hardRefs();
    }
public:
    bool operator==( const SharedPtr<T>& other ) const noexcept;
    bool operator==( T* const& other ) const noexcept;
    bool operator!=( const SharedPtr<T>& other ) const noexcept;
    bool operator!=( T* const& other ) const noexcept;

    template <typename T2> requires (std::is_base_of_v<T,T2>)
    bool operator==( const SharedPtr<T2>& other ) const noexcept;
    template <typename T2> requires (std::is_base_of_v<T,T2>)
    bool operator==( T2* const& other ) const noexcept;
    template <typename T2> requires (std::is_base_of_v<T,T2>)
    bool operator!=( const SharedPtr<T2>& other ) const noexcept;
    template <typename T2> requires (std::is_base_of_v<T,T2>)
    bool operator!=( T2* const& other ) const noexcept;
private:
    T* _ptr;

    RefCount* _controlBlock;

    using weakType = WeakPtr<T>;
};

#include "SharedPtr.tpp"

#endif // SHARED_PTR
