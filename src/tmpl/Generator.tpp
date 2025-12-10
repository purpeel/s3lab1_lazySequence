template <typename T>
FiniteGenerator<T>::FiniteGenerator() {
    _data = ArraySequence<T>();
    _lastMaterialized = 0;
}

template <typename T>
FiniteGenerator<T>::FiniteGenerator( ArraySequence<T>&& data ) {
    _data = ArraySequence<T>( std::move(data) );
    _lastMaterialized = 0;
}

template <typename T>
FiniteGenerator<T>::FiniteGenerator( const T& value ) {
    _data = ArraySequence<T>();
    _data.append(value);
    _lastMaterialized = 0;
}

template <typename T>
FiniteGenerator<T>::FiniteGenerator( const FiniteGenerator<T>& other ) {
    _data = other._data;
    _lastMaterialized = other._lastMaterialized;
}

template <typename T>
FiniteGenerator<T>& FiniteGenerator<T>::operator=( const FiniteGenerator<T>& other ) {
    if (this != &other) {
        _data = other._data;
        _lastMaterialized = other._lastMaterialized;
    }
    return *this;
}

template <typename T>
FiniteGenerator<T>::FiniteGenerator( FiniteGenerator<T>&& other ) {
    _data = std::move(other._data);
    _lastMaterialized = other._lastMaterialized;
    other._lastMaterialized = 0;
}

template <typename T>
FiniteGenerator<T>& FiniteGenerator<T>::operator=( FiniteGenerator<T>&& other ) {
    if (this != &other) {
        _data = std::move(other._data);
        _lastMaterialized = other._lastMaterialized;
        other._lastMaterialized = 0;
    }
    return *this;
}

template <typename T>
T FiniteGenerator<T>::getNext() {
    if (!hasNext()) { throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); }
    return _data[static_cast<size_t>(_lastMaterialized++)];
}

template <typename T>
T FiniteGenerator<T>::get( const Ordinal& index ) {
    return _data[static_cast<size_t>(index)];
}

template <typename T>
bool FiniteGenerator<T>::hasNext() {
    return _lastMaterialized < _data.getSize();
}

template <typename T>
Option<T> FiniteGenerator<T>::tryGetNext() {
    if (hasNext()) {
        return Option( getNext() );
    } else {
        return Option<T>();
    }
}

template <typename T>
InfiniteGenerator<T>::InfiniteGenerator( const InfiniteGenerator<T>& other ) 
: _arity(other._arity), _lastMaterialized(0)
, _window(other._window)
, _producingFunc(other._producingFunc) {}

template <typename T>
InfiniteGenerator<T>& InfiniteGenerator<T>::operator=( const InfiniteGenerator<T>& other ) {
    if (this != &other) {
        _arity            = other._arity;
        _lastMaterialized = 0;
        _window           = other._window;
        _producingFunc    = other._producingFunc;
    }
    return *this;
}

template <typename T>
InfiniteGenerator<T>::InfiniteGenerator( InfiniteGenerator<T>&& other ) {
    _arity            = other._arity;
    _lastMaterialized = other._lastMaterialized;
    _window        = std::move(other._window);
    _producingFunc = std::move(other._producingFunc);
    other._arity            = 0;
    other._lastMaterialized = 0;
}

