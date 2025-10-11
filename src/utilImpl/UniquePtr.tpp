//можно использовать команды препроцессора дл€ защиты от повторной компил€ции тпп
template <typename T>
UniquePtr<T>& UniquePtr<T>::operator=( const UniquePtr<T>& other ) {
    if (*this != other) {
        delete this->ptr;
        this->ptr = new T(*other.ptr);
    }
    return *this;
}

template <typename T>
UniquePtr<T>::UniquePtr( UniquePtr<T>&& other ) {
    this->ptr = other.ptr;
    other.ptr = nullptr;
}

template <typename T>
UniquePtr<T>& UniquePtr<T>::operator=( UniquePtr<T>&& other ) {
    if (*this != other ) {
        delete this->ptr;
        this->ptr = other.ptr;
        other.ptr = nullptr;
    }
    return *this;
}

template <typename T>
T& UniquePtr<T>::operator*() {
    if (this->ptr == nullptr) {
        throw Exception( Exception::ErrorCode::NULL_DEREFERENCE );
    }
    return *this->ptr;
}

template <typename T>
const T& UniquePtr<T>::operator*() const {
    if (this->ptr == nullptr) {
        throw Exception( Exception::ErrorCode::NULL_DEREFERENCE );
    }
    return *this->ptr;
}

template <typename T>
UniquePtr<T>::operator bool() const noexcept {
    return this->ptr != nullptr;
}

template <typename T>
T* UniquePtr<T>::release() noexcept {
    auto *res = this->ptr;
    this->ptr = nullptr;
    return res;
}

template <typename T>
void UniquePtr<T>::swap( UniquePtr<T>& other ) noexcept {
    auto *tmp = this->ptr;
    this->ptr = other.ptr;
    other.ptr = tmp;
}

template <typename T>
bool UniquePtr<T>::operator==( T* const& other ) const noexcept {
    return this->ptr == other;
}

template <typename T>
bool UniquePtr<T>::operator!=( T* const& other ) const noexcept {
    return this->ptr != other;
}