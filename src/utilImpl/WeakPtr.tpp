template <typename T>
WeakPtr<T>::WeakPtr( const WeakPtr<T>& other ) : _ptr(other._ptr) { 
    this->_controlBlock = other._controlBlock;
    this->_controlBlock->increaseWeakRefs(); 
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=( const WeakPtr<T>& other ) {
    this->_ptr = other._ptr;
    this->_controlBlock = other._controlBlock;
    this->_controlBlock->increaseWeakRefs();
    return *this;
}

template <typename T>
WeakPtr<T>::WeakPtr( const SharedPtr<T>& other ) : _ptr(other._ptr) {
    this->_controlBlock = other._controlBlock;
    this->_controlBlock->increaseWeakRefs();
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=( const SharedPtr<T>& other ) {
    this->_ptr = other._ptr;
    this->_controlBlock = other._controlBlock;
    this->_controlBlock->increaseWeakRefs();
    return *this;
}

template <typename T>
template<typename T2> requires (std::is_base_of_v<T,T2>)
WeakPtr<T>::WeakPtr( const WeakPtr<T2>& other ) : _ptr(other._ptr) { 
    this->_controlBlock = other._controlBlock;
    this->_controlBlock->increaseWeakRefs(); 
}

template <typename T>
template<typename T2> requires (std::is_base_of_v<T,T2>)
WeakPtr<T>& WeakPtr<T>::operator=( const WeakPtr<T2>& other ) {
    this->_ptr = other._ptr;
    this->_controlBlock = other._controlBlock;
    this->_controlBlock->increaseWeakRefs();
    return *this;
}

template <typename T>
template<typename T2> requires (std::is_base_of_v<T,T2>)
WeakPtr<T>::WeakPtr( const SharedPtr<T2>& other ) : _ptr(other._ptr) {
    this->_controlBlock = other._controlBlock;
    this->_controlBlock->increaseWeakRefs();
}

template <typename T>
template<typename T2> requires (std::is_base_of_v<T,T2>)
WeakPtr<T>& WeakPtr<T>::operator=( const SharedPtr<T2>& other ) {
    this->_ptr = other._ptr;
    this->_controlBlock = other._controlBlock;
    this->_controlBlock->increaseWeakRefs();
    return *this;
}

template <typename T>
WeakPtr<T>::~WeakPtr() {
    this->_controlBlock->decreaseWeakRefs();
    if ( this->_controlBlock->weakRefs() == 0 && this->_controlBlock->hardRefs() == 0 ) {
        delete this->_controlBlock;
    }
}

template <typename T>
void WeakPtr<T>::reset() noexcept {
    this->_ptr = nullptr;
    this->_controlBlock->decreaseWeakRefs();
    if (this->_controlBlock->weakRefs() == 0 && this->_controlBlock->hardRefs() == 0) {
        delete this->_controlBlock;
    }
    this->_controlBlock = new RefCount(0, 1);
}

template <typename T>
SharedPtr<T> WeakPtr<T>::lock() const noexcept {
    return SharedPtr<T>(this->_ptr, this->_controlBlock);
}

template <typename T>
void WeakPtr<T>::swap( WeakPtr<T>& other ) noexcept {
    auto *temp = this->_ptr;
    this->_ptr = other._ptr;
    other._ptr = temp;

    auto *tempCount = this->_controlBlock;
    this->_controlBlock = other._controlBlock;
    other._controlBlock = temp;
}

template <typename T>
WeakPtr<T>::operator bool() const noexcept {
    return this->_ptr;
}

template <typename T>
bool WeakPtr<T>::operator==( const WeakPtr<T>& other ) const noexcept {
    return this->_ptr == other._ptr;
}

template <typename T>
bool WeakPtr<T>::operator==( const SharedPtr<T>& other ) const noexcept {
    return this->_ptr == other._ptr;
}

template <typename T>
bool WeakPtr<T>::operator==( T* const& other ) const noexcept {
    return **this->_ptr == other;
}

template <typename T>
bool WeakPtr<T>::operator!=( const WeakPtr<T>& other ) const noexcept {
    return this->_ptr != other._ptr;
}

template <typename T>
bool WeakPtr<T>::operator!=( const SharedPtr<T>& other ) const noexcept {
    return this->_ptr != other._ptr;
}

template <typename T>
bool WeakPtr<T>::operator!=( T* const& other ) const noexcept {
    return **this->_ptr != other;
}

template <typename T>
template<typename T2> requires (std::is_base_of_v<T,T2>)
bool WeakPtr<T>::operator==( const WeakPtr<T2>& other ) const noexcept {
    return this->_ptr == other._ptr;
}

template <typename T>
template<typename T2> requires (std::is_base_of_v<T,T2>)
bool WeakPtr<T>::operator==( const SharedPtr<T2>& other ) const noexcept {
    return this->_ptr == other._ptr;
}

template <typename T>
template<typename T2> requires (std::is_base_of_v<T,T2>)
bool WeakPtr<T>::operator==( T2* const& other ) const noexcept {
    return **this->_ptr == other;
}

template <typename T>
template<typename T2> requires (std::is_base_of_v<T,T2>)
bool WeakPtr<T>::operator!=( const WeakPtr<T2>& other ) const noexcept {
    return this->_ptr != other._ptr;
}

template <typename T>
template<typename T2> requires (std::is_base_of_v<T,T2>)
bool WeakPtr<T>::operator!=( const SharedPtr<T2>& other ) const noexcept {
    return this->_ptr != other._ptr;
}

template <typename T>
template<typename T2> requires (std::is_base_of_v<T,T2>)
bool WeakPtr<T>::operator!=( T2* const& other ) const noexcept {
    return **this->_ptr != other;
}