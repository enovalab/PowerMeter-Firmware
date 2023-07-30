#include "AverageAccumulator.h"


AverageAccumulator::AverageAccumulator(const JsonURI& storageURI) : m_storageURI(storageURI)
{}


float AverageAccumulator::add(float value)
{
    deserialize();
    m_sum += value;
    m_count++;
    serialize();
    if(m_count == 0.0f)
        return 0.0f;
    return m_sum / m_count;
}


float AverageAccumulator::getAverage()
{
    deserialize();
    if(m_count == 0.0f)
        return 0.0f;
    return m_sum / m_count;
}


size_t AverageAccumulator::getCount()
{
    deserialize();
    return m_count;
}


void AverageAccumulator::reset()
{
    m_count = 0;
    m_sum = 0.0f;
    serialize();
}


void AverageAccumulator::erase() const
{
    m_storageURI.erase();
}


void AverageAccumulator::serialize()
{
    json data;
    data["count"] = m_count;
    data["sum"] = m_sum;
    m_storageURI.serialize(data);
}


void AverageAccumulator::deserialize()
{
    try
    {
        json data = m_storageURI.deserialize();
        m_count = data.at("count");
        m_sum = data.at("sum");
    }
    catch(const std::exception& exception)
    {
        reset();
    }
}