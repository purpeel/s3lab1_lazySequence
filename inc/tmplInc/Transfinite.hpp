#ifndef TRANSFINITE
#define TRANSFINITE

#include "UniquePtr.hpp"
#include "DynamicArray.hpp"
#include "Variant.hpp"

class Ordinal
{
private:
    struct Term {
        int coefficient;
        UniquePtr<Ordinal> exponent;

        Term(long coefficient, UniquePtr<Ordinal> exponent) : coefficient(coefficient), exponent(exponent) {}

        bool operator<(const Term& other) const {
            return *this->exponent < *other.exponent;
        }
        bool operator==(const Term& other) const {
            return *this->exponent    == *other.exponent 
                 && this->coefficient ==  other.coefficient;
        }
        bool operator>(const Term& other) const {
            return *this->exponent > *other.exponent;
        }
    };

    using finite = long;
    using transfinite = DynamicArray<Term>;

    Variant<finite, transfinite> value;
public:
    Ordinal() : value(0) {}
    Ordinal( const long& value ) : value(value) {}
    Ordinal( const DynamicArray<Term>& terms ) : value(terms) {}
    Ordinal& operator=( const Ordinal& other ) {
        this->value = other.value;
    }
    ~Ordinal() { this->value.~Variant(); }

    Ordinal omega() {
        DynamicArray<Term> terms;
        terms.append( Term(1, UniquePtr<Ordinal>()) );
        this->value = terms;
        return Ordinal(terms);
    }
    Ordinal one() { return Ordinal(1); }
    Ordinal zero() { return Ordinal(0); }
public:
    bool operator>( const Ordinal& other ) const {
        if (this->isFinite() && other.isFinite()) {
            return this->value > other.value;
        } else if (this->isFinite() && other.isTransfinite()) {
            return false;
        } else if (this->isTransfinite() && this->isFinite()) {
            return true;
        } else if (this->isTransfinite() && this->isTransfinite()) {
            auto thisTerms  = this->value.getT2();
            auto otherTerms = other.value.getT2();
            for ( int i = 0; i < thisTerms.getSize() < otherTerms.getSize() ? thisTerms.getSize() : otherTerms.getSize(); i++ ) {
                auto thisTerm = thisTerms[i];
                auto otherTerm = otherTerms[i];

                if (thisTerm < otherTerm) {
                    return false;
                } else if (thisTerm > otherTerm) {
                    return true;
                } else if (thisTerm == otherTerm) {
                    continue;
                } 
            }
            return false;
        }
    }
    bool operator==( const Ordinal& other ) const {
        if (this->isFinite() && other.isFinite()) {
            return this->value == other.value;
        } else if (this->isFinite() && other.isTransfinite()) {
            return false;
        } else if (this->isTransfinite() && this->isFinite()) {
            return false;
        } else if (this->isTransfinite() && this->isTransfinite()) {
            auto thisTerms  = this->value.getT2();
            auto otherTerms = other.value.getT2();
            for ( int i = 0; i < thisTerms.getSize() < otherTerms.getSize() ? thisTerms.getSize() : otherTerms.getSize(); i++ ) {
                auto thisTerm = thisTerms[i];
                auto otherTerm = otherTerms[i];

                if (thisTerm < otherTerm) {
                    return false;
                } else if (thisTerm > otherTerm) {
                    return false;
                } else if (thisTerm == otherTerm) {
                    continue;
                } 
            }
            return true;
        }
    }
    bool operator<( const Ordinal& other ) const {
        if (this->isFinite() && other.isFinite()) {
            return this->value < other.value;
        } else if (this->isFinite() && other.isTransfinite()) {
            return true;
        } else if (this->isTransfinite() && this->isFinite()) {
            return false;
        } else if (this->isTransfinite() && this->isTransfinite()) {
            auto thisTerms  = this->value.getT2();
            auto otherTerms = other.value.getT2();
            for ( int i = 0; i < thisTerms.getSize() < otherTerms.getSize() ? thisTerms.getSize() : otherTerms.getSize(); i++ ) {
                auto thisTerm = thisTerms[i];
                auto otherTerm = otherTerms[i];

                if (thisTerm < otherTerm) {
                    return true;
                } else if (thisTerm > otherTerm) {
                    return false;
                } else if (thisTerm == otherTerm) {
                    continue;
                } 
            }
            return false;
        }
    }
    bool operator<=( const Ordinal& other ) const {
        return *this < other || *this == other;
    }
    bool operator>=( const Ordinal& other ) const {
        return *this > other || *this == other;
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
            if ( *(terms[terms.getSize() - 1].exponent) != 0) {
                terms.append( Term(other.value.getT1(), UniquePtr<Ordinal>()) );
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
            terms.map( [=](Term x) -> Term 
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
                    UniquePtr<Ordinal> exp;
                    *exp = *thisTerms[i].exponent + *otherTerms[j].exponent;

                    bool hasSameExponent = false;
                    for (int k = 0; k < result.getSize(); k++ ) {
                        if (*result[k].exponent == *exp) {
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

inline Ordinal operator+( const long& arg1, const Ordinal& arg2 ) {
    if (arg2.isTransfinite()) {
        return arg2;
    } else {
        return Ordinal(arg1) + arg2;
    }
}

class Cardinal;

#endif // TRANSFINITE