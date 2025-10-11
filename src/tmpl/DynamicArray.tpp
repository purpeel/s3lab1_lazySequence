#include <cstdlib>

template <typename T>
DynamicArray<T>::DynamicArray() {
    try {
        this->size = 0;
        this->capacity = 2;
        this->allocBegin = new T[this->capacity];
        this->data = this->allocBegin + 1;
        this->allocEnd = this->allocBegin + this->capacity;
    } catch ( std::bad_alloc &ex ) {
        throw Exception(ex);
    }
}

template <typename T>
DynamicArray<T>::DynamicArray( const int capacity ) {
    try {
        this->size = 0;
        this->capacity = capacity;
        this->allocBegin = new T[this->capacity];
        this->data = this->allocBegin + (this->capacity / 4 + 1);
        this->allocEnd = this->allocBegin + this->capacity;
    } catch ( std::bad_alloc &ex ) {
        throw Exception(ex);
    }
}

template <typename T>
DynamicArray<T>::DynamicArray( const DynamicArray<T>& src ) {
    try {
        this->size = src.size;
        this->capacity = src.capacity;
        this->allocBegin = new T[this->capacity];
        this->data = this->allocBegin + (src.data - src.allocBegin);
        this->allocEnd = this->allocBegin + this->capacity;

        for ( int index = 0; index < this->size; index++ ) {
            (*this)[index] = src[index];
        }
    } catch ( std::bad_alloc &ex ) {
        throw Exception(ex);
    }
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator=( const DynamicArray<T>& src ) {
    try {
        if ( this != &src ) {
            delete[] this->allocBegin;

            this->size = src.size;
            this->capacity = src.capacity;
            this->allocBegin = new T[this->capacity];
            this->data = this->allocBegin + (src.data - src.allocBegin);
            this->allocEnd = this->allocBegin + this->capacity;
            for ( int index = 0; index < this->size; index++ ) {
                (*this)[index] = src[index];
            }
        }
    } catch ( std::bad_alloc &ex ) {
        throw Exception(ex);
    }
    return *this;
}

template <typename T>
DynamicArray<T>::DynamicArray( DynamicArray<T>&& src ) {
    try {
        this->size = src.size;
        this->capacity = src.capacity;
        this->allocBegin = src.allocBegin;
        this->data = src.data;
        this->allocEnd = src.allocEnd;

        src.size = 0;
        src.capacity = 2;
        src.allocBegin = new T[src.capacity];
        src.data = src.allocBegin + 1;
        src.allocEnd = src.allocBegin + src.capacity;
    } catch ( std::bad_alloc &ex ) {
        throw Exception(ex);
    }
}

template <typename T>
DynamicArray<T>& DynamicArray<T>::operator=( DynamicArray<T>&& src ) {
    try {
        if ( this != &src ) {
            delete[] this->data;
            this->size = src.size;
            this->capacity = src.capacity;
            this->allocBegin = src.allocBegin;
            this->data = src.data;
            this->allocEnd = src.allocEnd;

            src.size = 0;
            src.capacity = 2;
            src.allocBegin = new T[src.capacity];
            src.data = src.allocBegin + 1;
            src.allocEnd = src.allocBegin + src.capacity;
        }
    } catch ( std::bad_alloc &ex ) {
        throw Exception(ex);
    }
    return *this;
}

template <typename T>
DynamicArray<T>::~DynamicArray() {
    delete[] this->allocBegin;
}

template <typename T>
void DynamicArray<T>::append( const T& value ) {
    int sizeDiff = 1;
    try {
        this->extend( sizeDiff );
        (*this)[this->size - 1] = value;
    } catch( std::bad_alloc &ex ) {
        throw Exception(ex);
    }
}

template <typename T>
void DynamicArray<T>::prepend( const T& value ) {
    int sizeDiff = 1;
    try {
        this->extend( sizeDiff );
        this->data--;
        (*this)[0] = value;
    } catch( std::bad_alloc &ex ) {
        throw Exception(ex);
    }
}

template <typename T>
void DynamicArray<T>::insertAt( const T& value, const int pos ) {
    if ( pos < 0 || pos > this->size ) {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    int sizeDiff = 1;
    try {
        this->extend( sizeDiff );
        for ( int index = this->size; index >= pos; index-- ) {
            *(data + index) = *(data + index - 1);
        }
        (*this)[pos] = value;
    } catch ( std::bad_alloc &ex ) {
        throw Exception(ex);
    }
}

template <typename T>
void DynamicArray<T>::setAt( const T& value, const int pos ) {
    if ( pos < 0 || pos > this->size ) {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    this->data[pos] = value;
}

template <typename T>
void DynamicArray<T>::removeAt( const int pos ) {
    if ( pos < 0 || pos > this->size ) {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    int sizeDiff = 1;
    try {
        T temp;
        for ( int index = pos; index < this->size - 1; index++ ) {
            temp = (*this)[index];
            (*this)[index] = (*this)[index + 1];
            (*this)[index + 1] = temp;
        }
        shrink( sizeDiff );
    } catch ( std::bad_alloc &ex ) {
        throw Exception(ex);
    }
}

template <typename T>
void DynamicArray<T>::swap( const int pos1, const int pos2 ) {
    if ( pos1 < 0 || pos1 >= this->size || pos2 < 0 || pos2 >= this->size ) {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    T temp = this->data[pos1];
    this->data[pos1] = this->data[pos2];
    this->data[pos2] = temp;
}

template <typename T>
void DynamicArray<T>::extend( int sizeDiff ) {
    if ( sizeDiff < 0 ) {
        throw Exception( Exception::ErrorCode::NEGATIVE_SIZE_DIFFERENCE );
    }

    this->size += sizeDiff;
    int newCapacity = this->capacity;

    if ( newCapacity < 10000 ) {
        newCapacity = ( this->size > newCapacity * 0.75 ) 
            ? newCapacity * 2
            : newCapacity;
    } else {
        newCapacity = ( this->size > newCapacity * 0.25 )
            ? newCapacity + 1000
            : newCapacity;
    }
    try {
        if ( newCapacity != this->capacity ) {
            T* newAllocBegin = new T[newCapacity];
            T* newData = newAllocBegin + (newCapacity / 4 + 1);
            T* newAllocEnd = newAllocBegin + newCapacity;
            for ( int index = 0; index < this->size; index++ ) {
                newData[index] = this->data[index];
            }
            delete[] this->allocBegin; 
            this->capacity = newCapacity;
            this->allocBegin = newAllocBegin;
            this->data = newData;
            this->allocEnd = newAllocEnd;    
        }
    } catch( std::bad_alloc &ex ) {
        throw Exception(ex);
    }
}

template <typename T>
void DynamicArray<T>::shrink( const  int sizeDiff ) {
    if ( sizeDiff < 0 ) {
        throw Exception( Exception::ErrorCode::NEGATIVE_SIZE_DIFFERENCE );
    }

    this->size -= sizeDiff;
    int newCapacity = this->capacity;

    if ( newCapacity < 10000 ) {
        newCapacity = ( this->size < newCapacity * 0.25 )
            ? newCapacity / 2
            : newCapacity;
    } else {
        newCapacity = ( this->size < newCapacity * 0.75 )
            ? newCapacity - 1000
            : newCapacity;
    }
    try {
        if ( newCapacity != this->capacity ) {
            T* newAllocBegin = new T[newCapacity];
            T* newData = newAllocBegin + (newCapacity / 4 + 1);
            T* newAllocEnd = newAllocBegin + newCapacity;
            for ( int index = 0; index < this->size; index++ ) {
                newData[index] = this->data[index];
            }
            delete[] this->allocBegin; 
            this->capacity = newCapacity;
            this->allocBegin = newAllocBegin;
            this->data = newData;
            this->allocEnd = newAllocEnd;    
        }
    } catch( std::bad_alloc &ex ) {
        throw Exception(ex);
    }
}

template <typename T>
DynamicArray<T>* DynamicArray<T>::subArray( const int startIndex, const int endIndex ) const {
    if (   startIndex < 0 || endIndex < 0 ||
    startIndex > endIndex || startIndex >= this->getSize() || endIndex >= this->getSize() ) {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    try {
        DynamicArray<T>* res = new DynamicArray<T>(endIndex - startIndex);
        for ( int index = 0; index < endIndex - startIndex; index++ ) {
            res->append((*this)[index + startIndex]);
        }
        return res;
    } catch ( Exception& ex ) {
        throw Exception(ex);
    }
}

template <typename T>
DynamicArray<T>* DynamicArray<T>::concat( const DynamicArray<T>& other ) {
    try {
        DynamicArray<T>* res = new DynamicArray<T>(this->size + other.size);
        for ( int index = 0; index < this->size; index++ ) {
            (*res)[index] = (*this)[index];
        }
        for ( int index = 0; index < other.size; index++ ) {
            (*res)[this->size + index] = other[index];
        }
        return res;
    } catch ( Exception& ex ) {
        throw Exception(ex);
    }
}

template <typename T>
T& DynamicArray<T>::operator[]( const int index ) {
    if ( index < 0 || index >= this->size ) {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    return this->data[index];
}

template <typename T>
const T& DynamicArray<T>::operator[]( const int index ) const {
    if ( index < 0 || index >= this->size ) {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    return this->data[index];
}

template <typename T>
void DynamicArray<T>::clear() {
    this->size = 0;
    this->capacity = 2;
    delete[] this->allocBegin;
    this->allocBegin = new T[this->capacity];
    this->data = this->allocBegin + (this->capacity / 4 + 1);
    this->allocEnd = this->allocBegin + this->capacity;
}

template <typename T>
int DynamicArray<T>::getSize() const {
    return this->size;
}

template <typename T>
int DynamicArray<T>::getCapacity() const {
    return this->capacity;
}

template <typename T>
const std::string DynamicArray<T>::print() const {
    std::string result = "{";
    for ( int index = 0; index < this->size; index++ ) {
        result += std::to_string( this->data[index] ) + " ";
    }
    result += "}";
    return result;
}

template <typename T>
bool DynamicArray<T>::isEmpty() const {
    return this->size == 0;
}

template <typename T>
DynamicArray<T>* DynamicArray<T>::appendImmutable( const T& value ) const {
    try {
        DynamicArray<T>* res = new DynamicArray<T>(*this);
        res->append( value );
        return res;
    } catch ( Exception& ex ) {
        throw Exception(ex);
    }
}

template <typename T>
DynamicArray<T>* DynamicArray<T>::prependImmutable( const T& value ) const {
    try {
        DynamicArray<T>* res = new DynamicArray<T>(*this);
        res->prepend( value );
        return res;
    } catch ( Exception& ex ) {
        throw Exception(ex);
    }
}

template <typename T>
DynamicArray<T>* DynamicArray<T>::insertAtImmutable( const T& value, const int pos ) const {
    try {
        DynamicArray<T>* res = new DynamicArray<T>(*this);
        res->insertAt( value, pos );
        return res;
    } catch ( Exception& ex ) {
        throw Exception(ex);
    }
}

template <typename T>
DynamicArray<T>* DynamicArray<T>::removeAtImmutable( const int pos ) const {
    try {
        DynamicArray<T>* res = new DynamicArray<T>(*this);
        res->removeAt(pos);
        return res;
    } catch ( Exception& ex ) {
        throw Exception(ex);
    }
}

template <typename T>
DynamicArray<T>* DynamicArray<T>::setAtImmutable( const T& value, const int pos ) const {
    try {
        DynamicArray<T>* res = new DynamicArray<T>(*this);
        res->setAt( value, pos );
        return res;
    } catch ( Exception& ex ) {
        throw Exception(ex);
    }
}

template <typename T>
DynamicArray<T>* DynamicArray<T>::swapImmutable(const int pos1, const int pos2) const {
    try {
        DynamicArray<T>* res = new DynamicArray<T>(*this);
        res->swap(pos1, pos2);
        return res;
    } catch ( Exception& ex ) {
        throw Exception(ex);
    }
}

template <typename T>
DynamicArray<T>* DynamicArray<T>::concatImmutable( const DynamicArray<T>& other ) const {
    try {
        DynamicArray<T>* res = new DynamicArray<T>(*this);
        return res->concat(other);
    } catch ( Exception& ex ) {
        throw Exception(ex);
    }
}