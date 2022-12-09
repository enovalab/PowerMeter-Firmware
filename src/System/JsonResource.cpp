#include "System/JsonResource.h"
#include "Logging/Log.h"
#include <fstream>

using namespace System;


JsonResource::JsonResource(const std::string& path, const json::json_pointer& jsonPointer) :
    m_path(path),
    m_jsonPointer(jsonPointer)
{}


JsonResource::JsonResource(const std::string& uri)
{
    size_t seperatorIndex = uri.find('#');
    if(seperatorIndex != std::string::npos)
    {
        m_path = uri.substr(0, seperatorIndex);
        try
        {
            m_jsonPointer = json::json_pointer(uri.substr(seperatorIndex + 1, uri.size()));
        }
        catch(const std::exception& e)
        {
            Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << e.what() << std::endl;
        }
        catch(...)
        {
            Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
        }
    }
    else
    {
        m_path = uri;
    }
}

void JsonResource::setJsonPointer(const json::json_pointer& jsonPointer)
{
    m_jsonPointer = jsonPointer;
}


void JsonResource::setFilePath(const std::string& path)
{
    m_path = path;
}



std::string JsonResource::getFilePath() const
{
    return m_path;
}


json::json_pointer JsonResource::getJsonPointer() const
{
    return m_jsonPointer;
}


json JsonResource::deserialize() const
{
    std::ifstream file(m_path);
    try
    {
        return json::parse(file).at(m_jsonPointer);
    }
    catch(const json::parse_error& e)
    {}
    catch(const std::exception& e)
    {
        Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << e.what() << std::endl;
    }
    catch(...)
    {
        Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
    }
    return json();
}


void JsonResource::serialize(const json& data) const
{
    try
    {
        if(m_jsonPointer.empty())
        {
            std::ofstream file(m_path);
            file << data.dump(1, '\t') << std::flush;
        }
        else
        {
            json parentData = JsonResource(m_path, json::json_pointer()).deserialize();
            std::ofstream file(m_path);
            parentData[m_jsonPointer] = data;
            file << parentData.dump(1, '\t') << std::flush;
        }
    }
    catch(const std::exception& e)
    {
        Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << e.what() << std::endl;
    }
    catch(...)
    {
        Logging::Logger[Logging::Level::Error] << SOURCE_LOCATION << "Unexpected Exception" << std::endl;
    }
}