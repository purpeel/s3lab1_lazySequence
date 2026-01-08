template <typename T>
LazySequence<T>::LazySequence() 
: _size(0), _offset(0)
, _ordinality( Option<Ordinal>(0) )
, _generator( makeShared<FiniteGenerator<T>>() )
, _items( makeUnique<ArraySequence<T>>() ) {}

template <typename T>
LazySequence<T>::LazySequence( const T& value ) 
: _size(1), _offset(0)
, _ordinality( Option<Ordinal>(1) )
, _generator( makeShared<FiniteGenerator<T>>( value ) )
, _items( makeUnique<ArraySequence<T>>() ) {}

template <typename T>
LazySequence<T>::LazySequence( ArraySequence<T>& data ) 
: _size(data.getSize()), _offset(0)
, _ordinality( Option<Ordinal>(data.getSize()) )
, _generator( makeShared<FiniteGenerator<T>>( std::move(data) ) )
, _items( makeUnique<ArraySequence<T>>( std::move(data) ) ) {}

template <typename T>
LazySequence<T>::LazySequence( const size_t arity
                             , const std::function<T(ArraySequence<T>&)>& func
                             , ArraySequence<T>& data )
: _size( Cardinal::infiniteCardinal::BETH_0 ), _offset(0)
, _ordinality( Ordinal::omega() )
, _generator( makeShared<InfiniteGenerator<T>>(arity, func, data) )
, _items( makeUnique<ArraySequence<T>>( std::move(data)) ) {}

template <typename T>
LazySequence<T>::LazySequence( UniquePtr<IGenerator<T>>&& generator
                             , const Cardinal& size
                             , const Option<Ordinal>& ordinality ) 
: _size( size ), _offset(0)
, _ordinality( ordinality )
, _generator( std::move(generator) ) {}

template <typename T>
LazySequence<T>::LazySequence( UniquePtr<IGenerator<T>>&& generator
                             , const Cardinal& size
                             , const Option<Ordinal>& ordinality
                             , const ArraySequence<T>& data ) 
: _size( size ), _offset(0)
, _ordinality( ordinality )
, _generator( std::move(generator) )
, _items( makeUnique<ArraySequence<T>>( data) ) {}

template <typename T>
LazySequence<T>::LazySequence( const LazySequence<T>& other ) {
    _size       = other._size;
    _offset     = other._offset;
    _ordinality = other._ordinality;
    _generator  = other._generator;
    *_items     = *other._items;
}

template <typename T>
LazySequence<T>& LazySequence<T>::operator=( const LazySequence<T>& other ) {
    if (this != &other) {
        _size       = other._size;
        _offset     = other._offset;
        _ordinality = other._ordinality;
        _generator  = other._generator;
        *_items     = *other._items;
    }
    return *this;
}

template <typename T>
LazySequence<T>::LazySequence( LazySequence<T>&& other ) {
    _generator    = std::move(other._generator);
    _items        = std::move(other._items);
    _offset       = other._offset;
    _size         = other._size;
    _ordinality   = other._ordinality;
    other._offset = 0;
    other._size   = 0;
}

