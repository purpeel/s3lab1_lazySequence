template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=( UniquePtr<T>&& other ) {
    this->_ptr = other.release();
    this._controlBlock = new RefCount(1, 0);
    return *this;    
}

template <typename T>
SharedPtr<T>::SharedPtr( const SharedPtr<T>& other ) : _ptr( other._ptr ) { 
    this->_controlBlock = other._controlBlock;
    this->_controlBlock->increaseHardRefs();
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=( const SharedPtr<T>& other ) {
    if (this != &other) {
        this->_ptr = other._ptr;
        this->_controlBlock = other._controlBlock;
        this->_controlBlock->increaseHardRefs();
    }
    return *this;
}

template <typename T>
SharedPtr<T>::SharedPtr( SharedPtr<T>&& other ) {
    this->_ptr = other._ptr;
    this->_controlBlock = other._controlBlock;
    other._ptr = new T();
    other._controlBlock = new RefCount(1, 0);
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=( SharedPtr<T>&& other ) {
    if (this != &other) {
        this->_ptr = other._ptr;
        this->_controlBlock = other._controlBlock;
        other._ptr = new T();
        other._controlBlock = new RefCount(1, 0);
    }
    return *this;
}

template <typename T>
SharedPtr<T>::~SharedPtr() {
    if (this->_controlBlock->hardRefs() > 1) {
        this->_controlBlock->decreaseHardRefs();
    } else {
        delete this->_ptr;
        this->_controlBlock->decreaseHardRefs();
        if (!(this->_controlBlock->hasWeakRefs())) {
            delete this->_controlBlock;
        }
    }
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
SharedPtr<T>& SharedPtr<T>::operator=( UniquePtr<T2>&& other ) {
    *this->_ptr = std::move(other);
    this._controlBlock = new RefCount(1, 0);

    return *this;
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
SharedPtr<T>& SharedPtr<T>::operator=( const SharedPtr<T2>& other ) {
    if (this != &other) {
        this->_ptr = other._ptr;
        this->_controlBlock = other._controlBlock;
        this->_controlBlock->increaseHardRefs();
    }
    return *this;
}


template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
SharedPtr<T>::SharedPtr( SharedPtr<T2>&& other ) {
    this->_ptr = other._ptr;
    this->_controlBlock = other._controlBlock;
    other._ptr = new T2();
    other._controlBlock = new RefCount(1, 0);
}


template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
SharedPtr<T>& SharedPtr<T>::operator=( SharedPtr<T2>&& other ) {
    if (static_cast<void*>(this) != static_cast<void*>(&other)) {
        this->_ptr = other._ptr;
        this->_controlBlock = other._controlBlock;
        other._ptr = new T2();
        other._controlBlock = new RefCount(1, 0);
    }
    return *this;
}

template <typename T>
T& SharedPtr<T>::operator*() {
    if (!this->_ptr) {
        throw Exception( Exception::ErrorCode::NULL_DEREFERENCE );
    } 
    return *this->_ptr;
}

template <typename T>
const T& SharedPtr<T>::operator*() const {
    if (!this->_ptr) {
        throw Exception( Exception::ErrorCode::NULL_DEREFERENCE );
    } 
    return *this->_ptr;
}

template <typename T>
SharedPtr<T>::operator bool() const noexcept {
    return this->_ptr != nullptr;
}

template <typename T>
void SharedPtr<T>::reset() noexcept {
    this->_controlBlock->decreaseHardRefs();
    if (!this->_controlBlock->hasHardRefs()) {
        delete this->_ptr;
    } else {
        this->_ptr = nullptr;
        this->_controlBlock = new RefCount(1, 0);
    }
}

template <typename T>
void SharedPtr<T>::swap( SharedPtr<T>& other ) noexcept {
    auto temp = this->_ptr;
    this->_ptr = other._ptr;
    other._ptr = temp;
}

template <typename T>
bool SharedPtr<T>::operator==( const SharedPtr<T>& other ) const noexcept {
    return *this->_ptr == *other._ptr;
}

template <typename T>
bool SharedPtr<T>::operator==( T* const& other ) const noexcept {
    return *this->_ptr == *other;
}


template <typename T>
bool SharedPtr<T>::operator!=( const SharedPtr<T>& other ) const noexcept {
    return *this->_ptr != *other._ptr;
}

template <typename T>
bool SharedPtr<T>::operator!=( T* const& other ) const noexcept {
    return *this->_ptr != *other;
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
bool SharedPtr<T>::operator==( const SharedPtr<T2>& other ) const noexcept {
    return *this->_ptr == *other._ptr;
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
bool SharedPtr<T>::operator==( T2* const& other ) const noexcept {
    return *this->_ptr == *other;
}


template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
bool SharedPtr<T>::operator!=( const SharedPtr<T2>& other ) const noexcept {
    return *this->_ptr != *other._ptr;
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
bool SharedPtr<T>::operator!=( T2* const& other ) const noexcept {
    return *this->_ptr != *other;
}