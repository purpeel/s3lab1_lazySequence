template <typename T>
LazySequence<T>::LazySequence() {
    this->_items = *(new UniquePtr<Sequence<T>>());
    this->_generator = *(new UniquePtr<StandardGenerator>>());

}

template <typename T>
LazySequence<T>::LazySequence( const Sequence<T>& data ) {
    this->_items = *(new UniquePtr<Sequence<T>>());
    *this->_items = data;
}

template <typename T>
LazySequence<T>::LazySequence( const Sequence<T>& data, std::function<T(Sequence<T>)> producingFunc ) {
    this->_items = *(new UniquePtr<Sequence<T>>());
    *this->_items = data;

}