template <typename T>
LazySequence<T>& LazySequence<T>::operator=( LazySequence<T>&& other ) {
    if (this != &other) {
        _generator    = std::move(other._generator);
        _items        = std::move(other._items);
        _offset       = other._offset;
        _size         = other._size;
        _ordinality   = other._ordinality;
        other._offset = 0;
        other._size   = 0;
    }
    return *this;
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::create() {
    return makeShared<LazySequence<T>>();
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::create( const T& value ) {
    return makeShared<LazySequence<T>>( value );
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::create( ArraySequence<T>& data ) {
    return makeShared<LazySequence<T>>( data );
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::create( const size_t arity
                                                  , const std::function<T(ArraySequence<T>&)>& producingFunc
                                                  , ArraySequence<T>& data ) {
    return makeShared<LazySequence<T>>(arity, producingFunc, data);
}

template <typename T>
template <typename T2>
SharedPtr<LazySequence<T2>> LazySequence<T>::create( UniquePtr<IGenerator<T2>>&& generator
                                            , const Cardinal& size
                                            , const Option<Ordinal>& ordinality ) {
    return makeShared<LazySequence<T2>>( std::move(generator), size, ordinality );
}

template <typename T>
template <typename T2>
SharedPtr<LazySequence<T2>> LazySequence<T>::create( UniquePtr<IGenerator<T2>>&& generator
                                            , const Cardinal& size
                                            , const Option<Ordinal>& ordinality
                                            , const ArraySequence<T>& data ) {
    return makeShared<LazySequence<T2>>( std::move(generator), size, ordinality, data );
}

template <typename T>
T LazySequence<T>::getFirst() {
    if (_offset > 0) {
        throw Exception( Exception::ErrorCode::DEMATERIALIZED_ACCESS );
    } else {
        return (*this)[0];
    }
}

template <typename T>
T LazySequence<T>::getLast() {
    if (!isFinite()) {
        throw Exception( Exception::ErrorCode::INFINITE_CALCULATION );
    } else {
        return _generator->get(static_cast<size_t>(getSize()) - 1);
    }
}

template <typename T>
T LazySequence<T>::operator[]( const Ordinal& index ) {
    trimCache();
    if (_ordinality.hasValue()) {
        if (index < 0 || index >= _ordinality.get()) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS ); 
        }
        if (index.isFinite()) {
            if (_items->isEmpty()) {
                for ( auto i = 0; i <= index; i++ ) {
                    auto a = _generator->getNext();
                    _items->append( a );
                }
                return (*_items)[static_cast<size_t>(index)];
            } else {
                if (_offset <= index && index < _offset + _items->getSize()) {
                    return (*_items)[static_cast<size_t>(index) - static_cast<size_t>(_offset)];
                } else if (index >= _offset + _items->getSize()) {
                    while (index > _offset + _items->getSize() - 1) {
                        _items->append( _generator->getNext() );
                    }
                    return (*_items)[static_cast<size_t>(index) - static_cast<size_t>(_offset)];
                } else {
                    throw Exception( Exception::ErrorCode::DEMATERIALIZED_ACCESS );
                }
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
        if (_items->isEmpty()) {
                for ( auto i = 0; i <= index; i++ ) {
                    auto a = _generator->getNext();
                    _items->append( a );
                }
                return (*_items)[static_cast<size_t>(index)];
        } else {
            if (_offset <= index && index < _offset + _items->getSize()) {
                return (*_items)[static_cast<size_t>(index) - static_cast<size_t>(_offset)];
            } else if (index >= _offset + _items->getSize()) {
                while (index > _offset + _items->getSize() - 1) {
                    _items->append( _generator->getNext() );
                }
                return (*_items)[static_cast<size_t>(index) - static_cast<size_t>(_offset)];
            } else {
                throw Exception( Exception::ErrorCode::DEMATERIALIZED_ACCESS );
            }
        }
    }
}

template <typename T>
Cardinal LazySequence<T>::getSize() const {
    return _size;
}

template <typename T>
size_t LazySequence<T>::getMaterializedCount() const { 
    return _generator->_items->getSize();
}

template <typename T>
ArraySequence<T> LazySequence<T>::getMaterialized() const {
    return *_items;
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
    auto gen = makeUnique<AppendGenerator<T>>( value, this->sharedFromThis(), _ordinality );
    auto newOrd = _ordinality.hasValue()
            ? Option<Ordinal>( _ordinality.get() + 1 )
            : Option<Ordinal>();
    return create<T>( std::move(gen), _size + 1, newOrd, *_items );
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::append( const LazySequence<T>& value ) {
    auto gen = makeUnique<AppendGenerator<T>>( value, this->sharedFromThis(), _ordinality ); 
    auto newOrd = _ordinality.hasValue() && value._ordinality.hasValue()
            ? Option<Ordinal>( _ordinality.get() + value._ordinality.get() ) 
            : Option<Ordinal>();
    return create<T>( std::move(gen), _size + 1, newOrd, *_items );
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::prepend( const T& value ) {
    auto gen = makeUnique<PrependGenerator<T>>( value, this->sharedFromThis(), Option<Ordinal>(1) );
    auto newOrd = _ordinality.hasValue()
            ? Option<Ordinal>( 1 + _ordinality.get()) 
            : Option<Ordinal>();
    ArraySequence<T> newItems;
    if (_offset == 0) {
        newItems.prepend(value);
    }
    newItems.concat( *_items );
    return create<T>( std::move(gen), _size + 1, newOrd, newItems );
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::prepend( const LazySequence<T>& value ) {
    auto gen = makeUnique<PrependGenerator<T>>( value, this->sharedFromThis(), value._ordinality );
    auto newOrd = _ordinality.hasValue() && value._ordinality.hasValue()
            ? Option<Ordinal>( value._ordinality.get() + _ordinality.get()) 
            : Option<Ordinal>();
        
    ArraySequence<T> newItems = *value._items;
    return create<T>( std::move(gen), _size + value.getSize(), newOrd );
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::insertAt( const T& value, const Ordinal& index ) {
    if (_ordinality.hasValue()) {
        if (index < 0 || index > _ordinality.get()) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
        if (index == 0)                      { return prepend( value ); } 
        else if (index == _ordinality.get()) { return append ( value ); } 
        else {
            auto gen = makeUnique<InsertGenerator<T>>( value, index, this->sharedFromThis() );
            
            return create<T>( std::move(gen), _size + 1, 1 + _ordinality.get() );
        }
    } else {
        if (index < 0) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        } 
        if (index == 0) { return prepend( value ); }
        else {
            auto gen = makeUnique<InsertGenerator<T>>( value, index, this->sharedFromThis() );
            
            return create<T>( std::move(gen), _size + 1, _ordinality );
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
        else {
            auto gen = makeUnique<InsertGenerator<T>>( value, index, this->sharedFromThis(), value._ordinality );
            Option<Ordinal> newOrd = value._ordinality.hasValue() 
                ? Option<Ordinal>(value._ordinality.get() + _ordinality.get()) 
                : Option<Ordinal>();
            return create<T>( std::move(gen), _size + value.getSize(), newOrd );
        }
    } else {
        if (index < 0) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        } 
        if (index == 0) { return prepend( value ); } 
        else {
            auto gen = makeUnique<InsertGenerator<T>>( value, index, this->sharedFromThis(), value._ordinality );
            return create<T>( std::move(gen), _size + value.getSize(), _ordinality );
        }
    }
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::skip( const Ordinal& index ) {
    if (_ordinality.hasValue()) {
        if (index < 0 || index >= _ordinality.get()) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
        auto gen = makeUnique<SkipGenerator<T>>( index, this->sharedFromThis() );
        return create<T>( std::move(gen), _size - 1, Option<Ordinal>(_ordinality.get() - 1) );
    } else {
        if (index < 0) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
        auto gen = makeUnique<SkipGenerator<T>>( index, this->sharedFromThis() );
        return create<T>( std::move(gen), _size - 1, _ordinality );
    }
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::skip( const Ordinal& start, const Ordinal& end ) {
    if (_ordinality.hasValue()) {
        if (end < start || end > _ordinality.get()) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
        auto gen = makeUnique<SkipGenerator<T>>( start, end, this->sharedFromThis() );
        Option<Ordinal> newOrd = Option<Ordinal>(start + (_ordinality.get() - end));
        Cardinal newSize = newOrd.get().isFinite() ? Cardinal( static_cast<size_t>(newOrd.get()) ) : _size;
        return create<T>( std::move(gen), newSize, newOrd );
    } else {
        if (start < 0 || end < 0 || end < start) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
        auto gen = makeUnique<SkipGenerator<T>>( start, end, this->sharedFromThis() );
        return create<T>( std::move(gen), _size, _ordinality );
    }
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::getSubSequence( const Ordinal& start, const Ordinal& end ) {
    if (_ordinality.hasValue()) {
        if (end < start || end > _ordinality.get()) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
    } else {
        if (end < start) {
            throw Exception( Exception::ErrorCode::INDEX_OUT_OF_BOUNDS );
        }
    }
    if (start == end) { return create(); }
    auto gen = makeUnique<SubSequenceGenerator<T>>( start, end, this->sharedFromThis() );
    Option<Ordinal> newOrd = Option<Ordinal>(end - start);
    Cardinal newSize = newOrd.get().isFinite() ? static_cast<size_t>(newOrd.get()) : Cardinal::BethNull();
    return create<T>( std::move(gen), newSize, newOrd );
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::concat( const LazySequence<T>& other ) {
    auto gen = makeUnique<ConcatGenerator<T>>( this->sharedFromThis(), other.sharedFromThis(), _ordinality );
    Option<Ordinal> newOrd = _ordinality.hasValue() && other._ordinality.hasValue() 
                ? Option<Ordinal>(_ordinality.get() + other._ordinality.get())
                : Option<Ordinal>();
    Cardinal newSize = _size + other._size;
    return create<T>( std::move(gen), newSize, newOrd );
}

template <typename T>
template <typename T2>
SharedPtr<LazySequence<T2>> LazySequence<T>::map( const std::function<T2(T)>& func ) {
    auto gen = makeUnique<MapGenerator<T, T2>>( func, this->sharedFromThis() );
    return create<T2>( std::move(gen), _size, _ordinality ); 
}

template <typename T>
SharedPtr<LazySequence<T>> LazySequence<T>::where( const std::function<bool(T)>& func ) { // можно обезопасить для конечных последовательностей, но надо перед выполением операции 
    auto gen = makeUnique<WhereGenerator<T>>( func, this->sharedFromThis() );             // материализовать весь буфер
    return create<T>( std::move(gen), _size, Option<Ordinal>() ); 
}

template <typename T>
template <typename T2>
T2 LazySequence<T>::foldl( const std::function<T2(T2, T)>& func, const T2& base ) {
    if (isFinite() && _ordinality.hasValue()) {
        T2 res = base;
        size_t index = 0;
        while (index < static_cast<size_t>(_ordinality.get())) {
            res = func( res, (*this)[index] );
            index++;
        }
        return res;
    } else {
        throw Exception( Exception::ErrorCode::INFINITE_CALCULATION );
    }
}

template <typename T>
template <typename T2>
T2 LazySequence<T>::foldr( const std::function<T2(T, T2)>& func, const T2& base ) {
    if (isFinite() && _ordinality.hasValue()) {
        T2 res = base;
        auto index = static_cast<size_t>(_ordinality.get() - 1);
        while (index > 0) {
            res = func( (*this)[index], res );
            index--;
        }
        res =  func( (*this)[0], res );
        return res;
    } else {
        throw Exception( Exception::ErrorCode::INFINITE_CALCULATION );
    }
}

template <typename T>
const T& LazySequence<T>::memoiseNext() {
    _items->append( _generator->getNext() );
    trimCache();
    return (*_items)[ _items->getSize() - 1];
}

template <typename T>
T LazySequence<T>::get( const Ordinal& index ) {
    if (index.isFinite()) {
        return (*this)[index];
    } else {
        return this->_generator->get( index );
    }
}

template <typename T>
bool LazySequence<T>::canMemoiseNext() {
    return _generator->hasNext();
}

template <typename T>
Option<T> LazySequence<T>::tryMemoiseNext() {
    if (canMemoiseNext()) {
        return Option<T>( memoiseNext() );
    } else {
        return Option<T>();
    }
}

template <typename T>
void LazySequence<T>::trimCache() {
    if (_items->getSize() >= CACHE_MAX_SIZE) {
        *_items = *static_cast<ArraySequence<T>*>(_items->getSubSequence( _items->getSize() - CACHE_DECREASE_SIZE, _items->getSize()));
        _offset += CACHE_OFFSET_INCREASE;
    }
}