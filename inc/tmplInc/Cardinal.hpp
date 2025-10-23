#ifndef CARDINAL_H
#define CARDINAL_H

#include "Variant.hpp"

enum infiniteCardinal;

class Cardinal 
{
public:
    Cardinal() : value(0) {}
    Cardinal( const long& other );
    Cardinal( const Cardinal& other ) : value(other.value) {}
    Cardinal& operator=( const Cardinal& other ) {
        this->value = other.value;
        return *this;
    }
    Cardinal& operator=( const long& other ) {
        this->value = other;
        return *this;
    }
    ~Cardinal() { this->value.~Variant(); }
public:
    bool isFinite() const {
        return this->value.isInstanceOfT1();
    }
    bool isTransfinite() const {
        return this->value.isInstanceOfT2();
    }


private:
    using finite = long;
    using transfinite = infiniteCardinal;
    Variant<finite, transfinite> value;
};

enum infiniteCardinal
{
    ALEPH_NULL = 0,
    CONTINUUM = 1
};

#endif // CARDINAL_H
