#ifndef ORDINAL_H
#define ORDINAL_H

#include "DynamicArray.hpp"
#include "Variant.hpp"

class Ordinal
{
private:
    struct Term {
        unsigned _coefficient;
        unsigned _exponent;

        Term() : _coefficient(0), _exponent(0) {}

        Term( const Term& other ) = default;
        Term& operator=( const Term& other ) = default;

        Term( unsigned coefficient, unsigned exponent) : _coefficient(coefficient), _exponent(exponent) {}

        bool operator<(const Term& other) const {
            return (_exponent  < other._exponent
                || (_exponent == other._exponent && _coefficient < other._coefficient ));
        }
        bool operator==(const Term& other) const {
            return  _exponent    ==  other._exponent 
                    && _coefficient ==  other._coefficient;
        }
        bool operator>(const Term& other) const {
            return (_exponent > other._exponent
                || (_exponent == other._exponent && _coefficient > other._coefficient ));
        }
        bool operator!=(const Term& other) const {
            return !(*this == other);
        }
        bool operator>=(const Term& other) const {
            return (*this > other || *this == other);
        }
        bool operator<=(const Term& other) const {
            return (*this < other || *this == other);
        }
    };
public:
    using finite = size_t;
    using transfinite = DynamicArray<Term>;
private:
    Variant<finite, transfinite> _value;
    finite getFinite() const {
        return this->_value.isInstanceOfT1() ? _value.getT1() : _value.getT2()[0]._coefficient;
    }
public:
    Ordinal() : _value(0) {}
    Ordinal( const size_t& other ) : _value(other) {}
    Ordinal& operator=( const size_t& other ) {
        _value = other;
        return *this;
    }
    Ordinal( const transfinite& terms ) : _value(terms) {}
    Ordinal( const Ordinal& other ) : _value(other._value) {}
    Ordinal& operator=( const Ordinal& other ) {
        _value = other._value;
        return *this;
    }
    static const Ordinal omega() {
        transfinite terms;
        terms.append( Term(1, 1) );
        terms.append( Term(0, 0) );
        static const Ordinal res = Ordinal(terms);
        return res;
    }
    ~Ordinal() = default;
public:
    explicit operator size_t() const {
        if (isFinite()) {
            return getFinite();
        } else {
            throw Exception( Exception::ErrorCode::INVALID_TYPE );
        }
    }
    bool operator>( const Ordinal& other ) const {
        if (isFinite() && other.isFinite()) {
            auto arg1 = getFinite();
            auto arg2 = other.getFinite();
            return arg1 > arg2;
        } else if (isFinite() && other.isTransfinite()) {
            return false;
        } else if (isTransfinite() && isFinite()) {
            return true;
        } else {
            auto thisTerms  = _value.getT2();
            auto otherTerms = other._value.getT2();
            for ( size_t i = 0; i < (thisTerms.getSize() < otherTerms.getSize() ? thisTerms.getSize() : otherTerms.getSize()); i++ ) {
                auto thisTerm = thisTerms[i];
                auto otherTerm = otherTerms[i];

                if (thisTerm < otherTerm) {
                    return false;
                } else if (thisTerm > otherTerm) {
                    return true;
                }
            }
            return false;
        }
    }
    bool operator==( const Ordinal& other ) const {
        if (isFinite() && other.isFinite()) {
            auto arg1 = getFinite();
            auto arg2 = other.getFinite();
            return arg1 > arg2;
        } else if (isTransfinite() != other.isTransfinite()) {
            return false;
        } else {
            auto thisTerms  = _value.getT2();
            auto otherTerms = other._value.getT2();
            for ( size_t i = 0; i < (thisTerms.getSize() < otherTerms.getSize() ? thisTerms.getSize() : otherTerms.getSize()); i++ ) {
                auto thisTerm = thisTerms[i];
                auto otherTerm = otherTerms[i];

                if (thisTerm != otherTerm) {
                    return false;
                } 
            }
            return true;
        }
    }
    bool operator<( const Ordinal& other ) const {
        return !(*this >= other);
    }
    bool operator<=( const Ordinal& other ) const {
        return !(*this > other);
    }
    bool operator>=( const Ordinal& other ) const {
        if (isFinite() && other.isFinite()) {
            auto arg1 = getFinite();
            auto arg2 = other.getFinite();
            return arg1 >= arg2;
        } else if (isFinite() && other.isTransfinite()) {
            return false;
        } else if (isTransfinite() && other.isFinite()) {
            return true;
        } else {
            auto thisTerms  = _value.getT2();
            auto otherTerms = other._value.getT2();
            for ( size_t i = 0; i < (thisTerms.getSize() < otherTerms.getSize() ? thisTerms.getSize() : otherTerms.getSize()); i++ ) {
                auto thisTerm = thisTerms[i];
                auto otherTerm = otherTerms[i];

                if (thisTerm < otherTerm) {
                    return false;
                } else if (thisTerm > otherTerm) {
                    return true;
                }
            }
            return true;
        }
    }
    bool operator!=( const Ordinal& other ) const {
        return !(*this == other);
    }

