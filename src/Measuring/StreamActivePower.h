#ifndef MEASURING_STREAMACTIVEPOWER_H
#define MEASURING_STREAMACTIVEPOWER_H

#include <stddef.h>

namespace Measuring
{
    template<typename T>
    class StreamActivePower
    {
    private:
        size_t numValues = 0;
        T sum = 0;

    public:
        inline T add(T instantU, T instantI) noexcept
        {
            sum += instantU * instantI;
            return sum / ++numValues;
        }


        inline T operator<<(T instantP) noexcept
        {
            sum += instantP;
            return sum / ++numValues;
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
            if(numValues == 0) 
                return 0;

            return sum / numValues;
        }


        inline operator T() const noexcept
        {
            return get();
        }
    };
};

#endif