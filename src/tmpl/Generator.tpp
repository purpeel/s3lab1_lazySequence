template <typename T>
LazySequence<T>::FiniteGenerator::FiniteGenerator() {
    _data = ArraySequence<T>();
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
    _data.append(value);
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::FiniteGenerator::FiniteGenerator( const FiniteGenerator& other ) {
    _data = other._data;
    _lastMaterialized = other._lastMaterialized;
}

template <typename T>
LazySequence<T>::FiniteGenerator& LazySequence<T>::FiniteGenerator::operator=( const FiniteGenerator& other ) {
    if (this != &other) {
        _data = other._data;
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
    return _data[static_cast<size_t>(_lastMaterialized++)];
}

template <typename T>
T LazySequence<T>::FiniteGenerator::get( const Ordinal& index ) {
    return _data[static_cast<size_t>(index)];
}

template <typename T>
bool LazySequence<T>::FiniteGenerator::hasNext() {
    return _lastMaterialized < _data.getSize();
}

template <typename T>
Option<T> LazySequence<T>::FiniteGenerator::tryGetNext() {
    if (hasNext()) {
        return Option( getNext() );
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
                                                     : _arity(arity), _producingFunc(func) {
    _window = *data.getSubSequence(0, arity);
}

template <typename T>
T LazySequence<T>::InfiniteGenerator::getNext() {
    auto next = _producingFunc( _window );
    _window.removeAt(0);
    _window.append(next);
    _lastMaterialized++;
    return next;
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
LazySequence<T>::AppendGenerator::AppendGenerator( const T& value, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& border ) {
    _initial    = parent;
    _added      = makeShared<LazySequence<T>>(value);
    _border     = border;
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::AppendGenerator::AppendGenerator( const LazySequence<T>& value, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& border ) {
    _initial    = parent;
    _added      = value;    
    _border     = border;
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::AppendGenerator::AppendGenerator( const AppendGenerator& other ) {
    _initial    = other._initial;
    _added      = other._added;
    _border     = other._border;
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::AppendGenerator& LazySequence<T>::AppendGenerator::operator=( const AppendGenerator& other ) {
    if (this != &other) {
        _initial    = other._initial;
        _added      = other._added;
        _border     = other._border;
        _lastMaterialized = 0;
    }
    return *this;
}

template <typename T>
LazySequence<T>::AppendGenerator::AppendGenerator( AppendGenerator&& other ) {
    _initial    = std::move(other._initial);
    _added      = std::move(other._added);
    _lastMaterialized = other._lastMaterialized;
    _border = other._border;
    other._border = 0;
    other._lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::AppendGenerator& LazySequence<T>::AppendGenerator::operator=( AppendGenerator&& other ) {
    if (this != &other) {
        _initial    = std::move(other._initial);
        _added      = std::move(other._added);
        _lastMaterialized = other._lastMaterialized;
        _border = other._border;
        other._border = 0;
        other._lastMaterialized = 0;
    }
    return *this;
}

template <typename T>
T LazySequence<T>::AppendGenerator::getNext() {
    if ( !hasNext() ) { throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); }
    auto current = _lastMaterialized++;
    if (_initial->canMemoiseNext()) {
        return _initial->memoiseNext();
    } else if (_added->canMemoiseNext()) {
        return _added->memoiseNext();
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
}

template <typename T>
T LazySequence<T>::AppendGenerator::get( const Ordinal& index ) {
    if (_border.hasValue()) {
        if (index > _border.get()) {
            return _added->get( index - _border.get() );
        } else {
            return _initial->get( index );
        }
    } else {
        throw Exception( Exception::ErrorCode::UNKNOWN_ORDINALITY );
    }
}

template <typename T>
bool LazySequence<T>::AppendGenerator::hasNext() {
    return _initial->canMemoiseNext() || _added->canMemoiseNext();
}

template <typename T>
Option<T> LazySequence<T>::AppendGenerator::tryGetNext() {
    if (hasNext()) {
        return Option<T>( getNext() );
    } else {
        return Option<T>();
    }
}

template <typename T>
LazySequence<T>::PrependGenerator::PrependGenerator( const T& value, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& border ) {
    _initial    = parent;
    _added      = makeShared<LazySequence<T>>( value );
    _border     = border;
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::PrependGenerator::PrependGenerator( const LazySequence<T>& value , SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& border ) {
    _initial    = parent;
    _added      = value;
    _border     = border;
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::PrependGenerator::PrependGenerator( const PrependGenerator& other ) {
    _initial    = other._initial;
    _added      = other._added;
    _border     = other._border;
    _lastMaterialized = other._lastMaterialized;
}

template <typename T>
LazySequence<T>::PrependGenerator& LazySequence<T>::PrependGenerator::operator=( const PrependGenerator& other ) {
    if (this != &other) {    
        _initial    = other._initial;
        _added      = other._added;
        _border     = other._border;
        _lastMaterialized = other._lastMaterialized;
    }
    return *this;
}

template <typename T>
LazySequence<T>::PrependGenerator::PrependGenerator( PrependGenerator&& other ) {
    _initial    = std::move(other._initial);
    _added      = std::move(other._added);
    _border     = other._border;
    _lastMaterialized = other._lastMaterialized;

    other._lastMaterialized = 0;
    other._border = 0;

}

template <typename T>
LazySequence<T>::PrependGenerator& LazySequence<T>::PrependGenerator::operator=( PrependGenerator&& other ) {
    if (this != &other) {
        _initial    = std::move(other._initial);
        _added      = std::move(other._added);
        _border     = other._border;
        _lastMaterialized = other._lastMaterialized;
        
        other._lastMaterialized = 0;
        other._border = 0;
    }
    return *this;
}

template <typename T>
T LazySequence<T>::PrependGenerator::getNext() {
    if ( !hasNext() ) { throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); }
    _lastMaterialized++;
    if (_added->canMemoiseNext()) {
        return _added->memoiseNext();
    } else if (_initial->canMemoiseNext()) {
        return _initial->memoiseNext();
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
}

template <typename T>
T LazySequence<T>::PrependGenerator::get( const Ordinal& index ) {
    if (_border.hasValue()) {
        if (index > _border.get()) {
            return _initial->get( index - _border.get() );
        } else {
            return _added->get( index );
        }
    } else {
        throw Exception( Exception::ErrorCode::UNKNOWN_ORDINALITY );
    }
}

template <typename T>
bool LazySequence<T>::PrependGenerator::hasNext() {
    return _added->canMemoiseNext() || _initial->canMemoiseNext();
}

template <typename T>
Option<T> LazySequence<T>::PrependGenerator::tryGetNext() {
    if (hasNext()) {
        return Option<T>( getNext() );
    } else {
        return Option<T>();
    }
}

template <typename T>
LazySequence<T>::InsertGenerator::InsertGenerator( const T& value, const Ordinal& index, SharedPtr<LazySequence<T>> parent ) {
    _initial     = parent;
    _added       = makeShared<LazySequence<T>>(value);
    _targetIndex = index;
    _border      = _targetIndex + 1;
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::InsertGenerator::InsertGenerator( const LazySequence<T>& value, const Ordinal& index, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& addedOrdinality ) {
    _initial     = parent;
    _added       = value._generator;
    _targetIndex = index;
    _border      = addedOrdinality.hasValue() ? _targetIndex + addedOrdinality : Option<Ordinal>();
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::InsertGenerator::InsertGenerator( const InsertGenerator& other ) {
    _initial     = other._initial;
    _added       = other._added;
    _targetIndex = other._targetIndex;
    _border      = other._border;
    _lastMaterialized = other._lastMaterialized;
}

template <typename T>
LazySequence<T>::InsertGenerator& LazySequence<T>::InsertGenerator::operator=( const InsertGenerator& other ) {
    if (this != &other) {
        _initial     = other._initial;
        _added       = other._added;
        _border      = other._border;
        _targetIndex = other._targetIndex;
        _lastMaterialized = other._lastMaterialized;
    }
    return *this;
}

template <typename T>
LazySequence<T>::InsertGenerator::InsertGenerator( InsertGenerator&& other ) {
    _initial     = std::move(other._initial);
    _added       = std::move(other._added);
    _border      = other._border;
    _targetIndex = other._targetIndex;
    _lastMaterialized = other._lastMaterialized;

    other._border           = 0;
    other._lastMaterialized = 0;
    other._targetIndex      = 0;
}

template <typename T>
LazySequence<T>::InsertGenerator& LazySequence<T>::InsertGenerator::operator=( InsertGenerator&& other ) {
    if (this != &other) {
        _initial     = std::move(other._initial);
        _added       = std::move(other._added);
        _border      = other._border;
        _targetIndex = other._targetIndex;
        _lastMaterialized = other._lastMaterialized;

        other._border           = 0;
        other._lastMaterialized = 0;
        other._targetIndex      = 0;
    }
    return *this;
}

template <typename T>
T LazySequence<T>::InsertGenerator::getNext() {
    auto current = _lastMaterialized++;
    if ( !hasNext() ) { throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); }
    if (   current <  _targetIndex 
        || current >= _targetIndex && _initial->canMemoiseNext()) {
        return _initial->memoiseNext();
    } else if (current >= _targetIndex && _added->canMemoiseNext()) {
        return _added->memoiseNext();
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
}

template <typename T>
T LazySequence<T>::InsertGenerator::get( const Ordinal& index ) {
    if (index < _targetIndex) { return (*_initial)[index]; } 
    if (_border.hasValue()) {
        if (index > _border) {
            return _initial->get(index - _border + _targetIndex);
        } else if (index >= _targetIndex && index <= _border) {
            return _added->get(index - _targetIndex);
        }
    } else {
        throw Exception( Exception::ErrorCode::UNKNOWN_ORDINALITY );
    }
}

template <typename T>
bool LazySequence<T>::InsertGenerator::hasNext() {
    return _initial->canMemoiseNext() || _added->canMemoiseNext();
}

template <typename T>
Option<T> LazySequence<T>::InsertGenerator::tryGetNext() {
    if ( hasNext() ) {
        return Option<T>( getNext() );
    } else {
        return Option<T>();
    }
}

template <typename T>
LazySequence<T>::SkipGenerator::SkipGenerator( const Ordinal& index, SharedPtr<LazySequence<T>> parent ) {
    _lastMaterialized = 0;
    _from = index;
    _to = index + 1;
    _parent = parent;
}

template <typename T>
LazySequence<T>::SkipGenerator::SkipGenerator( const Ordinal& start, const Ordinal& end, SharedPtr<LazySequence<T>> parent ) {
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
    _parent = other._parent;
}

template <typename T>
LazySequence<T>::SkipGenerator& LazySequence<T>::SkipGenerator::operator=( const SkipGenerator& other ) {
    if (this != &other) {
        _lastMaterialized = other._lastMaterialized;
        _from = other._from;
        _to = other._to;
        _parent = other._parent;
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
    auto current = _lastMaterialized++;
    if (current < _from) {
        return _parent->memoiseNext();
    } else if (current == _from) {
        if (_to.isFinite()) {
            for (size_t i = 0; i < _to - _from - 1; i++) {
                _parent->memoiseNext();
            }
            return _parent->memoiseNext();
        } else {
            return _parent->get( _to );
        }
    } else {
        if (_to.isFinite()) {
            return _parent->memoiseNext();
        } else {
            return _parent->get( _to - _from + _lastMaterialized );
        }
    }
}

template <typename T>
T LazySequence<T>::SkipGenerator::get( const Ordinal& index ) {
    if (index < _from) {
        return _parent->get(index);
    } else {
        return _parent->get( _to - _from + index );
    }
}

template <typename T>
bool LazySequence<T>::SkipGenerator::hasNext() {
    return _parent->canMemoiseNext();
}

template <typename T>
Option<T> LazySequence<T>::SkipGenerator::tryGetNext() {
    if (hasNext()) {
        return Option<T>( getNext() );
    } else {
        return Option<T>();
    }
}

template <typename T>
LazySequence<T>::SubSequenceGenerator::SubSequenceGenerator( const Ordinal& start, const Ordinal& end, SharedPtr<LazySequence<T>> parent ) {
    _from   = start;
    _to     = end;
    _parent = parent;
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::SubSequenceGenerator::SubSequenceGenerator( const SubSequenceGenerator& other ) {
    _from   = other._from;
    _to     = other._to;
    _parent = other._parent;
    _lastMaterialized = other._lastMaterialized;
}

template <typename T>
LazySequence<T>::SubSequenceGenerator& LazySequence<T>::SubSequenceGenerator::operator=( const SubSequenceGenerator& other ) {
    if (this != &other) {
        _from   = other._from;
        _to     = other._to;
        _parent = other._parent;
        _lastMaterialized = other._lastMaterialized;
    }
    return *this;
}

template <typename T>
LazySequence<T>::SubSequenceGenerator::SubSequenceGenerator( SubSequenceGenerator&& other ) {
    _from   = other._from;
    _to     = other._to;
    _parent = std::move(other._parent);
    _lastMaterialized = other._lastMaterialized;

    other._from = 0;
    other._to   = 0;
    other._lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::SubSequenceGenerator& LazySequence<T>::SubSequenceGenerator::operator=( SubSequenceGenerator&& other ) {
    if (this != &other) {
        _from   = other._from;
        _to     = other._to;
        _parent = std::move(other._parent);
        _lastMaterialized = other._lastMaterialized;
        
        other._from = 0;
        other._to   = 0;
        other._lastMaterialized = 0;
    }
    return *this;
}

template <typename T>
T LazySequence<T>::SubSequenceGenerator::getNext() {
    if (!hasNext()) { throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); }
    auto current = _lastMaterialized++;
    if (_from.isFinite()) {
        if ( current == 0 ) {
            while ( current < _from - 1 ) {
                _parent->memoiseNext();
            }
            return _parent->memoiseNext();
        } else if ( current < _to - _from ) {
            return _parent->memoiseNext();
        } else {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
    } else {
        if ( current < _to - _from ) {
            return _parent->get( _from + current );
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
    return _from + _lastMaterialized < _to;
}

template <typename T>
Option<T> LazySequence<T>::SubSequenceGenerator::tryGetNext() {
    if (hasNext()) {
        return Option<T>( getNext() );
    } else {
        return Option<T>();
    }
}

template <typename T>
LazySequence<T>::ConcatGenerator::ConcatGenerator( SharedPtr<LazySequence<T>> first, SharedPtr<LazySequence<T>> second, const Option<Ordinal>& border ) {
    _border = border;
    _first  = first;
    _second = second;
}

template <typename T>
LazySequence<T>::ConcatGenerator::ConcatGenerator( const ConcatGenerator& other ) {
    _border = other._border;
    _first  = other._first;
    _second = other._second;
}

template <typename T>
LazySequence<T>::ConcatGenerator& LazySequence<T>::ConcatGenerator::operator=( const ConcatGenerator& other ) {
    if (this != &other) {
        _border = other._border;
        _first  = other._first;
        _second = other._second;
    }
    return *this;
}

template <typename T>
LazySequence<T>::ConcatGenerator::ConcatGenerator( ConcatGenerator&& other ) {
    _border = other._border;
    _first  = std::move(other._first);
    _second = std::move(other._second);

    other._border = 0;
}

template <typename T>
LazySequence<T>::ConcatGenerator& LazySequence<T>::ConcatGenerator::operator=( ConcatGenerator&& other ) {
    if (this != &other) {
        _border = other._border;
        _first  = std::move(other._first);
        _second = std::move(other._second);

        other._border = 0;
    }
    return *this;
}

template <typename T>
T LazySequence<T>::ConcatGenerator::getNext() {
    if (_first->canMemoiseNext()) {
        return _first->memoiseNext();
    } else if (_second->canMemoiseNext()) {
        return _second->memoiseNext();
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
}

template <typename T>
T LazySequence<T>::ConcatGenerator::get( const Ordinal& index ) {
    if (_border.hasValue()) {
        if (index <= _border.get()) {
            return _first->get(index);
        } else {
            return _second->get(index - _border.get());
        }
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
}

template <typename T>
bool LazySequence<T>::ConcatGenerator::hasNext() {
    return _first->canMemoiseNext() || _second->canMemoiseNext();
}

template <typename T>
Option<T> LazySequence<T>::ConcatGenerator::tryGetNext() {
    if (hasNext()) {
        return Option<T>( getNext());
    } else {
        return Option<T>();
    }
}

template <typename T>
template <typename T2>
LazySequence<T>::MapGenerator<T2>::MapGenerator( const std::function<T2(T)>& func, SharedPtr<LazySequence<T>> parent ) {
    _parent = parent;
    _func   = func;
}

template <typename T>
template <typename T2>
LazySequence<T>::MapGenerator<T2>::MapGenerator( const MapGenerator<T2>& other ) {
    _parent = other._parent;
    _func   = other._func;
}

template <typename T>
template <typename T2>
LazySequence<T>::MapGenerator<T2>& LazySequence<T>::MapGenerator<T2>::operator=( const MapGenerator<T2>& other ) {
    if (this != &other) {
        _parent = other._parent;
        _func   = other._func;
    }
    return *this;
}

template <typename T>
template <typename T2>
LazySequence<T>::MapGenerator<T2>::MapGenerator( MapGenerator<T2>&& other ) {
    _parent = std::move(other._parent);
    _func   = std::move(other._func);
}

template <typename T>
template <typename T2>
LazySequence<T>::MapGenerator<T2>& LazySequence<T>::MapGenerator<T2>::operator=( MapGenerator<T2>&& other ) {
    if (this != &other) {
        _parent = std::move(other._parent);
        _func = std::move(other._func);
    }
    return *this;
}

template <typename T>
template <typename T2>
T2 LazySequence<T>::MapGenerator<T2>::getNext() {
    return _func( _parent->memoiseNext() );
}

template <typename T>
template <typename T2>
T2 LazySequence<T>::MapGenerator<T2>::get( const Ordinal& index ) {
    return _func( _parent->get( index ) );
}

template <typename T>
template <typename T2>
bool LazySequence<T>::MapGenerator<T2>::hasNext() {
    return _parent->canMemoiseNext();
}

template <typename T>
template <typename T2>
Option<T2> LazySequence<T>::MapGenerator<T2>::tryGetNext() {
    if (hasNext()) {
        return Option<T2>( getNext() );
    } else {
        return Option<T2>();
    }
}

template <typename T>
LazySequence<T>::WhereGenerator::WhereGenerator( const std::function<bool(T)>& func, SharedPtr<LazySequence<T>> parent )
: _predicate( func )
, _parent( parent )
, _memoized( Option<T>() )
, _isFinished( false ) {}

template <typename T>
LazySequence<T>::WhereGenerator::WhereGenerator( const WhereGenerator& other )
: _predicate( other._predicate )
, _parent( other._parent )
, _memoized( other._memoized ) 
, _isFinished( other._isFinished ){}

template <typename T>
LazySequence<T>::WhereGenerator& LazySequence<T>::WhereGenerator::operator=( const WhereGenerator& other ) {
    if (this != &other) {
        _predicate  = other._predicate;
        _parent     = other._parent;
        _memoized   = other._memoized;
        _isFinished = other._isFinished;
    } 
    return *this;
}

template <typename T>
LazySequence<T>::WhereGenerator::WhereGenerator( WhereGenerator&& other )
: _predicate( std::move( other._predicate ))
, _parent( std::move( other._parent ))
, _memoized( std::move( other._memoized )) 
, _isFinished( other._isFinished ) { other._isFinished = false; }

template <typename T>
LazySequence<T>::WhereGenerator& LazySequence<T>::WhereGenerator::operator=( WhereGenerator&& other ) {
    if (this != &other) {
        _predicate  = std::move(other._predicate);
        _parent     = std::move(other._parent);
        _memoized   = std::move(other._memoized);
        _isFinished = other._isFinished;

        other._isFinished = 0;
    } 
    return *this;
}

template <typename T>
T LazySequence<T>::WhereGenerator::getNext() {
    if (!_isFinished) { 
        if (!hasNext()) { 
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
        auto res = _memoized.get();
        _memoized = Option<T>();
        return res;
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
}

template <typename T>
T LazySequence<T>::WhereGenerator::get( const Ordinal& index ) {
    throw Exception( Exception::ErrorCode::UNKNOWN_ORDINALITY );
    //? maybe too strict, but causing a potentially huge change of inner state of generator or even invalidating it
    //? for getting some element is not really cool
}

template <typename T>
bool LazySequence<T>::WhereGenerator::hasNext() {
    if (!_parent->canMemoiseNext() || _isFinished) {
        return false;
    } else {
        auto allowance = 0;
        while (_parent->canMemoiseNext()) {
            auto candidate = _parent->memoiseNext();
            if (_predicate(candidate)) {
                _memoized = candidate;
                return true;
            }
            if (++allowance > 500000) {
                _isFinished = true;
                break;
            }
        }
        return false;
    }
}