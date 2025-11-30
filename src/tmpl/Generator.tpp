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
    if (_lastMaterialized >= _data.getSize()) {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    return _data[static_cast<size_t>(_lastMaterialized++)];
}

template <typename T>
T LazySequence<T>::FiniteGenerator::get( const Ordinal& index ) {
    if (index >= _data.getSize()) {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
    return _data[static_cast<size_t>(index)];
}

template <typename T>
bool LazySequence<T>::FiniteGenerator::hasNext() {
    return _lastMaterialized < _data.getSize();
}

template <typename T>
Option<T> LazySequence<T>::FiniteGenerator::tryGetNext() {
    if (hasNext()) {
        return Option( _data[static_cast<size_t>(_lastMaterialized++)] );
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
T LazySequence<T>::InfiniteGenerator::get( const Ordinal& index ) { //TODO all occurences of accessing getNext() or hasNext() methods need to be replaced with 
                                                                    //TODO just calling get(_lastMaterialized++) or smth to avoid losing materialized elements
                                                                    //TODO since members like _initial and _added are no more generators but lazySequences.
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
    _initial = parent;
    _added = makeShared<FiniteGenerator>(value);
    _border = border;
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::AppendGenerator::AppendGenerator( const LazySequence<T>& value, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& border ) {
    _initial = parent;
    _added = value._generator;
    _border = border;
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::AppendGenerator::AppendGenerator( const AppendGenerator& other ) {
    _initial = other._initial;
    _added = other._added;
    _border = other._border;
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::AppendGenerator& LazySequence<T>::AppendGenerator::operator=( const AppendGenerator& other ) {
    if (this != &other) {
        _initial = other._initial;
        _added = other._added;
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
    _lastMaterialized++;
    if (_initial->hasNext()) {
        return _initial->getNext();
    } else if (_added->hasNext()) {
        return _added->getNext();
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
}

template <typename T>
T LazySequence<T>::AppendGenerator::get( const Ordinal& index ) {
    if (_border.hasValue()) {
        if (index > _border) {
            return _added->get(index);
        } else {
            return _initial->get(index); // пересчитать индекс для второй последовательности
        }
    } else {
        throw Exception( Exception::ErrorCode::UNKNOWN_ORDINALITY );
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
LazySequence<T>::PrependGenerator::PrependGenerator( const T& value, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& border ) {
    _added = makeShared<FiniteGenerator>( value );
    _initial = parent;
    _lastMaterialized = 0;
    _border = border;
}

template <typename T>
LazySequence<T>::PrependGenerator::PrependGenerator( const LazySequence<T>& value , SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& border ) {
    _added = value._generator;
    _initial = parent;
    _lastMaterialized = 0;
    _border = border;
}

template <typename T>
LazySequence<T>::PrependGenerator::PrependGenerator( const PrependGenerator& other ) {
    _initial = other._initial;
    _added = other._added;
    _border = other._border;
    _lastMaterialized = 0;
}

template <typename T>
LazySequence<T>::PrependGenerator& LazySequence<T>::PrependGenerator::operator=( const PrependGenerator& other ) {
    if (this != &other) {
        _initial = other._initial;
        _added = other._added;
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
    _lastMaterialized++;
    if (_added->hasNext()) {
        return _added->getNext();
    } else if (_initial->hasNext()) {
        return _initial->getNext();
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
}

template <typename T>
T LazySequence<T>::PrependGenerator::get( const Ordinal& index ) {
    if (_border.hasValue()) {
            if (index > _border) {
            return _initial->get(index);
        } else {
            return _added->get(index);
        }
    } else {
        throw Exception( Exception::ErrorCode::UNKNOWN_ORDINALITY );
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
LazySequence<T>::InsertGenerator::InsertGenerator( const T& value, const Ordinal& index, SharedPtr<LazySequence<T>> parent ) {
    _initial = parent;
    _added = makeShared<FiniteGenerator>(value);
    _lastMaterialized = 0;
    _targetIndex = index;
    _border = _targetIndex + 1;
}

template <typename T>
LazySequence<T>::InsertGenerator::InsertGenerator( const LazySequence<T>& value, const Ordinal& index, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& addedOrdinality ) {
    _initial = parent;
    _added = value._generator;
    _lastMaterialized = 0;
    _targetIndex = index;
    _border = addedOrdinality.hasValue() ? _targetIndex + addedOrdinality : Option<Ordinal>();
}

template <typename T>
LazySequence<T>::InsertGenerator::InsertGenerator( const InsertGenerator& other ) {
    _initial = other._initial;
    _added = other._added;
    _border = other._border;
    _lastMaterialized = other._lastMaterialized;
    _targetIndex = other._targetIndex;
}

template <typename T>
LazySequence<T>::InsertGenerator& LazySequence<T>::InsertGenerator::operator=( const InsertGenerator& other ) {
    if (this != &other) {
        _initial = other._initial;
        _added = other._added;
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
    auto current = _lastMaterialized++;
    if (current <  _targetIndex ||
        current >= _targetIndex && _initial->hasNext()) {
        return _initial->getNext();
    } else if (current >= _targetIndex && _added->hasNext()) {
        return _added->getNext();
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
}

template <typename T>
T LazySequence<T>::InsertGenerator::get( const Ordinal& index ) {
    if (index < _targetIndex) { return _initial->get( index ); } 
    if (_border.hasValue()) {
        if (index > _border) {
            return _initial->get( index - _border + _targetIndex );
        } else if (index >= _targetIndex && index <= _border) {
            return _added->get( index - _targetIndex );
        }
    } else {
        throw Exception( Exception::ErrorCode::UNKNOWN_ORDINALITY );
    }
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
        return _parent->getNext();
    } else if (current == _from) {
        if (_to.isFinite()) {
            for (size_t i = 0; i < _to - _from - 1; i++) {
                _parent->getNext();
            }
            return _parent->getNext();
        } else {
            return _parent->get( _to + current );
        }
    } else {
        if (_to.isFinite()) {
            return _parent->getNext();
        } else {
            return _parent->get( _to + current - _from );
        }
    }
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
LazySequence<T>::SubSequenceGenerator::SubSequenceGenerator( const Ordinal& start, const Ordinal& end, SharedPtr<LazySequence<T>> parent ) {
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
    _parent = other._parent;
}

template <typename T>
LazySequence<T>::SubSequenceGenerator& LazySequence<T>::SubSequenceGenerator::operator=( const SubSequenceGenerator& other ) {
    if (this != &other) {
        _lastMaterialized = other._lastMaterialized;
        _from = other._from;
        _to = other._to;
        _parent = other._parent;
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
    auto current = _lastMaterialized++;
    if (_from.isFinite()) {
        if ( current == 0 ) {
            while ( current < _from - 1 ) {
                _parent->getNext();
            }
            return _parent->getNext();
        } else if ( current < _to - _from ) {
            return _parent->getNext(); 
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
        return Option<T>(getNext());
    } else {
        return Option<T>();
    }
}

template <typename T>
LazySequence<T>::ConcatGenerator::ConcatGenerator( SharedPtr<LazySequence<T>> first, SharedPtr<LazySequence<T>> second, const Option<Ordinal>& border ) {
    _border = border;
    _first = first;
    _second = second;
}

template <typename T>
LazySequence<T>::ConcatGenerator::ConcatGenerator( const ConcatGenerator& other ) {
    _border = other._border;
    _first = other._first;
    _second = other._second;
}

template <typename T>
LazySequence<T>::ConcatGenerator& LazySequence<T>::ConcatGenerator::operator=( const ConcatGenerator& other ) {
    if (this != &other) {
        _border = other._border;
        _first = other._first;
        _second = other._second;
    }
    return *this;
}

template <typename T>
LazySequence<T>::ConcatGenerator::ConcatGenerator( ConcatGenerator&& other ) {
    _border = other._border;
    other._border = 0;
    _first = std::move(other._first);
    _second = std::move(other._second);
}

template <typename T>
LazySequence<T>::ConcatGenerator& LazySequence<T>::ConcatGenerator::operator=( ConcatGenerator&& other ) {
    if (this != &other) {
        _border = other._border;
        other._border = 0;
        _first = std::move(other._first);
        _second = std::move(other._second);
    }
    return *this;
}

template <typename T>
T LazySequence<T>::ConcatGenerator::getNext() {
    if (_first->hasNext()) {
        return _first->getNext();
    } else if (_second->hasNext()) {
        return _second->getNext();
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
    return _first->hasNext() || _second->hasNext();
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
    _func = func;
}

template <typename T>
template <typename T2>
LazySequence<T>::MapGenerator<T2>::MapGenerator( const MapGenerator<T2>& other ) {
    _parent = other._parent;
    _func = other._func;
}

template <typename T>
template <typename T2>
LazySequence<T>::MapGenerator<T2>& LazySequence<T>::MapGenerator<T2>::operator=( const MapGenerator<T2>& other ) {
    if (this != &other) {
        _parent = other._parent;
        _func = other._func;
    }
    return *this;
}

template <typename T>
template <typename T2>
LazySequence<T>::MapGenerator<T2>::MapGenerator( MapGenerator<T2>&& other ) {
    _parent = std::move(other._parent);
    _func = std::move(other._func);
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
    return _func( _parent->getNext() );
}

template <typename T>
template <typename T2>
T2 LazySequence<T>::MapGenerator<T2>::get( const Ordinal& index ) {
    return _func( _parent->get( index ) );
}

template <typename T>
template <typename T2>
bool LazySequence<T>::MapGenerator<T2>::hasNext() {
    return _parent->hasNext();
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
, _memoized( Option<T>() ) {}

template <typename T>
LazySequence<T>::WhereGenerator::WhereGenerator( const WhereGenerator& other )
: _predicate( other._predicate )
, _parent( other._parent )
, _memoized( other._memoized ) {}

template <typename T>
LazySequence<T>::WhereGenerator& LazySequence<T>::WhereGenerator::operator=( const WhereGenerator& other ) {
    if (this != &other) {
        _predicate = other._predicate;
        _parent = other._parent;
        _memoized = other._memoized;
    } 
    return *this;
}

template <typename T>
LazySequence<T>::WhereGenerator::WhereGenerator( WhereGenerator&& other )
: _predicate( std::move( other._predicate ))
, _parent( std::move( other._parent ))
, _memoized( std::move( other._memoized )) {}

template <typename T>
LazySequence<T>::WhereGenerator& LazySequence<T>::WhereGenerator::operator=( WhereGenerator&& other ) {
    if (this != &other) {
        _predicate = std::move(other._predicate);
        _parent = std::move(other._parent);
        _memoized = std::move(other._memoized);
    } 
    return *this;
}

template <typename T>
T LazySequence<T>::WhereGenerator::getNext() {
    if (_memoized.hasValue()) {
        auto res = _memoized.get();
        _memoized = Option<T>();
        return res;
    }
    while (true) {
        if (!hasNext()) { throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); }
        auto res = _parent->getNext();
        if (_predicate(res)) {
            return res;
        }
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
    if (!_parent->hasNext()) {
        return false;
    } else {
        while (_parent->hasNext()) {
            auto candidate = _parent->getNext();
            if (_predicate(candidate)) {
                _memoized = candidate;
                return true;
            }
        }
        return false;
    }
}