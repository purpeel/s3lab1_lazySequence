template <typename T>
WeakPtr<T>::WeakPtr() 
              : ptr(nullptr) 
              {}

template <typename T>
WeakPtr<T>::WeakPtr( T* ptr )           
              : ptr(ptr) 
              {}

template <typename T>
WeakPtr<T>::WeakPtr( const WeakPtr<T>& other ) 
              : ptr(other.ptr) 
              {}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=( const WeakPtr<T>& other ) {
    this->ptr = other.ptr;
    return *this;
}

template <typename T>
WeakPtr<T>::WeakPtr( WeakPtr<T>&& other ) {
    this->ptr = other.ptr;
    other.ptr = nullptr;
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=( WeakPtr<T>&& other ) {
    if (*this != other ) {
        this->ptr = other.ptr;
        other.ptr = nullptr;
    }
    return *this;
}

template <typename T>
void WeakPtr<T>::reset() noexcept {
    this->ptr = nullptr;
}

template <typename T>
int WeakPtr<T>::getCount() const noexcept {
    auto res = SharedPtr<T>(this->ptr).getCount() - 1;
    return temp;
}

template <typename T>
SharedPtr<T> WeakPtr<T>::lock() const noexcept {
    return *(new SharedPtr<T>(this->ptr));
}

template <typename T>
void WeakPtr<T>::swap( WeakPtr<T>& other ) noexcept {
    auto *temp = this->ptr;
    other.ptr = temp;
    this->ptr = other.ptr;
}

template <typename T>
WeakPtr<T>::operator bool() const noexcept {
    return this->ptr != nullptr;
}

template <typename T>
bool WeakPtr<T>::operator==( const WeakPtr<T>& other ) const noexcept {
    return this->ptr == other.ptr;
}

template <typename T>
bool WeakPtr<T>::operator==( T* const& other ) const noexcept {
    return this->ptr == other;
}

template <typename T>
bool WeakPtr<T>::operator!=( const WeakPtr<T>& other ) const noexcept {
    return this->ptr != other.ptr;
}

template <typename T>
bool WeakPtr<T>::operator!=( T* const& other ) const noexcept {
    return this->ptr != other;
}