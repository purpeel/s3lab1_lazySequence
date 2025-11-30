#ifndef CARDINAL_H
#define CARDINAL_H

#include "Variant.hpp"

class Cardinal 
{
public:
    enum class infiniteCardinal
    {
        BETH_0 = 0, // aleph-null
        BETH_1 = 1  // continuum
    };
    using finite = size_t;
    using transfinite = infiniteCardinal;
public:
    Cardinal() : _value(0) {}
    Cardinal( const size_t& other ) : _value( other ) {}
    Cardinal& operator=( const size_t& other ) {
        this->_value = other;
        return *this;
    }

    Cardinal( const transfinite& other ) : _value( other ) {}
    Cardinal& operator=( const transfinite& other ) {
        this->_value = other;
        return *this;
    }

    Cardinal( const Cardinal& other ) : _value( other._value ) {}
    Cardinal& operator=( const Cardinal& other ) {
        this->_value = other._value;
        return *this;
    }
    ~Cardinal() = default;
public:
    static const Cardinal& BethNull() {
        static const Cardinal res( infiniteCardinal::BETH_0 );
        return res;
    }
    static const Cardinal& BethOne() {
        static const Cardinal res( infiniteCardinal::BETH_1 );
        return res;
    }
public:
    bool isFinite() const {
        return this->_value.isInstanceOfT1();
    }
    bool isTransfinite() const {
        return this->_value.isInstanceOfT2();
    }
    explicit operator size_t() const {
        if (this->isFinite()) {
            return static_cast<size_t>(this->_value.getT1());
        } else {
            throw Exception( Exception::ErrorCode::INVALID_TYPE );
        }
    }
public:
    bool operator<( const Cardinal& other ) const {
        if (this->isFinite() && other.isFinite()) {
            return this->_value.getT1() < other._value.getT1();
        } else if (this->isFinite() && other.isTransfinite()) {
            return true;
        } else if (this->isTransfinite() && other.isFinite()) {
            return false;
        } else {
            return this->_value.getT2() < other._value.getT2();
        }
    }
    bool operator>( const Cardinal& other ) const { 
        if (this->isFinite() && other.isFinite()) {
            return this->_value.getT1() > other._value.getT1();
        } else if (this->isFinite() && other.isTransfinite()) {
            return false;
        } else if (this->isTransfinite() && other.isFinite()) {
            return true;
        } else {
            return this->_value.getT2() > other._value.getT2();
        }
    }
    bool operator<=( const Cardinal& other ) const { return !(*this > other); }
    bool operator>=( const Cardinal& other ) const { return !(*this < other); }
    bool operator==( const Cardinal& other ) const {
        if (this->isFinite() && other.isFinite()) {
            return this->_value.getT1() == other._value.getT1();
        } else if (this->isFinite() != other.isFinite()) {
            return false;
        } else {
            return this->_value.getT2() == other._value.getT2();
        }
    }
    bool operator!=( const Cardinal& other ) { return !(*this == other); }

    bool operator<( const size_t& other ) const {
        if (this->isFinite()) { return this->_value.getT1() < other; } 
        else { false; }
    }
    bool operator>( const size_t& other ) const { 
        if (this->isFinite()) { return this->_value.getT1() > other; } 
        else { return true; }
    }
    bool operator<=( const size_t& other ) const { return !(*this > other); }
    bool operator>=( const size_t& other ) const { return !(*this < other); }
    bool operator==( const size_t& other ) const {
        if (this->isFinite()) { return this->_value.getT1() == other; } 
        else { return false; }
    }
    bool operator!=( const size_t& other ) { return !(*this == other); }
public:
    Cardinal operator+( const Cardinal& other ) const {
        if (this->isFinite() && other.isFinite()) {
            return Cardinal( this->_value.getT1() + other._value.getT1());
        } else {
            return *this > other ? *this : other;
        }
    }
    Cardinal operator+( const size_t& other ) const {
        if (this->isFinite()) {
            return Cardinal( this->_value.getT1() + other );
        } else {
            return *this;
        }
    }
    // Cardinal operator+( const Ordinal& other ) const {
    //     if (this->isFinite() && other.isFinite()) {
    //         return Cardinal( this->_value.getT1() + static_cast<size_t>(other) );
    //     } else if (this->isTransfinite() && other.isFinite()) {
    //         return *this;
    //     } else {
    //         throw Exception( Exception::ErrorCode::INVALID_TYPE );
    //     }
    // }   

    Cardinal operator-( const Cardinal& other ) {
        if (this->isTransfinite() || other.isTransfinite()) {
            if (other > *this) {
                throw Exception( Exception::ErrorCode::TRANSFINITE_ARITHMETIC );
            } else if (other == *this ) {
                return Cardinal(0);
            } else {
                return *this;
            }
        } else if (static_cast<size_t>(other) > static_cast<size_t>(*this)) {
            throw Exception( Exception::ErrorCode::TRANSFINITE_ARITHMETIC );
        } else {
            return Cardinal(this->_value.getT1() - static_cast<size_t>(other));
        }
    }
    // size_t operator-( const Ordinal& other ) {
    //     if (this->isTransfinite() || other.isTransfinite()) {
    //         throw Exception( Exception::ErrorCode::INVALID_TYPE ); 
    //     } else if (static_cast<size_t>(other) > static_cast<size_t>(*this)) {
    //         throw Exception( Exception::ErrorCode::TRANSFINITE_ARITHMETIC );
    //     } else {
    //         return this->_value.getT1() - static_cast<size_t>(other);
    //     }
    // }
    Cardinal operator-( const size_t& other ) {
        if (this->isTransfinite()) {
            return *this;
        } else if (other > static_cast<size_t>(*this)) {
            throw Exception( Exception::ErrorCode::TRANSFINITE_ARITHMETIC );
        } else {
            return Cardinal(this->_value.getT1() - static_cast<size_t>(other));
        }
    }
    Cardinal operator*( const Cardinal& other ) const {
        if (this->isFinite() && other.isFinite()) {
            return Cardinal( this->_value.getT1() * other._value.getT1());
        } else {
            return *this > other ? *this : other;
        }
    }
    Cardinal operator*( const size_t& other ) const {
        if (this->isFinite()) {
            return Cardinal( this->_value.getT1() * other );
        } else {
            return *this;
        }
    }
    // Cardinal operator*( const Ordinal& other ) const {
    //     if (this->isFinite() && other.isFinite()) {
    //         return Cardinal( this->_value.getT1() * static_cast<size_t>(other) );
    //     } else if (this->isTransfinite() && other.isFinite()) {
    //         return *this;
    //     } else {
    //         throw Exception( Exception::ErrorCode::INVALID_TYPE );
    //     }
    // }   
private:
    Variant<finite, transfinite> _value;
};

inline size_t operator+( const size_t& arg1, const Cardinal& arg2 ) {
    if (arg2.isTransfinite()) {
        return static_cast<size_t>(arg2);
    } else {
        return arg1 + arg2;
    }
};
inline size_t operator-( const size_t& arg1, const Cardinal& arg2 ) {
    if (arg2 > arg1) {
        throw Exception( Exception::ErrorCode::TRANSFINITE_ARITHMETIC );
    } else {
        return static_cast<size_t>(arg1) - arg2;
    }
};
inline size_t operator*( const size_t& arg1, const Cardinal& arg2 ) {
    if (arg2.isTransfinite()) {
        return static_cast<size_t>(arg2);
    } else {
        return static_cast<size_t>(arg1) * arg2;
    }
};

#endif // CARDINAL_H
