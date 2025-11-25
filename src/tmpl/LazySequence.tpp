template <typename T>
LazySequence<T>::LazySequence() 
: _size(0), _offset(0), _isOfKnownOrdinality(true)
, _ordinality(0)
, _generator(SharedPtr<FiniteGenerator>())
, _items( makeUnique<ArraySequence<T>>() ) {}

template <typename T>
LazySequence<T>::LazySequence( const T& value ) 
: _size(0), _offset(0), _isOfKnownOrdinality(true)
, _ordinality(1)
, _generator(SharedPtr<FiniteGenerator>( value ))
, _items( makeUnique<ArraySequence<T>>() ) {}

template <typename T>
LazySequence<T>::LazySequence( const ArraySequence<T>& data ) 
: _size(data.getSize()), _offset(0), _isOfKnownOrdinality(true)
, _ordinality(data.getSize())
, _generator( makeShared<FiniteGenerator>(data) )
, _items( makeUnique<ArraySequence<T>>() ) {}

template <typename T>
LazySequence<T>::LazySequence( const size_t arity
                             , const std::function<T(ArraySequence<T>&)>& func
                             , const ArraySequence<T>& data ) 
: _size( Cardinal::infiniteCardinal::BETH_0 ), _offset(0), _isOfKnownOrdinality(true)
, _ordinality( Ordinal::omega() )
, _generator( makeShared<InfiniteGenerator>(arity, func, data) )
, _items( makeUnique<ArraySequence<T>>() ) {}

template <typename T>
LazySequence<T>::LazySequence( UniquePtr<IGenerator>&& generator
                       , const Cardinal& size
                       , const Option<Ordinal>& ordinality ) 
: _size( size ), _offset(0)
, _ordinality( ordinality )
, _generator( std::move(generator) )
, _items( makeUnique<ArraySequence<T>>() ) {}

template <typename T>
LazySequence<T>::LazySequence( const LazySequence<T>& other ) {
    _generator = other._generator;
    *_items    = *other._items;
    _ordinality = other._ordinality;
    _offset = other._offset;
    _size   = other._size;
}

template <typename T>
LazySequence<T>& LazySequence<T>::operator=( const LazySequence<T>& other ) {
    if (this != &other) {
        _generator = other._generator;
        *_items    = *other._items;
        _ordinality = other._ordinality;
        _offset = other._offset;
        _size   = other._size;
    }
    return *this;
}

template <typename T>
LazySequence<T>::LazySequence( LazySequence<T>&& other ) {
    _generator = std::move(other._generator);
    _items     = std::move(other._items);
    _offset = other._offset;
    _size   = other._size;
    _ordinality = other._ordinality;
    other._offset = 0;
    other._size   = 0;
}

