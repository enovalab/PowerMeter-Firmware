#ifndef StreamAverage_h
#define StreamAverage_h

/**
 * @brief Class for averaging without having huge 
 *        data Buffers consuming all your memory.
 * 
 * @tparam T The datatype to average
 */
template<typename T>
class StreamAverage
{
private:
    T max = 0;
    T min = 0;
    size_t numValues = 0;
    T sum = 0;

public:
    /**
     * @brief Add new value to the average
     * 
     * @param value The value to add
     * @return The new Average
     */
    inline T add(T value)
    {
        if(numValues == 0)
        {
            max = value;
            min = value;
        }
        
        if(value > max) max = value;
        if(value < min) min = value;

        sum += value;
        return (T)sum / ++numValues;
    }


    /**
     * @brief Add new value to the average
     * 
     * @param value The value to add
     * @return The new Average
     */
    inline T operator<<(T value)
    {
        return add(value);
    }


    /**
     * @brief Get the current average
     */
    inline T get() const
    {
        if(numValues == 0) return 0;
        return sum / numValues;
    }


    /**
     * @brief Get the maximum value added to the average
     *        since the last reset 
     */
    inline T getMax() const
    {
        return max;
    }


    /**
     * @brief Get the minimum value added to the average
     *        since the last reset
     */
    inline T getMin() const
    {
        return min;
    }

    /**
     * @brief Get the number of values the average holds
     * 
     * @return size_t 
     */
    inline size_t getNumValues() const
    {
        return numValues;
    }


    /**
     * @brief Resets the average, maximum and minimum
     * 
     */
    inline void reset()
    {
        max = 0;
        min = 0;
        numValues = 0;
        sum = 0;
    }
    
    /**
     * @brief Get the current average implicitly
     */
    inline operator T() const
    {
        return get();
    }
};

#endif