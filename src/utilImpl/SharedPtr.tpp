template <typename T>
SharedPtr<T>::SharedPtr() 
                  : ptr( *(new UniquePtr<T>()) ) 
                  , refCount( new long(1) )
                  {} 

template <typename T>
SharedPtr<T>::SharedPtr( T* ptr ) 
                  : ptr( *(new UniquePtr<T>(ptr)) ) 
                  , refCount( new long(1) )
                  {}

template <typename T>
SharedPtr<T>::SharedPtr( SharedPtr<T>& other ) : ptr( other.ptr ) { 
    *other.refCount++;
    this->refCount = other.refCount;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=( SharedPtr<T>& other ) {
    this->ptr = other.ptr;
    *other.refCount++;
    this->refCount = other.refCount;
    return *this;
}

template <typename T>
SharedPtr<T>::SharedPtr( SharedPtr<T>&& other ) {
    this->ptr = other.ptr;
    this->refCount = other.refCount;
    other.ptr = nullptr;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=( SharedPtr<T>&& other ) {
    if (*this != other) {
        this->ptr = other.ptr;
        this->refCount = other.refCount;
        other.ptr = nullptr;
    }
    return *this;
}

template <typename T>
SharedPtr<T>::~SharedPtr() {
    if (*this->refCount > 1) {
        *this->refCount -= 1;
    } else {
        delete ptr;
    }
}

template <typename T>
SharedPtr<T>::operator T*() noexcept {
    return *(this->ptr);
}

template <typename T>
T* SharedPtr<T>::operator->() noexcept {
    return *(this->ptr);
}

template <typename T>
T& SharedPtr<T>::operator*() {
    if (this->ptr == nullptr) {
        throw Exception( Exception::ErrorCode::NULL_DEREFERENCE );
    } 
    return *(this->ptr);
}

template <typename T>
const T& SharedPtr<T>::operator*() const {
    if (this->ptr == nullptr) {
        throw Exception( Exception::ErrorCode::NULL_DEREFERENCE );
    } 
    return *(this->ptr);
}

template <typename T>
SharedPtr<T>::operator bool() const noexcept {
    return this->ptr != nullptr;
}

template <typename T>
void SharedPtr<T>::reset() noexcept {
    *this->refCount--;
    if (*this->refCount == 0) {
        delete this->ptr;
    } else {
        this->ptr = *(new UniquePtr<T>(nullptr));
    }
}

// template <typename T>
// void SharedPtr<T>::reset( T* const& ptr ) noexcept {
//     *this->refCount--;
//     if (*this->refCount == 0) {
//         delete this->ptr;
//     } else {
//         this->ptr = *(new UniquePtr<T>( ptr ));
//     }
// }

template <typename T>
void SharedPtr<T>::swap( SharedPtr<T>& other ) noexcept {
    auto temp = this->ptr;
    this->ptr = other.ptr;
    other.ptr = temp;
}

template <typename T>
bool SharedPtr<T>::isUnique() const noexcept {
    return *refCount == 1;
}

template <typename T>
int SharedPtr<T>::getCount() const noexcept {
    return *this->refCount;
}

template <typename T>
bool SharedPtr<T>::operator==( const SharedPtr<T>& other ) const noexcept {
    return this->ptr == other.ptr;
}

template <typename T>
bool SharedPtr<T>::operator==( T* const& other ) const noexcept {
    return this->ptr == other;
}


template <typename T>
bool SharedPtr<T>::operator!=( const SharedPtr<T>& other ) const noexcept {
    return this->ptr != other.ptr;
}

template <typename T>
bool SharedPtr<T>::operator!=( T* const& other ) const noexcept {
    return this->ptr != other;
}
