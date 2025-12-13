template <typename T>
LazySequence<T>::LazySequenceIterator::LazySequenceIterator()
: _observed( SharedPtr<LazySequence<T>>() ), _pos(0) {}

template <typename T>
LazySequence<T>::LazySequenceIterator::LazySequenceIterator( SharedPtr<LazySequence<T>> parent, const Ordinal& index ) 
: _observed( parent ), _pos( index ) {}

template <typename T>
LazySequence<T>::LazySequenceIterator::LazySequenceIterator( const LazySequenceIterator& other ) 
: _observed( other._observed ), _pos( other._pos ) {}

template <typename T>
LazySequence<T>::LazySequenceIterator& LazySequence<T>::LazySequenceIterator::operator=( const LazySequenceIterator& other ) {
    if (this != &other) {
        this->_observed = other._observed;
        this->_pos = other._pos;
    }
    return *this;
}

template <typename T>
LazySequence<T>::LazySequenceIterator::LazySequenceIterator( LazySequenceIterator&& other ) {
    this->_observed = std::move(other._observed);
    this->_pos = other._pos;
    other._pos = 0;
}

template <typename T>
LazySequence<T>::LazySequenceIterator& LazySequence<T>::LazySequenceIterator::operator=( LazySequenceIterator&& other ) {
    if (this != &other) {
        this->_observed = std::move(other._observed);
        this->_pos = other._pos;
        other._pos = 0;
    }
    return *this;
}

template <typename T>
LazySequence<T>::LazySequenceIterator& LazySequence<T>::LazySequenceIterator::operator++() {
    this->_pos++;
    return *this;
}

template <typename T>
LazySequence<T>::LazySequenceIterator LazySequence<T>::LazySequenceIterator::operator++(int) {
    auto res = *this;
    this->_pos++;
    return res;
}

template <typename T>
T LazySequence<T>::LazySequenceIterator::operator*() {
    return this->_observed->get(this->_pos);
}

template <typename T>
bool LazySequence<T>::LazySequenceIterator::operator==( const LazySequenceIterator& other ) const noexcept {
    return (this->_observed == other._observed && this->_pos == other._pos);
}

template <typename T>
bool LazySequence<T>::LazySequenceIterator::operator!=( const LazySequenceIterator& other ) const noexcept {
    return (!(*this == other));
}

template <typename T>
LazySequence<T>::LazySequenceIterator LazySequence<T>::begin() {
    return LazySequenceIterator(this->sharedFromThis(), 0);
}

template <typename T>
LazySequence<T>::LazySequenceIterator LazySequence<T>::end() {
    if (_ordinality.hasValue() && _ordinality.get().isFinite()) {
        return LazySequenceIterator(this->sharedFromThis(), this->_ordinality.get() - 1);
    } else if ( _ordinality.hasValue() && _ordinality.get().isTransfinite() ) {
        throw Exception( Exception::ErrorCode::ITERATOR_AT_INFINITY );
    } else {
        throw Exception( Exception::ErrorCode::INVALID_ITERATOR );
    }
}