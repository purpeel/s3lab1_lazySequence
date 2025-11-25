template <typename T>
LazySequence<T>::FiniteGenerator::FiniteGenerator() {
    _data = makeUnique<ArraySequence<T>>();
    _lastMaterialized = 0;
} 

template <typename T>
LazySequence<T>::FiniteGenerator::FiniteGenerator( const ArraySequence<T>& data ) {
    _data = ArraySequence<T>(data);
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::FiniteGenerator::FiniteGenerator( const T& value ) {
    _data = ArraySequence<T>();
    (*_data).append(value);
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::FiniteGenerator::FiniteGenerator( const FiniteGenerator& other ) {
    *_data = *other._data;
    _lastMaterialized = other._lastMaterialized;
}

template <typename T>
LazySequence<T>::FiniteGenerator& LazySequence<T>::FiniteGenerator::operator=( const FiniteGenerator& other ) {
    if (this != &other) {
        *_data = *other._data;
        _lastMaterialized = other._lastMaterialized;
    }
    return *this;
}

template <typename T>
LazySequence<T>::FiniteGenerator::FiniteGenerator( FiniteGenerator&& other ) {
    _data = std::move(other._data);
    _lastMaterialized = other._lastMaterialized;
    other._lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::FiniteGenerator& LazySequence<T>::FiniteGenerator::operator=( FiniteGenerator&& other ) {
    if (this != &other) {
        _data = std::move(other._data);
        _lastMaterialized = other._lastMaterialized;
        other._lastMaterialized = 0;
    }
    return *this;
}

template <typename T>
T LazySequence<T>::FiniteGenerator::getNext() {
    if (_lastMaterialized >= _data->getSize()) {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    return (*_data)[_lastMaterialized++];
}

template <typename T>
T LazySequence<T>::FiniteGenerator::get( const Ordinal& index ) {
    if (index >= _data->getSize()) {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    return (*_data)[static_cast<size_t>(index)];
}

template <typename T>
bool LazySequence<T>::FiniteGenerator::hasNext() {
    return _lastMaterialized < _data->getSize();
}

template <typename T>
Option<T> LazySequence<T>::FiniteGenerator::tryGetNext() {
    if (hasNext()) {
        return Option( (*_data)[_lastMaterialized++] );
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
        _arity            = other._arity;
        _lastMaterialized = 0;
        _window           = other._window;
        _producingFunc    = other._producingFunc;
    }
    return *this;
}

template <typename T>
LazySequence<T>::InfiniteGenerator::InfiniteGenerator( InfiniteGenerator&& other ) {
    _arity            = other._arity;
    _lastMaterialized = other._lastMaterialized;
    _window        = std::move(other._window);
    _producingFunc = std::move(other._producingFunc);
    other._arity            = 0;
    other._lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::InfiniteGenerator& LazySequence<T>::InfiniteGenerator::operator=( InfiniteGenerator&& other ) {
    if (this != &other) {
        _arity            = other._arity;
        _lastMaterialized = other._lastMaterialized;
        _window        = std::move(other._window);
        _producingFunc = std::move(other._producingFunc);
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
    _window = *data.getSubSequence(0, arity);
}

template <typename T>
T LazySequence<T>::InfiniteGenerator::getNext() {
    auto next = _producingFunc( _window );
    _window.removeAt(0);
    _window.append(next);
    return next;
    _lastMaterialized++;
}

template <typename T>
T LazySequence<T>::InfiniteGenerator::get( const Ordinal& index ) {
    if (index < _lastMaterialized) {
        throw Exception( Exception::ErrorCode::DEMATERIALIZED_ACCESS );
    }
    auto temp = _window;
    for (size_t i = _lastMaterialized + _arity - 1; i < index; i++) {
        auto next = _producingFunc( temp );
        temp.removeAt(0);
        temp.append(next);
    }
    return temp[_arity-1];
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
LazySequence<T>::AppendGenerator::AppendGenerator( const T& value, SharedPtr<IGenerator> parent, const Ordinal& border ) {
    _initial = parent;
    _added = makeShared<FiniteGenerator>(value);
    _border = border;
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::AppendGenerator::AppendGenerator( const LazySequence<T>& value, SharedPtr<IGenerator> parent, const Ordinal& border ) {
    _initial = parent;
    _added = makeShared<FiniteGenerator>(value);
    _border = border;
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::AppendGenerator::AppendGenerator( const AppendGenerator& other ) {
    *_initial = *other._initial;
    *_added = *other._added;
    _border = other._border;
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::AppendGenerator& LazySequence<T>::AppendGenerator::operator=( const AppendGenerator& other ) {
    if (this != &other) {
        *_initial = *other._initial;
        *_added = *other._added;
        _border = other._border;
        _lastMaterialized = 0;
    }
    return *this;
}

template <typename T>
LazySequence<T>::AppendGenerator::AppendGenerator( AppendGenerator&& other ) {
    _initial = std::move(other._initial);
    _added = std::move(other._added);
    _lastMaterialized = other._lastMaterialized;
    _border = other._border;
    other._border = 0;
    other._lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::AppendGenerator& LazySequence<T>::AppendGenerator::operator=( AppendGenerator&& other ) {
    if (this != &other) {
        _initial = std::move(other._initial);
        _added = std::move(other._added);
        _lastMaterialized = other._lastMaterialized;
        _border = other._border;
        other._border = 0;
        other._lastMaterialized = 0;
    }
    return *this;
}

template <typename T>
T LazySequence<T>::AppendGenerator::getNext() {
    if (_initial->hasNext()) {
        return _initial->getNext();
    } else if (_added->hasNext()) {
        return _added->getNext();
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    _lastMaterialized++;
}

template <typename T>
T LazySequence<T>::AppendGenerator::get( const Ordinal& index ) {
    if (index > _border) {
        return _added->get(index);
    } else {
        return _initial->get(index);
    }
}

template <typename T>
bool LazySequence<T>::AppendGenerator::hasNext() {
    return _initial->hasNext() || _added->hasNext();
}

template <typename T>
Option<T> LazySequence<T>::AppendGenerator::tryGetNext() {
    if (!hasNext()) {
        return Option<T>();
    } else {
        return Option<T>( getNext() );
    }
}

template <typename T>
LazySequence<T>::PrependGenerator::PrependGenerator( const T& value, SharedPtr<IGenerator> parent, const Ordinal& border ) {
    _added = makeShared<FiniteGenerator>( value );
    _initial = parent;
    _lastMaterialized = 0;
    _border = border;
}

template <typename T>
LazySequence<T>::PrependGenerator::PrependGenerator( const LazySequence<T>& value , SharedPtr<IGenerator> parent, const Ordinal& border ) {
    _added = makeShared<FiniteGenerator>( value );
    _initial = parent;
    _lastMaterialized = 0;
    _border = border;
}

template <typename T>
LazySequence<T>::PrependGenerator::PrependGenerator( const PrependGenerator& other ) {
    *_initial = *other._initial;
    *_added = *other._added;
    _border = other._border;
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::PrependGenerator& LazySequence<T>::PrependGenerator::operator=( const PrependGenerator& other ) {
    if (this != &other) {
        *_initial = *other._initial;
        *_added = *other._added;
        _border = other._border;
        _lastMaterialized = 0;
    }
    return *this;
}

template <typename T>
LazySequence<T>::PrependGenerator::PrependGenerator( PrependGenerator&& other ) {
    _initial = std::move(other._initial);
    _added = std::move(other._added);
    _border = other._border;
    _lastMaterialized = 0;
    other._lastMaterialized = 0;
    other._border = 0;

}

template <typename T>
LazySequence<T>::PrependGenerator& LazySequence<T>::PrependGenerator::operator=( PrependGenerator&& other ) {
    if (this != &other) {
        _initial = std::move(other._initial);
        _added = std::move(other._added);
        _border = other._border;
        _lastMaterialized = 0;
        other._lastMaterialized = 0;
        other._border = 0;
    }
    return *this;
}

template <typename T>
T LazySequence<T>::PrependGenerator::getNext() {
    if (_added->hasNext()) {
        return _added->getNext();
    } else if (_initial->hasNext()) {
        return _initial->getNext();
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    _lastMaterialized++;
}

template <typename T>
T LazySequence<T>::PrependGenerator::get( const Ordinal& index ) {
    if (index > _border) {
        return _added->get(index);
    } else {
        return _initial->get(index);
    }
}

template <typename T>
bool LazySequence<T>::PrependGenerator::hasNext() {
    return _added->hasNext() || _initial->hasNext();
}

template <typename T>
Option<T> LazySequence<T>::PrependGenerator::tryGetNext() {
    if (!hasNext()) {
        return Option<T>();
    } else {
        return Option<T>( getNext() );
    }
}

template <typename T>
LazySequence<T>::InsertGenerator::InsertGenerator( const T& value, const Ordinal& index, SharedPtr<IGenerator> parent, const Ordinal& addedOrdinality ) {
    _initial = parent;
    _added = makeShared<FiniteGenerator>(value);
    _lastMaterialized = 0;
    _targetIndex = index;
    _border = _targetIndex + addedOrdinality;
}

template <typename T>
LazySequence<T>::InsertGenerator::InsertGenerator( const LazySequence<T>& value, const Ordinal& index, SharedPtr<IGenerator> parent, const Ordinal& addedOrdinality ) {
    _initial = parent;
    _added = makeShared<FiniteGenerator>(value);
    _lastMaterialized = 0;
    _targetIndex = index;
    _border = _targetIndex + addedOrdinality;
}

template <typename T>
LazySequence<T>::InsertGenerator::InsertGenerator( const InsertGenerator& other ) {
    *_initial = *other._initial;
    *_added = *other._added;
    _border = other._border;
    _lastMaterialized = other._lastMaterialized;
    _targetIndex = other._targetIndex;
}

template <typename T>
LazySequence<T>::InsertGenerator& LazySequence<T>::InsertGenerator::operator=( const InsertGenerator& other ) {
    if (this != &other) {
        *_initial = *other._initial; //??? to copy or not to copy
        *_added = *other._added;
        _border = other._border;
        _lastMaterialized = other._lastMaterialized;
        _targetIndex = other._targetIndex;
    }
    return *this;
}

template <typename T>
LazySequence<T>::InsertGenerator::InsertGenerator( InsertGenerator&& other ) {
    _initial = std::move(other._initial);
    _added = std::move(other._added);
    _border = other._border;
    _lastMaterialized = other._lastMaterialized;
    _targetIndex = other._targetIndex;
    other._border = 0;
    other._lastMaterialized = 0;
    other._targetIndex = 0;
}

template <typename T>
LazySequence<T>::InsertGenerator& LazySequence<T>::InsertGenerator::operator=( InsertGenerator&& other ) {
    if (this != &other) {
        _initial = std::move(other._initial);
        _added = std::move(other._added);
        _border = other._border;
        _lastMaterialized = other._lastMaterialized;
        _targetIndex = other._targetIndex;
        other._border = 0;
        other._lastMaterialized = 0;
        other._targetIndex = 0;
    }
    return *this;
}

template <typename T>
T LazySequence<T>::InsertGenerator::getNext() {
    if (!hasNext()) { throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); }
    if (_lastMaterialized < _targetIndex || _lastMaterialized > _border) {
        return _initial->getNext();
    } else if (_lastMaterialized > _targetIndex && _lastMaterialized < _border ) {
        return _added->getNext();
    }
    _lastMaterialized++;
}

template <typename T>
bool LazySequence<T>::InsertGenerator::hasNext() {
    return _initial->hasNext() || _added->hasNext();
}

template <typename T>
Option<T> LazySequence<T>::InsertGenerator::tryGetNext() {
    if (hasNext()) {
        return Option<T>(getNext());
    } else {
        return Option<T>();
    }
}

template <typename T>
LazySequence<T>::SkipGenerator::SkipGenerator( const Ordinal& index, SharedPtr<IGenerator> parent ) {
    _lastMaterialized = 0;
    _from = index;
    _to = index + 1;
    _parent = parent;
}

template <typename T>
LazySequence<T>::SkipGenerator::SkipGenerator( const Ordinal& start, const Ordinal& end, SharedPtr<IGenerator> parent ) {
    _lastMaterialized = 0;
    _from = start;
    _to = end;
    _parent = parent;
}

template <typename T>
LazySequence<T>::SkipGenerator::SkipGenerator( const SkipGenerator& other ) {
    _lastMaterialized = other._lastMaterialized;
    _from = other._from;
    _to = other._to;
    *_parent = *other._parent;
}

template <typename T>
LazySequence<T>::SkipGenerator& LazySequence<T>::SkipGenerator::operator=( const SkipGenerator& other ) {
    if (this != &other) {
        _lastMaterialized = other._lastMaterialized;
        _from = other._from;
        _to = other._to;
        *_parent = *other._parent;
    }
    return *this;
}

template <typename T>
LazySequence<T>::SkipGenerator::SkipGenerator( SkipGenerator&& other ) {
    _lastMaterialized = other._lastMaterialized;
    _from = other._from;
    _to = other._to;
    _parent = std::move(other._parent);
    other._from = 0;
    other._to = 0;
    other._lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::SkipGenerator& LazySequence<T>::SkipGenerator::operator=( SkipGenerator&& other ) {
    if (this != &other) {
        _lastMaterialized = other._lastMaterialized;
        _from = other._from;
        _to = other._to;
        _parent = std::move(other._parent);
        other._from = 0;
        other._to = 0;
        other._lastMaterialized = 0;
    }
    return *this;
}

template <typename T>
T LazySequence<T>::SkipGenerator::getNext() {
    if (!hasNext()) { throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); }
    if (_lastMaterialized < _from) {
        return _parent->getNext();
    } else if (_lastMaterialized == _from) {
        if (_to.isFinite()) {
            for (size_t i = 0; i < _to - _from - 1; i++) {
                _parent->getNext();
            }
            return _parent->getNext();
        } else {
            return _parent->get( _to + _lastMaterialized );
        }
    } else {
        if (_to.isFinite()) {
            return _parent->getNext();
        } else {
            return _parent->get( _to + _lastMaterialized - _from );
        }
    }
    _lastMaterialized++;
}

template <typename T>
T LazySequence<T>::SkipGenerator::get( const Ordinal& index ) {
    if (index < _from) {
        return _parent->get(index);
    } else {
        return _parent->get( index + (_to - _from) );
    }
}

template <typename T>
bool LazySequence<T>::SkipGenerator::hasNext() {
    return _parent->hasNext();
}

template <typename T>
Option<T> LazySequence<T>::SkipGenerator::tryGetNext() {
    if (hasNext()) {
        return Option<T>(getNext());
    } else {
        return Option<T>();
    }
}

template <typename T>
LazySequence<T>::SubSequenceGenerator::SubSequenceGenerator( const Ordinal& start, const Ordinal& end, SharedPtr<IGenerator> parent ) {
    _lastMaterialized = 0;
    _from = start;
    _to = end;
    _parent = parent;
}

template <typename T>
LazySequence<T>::SubSequenceGenerator::SubSequenceGenerator( const SubSequenceGenerator& other ) {
    _lastMaterialized = other._lastMaterialized;
    _from = other._from;
    _to = other._to;
    *_parent = *other._parent;
}

template <typename T>
LazySequence<T>::SubSequenceGenerator& LazySequence<T>::SubSequenceGenerator::operator=( const SubSequenceGenerator& other ) {
    if (this != &other) {
        _lastMaterialized = other._lastMaterialized;
        _from = other._from;
        _to = other._to;
        *_parent = *other._parent;
    }
    return *this;
}

template <typename T>
LazySequence<T>::SubSequenceGenerator::SubSequenceGenerator( SubSequenceGenerator&& other ) {
    _lastMaterialized = other._lastMaterialized;
    _from = other._from;
    _to = other._to;
    _parent = std::move(other._parent);
    other._from = 0;
    other._to = 0;
    other._lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::SubSequenceGenerator& LazySequence<T>::SubSequenceGenerator::operator=( SubSequenceGenerator&& other ) {
    if (this != &other) {
        _lastMaterialized = other._lastMaterialized;
        _from = other._from;
        _to = other._to;
        _parent = std::move(other._parent);
        other._from = 0;
        other._to = 0;
        other._lastMaterialized = 0;
    }
    return *this;
}

template <typename T>
T LazySequence<T>::SubSequenceGenerator::getNext() {
    if (!hasNext()) { throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); }
    if (_from.isFinite()) {
        if ( _lastMaterialized == 0) {
            while (_lastMaterialized < _from - 1) {
                _parent->getNext();
            }
            return _parent->getNext();
        } else if (_lastMaterialized < _to - _from) {
            return _parent->getNext(); 
        } else {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
    } else {
        if (_lastMaterialized < _to - _from) {
            return _parent->get( _from + _lastMaterialized );
        } else { 
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
    }
    _lastMaterialized++;
}

template <typename T>
T LazySequence<T>::SubSequenceGenerator::get( const Ordinal& index ) {
    if (index >= _from || index < _to) {
        return _parent->get(_from + index);
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
}

template <typename T>
bool LazySequence<T>::SubSequenceGenerator::hasNext() {
    return _parent->hasNext();
}

template <typename T>
Option<T> LazySequence<T>::SubSequenceGenerator::tryGetNext() {
    if (hasNext()) {
        return Option<T>(getNext());
    } else {
        return Option<T>();
    }
}
