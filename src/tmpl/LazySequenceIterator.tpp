// template <typename T>
// LazySequence<T>::LazySequenceIterator::LazySequenceIterator()
// : _observed( SharedPtr<IGenerator>() ), _pos(0) {}

// template <typename T>
// LazySequence<T>::LazySequenceIterator::LazySequenceIterator( SharedPtr<IGenerator> generator, const ordinal& index ) 
// : _observed( generator ), _pos( index ) {}

// template <typename T>
// LazySequence<T>::LazySequenceIterator::LazySequenceIterator( const LazySequenceIterator& other ) 
// : _observed( other._observed ), _pos( other._pos ) {}

// template <typename T>
// LazySequence<T>::LazySequenceIterator& LazySequence<T>::LazySequenceIterator::operator=( const LazySequenceIterator& other ) {
//     if (this != &other) {
//         this->_observed = other._observed;
//         this->_pos = other._pos;
//     }
//     return *this;
// }

// template <typename T>
// LazySequence<T>::LazySequenceIterator::LazySequenceIterator( LazySequenceIterator&& other ) {
//     this->_observed = std::move(other._observed);
//     this->_pos = other._pos;
//     other._pos = 0;
// }

// template <typename T>
// LazySequence<T>::LazySequenceIterator& LazySequence<T>::LazySequenceIterator::operator=( LazySequenceIterator&& other ) {
//     if (this != &other) {
//         this->_observed = std::move(other._observed);
//         this->_pos = other._pos;
//         other._pos = 0;
//     }
//     return *this;
// }

// template <typename T>
// LazySequence<T>::LazySequenceIterator& LazySequence<T>::LazySequenceIterator::operator++() {
//     this->_pos++;
//     return *this;
// }

// template <typename T>
// LazySequence<T>::LazySequenceIterator LazySequence<T>::LazySequenceIterator::operator++(int) {
//     auto res = *this;
//     this->_pos++;
//     return res;
// }

// template <typename T>
// T& LazySequence<T>::LazySequenceIterator::operator*() {
//     return this->_observed->get(this->_pos);
// }

// template <typename T>
// const T& LazySequence<T>::LazySequenceIterator::operator*() const {
//     return this->_observed->get(this->_pos);
// }

// template <typename T>
// T* LazySequence<T>::LazySequenceIterator::operator->() {
//     return std::addressof(this->_observed->get(this->_pos));
// }

// template <typename T>
// const T* LazySequence<T>::LazySequenceIterator::operator->() const {
//     return std::addressof(this->_observed->get(this->_pos));
// }

// template <typename T>
// bool LazySequence<T>::LazySequenceIterator::operator==( const LazySequenceIterator& other ) const noexcept {
//     return (this->_observed = other._observed && this->_pos == other._pos);
// }

// template <typename T>
// bool LazySequence<T>::LazySequenceIterator::operator!=( const LazySequenceIterator& other ) const noexcept {
//     return (!(*this == other));
// }

// template <typename T>
// LazySequence<T>::LazySequenceIterator LazySequence<T>::begin() {
//     return LazySequenceIterator(this->_generator, 0);
// }

// template <typename T>
// LazySequence<T>::LazySequenceIterator LazySequence<T>::end() {
//     return LazySequenceIterator(this->_generator, this->_size);
// }

// template <typename T>
// const LazySequence<T>::LazySequenceIterator LazySequence<T>::begin() const {
//     return LazySequenceIterator(this->_generator, 0);
// }

// template <typename T>
// const LazySequence<T>::LazySequenceIterator LazySequence<T>::end() const {
//     return LazySequenceIterator(this->_generator, this->_size);
// }