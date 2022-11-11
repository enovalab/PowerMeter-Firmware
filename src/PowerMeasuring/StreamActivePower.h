#ifndef STREAMACTIVEPOWER_H
#define STREAMACTIVEPOWER_H

#include <stddef.h>

template<typename T>
class StreamActivePower
{
private:
    size_t numValues = 0;
    T sum = 0;

public:
    inline T add(T instantU, T instantI)
    {
        sum += instantU * instantI;
        return sum / ++numValues;
    }


    inline T operator<<(T instantP)
    {
        sum += instantP;
        return sum / ++numValues;
    }

   
    inline size_t getNumValues() const
    {
        return numValues;
    }

    inline void reset()
    {
        numValues = 0;
        sum = 0;
    }


    inline T get() const
    {
        if(numValues == 0) return 0;
        return sum / numValues;
    }



    inline operator T() const
    {
        return get();
    }
};

#endif