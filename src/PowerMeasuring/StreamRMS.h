#ifndef STREAMRMS_H
#define STREAMRMS_H

#include <stddef.h>
#include <math.h>

template<typename T>
class StreamRMS
{
private:
    size_t numValues = 0;
    T sum = 0;

public:
    inline T add(T value)
    {
        sum += value * value;
        return sqrt(sum / ++numValues);
    }


    inline T operator<<(T value)
    {
        return add(value);
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
        return sqrt(sum / numValues);
    }


    inline operator T() const
    {
        return get();
    }
};

#endif