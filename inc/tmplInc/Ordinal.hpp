#ifndef ORDINAL_H
#define ORDINAL_H

#include "UniquePtr.hpp"
#include "DynamicArray.hpp"
#include "Variant.hpp"

struct Term;

class Ordinal
{
private:
    using finite = long;
    using transfinite = DynamicArray<Term>;

    Variant<finite, transfinite> value;
public:
    Ordinal() : value(0) {}
    Ordinal( const long& other ) : value(other) {}
    Ordinal& operator=( const long& other ) {
        this->value = other;
        return *this;
    }
    Ordinal( const DynamicArray<Term>& terms ) : value(terms) {}
    Ordinal& operator=( const Ordinal& other ) {
        this->value = other.value;
        return *this;
    }
    ~Ordinal() { this->value.~Variant(); }
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

    bool operator>( const long& other ) const {
        if (this->isFinite()) {
            return this->value > other;
        } else if (this->isTransfinite()) {
            return true;
        }
    }
    bool operator==( const long& other ) const {
        if (this->isFinite()) {
            return this->value == other;
        } else if (this->isTransfinite()) {
            return false;
        }
    }
    bool operator<( const long& other ) const {
        if (this->isFinite()) {
            return this->value < other;
        } else if (this->isTransfinite()) {
            return false;
        }
    }
    bool operator<=( const long& other ) const {
        return *this < other || *this == other;
    }
    bool operator>=( const long& other ) const {
        return *this > other || *this == other;
    }
    bool operator!=( const long& other ) const {
        return !(*this == other);
    }

    Ordinal operator+( const Ordinal& other ) const { 
        if (this->isFinite() && other.isFinite()) {
            return Ordinal(this->value + other.value);
        } else if (this->isTransfinite() && other.isFinite()) {
            auto terms = this->value.getT2();
            if ( terms[terms.getSize() - 1].exponent != 0) {
                terms.append( Term(other.value.getT1(), Ordinal()) );
            } else {
                terms[terms.getSize() - 1].coefficient += other.value;
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
                    if (thisTerms[thisIndex].exponent == otherTerms[i].exponent) {
                        thisTerms[thisIndex].coefficient += otherTerms[i].coefficient;
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
            terms.map( [](const Term& x, const Ordinal& other ) -> Term
                     { 
                         return Term(x.coefficient * other.value, x.exponent); 
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
                    auto coeff = thisTerms[i].coefficient * otherTerms[j].coefficient;
                    Ordinal exp;
                    exp = thisTerms[i].exponent + otherTerms[j].exponent;

                    bool hasSameExponent = false;
                    for (int k = 0; k < result.getSize(); k++ ) {
                        if (result[k].exponent == exp) {
                            result[k].coefficient += coeff;
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
public:
    bool isFinite() const {
        return this->value.isInstanceOfT2();
    }
    bool isTransfinite() const {
        return this->value.isInstanceOfT1();
    }
};

inline const Ordinal omega() {
    DynamicArray<Term> terms;
    terms.append( Term(1, Ordinal()) );
    return Ordinal(terms);
}
inline const Ordinal one() { return Ordinal(1); }
inline const Ordinal zero() { return Ordinal(0); }

inline Ordinal operator+( const long& arg1, const Ordinal& arg2 ) {
    if (arg2.isTransfinite()) {
        return arg2;
    } else {
        return Ordinal(arg1) + arg2;
    }
};

inline Ordinal operator*( const long& arg1, const Ordinal& arg2 ) {
    if (arg2.isTransfinite()) {
        return arg2;
    } else {
        return Ordinal(arg1) + arg2;
    }
}

struct Term {
        int coefficient;
        Ordinal exponent;

        Term(long coefficient, Ordinal exponent) : coefficient(coefficient), exponent(exponent) {}

        bool operator<(const Term& other) const {
            return this->exponent < other.exponent;
        }
        bool operator==(const Term& other) const {
            return  this->exponent    ==  other.exponent 
                 && this->coefficient ==  other.coefficient;
        }
        bool operator>(const Term& other) const {
            return this->exponent > other.exponent;
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

#endif // ORDINAL_H