template <typename T>
LazySequence<T>& LazySequence<T>::operator=( LazySequence<T>&& other ) {
    if (this != &other) {
        _generator = std::move(other._generator);
        _items     = std::move(other._items);
        _offset = other._offset;
        _size   = other._size;
        _ordinality = other._ordinality;
        other._offset = 0;
        other._size   = 0;
    }
    return *this;
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::create() {
    auto res = makeShared<LazySequence<T>>();
    return res;
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::create( const T& value ) {
    auto res = makeShared<LazySequence<T>>( value );
    return res;
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::create( const ArraySequence<T>& data ) {
    auto res = makeShared<LazySequence<T>>(data);
    return res;
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::create( const size_t arity
                                                  , const std::function<T(ArraySequence<T>&)>& producingFunc
                                                  , const ArraySequence<T>& data ) {
    auto res = makeShared<LazySequence<T>>(arity, producingFunc, data);
    return res;
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::create( UniquePtr<IGenerator>&& generator
                                            , const Cardinal& size
                                            , const Option<Ordinal>& ordinality  ) {
    auto res = makeShared<LazySequence<T>>( std::move(generator), size, ordinality );
    return res;
}

template <typename T>
const T& LazySequence<T>::getFirst() {
    if (_offset > 0) {
        return _generator->get(0);
    } else {
        return (*_items)[0];
    }
}

template <typename T>
const T& LazySequence<T>::getLast() {
    if (!isFinite()) {
        throw Exception( Exception::ErrorCode::INFINITE_CALCULATION )
    } else {
        return _generator->get(getSize() - 1);
    }
}

template <typename T>
const T& LazySequence<T>::operator[]( const Ordinal& index ) {
    if (_ordinality.hasValue()) {
        if (index < 0 || index > _ordinality.get()) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); 
        }
        if (index.isFinite()) {
            if (_offset < index && index < _offset + _items->getSize() - 1) {
                return (*_items)[static_cast<size_t>(index) - _offset]
            } else if (index > _offset + _items->getSize() - 1) {
                while (index > _offset + _items->getSize() - 1) {
                    _items->append( _generator->getNext() );
                }
                return (*_items)[static_cast<size_t>(index) - _offset];
            } else {
                throw Exception( Exception::ErrorCode::DEMATERIALIZED_ACCESS );
            }
        } else {
            return _generator->get( index );
        }
    } else { 
        if (index.isTransfinite()) {
            throw Exception( Exception::ErrorCode::UNKNOWN_ORDINALITY );
        } else if (index < 0) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); 
        } 
        if (_offset < index && index < _offset + _items->getSize() - 1) {
            return (*_items)[static_cast<size_t>(index) - _offset]
        } else if (index > _offset + _items->getSize() - 1) {
            while (index > _offset + _items->getSize() - 1) {
                _items->append( _generator->getNext() );
            }
            return (*_items)[static_cast<size_t>(index) - _offset];
        } else {
            throw Exception( Exception::ErrorCode::DEMATERIALIZED_ACCESS );
        }
    }
}

template <typename T>
Cardinal LazySequence<T>::getSize() {
    return _size;
}

template <typename T>
size_t LazySequence<T>::getMaterializedCount() { 
    return _generator->_items->getSize();
}

template <typename T>
bool LazySequence<T>::isEmpty() const {
    return getSize() == 0;
}

template <typename T>
bool LazySequence<T>::isFinite() const {
    return getSize().isFinite();
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::append( const T& value ) {
    Option<Ordinal> newOrd = _ordinality.hasValue() 
    ? Option<Ordinal>( _ordinality.get() + 1) 
    : Option<Ordinal>();
    auto gen = makeUnique<AppendGenerator>( value, _generator );
    return create( std::move(gen), _size + 1, newOrd );
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::append( const LazySequence<T>& value ) {
    Option<Ordinal> newOrd = _ordinality.hasValue() && value._ordinality.hasValue() 
    ? Option<Ordinal>(_ordinality.get() + value._ordinality.get()) 
    : Option<Ordinal>(); //TODO change passed ordinality to optional type; throw exceptions in get() when .hasValue() == false;
    auto gen = makeUnique<AppendGenerator>( value, _generator );
    return create( std::move(gen), _size + value.getSize(), newOrd );
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::prepend( const T& value ) {
    auto gen = makeUnique<PrependGenerator>( value, _generator );
    Option<Ordinal> newOrd = _ordinality.hasValue() 
            ? Option<Ordinal>(1 + _ordinality.get()) 
            : Option<Ordinal>();
    return create( std::move(gen), _size + 1, newOrd );
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::prepend( const LazySequence<T>& value ) {
    auto gen = makeUnique<PrependGenerator>( value, _generator );
    Option<Ordinal> newOrd = _ordinality.hasValue() && value._ordinality.hasValue() 
            ? Option<Ordinal>(value._ordinality.get() + _ordinality.get()) 
            : Option<Ordinal>();
    return create( std::move(gen), _size + value.getSize(), newOrd );
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::insertAt( const T& value, const Ordinal& index ) {
    if (_ordinality.hasValue()) {
        if (index < 0 || index > _ordinality.get()) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
        if (index == 0) { return prepend( value ); } 
        else if (index == _ordinality.get()) { return append( value ); } 
        else {
            auto gen = makeUnique<InsertGenerator>( value, index, _generator );
            return create( std::move(gen), _size + 1, _ordinality );
        }
    } else {
        if (index < 0) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        } 
        if (index == 0) { return prepend( value ); }
        else {
            auto gen = makeUnique<InsertGenerator>( value, index, _generator );
            return create( std::move(gen), _size + 1, _ordinality );
        }
    }
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::insertAt( const LazySequence<T>& value, const Ordinal& index ) {
    if (_ordinality.hasValue()) {
        if (index < 0 || index > _ordinality.get()) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
        if (index == 0) { return prepend( value ); } 
        else if (index == _ordinality.get()) { return append( value ); } 
        else { //TODO pass correct ordinality to all constuctors that require it
            auto gen = makeUnique<InsertGenerator>( value, index, _generator,  );
            Option<Ordinal> newOrd = value._ordinality.hasValue() 
                ? Option<Ordinal>(value._ordinality.get() + _ordinality.get()) 
                : Option<Ordinal>();
            return create( std::move(gen), _size + value.getSize(), newOrd );
        }
    } else {
        if (index < 0) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        } 
        if (index == 0) { return prepend( value ); } 
        else {
            auto gen = makeUnique<InsertGenerator>( value, index, _generator );
            return create( std::move(gen), _size + value.getSize(), _ordinality );
        }
    }
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::skip( const Ordinal& index ) {
    if (_ordinality.hasValue()) {
        if (index < 0 || index > _ordinality.get()) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
        auto gen = makeUnique<SkipGenerator>( index, _generator );
        return create( std::move(gen), _size - 1, Option<Ordinal>(_ordinality.get() - 1) );
    } else {
        if (index < 0) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
        auto gen = makeUnique<SkipGenerator>( index, _generator );
        return create( std::move(gen), _size - 1, _ordinality );
    }
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::skip( const Ordinal& start, const Ordinal& end ) {
    if (_ordinality.hasValue()) {
        if (start < 0 || end < 0 || end < start || end > _ordinality.get()) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
        auto gen = makeUnique<SkipGenerator>( start, end, _generator );
        Option<Ordinal> newOrd = Option<Ordinal>(start + (_ordinality.get() - end));
        Cardinal newSize = newOrd.get().isFinite() ? Cardinal( static_cast<size_t>(newOrd) + 1 ) : _size;
        return create( std::move(gen), newSize, newOrd );
    } else {
        if (start < 0 || end < 0 || end < start) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
        auto gen = makeUnique<SkipGenerator>( start, end, _generator );
        return create( std::move(gen), _size - (end - start + 1), _ordinality );
    }
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::getSubSequence( const Ordinal& start, const Ordinal& end ) {
    if (_ordinality.hasValue()) {
        if (start < 0 || end < 0 || end < start || end > _ordinality.get()) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
    } else {
        if (start < 0 || end < 0 || end < start) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
    }
    if (start == end) { return create(); }
    auto gen = makeUnique<SubSequenceGenerator>( start, end, _generator );
    Option<Ordinal> newOrd = Option<Ordinal>(end - start - 1);
    Cardinal newSize = newOrd.isFinite() ? newOrd + 1 : Cardinal::BethNull();
    return create( std::move(gen), newSize, newOrd );
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::concat( const LazySequence<T>& other ) {
    auto gen = makeUnique<ConcatGenerator>( _generator, other._generator );
    Option<Ordinal> newOrd = _ordinality.hasValue() && other._ordinality.hasValue() 
                ? Option<Ordinal>(_ordinality.get() + other._ordinality.get())
                : Option<Ordinal>();
    Cardinal newSize = _size + other._size;
    return create( std::move(gen), newSize, newOrd );
}

template <typename T>
template <typename T2>
SharedPtr<LazySequence<T2>> LazySequence<T>::map( const std::function<T2(T)>& func ) {
    auto gen = makeUnique<MapGenerator>( func, _generator );
    return create( std::move(gen), _size, _ordinality ); 
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::where( const std::function<bool(T)>& func ) {
    auto gen = makeUnique<WhereGenerator>( func, _generator );
    return create( std::move(gen), _size, Option<Ordinal>() ); 
}

template <typename T>
template <typename T2>
T2 LazySequence<T>::foldl( const std::function<T2(T)>& func, const T2& base ) {
    if (!isFinite() || !_ordinality.hasValue()) {
        throw Exception( Exception::ErrorCode::INFINITE_CALCULATION );
    }
    T2 res = base;
    for (int i = 0; i <= _ordinality.get(); i++) {
        res = func( (*this)[i], res );
    }
    return res;
}

template <typename T>
template <typename T2>
T2 LazySequence<T>::foldr( const std::function<T2(T)>& func, const T2& base ) {
    if (!isFinite() || !_ordinality.hasValue()) {
        throw Exception( Exception::ErrorCode::INFINITE_CALCULATION );
    }
    T2 res = base;
    for (int i = _ordinality.get(); i >= 0; i--) {
        res = func( (*this)[i], res );
    }
    return res;
}