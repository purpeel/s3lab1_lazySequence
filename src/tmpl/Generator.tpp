template <typename T>
LazySequence<T>::FiniteGenerator::FiniteGenerator() {
    this->_data = makeUnique<ArraySequence<T>>();
    this->_lastMaterialized = 0;
} 

template <typename T>
LazySequence<T>::FiniteGenerator::FiniteGenerator( const ArraySequence<T>& data ) {
    this->_data = ArraySequence<T>(data);
    this->_lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::FiniteGenerator::FiniteGenerator( const T& value ) {
    this->_data = ArraySequence<T>();
    (*this->_data).append(value);
    this->_lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::FiniteGenerator::FiniteGenerator( const FiniteGenerator& other ) {
    *this->_data = *other._data;
    this->_lastMaterialized = other._lastMaterialized;
}

template <typename T>
LazySequence<T>::FiniteGenerator& LazySequence<T>::FiniteGenerator::operator=( const FiniteGenerator& other ) {
    if (this != &other) {
        *this->_data = *other._data;
        this->_lastMaterialized = other._lastMaterialized;
    }
    return *this;
}

template <typename T>
LazySequence<T>::FiniteGenerator::FiniteGenerator( FiniteGenerator&& other ) {
    this->_data = std::move(other._data);
    this->_lastMaterialized = other._lastMaterialized;
    other._lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::FiniteGenerator& LazySequence<T>::FiniteGenerator::operator=( FiniteGenerator&& other ) {
    if (this != &other) {
        this->_data = std::move(other._data);
        this->_lastMaterialized = other._lastMaterialized;
        other._lastMaterialized = 0;
    }
    return *this;
}

template <typename T>
T LazySequence<T>::FiniteGenerator::getNext() {
    if (this->_lastMaterialized >= this->_data->getSize()) {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    return (*this->_data)[this->_lastMaterialized++];
}

template <typename T>
T LazySequence<T>::FiniteGenerator::get( const Ordinal& index ) {
    if (index >= this->_data->getSize()) {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    return (*this->_data)[static_cast<size_t>(index)];
}

template <typename T>
bool LazySequence<T>::FiniteGenerator::hasNext() {
    return this->_lastMaterialized < this->_data->getSize();
}

template <typename T>
Option<T> LazySequence<T>::FiniteGenerator::tryGetNext() {
    if (hasNext()) {
        return Option( this->(*this->_data)[this->_lastMaterialized++] );
    } else {
        return Option<T>();
    }
}

template <typename T>
LazySequence<T>::InfiniteGenerator::InfiniteGenerator( const InfiniteGenerator& other ) 
: _arity(other._arity), _lastMaterialized(0)
, _window(other._window)
, _producingFunc(other._producingFunc) {}

template <typename T>
LazySequence<T>::InfiniteGenerator& LazySequence<T>::InfiniteGenerator::operator=( const InfiniteGenerator& other ) {
    if (this != &other) {
        this->_arity            = other._arity;
        this->_lastMaterialized = 0;
        this->_window           = other._window;
        this->_producingFunc    = other._producingFunc;
    }
    return *this;
}

template <typename T>
LazySequence<T>::InfiniteGenerator::InfiniteGenerator( InfiniteGenerator&& other ) {
    this->_arity            = other._arity;
    this->_lastMaterialized = other._lastMaterialized;
    this->_window        = std::move(other._window);
    this->_producingFunc = std::move(other._producingFunc);
    other._arity            = 0;
    other._lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::InfiniteGenerator& LazySequence<T>::InfiniteGenerator::operator=( InfiniteGenerator&& other ) {
    if (this != &other) {
        this->_arity            = other._arity;
        this->_lastMaterialized = other._lastMaterialized;
        this->_window        = std::move(other._window);
        this->_producingFunc = std::move(other._producingFunc);
        other._arity            = 0;
        other._lastMaterialized = 0;
    }
    return *this;
}

template <typename T>
LazySequence<T>::InfiniteGenerator::InfiniteGenerator( const size_t arity
                                                     , const std::function<T(ArraySequence<T>&)>& func
                                                     , const ArraySequence<T>& data ) 
                                                     : _arity(arity), _offset(0), _producingFunc(func) {
    this->_window = *data.getSubSequence(0, arity);
}

template <typename T>
T LazySequence<T>::InfiniteGenerator::getNext() {
    this->_lastMaterialized++;
    auto next = this->_producingFunc( this->_window );
    this->_window.removeAt(0);
    this->_window.append(next);
    return next;
}

template <typename T>
T LazySequence<T>::InfiniteGenerator::get( const Ordinal& index ) {
    if (index < _lastMaterialized) {
        throw Exception( Exception::ErrorCode::DEMATERIALIZED_ACCESS );
    }
    auto temp = this->_window;
    for (size_t i = _lastMaterialized + this->_arity - 1; i < index; i++) {
        auto next = this->_producingFunc( temp );
        temp.removeAt(0);
        temp.append(next);
    }
    return temp[this->_arity-1];
}

template <typename T>
bool LazySequence<T>::InfiniteGenerator::hasNext() {
    return true;
}

template <typename T>
Option<T> LazySequence<T>::InfiniteGenerator::tryGetNext() {
    return Option<T>( getNext() );
}

template <typename T>
LazySequence<T>::AppendGenerator::AppendGenerator( const T& value, SharedPtr<IGenerator> parent ) {
    this->_initial = parent;
    this->_added = makeShared<FiniteGenerator>(value);
    this->_lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::AppendGenerator::AppendGenerator( const LazySequence<T>& value, SharedPtr<IGenerator> parent ) {
    this->_initial = parent;
    this->_added = makeShared<FiniteGenerator>(value);
    this->_lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::AppendGenerator::AppendGenerator( const AppendGenerator& other ) {
    *this->_initial = *other._initial;
    *this->_added = *other._added;
    this->_lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::AppendGenerator& LazySequence<T>::AppendGenerator::operator=( const AppendGenerator& other ) {
    if (this != &other) {
        *this->_initial = *other._initial;
        *this->_added = *other._added;
        this->_lastMaterialized = 0;
    }
    return *this;
}

template <typename T>
LazySequence<T>::AppendGenerator::AppendGenerator( AppendGenerator&& other ) {
    this->_initial = std::move(other._initial);
    this->_added = std::move(other._added);
    this->_lastMaterialized = other._lastMaterialized;
}

template <typename T>
LazySequence<T>::AppendGenerator& LazySequence<T>::AppendGenerator::operator=( AppendGenerator&& other ) {
    if (this != &other) {
        this->_initial = std::move(other._initial);
        this->_added = std::move(other._added);
        this->_lastMaterialized = other._lastMaterialized;
    }
    return *this;
}

template <typename T>
T LazySequence<T>::AppendGenerator::getNext() {
    this->_lastMaterialized++;
    if (this->_initial->hasNext()) {
        return this->_initial->getNext();
    } else {
        return this->_added->getNext();
    }
}

template <typename T>
T LazySequence<T>::AppendGenerator::get( const Ordinal& index ) {
    
}

template <typename T>
bool LazySequence<T>::AppendGenerator::hasNext() {
    return this->_initial->hasNext() || this->_added->hasNext();
}

template <typename T>
Option<T> LazySequence<T>::AppendGenerator::tryGetNext() {
    if (!this->hasNext()) {
        return Option<T>();
    } else {
        return Option<T>( this->getNext() );
    }
}
