template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=( const UniquePtr<T>& other ) {
    this->ptr = other;
    this->controlBlock = new refCount(1, 0);
    return *this;
}

template <typename T>
SharedPtr<T>::SharedPtr( const SharedPtr<T>& other ) : ptr( other.ptr ) { 
    this->controlBlock = other.controlBlock;
    this->controlBlock->increaseHardRefs();
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=( const SharedPtr<T>& other ) {
    this->ptr = other.ptr;
    this->controlBlock = other.controlBlock;
    this->controlBlock->increaseHardRefs();
    return *this;
}

template <typename T>
SharedPtr<T>::SharedPtr( SharedPtr<T>&& other ) {
    this->ptr = other.ptr;
    this->controlBlock = other.controlBlock;
    other.ptr = nullptr;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=( SharedPtr<T>&& other ) {
    if (*this != other) {
        this->ptr = other.ptr;
        this->controlBlock = other.controlBlock;
        other.ptr = nullptr;
    }
    return *this;
}

template <typename T>
SharedPtr<T>::~SharedPtr() {
    if (this->controlBlock->hardRefs() > 1) {
        this->controlBlock->decreaseHardRefs();
    } else {
        delete &this->ptr;
        if (this->controlBlock->weakRefs() == 0 ) {
            delete this->controlBlock;
        }
    }
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
    this->controlBlock->decreaseHardRefs();
    if (this->controlBlock->hardRefs() == 0) {
        delete &this->ptr;
    } else {
        this->ptr = *(new UniquePtr<T>(nullptr));
        this->refCount = new refCount(1, 0);
    }
}

// template <typename T>
// void SharedPtr<T>::reset( T* const& ptr ) noexcept {
//     (*this->refCount)--;
//     if (*this->refCount == 0) {
//         delete &this->ptr;
//     } else {
//         this->ptr = *(new UniquePtr<T>( ptr ));
//         this->refCount = new long(1);
//     }
// }

template <typename T>
void SharedPtr<T>::swap( SharedPtr<T>& other ) noexcept {
    auto temp = this->ptr;
    this->ptr = other.ptr;
    other.ptr = temp;
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
