template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=( UniquePtr<T>&& other ) {
    if (_controlBlock) {
        _controlBlock->decreaseHardRefs();
        if (!_controlBlock->hasHardRefs()) { delete _ptr; }
        if (!_controlBlock->hasWeakRefs() && !_controlBlock->hasHardRefs) { delete _controlBlock; }
    }
    _ptr = other.release();
    _controlBlock = new RefCount(1, 0);
    hookSharedToThis(_ptr);
    return *this;    
}

template <typename T>
SharedPtr<T>::SharedPtr( const SharedPtr<T>& other ) : _ptr( other._ptr ) { 
    _controlBlock = other._controlBlock;
    _controlBlock->increaseHardRefs();
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=( const SharedPtr<T>& other ) {
    if (this != &other) {
        if (_controlBlock) {
            _controlBlock->decreaseHardRefs();
            if (!_controlBlock->hasHardRefs()) { delete _ptr; }
            if (!_controlBlock->hasWeakRefs() && !_controlBlock->hasHardRefs) { delete _controlBlock; }
        }
        _ptr = other._ptr;
        _controlBlock = other._controlBlock;
        _controlBlock->increaseHardRefs();
    }
    return *this;
}

template <typename T>
SharedPtr<T>::SharedPtr( SharedPtr<T>&& other ) {
    _ptr = other._ptr;
    _controlBlock = other._controlBlock;
    other._ptr = nullptr;
    other._controlBlock = nullptr;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=( SharedPtr<T>&& other ) {
    if (this != &other) {
        if (_controlBlock) {
            _controlBlock->decreaseHardRefs();
            if (!_controlBlock->hasHardRefs()) { delete _ptr; }
            if (!_controlBlock->hasWeakRefs() && !_controlBlock->hasHardRefs) { delete _controlBlock; }
        }
        _ptr = other._ptr;
        _controlBlock = other._controlBlock;
        other._ptr = nullptr;
        other._controlBlock = nullptr;
    }
    return *this;
}


template <typename T>
SharedPtr<T>::SharedPtr( const WeakPtr<T>& other ) : _ptr( other._ptr ) { 
    _controlBlock = other._controlBlock;
    _controlBlock->increaseHardRefs();
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=( const WeakPtr<T>& other ) {
    if (this != &other) {
        if (_controlBlock) {
            _controlBlock->decreaseHardRefs();
            if (!_controlBlock->hasHardRefs()) { delete _ptr; }
            if (!_controlBlock->hasWeakRefs() && !_controlBlock->hasHardRefs) { delete _controlBlock; }
        }
        _ptr = other._ptr;
        _controlBlock = other._controlBlock;
        _controlBlock->increaseHardRefs();
    }
    return *this;
}

template <typename T>
SharedPtr<T>::SharedPtr( WeakPtr<T>&& other ) {
    _ptr = other._ptr;
    _controlBlock = other._controlBlock;
    other._ptr = nullptr;
    other._controlBlock = nullptr;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=( WeakPtr<T>&& other ) {
    if (this != &other) {
        if (_controlBlock) {
            _controlBlock->decreaseHardRefs();
            if (!_controlBlock->hasHardRefs()) { delete _ptr; }
            if (!_controlBlock->hasWeakRefs() && !_controlBlock->hasHardRefs) { delete _controlBlock; }
        }
        _ptr = other._ptr;
        _controlBlock = other._controlBlock;
        other._ptr = nullptr;
        other._controlBlock = nullptr;
    }
    return *this;
}

template <typename T>
SharedPtr<T>::~SharedPtr() {
    if (_controlBlock) {
        _controlBlock->decreaseHardRefs();
        if (_controlBlock->hardRefs() == 0) {
            delete _ptr;
            if (!(_controlBlock->hasWeakRefs())) {
                delete _controlBlock;
            }
        }
    }
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
SharedPtr<T>& SharedPtr<T>::operator=( UniquePtr<T2>&& other ) {
    _ptr = other.release;
    _controlBlock = new RefCount(1, 0);
    hookSharedToThis(_ptr);

    return *this;
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
SharedPtr<T>& SharedPtr<T>::operator=( const SharedPtr<T2>& other ) {
    if (this != &other) {
        if (_controlBlock) {
            _controlBlock->decreaseHardRefs();
            if (!_controlBlock->hasHardRefs()) { delete _ptr; }
            if (!_controlBlock->hasWeakRefs() && !_controlBlock->hasHardRefs) { delete _controlBlock; }
        }
        _ptr = other._ptr;
        _controlBlock = other._controlBlock;
        _controlBlock->increaseHardRefs();
    }
    return *this;
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
SharedPtr<T>::SharedPtr( SharedPtr<T2>&& other ) {
    _ptr = other._ptr;
    _controlBlock = other._controlBlock;
    other._ptr = nullptr;
    other._controlBlock = nullptr;
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
SharedPtr<T>& SharedPtr<T>::operator=( SharedPtr<T2>&& other ) {
    if (static_cast<void*>(this) != static_cast<void*>(&other)) {
        if (_controlBlock) {
            _controlBlock->decreaseHardRefs();
            if (!_controlBlock->hasHardRefs()) { delete _ptr; }
            if (!_controlBlock->hasWeakRefs() && !_controlBlock->hasHardRefs) { delete _controlBlock; }
        }
        _ptr = other._ptr;
        _controlBlock = other._controlBlock;
        other._ptr = nullptr;
        other._controlBlock = nullptr;
    }
    return *this;
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
SharedPtr<T>::SharedPtr( const WeakPtr<T2>& other ) : _ptr(other._ptr) {
    _controlBlock = other._controlBlock;
    _controlBlock->increaseHardRefs();
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
SharedPtr<T>& SharedPtr<T>::operator=( const WeakPtr<T2>& other ) {
    if (this != &other) {
        if (_controlBlock) {
            _controlBlock->decreaseHardRefs();
            if (!_controlBlock->hasHardRefs()) { delete _ptr; }
            if (!_controlBlock->hasWeakRefs() && !_controlBlock->hasHardRefs) { delete _controlBlock; }
        }
        _ptr = other._ptr;
        _controlBlock = other._controlBlock;
        _controlBlock->increaseHardRefs();
    }
    return *this;
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
SharedPtr<T>::SharedPtr( WeakPtr<T2>&& other ) {
    _ptr = other._ptr;
    _controlBlock = other._controlBlock;
    other._ptr = nullptr;
    other._controlBlock = nullptr;
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
SharedPtr<T>& SharedPtr<T>::operator=( WeakPtr<T2>&& other ) {
    if (static_cast<void*>(this) != static_cast<void*>(&other)) {
        if (_controlBlock) {
            _controlBlock->decreaseHardRefs();
            if (!_controlBlock->hasHardRefs()) { delete _ptr; }
            if (!_controlBlock->hasWeakRefs() && !_controlBlock->hasHardRefs) { delete _controlBlock; }
        }
        _ptr = other._ptr;
        _controlBlock = other._controlBlock;
        other._ptr = nullptr;
        other._controlBlock = nullptr;
    }
    return *this;
}

template <typename T>
T& SharedPtr<T>::operator*() {
    if (!_ptr) {
        throw Exception( Exception::ErrorCode::NULL_DEREFERENCE );
    } 
    return *_ptr;
}

template <typename T>
const T& SharedPtr<T>::operator*() const {
    if (!_ptr) {
        throw Exception( Exception::ErrorCode::NULL_DEREFERENCE );
    } 
    return *_ptr;
}

template <typename T>
SharedPtr<T>::operator bool() const noexcept {
    return _ptr != nullptr;
}

template <typename T>
void SharedPtr<T>::reset() noexcept {
    _controlBlock->decreaseHardRefs();
    if (!_controlBlock->hasHardRefs()) {
        delete _ptr;
    } else {
        _ptr = nullptr;
        _controlBlock = nullptr;
    }
}

template <typename T>
void SharedPtr<T>::swap( SharedPtr<T>& other ) noexcept {
    auto temp1 = _ptr;
    _ptr = other._ptr;
    other._ptr = temp1;

    auto temp2 = _controlBlock;
    _controlBlock = other._controlBlock;
    other._controlBlock = temp2;
}

template <typename T>
bool SharedPtr<T>::operator==( const SharedPtr<T>& other ) const noexcept {
    return _ptr == other._ptr;
}

template <typename T>
bool SharedPtr<T>::operator==( T* const& other ) const noexcept {
    return _ptr == other;
}


template <typename T>
bool SharedPtr<T>::operator!=( const SharedPtr<T>& other ) const noexcept {
    return _ptr != other._ptr;
}

template <typename T>
bool SharedPtr<T>::operator!=( T* const& other ) const noexcept {
    return _ptr != other;
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
bool SharedPtr<T>::operator==( const SharedPtr<T2>& other ) const noexcept {
    return _ptr == other._ptr;
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
bool SharedPtr<T>::operator==( T2* const& other ) const noexcept {
    return _ptr == other;
}


template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
bool SharedPtr<T>::operator!=( const SharedPtr<T2>& other ) const noexcept {
    return _ptr != other._ptr;
}

template <typename T>
template <typename T2> requires (std::is_base_of_v<T,T2>)
bool SharedPtr<T>::operator!=( T2* const& other ) const noexcept {
    return _ptr != other;
}