    bool operator>( const size_t& other ) const {
        if (isFinite()) {
            return _value > other;
        } else {
            return true;
        }
    }
    bool operator==( const size_t& other ) const {
        if (isFinite()) {
            return _value == other;
        } else  {
            return false;
        }
    }
    bool operator<( const size_t& other ) const {
        if (isFinite()) {
            return _value < other;
        } else  {
            return false;
        }
    }
    bool operator<=( const size_t& other ) const {
        return *this < other || *this == other;
    }
    bool operator>=( const size_t& other ) const {
        return *this > other || *this == other;
    }
    bool operator!=( const size_t& other ) const {
        return !(*this == other);
    }

    Ordinal& operator++() {
        if (isFinite()) {
            _value = _value.getT1() + 1;
            return *this;
        } else {
            if (_value.getT2()[_value.getT2().getSize()-1]._exponent == 0) {
                _value.getT2()[_value.getT2().getSize()-1]._coefficient++;
            } else {
                _value.getT2().append( Term(1, 0) );
            }
            return *this;
        }
    }

    Ordinal operator++(int) {
        auto temp = *this;
        ++(*this);
        return temp;
    }

    Ordinal& operator--() {
        if (isFinite()) {
            if (_value.getT1() > 0) { 
                _value = _value.getT1() - 1;
                return *this;
            } else {
                throw Exception( Exception::ErrorCode::TRANSFINITE_ARITHMETIC );
            }
        } else {
            if (_value.getT2()[_value.getT2().getSize()-1]._exponent > 0) {
                _value.getT2()[_value.getT2().getSize()-1]._coefficient--;
            } else {
                throw Exception( Exception::ErrorCode::TRANSFINITE_ARITHMETIC );
            }
            return *this;
        }
    }

    Ordinal operator--(int) {
        auto temp = *this;
        --(*this);
        return temp;
    }

    Ordinal operator+( const Ordinal& other ) const { 
        if (isFinite() && other.isFinite()) {
            return Ordinal(_value + other._value);
        } else if (isTransfinite() && other.isFinite()) {
            auto terms = _value.getT2();
            if ( terms[terms.getSize() - 1]._exponent != 0) {
                terms.append( Term(other._value.getT1(), 0) );
            } else {
                terms[terms.getSize() - 1]._coefficient += other._value;
            }
            return Ordinal(terms);
        } else if (isFinite() && other.isTransfinite()) {
            return other;
        } else {
            if (*this < other) {
                return other;
            } else if (*this == other) {
                return *this * 2;
            } else {
                auto thisTerms   = _value.getT2();
                auto otherTerms  = other._value.getT2();
                size_t thisIndex = 0;
                
                for (size_t i = 0; i < otherTerms.getSize(); i++) {
                    if (thisTerms[thisIndex]._exponent == otherTerms[i]._exponent) {
                        thisTerms[thisIndex]._coefficient += otherTerms[i]._coefficient;
                    } else {
                        thisTerms.insertAt(otherTerms[i], thisIndex);
                    }
                    thisIndex++;
                }
                return Ordinal(thisTerms);
            }
        }
    }
    Ordinal operator+( const size_t& other ) const { 
        if (isFinite()) {
            return Ordinal(_value + other);
        } else {
            auto terms = _value.getT2();
            if ( terms[terms.getSize() - 1]._exponent != 0) {
                terms.append( Term(other, 0) );
            } else {
                terms[terms.getSize() - 1]._coefficient += other;
            }
            return Ordinal(terms);
        }
    }

    Ordinal operator*( const Ordinal& other ) const { 
        if (isFinite() && other.isFinite()) {
            return Ordinal(_value * other._value);
        } else if (isTransfinite() && other.isFinite()) {
            auto terms = _value.getT2();
            terms.map( [other](const Term& x ) -> Term
                     { 
                         return Term(x._coefficient * other._value, x._exponent); 
                     } 
                     );
            return Ordinal(terms);
        } else if (isFinite() && other.isTransfinite()) {
            return other;
        } else {
            auto thisTerms = _value.getT2();
            auto otherTerms = other._value.getT2();
            transfinite result;

            for ( size_t i = 0; i < thisTerms.getSize(); i++ ) {
                for ( size_t j = 0; j < otherTerms.getSize(); j++ ) {
                    auto coeff = thisTerms[i]._coefficient * otherTerms[j]._coefficient;
                    unsigned exp;
                    exp = thisTerms[i]._exponent + otherTerms[j]._exponent;

                    bool hasSameExponent = false;
                    for (size_t k = 0; k < result.getSize(); k++ ) {
                        if (result[k]._exponent == exp) {
                            result[k]._coefficient += coeff;
                            hasSameExponent = true;
                        }
                    }

                    if (!hasSameExponent) {
                        result.append( Term(coeff, exp) );
                    }
                }
            }

            return Ordinal(result);
        }
    }
    Ordinal operator*( const size_t& other ) const { 
        if (isFinite()) {
            return Ordinal(_value * other);
        } else {
            auto terms = _value.getT2();
            terms.map( [other](const Term& x ) -> Term
                     { 
                        return Term(x._coefficient * other, x._exponent); 
                     } 
                     );
            return Ordinal(terms);
        }
    }

