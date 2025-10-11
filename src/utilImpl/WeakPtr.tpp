template <typename T>
WeakPtr<T>::WeakPtr() 
              : ptr( *(new UniquePtr<T>() )) 
              , controlBlock( new refCount(0, 1) )
              {}

template <typename T>
WeakPtr<T>::WeakPtr( T* (*FabricMethod)() )           
              : ptr( *(new UniquePtr<T>(FabricMethod)) )
              , controlBlock( new refCount(0, 1 ) )
              {}

template <typename T>
WeakPtr<T>::WeakPtr( const WeakPtr<T>& other ) : ptr(other.ptr) { 
    this->controlBlock = other.controlBlock;
    this->controlBlock->increaseWeakRefs(); 
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=( const WeakPtr<T>& other ) {
    this->ptr = other.ptr;
    this->controlBlock = other.controlBlock;
    this->controlBlock->increaseWeakRefs();
    return *this;
}

template <typename T>
WeakPtr<T>::WeakPtr( const SharedPtr<T>& other ) : ptr(other.ptr) {
    this->controlBlock = other.controlBlock;
    this->controlBlock->increaseWeakRefs();
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=( const SharedPtr<T>& other ) {
    this->ptr = other.ptr;
    this->controlBlock = other.controlBlock;
    this->controlBlock->increaseWeakRefs();
    return *this;
}

template <typename T>
WeakPtr<T>::WeakPtr( WeakPtr<T>&& other ) {
    this->ptr = other.ptr;
    this->controlBlock = other.controlBlock;
    //? consider deleting other
    other.controlBlock = new refCount(0, 0);
    other.ptr = nullptr;
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=( WeakPtr<T>&& other ) {
    if (*this != other ) {
        this->ptr = other.ptr;
        this->controlBlock = other.controlBlock;
        other.controlBlock = new refCount(0, 0);
        other.ptr = *(new UniquePtr<T>());
    }
    return *this;
}

template <typename T>
WeakPtr<T>::~WeakPtr() {
    this->controlBlock->decreaseWeakRefs();
    if ( this->controlBlock->weakRefs() == 0 && this->controlBlock->hardRefs() == 0 ) {
        delete this->controlBlock;
    }
}

template <typename T>
void WeakPtr<T>::reset() noexcept {
    this->ptr = nullptr;
    this->controlBlock->decreaseWeakRefs();
    this->controlBlock = new refCount(0, 0);
}

template <typename T>
SharedPtr<T> WeakPtr<T>::lock() const noexcept {
    return *(new SharedPtr<T>(this->ptr, this->controlBlock));
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
bool WeakPtr<T>::operator==( const SharedPtr<T>& other ) const noexcept {
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
bool WeakPtr<T>::operator!=( const SharedPtr<T>& other ) const noexcept {
    return this->ptr != other.ptr;
}

template <typename T>
bool WeakPtr<T>::operator!=( T* const& other ) const noexcept {
    return this->ptr != other;
}