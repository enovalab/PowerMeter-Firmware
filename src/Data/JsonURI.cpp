#include "Data/JsonURI.h"
#include "Logging/Log.h"
#include "ErrorHandling/ExceptionTrace.h"

#include <boost/filesystem/fstream.hpp>
#include <exception>
#include <sstream>

using namespace Data;


JsonURI::JsonURI(const fs::path& path, const json::json_pointer& jsonPointer) :
    m_path(path),
    m_jsonPointer(jsonPointer)
{}


JsonURI::JsonURI(const std::string& uri)
{
    try
    {
        size_t seperatorIndex = uri.find('#');
        if(seperatorIndex != std::string::npos)
        {
            m_path = uri.substr(0, seperatorIndex);
            m_jsonPointer = json::json_pointer(uri.substr(seperatorIndex + 1, uri.size()));
        }
        else
        {
            m_path = uri;
        }
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to construct from \"" << uri << "\"";
        ErrorHandling::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


void JsonURI::serialize(const json& data) const
{
    try
    {
        fs::ofstream file(m_path);
        if(m_jsonPointer.empty())
        {
            file << data.dump(1, '\t') << std::flush;
        }
        else
        {
            json parentData = JsonURI(m_path).deserialize();
            parentData[m_jsonPointer] = data;
            file << parentData.dump(1, '\t') << std::flush;
        }
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to serialize \"" << data.dump() << "\" to \"" << *this << "\"";
        ErrorHandling::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


json JsonURI::deserialize() const
{
    try
    {
        if(!fs::exists(m_path))
            throw std::runtime_error('"' + m_path.string() + "\" is does not exist");
        if(!fs::is_regular_file(m_path))
            throw std::runtime_error('"' + m_path.string() +"\" is not a file");
            
        fs::ifstream file(m_path);
        if(file.peek() < 0)
            return json();
        return json::parse(file).at(m_jsonPointer);
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to deserialize \"" << *this << "\"";
        ErrorHandling::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
    return json();
}


void JsonURI::setJsonPointer(const json::json_pointer& jsonPointer)
{
    m_jsonPointer = jsonPointer;
}


void JsonURI::setFilePath(const fs::path& path)
{
    m_path = path;
}


fs::path JsonURI::getFilePath() const
{
    return m_path;
}


json::json_pointer JsonURI::getJsonPointer() const
{
    return m_jsonPointer;
}


JsonURI::operator std::string() const
{
    std::string uri = m_path.string();
    if(!m_jsonPointer.empty())
    {
        uri += '#';
        uri += m_jsonPointer.to_string();
    }
    return uri;
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

std::ostream& Data::operator<<(std::ostream& os, const JsonURI& jsonURI)
{
    os << static_cast<std::string>(jsonURI);
    return os;
}