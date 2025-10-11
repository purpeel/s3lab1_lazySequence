#ifndef TRANSFINITE
#define TRANSFINITE

#include "../util/UniquePtr.hpp"

class Ordinal
{
public:
    Ordinal() : offset(0), count(0), power() {};
    
    Ordinal(const Ordinal& other) = default;
    Ordinal& operator=(const Ordinal& other) = default;
public:
    Ordinal operator+( const Ordinal& other ) const {
        Ordinal res;
        if (this->power > other.power) {
            res.count = this->count;
            res.power = this->power;
            res.offset = this->offset;
        } else {
            
        }

    }
private:
    int offset;
    int count;
    int power;
};

#endif // TRANSFINITE