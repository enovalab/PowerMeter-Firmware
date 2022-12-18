#include "Data/JsonURI.h"
#include "Logging/Log.h"
#include <fstream>

using namespace Data;


JsonURI::JsonURI(const std::string& path, const json::json_pointer& jsonPointer) :
    m_path(path),
    m_jsonPointer(jsonPointer)
{}


JsonURI::JsonURI(const std::string& uri)
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


void JsonURI::serialize(const json& data) const
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
            json parentData = JsonURI(m_path, json::json_pointer()).deserialize();
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


void JsonURI::setJsonPointer(const json::json_pointer& jsonPointer)
{
    m_jsonPointer = jsonPointer;
}


void JsonURI::setFilePath(const std::string& path)
{
    m_path = path;
}



std::string JsonURI::getFilePath() const
{
    return m_path;
}


json::json_pointer JsonURI::getJsonPointer() const
{
    return m_jsonPointer;
}


json JsonURI::deserialize() const
{
    std::ifstream file(m_path);
    if(file.peek() < 0)
        return json();
    return json::parse(file).at(m_jsonPointer);
}


JsonURI& JsonURI::operator/=(const json::json_pointer& jsonPointer)
{
    m_jsonPointer /= jsonPointer;
    return *this;
}


JsonURI Data::operator/(JsonURI lhs, const json::json_pointer& rhs)
{
    lhs /= rhs;
    return lhs;
}