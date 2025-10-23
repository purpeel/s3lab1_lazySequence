#ifndef VARIANT
#define VARIANT

#include "util.hpp"

template <typename T1, typename T2>
class Variant
{
private:
    enum class State {
        isEmpty = -1,
        isT1 = 0, 
        isT2 = 1
    }
    union
    {
        T1 _value1;
        T2 _value2;
    };
public:
    Variant() : State(State::isEmpty) {}

    Variant( const T1& value ) : State(State::isT1), _value1(value) {}
    Variant<T1,T2>& operator=( const T1& value ) {
        if (static_cast<int>(this->State) == static_cast<int>(State::isT1)) { 
            _value1.~T1();
        } else if (static_cast<int>(this->State) == static_cast<int>(State::isT2)) { 
            _value2.~T2();
            this->State = State::isT1;
        }
        new(&_value1) T1(value);
        return *this;
    }

    Variant( const T2& value ) : State(State::isT2), _value2(value) {}
    Variant<T1,T2>& operator=( const T2& value ) {
        if (static_cast<int>(this->State) == static_cast<int>(State::isT1)) { 
            _value1.~T1();
            this->State = State::isT2;
        } else if (static_cast<int>(this->State) == static_cast<int>(State::isT2)) { 
            _value2.~T2();
        }
        new(&_value2) T2(value);
        return *this;
    }

    Variant( T1&& value ) : State(State::isT1), _value1(std::move(value)) {}
    Variant<T1,T2>& operator=( T1&& value ) {
        if (static_cast<int>(this->State) == static_cast<int>(State::isT1)) { 
            _value1.~T1();
        } else if (static_cast<int>(this->State) == static_cast<int>(State::isT2)) { 
            _value2.~T2();
            this->State = State::isT1;
        }
        new(&_value1) T1(std::move(value));
        return *this;
    }
    
    Variant( T2&& value ) : State(State::isT2), _value2(std::move(value)) {}
    Variant<T1,T2>& operator=( T2&& value ) {
        if (static_cast<int>(this->State) == static_cast<int>(State::isT1)) { 
            _value1.~T1();
            this->State = State::isT2;
        } else if (static_cast<int>(this->State) == static_cast<int>(State::isT2)) { 
            _value2.~T2();
        }
        new(&_value2) T2(std::move(value));
        return *this;
    }

    Variant( const Variant<T1, T2>& other ) : State(other.State) {
        if (static_cast<int>(other.State) == static_cast<int>(State::isT1)) {
            new(&_value1) T1(other._value1); 
        } else if (static_cast<int>(other.State) == static_cast<int>(State::isT2)) {
            new(&_value2) T2(other._value2);
        }
    }
    Variant<T1,T2>& operator=( const Variant<T1, T2>& other ) {
        if (*this != other) {
            if (static_cast<int>(this->State) == static_cast<int>(State::isT1)) { 
                _value1.~T1();
            } else if (static_cast<int>(this->State) == static_cast<int>(State::isT2)) { 
                _value2.~T2();
            }
            this->State = other.State;
            if (static_cast<int>(this->State) == static_cast<int>(State::isT1)) {
                new(&_value1) T1(other._value1); 
            } else if (static_cast<int>(this->State) == static_cast<int>(State::isT2)) { 
                new(&_value2) T2(other._value2);
            }
        }
        return *this;
    }

    Variant( Variant<T1,T2>&& other ) : State(other.State) {
        if (static_cast<int>(other.State) == static_cast<int>(State::isT1)) {
            new(&_value1) T1(other._value1); 
        } else if (static_cast<int>(other.State) == static_cast<int>(State::isT2)) {
            new(&_value2) T2(other._value2);
        }
        other.State = State::isEmpty;
        new(&other._value1) T1();
        new(&other._value2) T2();
    }
    Variant<T1,T2>& operator=( Variant<T1,T2>&& other ) {
        if (*this != other) {
            if (static_cast<int>(this->State) == static_cast<int>(State::isT1)) { 
                _value1.~T1();
            } else if (static_cast<int>(this->State) == static_cast<int>(State::isT2)) { 
                _value2.~T2();
            }
            this->State = other.State;
            if (static_cast<int>(this->State) == static_cast<int>(State::isT1)) {
                new(&_value1) T1(std::move(other._value1)); 
            } else if (static_cast<int>(this->State) == static_cast<int>(State::isT2)) { 
                new(&_value2) T2(std::move(other._value2));
            }
        }
        return *this;
    }
    ~Variant() {
        if (static_cast<int>(this->State) == static_cast<int>(State::isT1)) {
            _value1.~T1();
        } else if (static_cast<int>(this->State) == static_cast<int>(State::isT2)) {
            _value2.~T2();
        }
    }
public:
    operator T1() {
        if (static_cast<int>(this->State) == static_cast<int>(State::isT1)) {
            return this->_value1;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }
    operator T1() const {
        if (static_cast<int>(this->State) == static_cast<int>(State::isT1)) {
            return this->_value1;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }
    operator T2() const {
        if (static_cast<int>(this->State) == static_cast<int>(State::isT2)) {
            return this->_value2;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }
    operator T2() const {
        if (static_cast<int>(this->State) == static_cast<int>(State::isT2)) {
            return this->_value2;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }
public:
    bool isInstanceOfT1() const noexcept { return (static_cast<int>(this->State) == static_cast<int>(State::isT1)); }
    bool isInstanceOfT2() const noexcept { return (static_cast<int>(this->State) == static_cast<int>(State::isT2)); }
    
    T1& getT1() {
        if (static_cast<int>(this->State) == static_cast<int>(State::isT1)) {
            return this->_value1;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }
    const T1& getT1() const {
        if (static_cast<int>(this->State) == static_cast<int>(State::isT1)) {
            return this->_value1;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }

    T2& getT2() {
        if (static_cast<int>(this->State) == static_cast<int>(State::isT2)) {
            return this->_value2;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }
    const T2& getT2() const {
        if (static_cast<int>(this->State) == static_cast<int>(State::isT2)) {
            return this->_value2;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }
};


#endif // VARIANT