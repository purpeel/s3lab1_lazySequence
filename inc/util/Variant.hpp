#ifndef VARIANT
#define VARIANT

#include "util.hpp"

template <typename T1, typename T2>
class Variant
{
private:
    bool _isInstanceOfT1;
    bool _isInstanceOfT2;
    union 
    {
        T1 _value1;
        T2 _value2;
    };
public:
    Variant() : _isInstanceOfT1(false), _isInstanceOfT2(false) {}

    Variant( const T1& value ) : _isInstanceOfT1(true), _isInstanceOfT2(false), _value1(value) {}
    Variant<T1,T2>& operator=( const T1& value ) { //todo move constructors from T1&& and T2&&
        if (this->_isInstanceOfT1) { 
            _value1.~T1();
        } else if (this->_isInstanceOfT2) { 
            _value2.~T2();
            this->_isInstanceOfT1 = true;
            this->_isInstanceOfT2 = false;
        }
        new(&_value1) T1(value);
        return *this;
    }

    Variant( const T2& value ) : _isInstanceOfT1(false), _isInstanceOfT2(true), _value2(value) {}
    Variant<T1,T2>& operator=( const T2& value ) {
        if (this->_isInstanceOfT1) { 
            _value1.~T1();
            this->_isInstanceOfT1 = false;
            this->_isInstanceOfT2 = true;
        } else if (this->_isInstanceOfT2) { 
            _value2.~T2();
        }
        new(&_value2) T2(value);
        return *this;
    }
    
    Variant( const Variant<T1, T2>& other ) 
            : _isInstanceOfT1(other._isInstanceOfT1)
            , _isInstanceOfT2(other._isInstanceOfT2) 
            {
                if      (this->_isInstanceOfT1) { 
                    _value1.~T1();
                    new(&_value1) T1(other._value1); 
                } else if (this->_isInstanceOfT2) { 
                    _value2.~T2();
                    new(&_value2) T2(other._value2);
                }
            }

    Variant<T1,T2>& operator=( const Variant<T1, T2>& other ) {
        if (*this != other) {
            this->_isInstanceOfT1 = other._isInstanceOfT1;
            this->_isInstanceOfT2 = other._isInstanceOfT2;
            if      (this->_isInstanceOfT1) { 
                _value1.~T1();
                new(&_value1) T1(other._value1); 
            } else if (this->_isInstanceOfT2) { 
                _value2.~T2();
                new(&_value2) T2(other._value2);
            }
        }
        return *this;
    }

    Variant( Variant<T1,T2>&& other ) {
        this->_isInstanceOfT1 = other._isInstanceOfT1;
        this->_isInstanceOfT2 = other._isInstanceOfT2;
        if      (this->_isInstanceOfT1) { 
            _value1.~T1();
            new(&_value1) T1(other._value1); 
        } else if (this->_isInstanceOfT2) { 
            _value2.~T2();
            new(&_value2) T2(other._value2);
        }

        other._isInstanceOfT1 = false;
        other._isInstanceOfT2 = false;
        new(&other._value1) T1();
        new(&other._value2) T2();
    }

    ~Variant() {
        if (this->_isInstanceOfT1) {
            _value1.~T1();
        } else if (this->_isInstanceOfT2) {
            _value2.~T2();
        }
    }
public:
    operator T1() {
        if (this->_isInstanceOfT1) {
            return this->_value1;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }
    operator T1() const {
        if (this->_isInstanceOfT1) {
            return this->_value1;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }
    operator T2() const {
        if (this->_isInstanceOfT2) {
            return this->_value2;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }
    operator T2() const {
        if (this->_isInstanceOfT2) {
            return this->_value2;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }

    // bool operator==(const T1& value) const {
    //     return this->_isInstanceOfT1 && this->_value1 == value;
    // }
    // bool operator==(const T2& value) const {
    //     return this->_isInstanceOfT2 && this->_value2 == value;
    // }
public:
    bool isInstanceOfT1() const noexcept { return this->_isInstanceOfT1; }
    bool isInstanceOfT2() const noexcept { return this->_isInstanceOfT2; }
    
    T1& getT1() {
        if (this->_isInstanceOfT1) {
            return this->_value1;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }
    const T1& getT1() const {
        if (this->_isInstanceOfT1) {
            return this->_value1;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }

    T2& getT2() {
        if (this->_isInstanceOfT2) {
            return this->_value2;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }
    const T2& getT2() const {
        if (this->_isInstanceOfT2) {
            return this->_value2;
        } else { throw Exception( Exception::ErrorCode::INVALID_TYPE ); }
    }
};


#endif // VARIANT