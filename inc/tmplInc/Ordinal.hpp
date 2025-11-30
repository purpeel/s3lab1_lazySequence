#ifndef ORDINAL_H
#define ORDINAL_H

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
public:
    using finite = size_t;
    using transfinite = DynamicArray<Term>;
private:
    Variant<finite, transfinite> _value;
public:
    Ordinal() : _value(0) {}
    Ordinal( const size_t& other ) : _value(other) {}
    Ordinal& operator=( const size_t& other ) {
        this->_value = other;
        return *this;
    }
    Ordinal( const transfinite& terms ) : _value(terms) {}
    Ordinal( const Ordinal& other ) : _value(other._value) {}
    Ordinal& operator=( const Ordinal& other ) {
        this->_value = other._value;
        return *this;
    }
    static const Ordinal omega() {
        transfinite terms;
        terms.append( Term(1, 1) );
        static const Ordinal res = Ordinal(terms);
        return res;
    }
    ~Ordinal() = default;
public:
    explicit operator size_t() const {
        if (this->isFinite()) {
            if (this->_value.isInstanceOfT1()) {
                return static_cast<size_t>(this->_value.getT1());
            } else {
                return this->_value.getT2()[0]._coefficient;
            }
        } else {
            throw Exception( Exception::ErrorCode::INVALID_TYPE );
        }
    }
    bool operator>( const Ordinal& other ) const {
        if (this->isFinite() && other.isFinite()) {
            return this->_value > other._value;
        } else if (this->isFinite() && other.isTransfinite()) {
            return false;
        } else if (this->isTransfinite() && this->isFinite()) {
            return true;
        } else {
            auto thisTerms  = this->_value.getT2();
            auto otherTerms = other._value.getT2();
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
            return this->_value == other._value;
        } else if (this->isTransfinite() != other.isTransfinite()) {
            return false;
        } else {
            auto thisTerms  = this->_value.getT2();
            auto otherTerms = other._value.getT2();
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
            return this->_value >= other._value;
        } else if (this->isFinite() && other.isTransfinite()) {
            return false;
        } else if (this->isTransfinite() && this->isFinite()) {
            return true;
        } else {
            auto thisTerms  = this->_value.getT2();
            auto otherTerms = other._value.getT2();
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
            return this->_value > other;
        } else {
            return true;
        }
    }
    bool operator==( const size_t& other ) const {
        if (this->isFinite()) {
            return this->_value == other;
        } else  {
            return false;
        }
    }
    bool operator<( const size_t& other ) const {
        if (this->isFinite()) {
            return this->_value < other;
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
        if (this->isFinite()) {
            this->_value = this->_value.getT1() + 1;
            return *this;
        } else {
            if (this->_value.getT2()[this->_value.getT2().getSize()-1]._exponent == 0) {
                this->_value.getT2()[this->_value.getT2().getSize()-1]._coefficient++;
            } else {
                this->_value.getT2().append( Term(1, 0) );
            }
            return *this;
        }
    }

    Ordinal operator++(int) {
        auto temp = *this;
        ++(*this);
        return temp;
    }

    Ordinal operator+( const Ordinal& other ) const { 
        if (this->isFinite() && other.isFinite()) {
            return Ordinal(this->_value + other._value);
        } else if (this->isTransfinite() && other.isFinite()) {
            auto terms = this->_value.getT2();
            if ( terms[terms.getSize() - 1]._exponent != 0) {
                terms.append( Term(other._value.getT1(), 0) );
            } else {
                terms[terms.getSize() - 1]._coefficient += other._value;
            }
            return Ordinal(terms);
        } else if (this->isFinite() && other.isTransfinite()) {
            return other;
        } else {
            if (*this < other) {
                return other;
            } else if (*this == other) {
                return *this * 2;
            } else {
                auto thisTerms   = this->_value.getT2();
                auto otherTerms  = other._value.getT2();
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
    Ordinal operator+( const size_t& other ) const { 
        if (this->isFinite()) {
            return Ordinal(this->_value + other);
        } else {
            auto terms = this->_value.getT2();
            if ( terms[terms.getSize() - 1]._exponent != 0) {
                terms.append( Term(other, 0) );
            } else {
                terms[terms.getSize() - 1]._coefficient += other;
            }
            return Ordinal(terms);
        }
    }
    // Ordinal operator+( const Cardinal& other ) const { 
    //     if (other.isTransfinite()) {
    //         throw Exception( Exception::ErrorCode::INVALID_TYPE );
    //     } else if (this->isTransfinite() && other.isFinite()) {
    //         auto terms = this->_value.getT2();
    //         if ( terms[terms.getSize() - 1]._exponent != 0) {
    //             terms.append( Term(static_cast<size_t>(other), 0) );
    //         } else {
    //             terms[terms.getSize() - 1]._coefficient += static_cast<size_t>(other);
    //         }
    //         return Ordinal(terms);
    //     } else if (this->isFinite() && other.isFinite()) {
    //         return Ordinal( static_cast<size_t>(*this) + static_cast<size_t>(other) );
    //     }
    // }

    Ordinal operator*( const Ordinal& other ) const { 
        if (this->isFinite() && other.isFinite()) {
            return Ordinal(this->_value * other._value);
        } else if (this->isTransfinite() && other.isFinite()) {
            auto terms = this->_value.getT2();
            terms.map( [other](const Term& x ) -> Term
                     { 
                         return Term(x._coefficient * other._value, x._exponent); 
                     } 
                     );
            return Ordinal(terms);
        } else if (this->isFinite() && other.isTransfinite()) {
            return other;
        } else {
            auto thisTerms = this->_value.getT2();
            auto otherTerms = other._value.getT2();
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
    Ordinal operator*( const size_t& other ) const { 
        if (this->isFinite()) {
            return Ordinal(this->_value * other);
        } else {
            auto terms = this->_value.getT2();
            terms.map( [other](const Term& x ) -> Term
                     { 
                        return Term(x._coefficient * other, x._exponent); 
                     } 
                     );
            return Ordinal(terms);
        }
    }

    Ordinal operator-( const Ordinal& other ) const {
        if (this->isTransfinite() && other.isFinite()) {
            return *this;
        } else if (this->isTransfinite() && other.isTransfinite() && (*this >= other)) {
            auto terms1 = this->_value.getT2();
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
                    auto* tail1 = terms1.subArray(1, terms1.getSize());
                    auto* tail2 = terms2.subArray(1, terms2.getSize());
                    auto res = Ordinal( Ordinal(*tail1) - Ordinal(*tail2) );
                    delete tail1;
                    delete tail2;
                    return res;
                } else {
                    throw Exception( Exception::ErrorCode::TRANSFINITE_ARITHMETIC );
                }
            }
            // if (terms1.getSize() == 1 && terms1[0]._exponent == 0) {
            //     return Ordinal(terms1[0]._coefficient);
            // }
            return Ordinal(terms1);
        } else if (this->isFinite() && other.isFinite() && (*this >= other)) {
            return Ordinal( this->_value.getT1() - other._value.getT1() );
        } else {
            throw Exception( Exception::ErrorCode::TRANSFINITE_ARITHMETIC );
        }
    }
    Ordinal operator-( const size_t& other ) const {
        if (this->isTransfinite()) {
            auto terms = this->_value.getT2();
            auto tail = terms[terms.getSize() - 1];
            if (tail._coefficient >= other && tail._exponent == 0) {
                terms[terms.getSize() - 1]._coefficient -= other;
            }
            return Ordinal(terms);
        } else {
            if (*this >= other) {
                return Ordinal(this->_value - other);
            } else {
                throw Exception( Exception::ErrorCode::TRANSFINITE_ARITHMETIC );
            }
        }
    }
    // Ordinal operator-( const Cardinal& other ) const {
    //     if (this->isTransfinite() || other.isTransfinite()) {
    //         throw Exception( Exception::ErrorCode::INVALID_TYPE ); 
    //     } else if (static_cast<size_t>(other) > static_cast<size_t>(*this)) {
    //         throw Exception( Exception::ErrorCode::TRANSFINITE_ARITHMETIC );
    //     } else {
    //         return Ordinal(this->_value.getT1() - static_cast<size_t>(other));
    //     }
    // }
public:
    bool isFinite() const {
        return this->_value.isInstanceOfT1()  // finite
            || this->_value.isInstanceOfT2()  // array of size 1 with term which exponent equals 0
            && this->_value.getT2()[0]._exponent == 0 
            && this->_value.getT2().getSize() == 1;
    }
    bool isTransfinite() const {
        return this->_value.isInstanceOfT2();
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