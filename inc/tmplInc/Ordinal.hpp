#ifndef ORDINAL_H
#define ORDINAL_H

#include "UniquePtr.hpp"
#include "DynamicArray.hpp"
#include "Variant.hpp"

class Ordinal
{
private:
    struct Term {
        int _coefficient;
        int _exponent;

        Term() : _coefficient(0), _exponent(0) {}

        Term( const Term& other ) : _coefficient(other._coefficient), _exponent(other._exponent) {}

        Term(long coefficient, int exponent) : _coefficient(coefficient), _exponent(exponent) {}

        bool operator<(const Term& other) const {
            return this->_exponent < other._exponent;
        }
        bool operator==(const Term& other) const {
            return  this->_exponent    ==  other._exponent 
                    && this->_coefficient ==  other._coefficient;
        }
        bool operator>(const Term& other) const {
            return this->_exponent > other._exponent;
        }
        bool operator!=(const Term& other) const {
            return (*this != other);
        }
        bool operator>=(const Term& other) const {
            return (*this > other || *this == other);
        }
        bool operator<=(const Term& other) const {
            return (*this < other || *this == other);
        }
    };
    using finite = size_t;
    using transfinite = DynamicArray<Term>;

    Variant<finite, transfinite> value;
public:
    Ordinal() : value(0) {}
    Ordinal( const size_t& other ) : value(other) {}
    Ordinal& operator=( const size_t& other ) {
        this->value = other;
        return *this;
    }
    Ordinal( const DynamicArray<Term>& terms ) : value(terms) {}
    Ordinal( const Ordinal& other ) : value(other.value) {}
    Ordinal& operator=( const Ordinal& other ) {
        this->value = other.value;
        return *this;
    }
    const Ordinal omega() {
        DynamicArray<Ordinal::Term> terms;
        terms.append( Term(1, 1) );
        return Ordinal(terms);
    }
    const Ordinal one() { return Ordinal(1); }
    const Ordinal zero() { return Ordinal(0); }
    ~Ordinal() = default;
public:
    bool operator>( const Ordinal& other ) const {
        if (this->isFinite() && other.isFinite()) {
            return this->value > other.value;
        } else if (this->isFinite() && other.isTransfinite()) {
            return false;
        } else if (this->isTransfinite() && this->isFinite()) {
            return true;
        } else {
            auto thisTerms  = this->value.getT2();
            auto otherTerms = other.value.getT2();
            for ( int i = 0; i < thisTerms.getSize() < otherTerms.getSize() ? thisTerms.getSize() : otherTerms.getSize(); i++ ) {
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
        if (this->isFinite() && other.isFinite()) {
            return this->value == other.value;
        } else if (this->isFinite() != other.isTransfinite()) {
            return false;
        } else if (this->isTransfinite() && this->isTransfinite()) {
            auto thisTerms  = this->value.getT2();
            auto otherTerms = other.value.getT2();
            for ( int i = 0; i < thisTerms.getSize() < otherTerms.getSize() ? thisTerms.getSize() : otherTerms.getSize(); i++ ) {
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
        if (this->isFinite() && other.isFinite()) {
            return this->value >= other.value;
        } else if (this->isFinite() && other.isTransfinite()) {
            return false;
        } else if (this->isTransfinite() && this->isFinite()) {
            return true;
        } else {
            auto thisTerms  = this->value.getT2();
            auto otherTerms = other.value.getT2();
            for ( int i = 0; i < thisTerms.getSize() < otherTerms.getSize() ? thisTerms.getSize() : otherTerms.getSize(); i++ ) {
                auto thisTerm = thisTerms[i];
                auto otherTerm = otherTerms[i];

                if (thisTerm < otherTerm) {
                    return false;
                } else if (thisTerm >= otherTerm) {
                    return true;
                }
            }
            return false;
        }
    }
    bool operator!=( const Ordinal& other ) const {
        return !(*this == other);
    }

    bool operator>( const size_t& other ) const {
        if (this->isFinite()) {
            return this->value > other;
        } else if (this->isTransfinite()) {
            return true;
        }
    }
    bool operator==( const size_t& other ) const {
        if (this->isFinite()) {
            return this->value == other;
        } else if (this->isTransfinite()) {
            return false;
        }
    }
    bool operator<( const size_t& other ) const {
        if (this->isFinite()) {
            return this->value < other;
        } else if (this->isTransfinite()) {
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

    Ordinal operator+( const Ordinal& other ) const { 
        if (this->isFinite() && other.isFinite()) {
            return Ordinal(this->value + other.value);
        } else if (this->isTransfinite() && other.isFinite()) {
            auto terms = this->value.getT2();
            if ( terms[terms.getSize() - 1]._exponent != 0) {
                terms.append( Term(other.value.getT1(), 0) );
            } else {
                terms[terms.getSize() - 1]._coefficient += other.value;
            }
        } else if (this->isFinite() && other.isTransfinite()) {
            return other;
        } else {
            if (*this < other) {
                return other;
            } else if (*this == other) {
                return *this * 2;
            } else {
                auto thisTerms  = this->value.getT2();
                auto otherTerms  = other.value.getT2();
                size_t thisIndex = 0;
                
                for (int i = 0; i < otherTerms.getSize(); i++) {
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

    Ordinal operator*( const Ordinal& other ) const { 
        if (this->isFinite() && other.isFinite()) {
            return Ordinal(this->value * other.value);
        } else if (this->isTransfinite() && other.isFinite()) {
            auto terms = this->value.getT2();
            terms.map( [other](const Term& x ) -> Term
                     { 
                         return Term(x._coefficient * other.value, x._exponent); 
                     } 
                     );
            return Ordinal(terms);
        } else if (this->isFinite() && other.isTransfinite()) {
            return other;
        } else {
            auto thisTerms = this->value.getT2();
            auto otherTerms = other.value.getT2();
            transfinite result;

            for ( int i = 0; i < thisTerms.getSize(); i++ ) {
                for ( int j = 0; j < otherTerms.getSize(); j++ ) {
                    auto coeff = thisTerms[i]._coefficient * otherTerms[j]._coefficient;
                    int exp;
                    exp = thisTerms[i]._exponent + otherTerms[j]._exponent;

                    bool hasSameExponent = false;
                    for (int k = 0; k < result.getSize(); k++ ) {
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

    Ordinal operator-( const Ordinal& other ) {
        if (this->isTransfinite() && other.isFinite()) {
            auto terms = this->value.getT2();
            terms[terms.getSize() - 1]._coefficient -= other.value;
            return Ordinal(terms);
        } else if (*this >= other) {
            
        }
    }
public:
    bool isFinite() const {
        return this->value.isInstanceOfT1();
    }
    bool isTransfinite() const {
        return this->value.isInstanceOfT2();
    }
};

inline Ordinal operator+( const size_t& arg1, const Ordinal& arg2 ) {
    if (arg2.isTransfinite()) {
        return arg2;
    } else {
        return Ordinal(arg1) + arg2;
    }
};

inline Ordinal operator*( const size_t& arg1, const Ordinal& arg2 ) {
    if (arg2.isTransfinite()) {
        return arg2;
    } else {
        return Ordinal(arg1) + arg2;
    }
}

#endif // ORDINAL_H