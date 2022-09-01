#ifndef AccumulationAverage_h
#define AccumulationAverage_h


/**
 * @brief Class for averaging without having huge 
 *        data Buffers consuming all your memory.
 * 
 * @tparam T Type of the data to average
 */
template<typename T>
class AccumulationAverage
{
    private:
    size_t iterations = 0;
    T accu;

    public:
    /**
     * @brief Add new value to the average
     * 
     * @param value The value to add
     * @return The new Average
     */
    T add(T value)
    {
        if(iterations == 0)
            accu = value;
        else
            accu = (value + accu * iterations) / (1 + iterations);
        
        iterations++;
        return accu;
    }


    /**
     * @brief Add new value to the average
     * 
     * @param value The value to add
     * @return The new Average
     */
    T operator<<(T value)
    {
        if(iterations == 0)
            accu = value;
        else
            accu = (value + accu * iterations) / (1 + iterations);
        
        iterations++;
        return accu;
    }


    /**
     * @brief Add new value to the average
     * 
     * @param value The value to add
     * @return The new Average
     */
    T operator+=(T value)
    {
        if(iterations == 0)
            accu = value;
        else
            accu = (value + accu * iterations) / (1 + iterations);
        
        iterations++;
        return accu;
    }


    /**
     * @brief Get the current average
     * 
     * @return The Momentary average
     */
    T get() const
    {
        return accu;
    }


    /**
     * @brief Resets the average
     * 
     */
    void reset()
    {
        iterations = 0;
        accu = 0;
    }
};

#endif