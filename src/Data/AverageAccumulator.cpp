#include "Data/AverageAccumulator.h"

using namespace Data;

AverageAccumulator::AverageAccumulator(const JsonURI& storageURI) : m_storageURI(storageURI)
{}


float AverageAccumulator::add(float value)
{
    deserialize();
    m_accumulator += value;
    m_count++;
    serialize();
    if(m_count == 0.0f)
        return 0.0f;
    return m_accumulator / m_count;
}


float AverageAccumulator::getAverage()
{
    deserialize();
    if(m_count == 0.0f)
        return 0.0f;
    return m_accumulator / m_count;
}


size_t AverageAccumulator::getCount()
{
    deserialize();
    return m_count;
}


void AverageAccumulator::reset()
{
    m_count = 0;
    m_accumulator = 0.0f;
    serialize();
}


void AverageAccumulator::serialize()
{
    json data;
    data["count"] = m_count;
    data["accumulator"] = m_accumulator;
    m_storageURI.serialize(data);
}


void AverageAccumulator::deserialize()
{
    try
    {
        json data = m_storageURI.deserialize();
        m_count = data.at("count");
        m_accumulator = data.at("accumulator");
    }
    catch(const std::exception& exception)
    {
        reset();
    }
}