template <typename T>
InfiniteGenerator<T>& InfiniteGenerator<T>::operator=( InfiniteGenerator<T>&& other ) {
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
InfiniteGenerator<T>::InfiniteGenerator( const size_t arity
                                       , const std::function<T(ArraySequence<T>&)>& func
                                       , const ArraySequence<T>& data ) : _arity(arity), _producingFunc(func) {
    _window = *static_cast<ArraySequence<T>*>(data.getSubSequence(0, arity));
}

template <typename T>
T InfiniteGenerator<T>::getNext() {
    auto next = _producingFunc( _window );
    _window.removeAt(0);
    _window.append(next);
    _lastMaterialized++;
    return next;
}

template <typename T>
T InfiniteGenerator<T>::get( const Ordinal& index ) {
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
bool InfiniteGenerator<T>::hasNext() {
    return true;
}

template <typename T>
Option<T> InfiniteGenerator<T>::tryGetNext() {
    return Option<T>( getNext() );
}

template <typename T>
AppendGenerator<T>::AppendGenerator( const T& value, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& border ) {
    _initial    = parent;
    _added      = makeShared<LazySequence<T>>(value);
    _border     = border;
    _lastMaterialized = 0;
}

template <typename T>
AppendGenerator<T>::AppendGenerator( const LazySequence<T>& value, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& border ) {
    _initial    = parent;
    _added      = makeShared<LazySequence<T>>(value);    
    _border     = border;
    _lastMaterialized = 0;
}

template <typename T>
AppendGenerator<T>::AppendGenerator( const AppendGenerator<T>& other ) {
    _initial    = other._initial;
    _added      = other._added;
    _border     = other._border;
    _lastMaterialized = 0;
}

template <typename T>
AppendGenerator<T>& AppendGenerator<T>::operator=( const AppendGenerator<T>& other ) {
    if (this != &other) {
        _initial    = other._initial;
        _added      = other._added;
        _border     = other._border;
        _lastMaterialized = 0;
    }
    return *this;
}

template <typename T>
AppendGenerator<T>::AppendGenerator( AppendGenerator<T>&& other ) {
    _initial    = std::move(other._initial);
    _added      = std::move(other._added);
    _lastMaterialized = other._lastMaterialized;
    _border = other._border;
    other._border = 0;
    other._lastMaterialized = 0;
}

template <typename T>
AppendGenerator<T>& AppendGenerator<T>::operator=( AppendGenerator<T>&& other ) {
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
T AppendGenerator<T>::getNext() {
    if ( !hasNext() ) { throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); }
    if (_initial->canMemoiseNext()) {
        return _initial->memoiseNext();
    } else if (_added->canMemoiseNext()) {
        return _added->memoiseNext();
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
}

template <typename T>
T AppendGenerator<T>::get( const Ordinal& index ) {
    if (_border.hasValue()) {
        if (index >= _border.get()) {
            return _added->get( index - _border.get() );
        } else {
            return _initial->get( index );
        }
    } else {
        throw Exception( Exception::ErrorCode::UNKNOWN_ORDINALITY );
    }
}

template <typename T>
bool AppendGenerator<T>::hasNext() {
    return _initial->canMemoiseNext() || _added->canMemoiseNext();
}

template <typename T>
Option<T> AppendGenerator<T>::tryGetNext() {
    if (hasNext()) {
        return Option<T>( getNext() );
    } else {
        return Option<T>();
    }
}

template <typename T>
PrependGenerator<T>::PrependGenerator( const T& value, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& border ) {
    _initial    = parent;
    _added      = makeShared<LazySequence<T>>( value );
    _border     = border;
    _lastMaterialized = 0;
}

template <typename T>
PrependGenerator<T>::PrependGenerator( const LazySequence<T>& value , SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& border ) {
    _initial    = parent;
    _added      = makeShared<LazySequence<T>>( value );
    _border     = border;
    _lastMaterialized = 0;
}

template <typename T>
PrependGenerator<T>::PrependGenerator( const PrependGenerator<T>& other ) {
    _initial    = other._initial;
    _added      = other._added;
    _border     = other._border;
    _lastMaterialized = other._lastMaterialized;
}

template <typename T>
PrependGenerator<T>& PrependGenerator<T>::operator=( const PrependGenerator<T>& other ) {
    if (this != &other) {    
        _initial    = other._initial;
        _added      = other._added;
        _border     = other._border;
        _lastMaterialized = other._lastMaterialized;
    }
    return *this;
}

template <typename T>
PrependGenerator<T>::PrependGenerator( PrependGenerator<T>&& other ) {
    _initial    = std::move(other._initial);
    _added      = std::move(other._added);
    _border     = other._border;
    _lastMaterialized = other._lastMaterialized;

    other._lastMaterialized = 0;
    other._border = 0;

}

template <typename T>
PrependGenerator<T>& PrependGenerator<T>::operator=( PrependGenerator<T>&& other ) {
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
T PrependGenerator<T>::getNext() {
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
T PrependGenerator<T>::get( const Ordinal& index ) {
    if (_border.hasValue()) {
        if (index >= _border.get()) {
            return _initial->get( index - _border.get() );
        } else {
            return _added->get( index );
        }
    } else {
        throw Exception( Exception::ErrorCode::UNKNOWN_ORDINALITY );
    }
}

template <typename T>
bool PrependGenerator<T>::hasNext() {
    return _added->canMemoiseNext() || _initial->canMemoiseNext();
}

template <typename T>
Option<T> PrependGenerator<T>::tryGetNext() {
    if (hasNext()) {
        return Option<T>( getNext() );
    } else {
        return Option<T>();
    }
}

template <typename T>
InsertGenerator<T>::InsertGenerator( const T& value, const Ordinal& index, SharedPtr<LazySequence<T>> parent ) {
    _initial     = parent;
    _added       = makeShared<LazySequence<T>>(value);
    _targetIndex = index;
    _border      = _targetIndex + 1;
    _lastMaterialized = 0;
}

template <typename T>
InsertGenerator<T>::InsertGenerator( const LazySequence<T>& value, const Ordinal& index, SharedPtr<LazySequence<T>> parent, const Option<Ordinal>& addedOrdinality ) {
    _initial     = parent;
    _added       = value._generator;
    _targetIndex = index;
    _border      = addedOrdinality.hasValue() ? _targetIndex + addedOrdinality : Option<Ordinal>();
    _lastMaterialized = 0;
}

template <typename T>
InsertGenerator<T>::InsertGenerator( const InsertGenerator<T>& other ) {
    _initial     = other._initial;
    _added       = other._added;
    _targetIndex = other._targetIndex;
    _border      = other._border;
    _lastMaterialized = other._lastMaterialized;
}

template <typename T>
InsertGenerator<T>& InsertGenerator<T>::operator=( const InsertGenerator<T>& other ) {
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
InsertGenerator<T>::InsertGenerator( InsertGenerator<T>&& other ) {
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
InsertGenerator<T>& InsertGenerator<T>::operator=( InsertGenerator<T>&& other ) {
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
T InsertGenerator<T>::getNext() {
    if ( !hasNext() ) { throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); }
    auto current = _lastMaterialized++;
    if ((current <  _targetIndex || (_border.hasValue() && current >= _border.get())) 
        && _initial->canMemoiseNext())  {
        return _initial->memoiseNext();
    } else if ((current >= _targetIndex && _border.hasValue() && current < _border.get()) 
                && _added->canMemoiseNext()) {
        return _added->memoiseNext();
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
}

template <typename T>
T InsertGenerator<T>::get( const Ordinal& index ) {
    if (index < _targetIndex) { return (*_initial)[index]; } 
    if (_border.hasValue()) {
        auto border = _border.get();
        if (index >= border) {
            return _initial->get(index - border + _targetIndex); //?? probably incorrect index logic, but firstly need to make _ordinality correct
        } else if (index >= _targetIndex && index < border) {
            return _added->get(index - _targetIndex);
        } else if (index < _targetIndex) { 
            return (*_initial)[index];
        } else {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
    } else {
        throw Exception( Exception::ErrorCode::UNKNOWN_ORDINALITY );
    }
}

template <typename T>
bool InsertGenerator<T>::hasNext() {
    return _initial->canMemoiseNext() || _added->canMemoiseNext();
}

template <typename T>
Option<T> InsertGenerator<T>::tryGetNext() {
    if ( hasNext() ) {
        return Option<T>( getNext() );
    } else {
        return Option<T>();
    }
}

template <typename T>
SkipGenerator<T>::SkipGenerator( const Ordinal& index, SharedPtr<LazySequence<T>> parent ) {
    _lastMaterialized = 0;
    _from = index;
    _to = index + 1;
    _parent = parent;
}

template <typename T>
SkipGenerator<T>::SkipGenerator( const Ordinal& start, const Ordinal& end, SharedPtr<LazySequence<T>> parent ) {
    _lastMaterialized = 0;
    _from = start;
    _to = end;
    _parent = parent;
}

template <typename T>
SkipGenerator<T>::SkipGenerator( const SkipGenerator<T>& other ) {
    _lastMaterialized = other._lastMaterialized;
    _from = other._from;
    _to = other._to;
    _parent = other._parent;
}

template <typename T>
SkipGenerator<T>& SkipGenerator<T>::operator=( const SkipGenerator<T>& other ) {
    if (this != &other) {
        _lastMaterialized = other._lastMaterialized;
        _from = other._from;
        _to = other._to;
        _parent = other._parent;
    }
    return *this;
}

template <typename T>
SkipGenerator<T>::SkipGenerator( SkipGenerator<T>&& other ) {
    _lastMaterialized = other._lastMaterialized;
    _from = other._from;
    _to = other._to;
    _parent = std::move(other._parent);
    other._from = 0;
    other._to = 0;
    other._lastMaterialized = 0;
}

template <typename T>
SkipGenerator<T>& SkipGenerator<T>::operator=( SkipGenerator<T>&& other ) {
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
T SkipGenerator<T>::getNext() {
    if (!hasNext()) { throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); }
    auto current = _lastMaterialized++;
    if (current < _from) {
        return _parent->memoiseNext();
    } else if (current == _from) {
        if (_to.isFinite()) {
            for (size_t i = 0; i < _to - _from; i++) {
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
            return _parent->get( _to - _from + current );
        }
    }
}

template <typename T>
T SkipGenerator<T>::get( const Ordinal& index ) {
    if (index < _from) {
        return _parent->get(index);
    } else {
        return _parent->get( _to - _from + index );
    }
}

template <typename T>
bool SkipGenerator<T>::hasNext() {
    return _parent->canMemoiseNext();
}

template <typename T>
Option<T> SkipGenerator<T>::tryGetNext() {
    if (hasNext()) {
        return Option<T>( getNext() );
    } else {
        return Option<T>();
    }
}

template <typename T>
SubSequenceGenerator<T>::SubSequenceGenerator( const Ordinal& start, const Ordinal& end, SharedPtr<LazySequence<T>> parent ) {
    _from   = start;
    _to     = end;
    _parent = parent;
    _lastMaterialized = 0;
}

template <typename T>
SubSequenceGenerator<T>::SubSequenceGenerator( const SubSequenceGenerator<T>& other ) {
    _from   = other._from;
    _to     = other._to;
    _parent = other._parent;
    _lastMaterialized = other._lastMaterialized;
}

template <typename T>
SubSequenceGenerator<T>& SubSequenceGenerator<T>::operator=( const SubSequenceGenerator<T>& other ) {
    if (this != &other) {
        _from   = other._from;
        _to     = other._to;
        _parent = other._parent;
        _lastMaterialized = other._lastMaterialized;
    }
    return *this;
}

template <typename T>
SubSequenceGenerator<T>::SubSequenceGenerator( SubSequenceGenerator<T>&& other ) {
    _from   = other._from;
    _to     = other._to;
    _parent = std::move(other._parent);
    _lastMaterialized = other._lastMaterialized;

    other._from = 0;
    other._to   = 0;
    other._lastMaterialized = 0;
}

template <typename T>
SubSequenceGenerator<T>& SubSequenceGenerator<T>::operator=( SubSequenceGenerator<T>&& other ) {
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
T SubSequenceGenerator<T>::getNext() {
    if (!hasNext()) { throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); }
    auto current = _lastMaterialized++;
    if (_from.isFinite()) {
        if ( current == 0 ) {
            while ( current < _from ) {
                _parent->memoiseNext();
                current++;
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
T SubSequenceGenerator<T>::get( const Ordinal& index ) {
    if (index >= _from || index < _to) {
        return _parent->get(_from + index);
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
}

template <typename T>
bool SubSequenceGenerator<T>::hasNext() {
    return _from + _lastMaterialized < _to;
}

template <typename T>
Option<T> SubSequenceGenerator<T>::tryGetNext() {
    if (hasNext()) {
        return Option<T>( getNext() );
    } else {
        return Option<T>();
    }
}

template <typename T>
ConcatGenerator<T>::ConcatGenerator( SharedPtr<LazySequence<T>> first, SharedPtr<LazySequence<T>> second, const Option<Ordinal>& border ) {
    _border = border;
    _first  = first;
    _second = second;
}

template <typename T>
ConcatGenerator<T>::ConcatGenerator( const ConcatGenerator<T>& other ) {
    _border = other._border;
    _first  = other._first;
    _second = other._second;
}

template <typename T>
ConcatGenerator<T>& ConcatGenerator<T>::operator=( const ConcatGenerator<T>& other ) {
    if (this != &other) {
        _border = other._border;
        _first  = other._first;
        _second = other._second;
    }
    return *this;
}

template <typename T>
ConcatGenerator<T>::ConcatGenerator( ConcatGenerator<T>&& other ) {
    _border = other._border;
    _first  = std::move(other._first);
    _second = std::move(other._second);

    other._border = 0;
}

template <typename T>
ConcatGenerator<T>& ConcatGenerator<T>::operator=( ConcatGenerator<T>&& other ) {
    if (this != &other) {
        _border = other._border;
        _first  = std::move(other._first);
        _second = std::move(other._second);

        other._border = 0;
    }
    return *this;
}

template <typename T>
T ConcatGenerator<T>::getNext() {
    if (_first->canMemoiseNext()) {
        return _first->memoiseNext();
    } else if (_second->canMemoiseNext()) {
        return _second->memoiseNext();
    } else {
        throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
    }
}

template <typename T>
T ConcatGenerator<T>::get( const Ordinal& index ) {
    if (_border.hasValue()) {
        if (index <= _border.get()) {
            return _first->get(index);
        } else {
            return _second->get(index - _border.get());
        }
    } else {
        throw Exception( Exception::ErrorCode::UNKNOWN_ORDINALITY );
    }
}

template <typename T>
bool ConcatGenerator<T>::hasNext() {
    return _first->canMemoiseNext() || _second->canMemoiseNext();
}

template <typename T>
Option<T> ConcatGenerator<T>::tryGetNext() {
    if (hasNext()) {
        return Option<T>( getNext());
    } else {
        return Option<T>();
    }
}

template <typename TIn, typename TOut>
MapGenerator<TIn, TOut>::MapGenerator( const std::function<TOut(TIn)>& func, SharedPtr<LazySequence<TIn>> parent ) {
    _parent = parent;
    _func   = func;
}

template <typename TIn, typename TOut>
MapGenerator<TIn, TOut>::MapGenerator( const MapGenerator<TIn, TOut>& other ) {
    _parent = other._parent;
    _func   = other._func;
}

template <typename TIn, typename TOut>
MapGenerator<TIn, TOut>& MapGenerator<TIn, TOut>::operator=( const MapGenerator<TIn, TOut>& other ) {
    if (this != &other) {
        _parent = other._parent;
        _func   = other._func;
    }
    return *this;
}

template <typename TIn, typename TOut>
MapGenerator<TIn, TOut>::MapGenerator( MapGenerator<TIn, TOut>&& other ) {
    _parent = std::move(other._parent);
    _func   = std::move(other._func);
}

template <typename TIn, typename TOut>
MapGenerator<TIn, TOut>& MapGenerator<TIn, TOut>::operator=( MapGenerator<TIn, TOut>&& other ) {
    if (this != &other) {
        _parent = std::move(other._parent);
        _func = std::move(other._func);
    }
    return *this;
}

template <typename TIn, typename TOut>
TOut MapGenerator<TIn, TOut>::getNext() {
    return _func( _parent->memoiseNext() );
}

template <typename TIn, typename TOut>
TOut MapGenerator<TIn, TOut>::get( const Ordinal& index ) {
    return _func( _parent->get( index ) );
}

template <typename TIn, typename TOut>
bool MapGenerator<TIn, TOut>::hasNext() {
    return _parent->canMemoiseNext();
}

template <typename TIn, typename TOut>
Option<TOut> MapGenerator<TIn, TOut>::tryGetNext() {
    if (hasNext()) {
        return Option<TOut>( getNext() );
    } else {
        return Option<TOut>();
    }
}

template <typename T>
WhereGenerator<T>::WhereGenerator( const std::function<bool(T)>& func, SharedPtr<LazySequence<T>> parent )
: _predicate( func )
, _parent( parent )
, _memoized( Option<T>() )
, _isFinished( false ) {}

template <typename T>
WhereGenerator<T>::WhereGenerator( const WhereGenerator<T>& other )
: _predicate( other._predicate )
, _parent( other._parent )
, _memoized( other._memoized ) 
, _isFinished( other._isFinished ){}

template <typename T>
WhereGenerator<T>& WhereGenerator<T>::operator=( const WhereGenerator<T>& other ) {
    if (this != &other) {
        _predicate  = other._predicate;
        _parent     = other._parent;
        _memoized   = other._memoized;
        _isFinished = other._isFinished;
    } 
    return *this;
}

template <typename T>
WhereGenerator<T>::WhereGenerator( WhereGenerator<T>&& other )
: _predicate( std::move( other._predicate ))
, _parent( std::move( other._parent ))
, _memoized( std::move( other._memoized )) 
, _isFinished( other._isFinished ) { other._isFinished = false; }

template <typename T>
WhereGenerator<T>& WhereGenerator<T>::operator=( WhereGenerator<T>&& other ) {
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
T WhereGenerator<T>::getNext() {
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
T WhereGenerator<T>::get( const Ordinal& index ) {
    if (index.isFinite()) {}
    throw Exception( Exception::ErrorCode::UNKNOWN_ORDINALITY );
}

template <typename T>
bool WhereGenerator<T>::hasNext() {
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

template <typename T>
Option<T> WhereGenerator<T>::tryGetNext() {
    if (hasNext()) {
        return Option<T>( getNext());
    } else {
        return Option<T>();
    }
}