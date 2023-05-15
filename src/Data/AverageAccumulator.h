#ifndef DATA_AVERAGEACCUMULATOR_H
#define DATA_AVERAGEACCUMULATOR_H

#include "Data/JsonURI.h"

namespace Data
{
    class AverageAccumulator
    {
    public:
        AverageAccumulator(const JsonURI& storageURI);
        float get();
        float add(float value);
        void reset();
    
    private:
        void deserialize();
        void serialize();

        size_t m_count = 0;
        float m_accumulator = 0.0f;
        JsonURI m_storageURI;
    };
}

#endif