#include "Data/AverageAccumulator.h"

using namespace Data;

AverageAccumulator::AverageAccumulator(const JsonURI& storageURI) : m_storageURI(storageURI)
{
    serialize();
}


float AverageAccumulator::add(float value)
{
    deserialize();
    m_accumulator += value;
    m_count++;
    serialize();
    return m_accumulator / m_count;
}


float AverageAccumulator::get()
{
    deserialize();
    return m_accumulator / m_count;
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
    json data = m_storageURI.deserialize();
    m_count = data.at("count");
    m_accumulator = data.at("accumulator");
}