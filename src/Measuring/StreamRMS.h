#ifndef MEASURING_STREAMRMS_H
#define MEASURING_STREAMRMS_H

#include <stddef.h>
#include <math.h>

namespace Measuring
{
    template<typename T>
    class StreamRMS
    {
    private:
        size_t numValues = 0;
        T sum = 0;

    public:
        inline T add(T value) noexcept
        {
            sum += value * value;
            return sqrt(sum / ++numValues);
        }


        inline T operator<<(T value) noexcept
        {
            return add(value);
        }

    
        inline size_t getNumValues() const noexcept
        {
            return numValues;
        }


        inline void reset() noexcept
        {
            numValues = 0;
            sum = 0;
        }


        inline T get() const noexcept
        {
            if(numValues == 0) return 0;
            return sqrt(sum / numValues);
        }


        inline operator T() const noexcept
        {
            return get();
        }
    };
}

#endif