    Ordinal operator-( const Ordinal& other ) const {
        if (isTransfinite() && other.isFinite()) {
            return *this;
        } else if (isTransfinite() && other.isTransfinite() && (*this >= other)) {
            auto terms1 = _value.getT2();
            auto terms2 = other._value.getT2();
            if (terms1[0]._exponent > terms2[0]._exponent) {
                return *this;
            } else if (terms1[0]._exponent == terms2[0]._exponent) {
                transfinite terms;
                if (terms1[0]._coefficient > terms2[0]._coefficient) {
                    terms.append( Term(terms1[0]._coefficient - terms2[0]._coefficient, terms1[0]._exponent) );
                    for (size_t i = 1; i < terms1.getSize(); i++) {
                        terms.append( terms1[i] );
                    }
                    return Ordinal(terms);
                } else if (terms1[0]._coefficient == terms2[0]._coefficient) {
                    auto tail1 = terms1.subArray(1, terms1.getSize());
                    auto tail2 = terms2.subArray(1, terms2.getSize());
                    auto res = Ordinal( Ordinal(tail1) - Ordinal(tail2) );
                    return res;
                } else {
                    throw Exception( Exception::ErrorCode::TRANSFINITE_ARITHMETIC );
                }
            }
            return Ordinal(terms1);
        } else if (isFinite() && other.isFinite() && (*this >= other)) {
            auto arg1 =       _value.isInstanceOfT1() ?       _value.getT1() :       _value.getT2()[0]._coefficient;
            auto arg2 = other._value.isInstanceOfT1() ? other._value.getT1() : other._value.getT2()[0]._coefficient;
            return Ordinal( arg1 - arg2 );
        } else {
            throw Exception( Exception::ErrorCode::TRANSFINITE_ARITHMETIC );
        }
    }
    Ordinal operator-( const size_t& other ) const {
        if (isTransfinite()) {
            auto terms = _value.getT2();
            auto tail = terms[terms.getSize() - 1];
            if (tail._coefficient >= other && tail._exponent == 0) {
                terms[terms.getSize() - 1]._coefficient -= other;
            }
            return Ordinal(terms);
        } else {
            if (*this >= other) {
                return Ordinal(_value - other);
            } else {
                throw Exception( Exception::ErrorCode::TRANSFINITE_ARITHMETIC );
            }
        }
    }
public:
    bool isFinite() const {
        return (_value.isInstanceOfT1()  // finite
            || (_value.isInstanceOfT2()  // array of size 1 with term which exponent equals 0
            &&  _value.getT2()[0]._exponent == 0 
            &&  _value.getT2().getSize() == 1));
    }
    bool isTransfinite() const {
        return (_value.isInstanceOfT2() && _value.getT2().getSize() > 1);
    }
};

inline Ordinal operator+( const size_t& arg1, const Ordinal& arg2 ) {
    if (arg2.isTransfinite()) {
        return arg2;
    } else {
        return Ordinal(arg1) + arg2;
    }
};

inline Ordinal operator-( const size_t& arg1, const Ordinal& arg2 ) {
    if (arg2 > arg1) {
        throw Exception( Exception::ErrorCode::TRANSFINITE_ARITHMETIC );
    } else {
        return Ordinal(arg1) - arg2;
    }
};

inline Ordinal operator*( const size_t& arg1, const Ordinal& arg2 ) {
    if (arg2.isTransfinite()) {
        return arg2;
    } else {
        return Ordinal(arg1) + arg2;
    }
}

inline bool operator<( const size_t& arg1, const Ordinal& arg2 ) {
    if (arg2.isTransfinite()) {
        return true;
    } else {
        return arg1 < static_cast<size_t>(arg2);
    }
}

inline bool operator>( const size_t& arg1, const Ordinal& arg2 ) {
    if (arg2.isTransfinite()) {
        return false;
    } else {
        return arg1 > static_cast<size_t>(arg2);
    }
}

inline bool operator==( const size_t& arg1, const Ordinal& arg2 ) {
    if (arg2.isTransfinite()) {
        return false;
    } else {
        return arg1 == static_cast<size_t>(arg2);
    }
}

inline bool operator>=( const size_t& arg1, const Ordinal& arg2 ) {
    return arg1 > arg2 || arg1 == arg2;
}

inline bool operator<=( const size_t& arg1, const Ordinal& arg2 ) {
    return arg1 < arg2 || arg1 == arg2;
}

inline bool operator!=( const size_t& arg1, const Ordinal& arg2 ) {
    return !(arg1 == arg2);
}

#endif // ORDINAL_H