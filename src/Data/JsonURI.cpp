#include "Data/JsonURI.h"
#include "Diagnostics/Log.h"
#include "Diagnostics/ExceptionTrace.h"

#include <fstream>
#include <exception>
#include <sstream>

using namespace Data;


JsonURI::JsonURI(const std::string& filePath, const json::json_pointer& jsonPointer) noexcept :
    m_filePath(filePath),
    m_jsonPointer(jsonPointer)
{}


JsonURI::JsonURI(const std::string& uri)
{
    try
    {
        size_t seperatorIndex = uri.find('#');
        if(seperatorIndex != std::string::npos)
        {
            m_filePath = uri.substr(0, seperatorIndex);
            m_jsonPointer = json::json_pointer(uri.substr(seperatorIndex + 1, uri.size()));
        }
        else
        {
            m_filePath = uri;
        }
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to construct from \"" << uri << "\"";
        Diagnostics::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


void JsonURI::serialize(const json& data) const
{
    try
    {
        std::ofstream file;
        if(m_jsonPointer.empty())
        {
            file.open(m_filePath);
            file << data.dump(1, '\t') << std::flush;
        }
        else
        {
            json parentData;
            
            try
            {
                parentData = JsonURI(m_filePath, json::json_pointer()).deserialize();
            }
            catch(json::exception)
            {}
            catch(std::runtime_error)
            {}

            file.open(m_filePath);
            parentData[m_jsonPointer] = data;
            file << parentData.dump(1, '\t') << std::flush;
        }
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to serialize \"" << data.dump() << "\" to \"" << *this << "\"";
        Diagnostics::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


json JsonURI::deserialize() const
{
    try
    {
        std::ifstream file(m_filePath);
        if(!file.good())
            throw std::runtime_error('"' + m_filePath + "\" is not a valid filepath");
            
        if(file.peek() == EOF)
            return json();
        return json::parse(file).at(m_jsonPointer);
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to deserialize \"" << *this << "\"";
        Diagnostics::ExceptionTrace::trace(errorMessage.str());
        throw;
    }
    return json();
}


void JsonURI::setJsonPointer(const json::json_pointer& jsonPointer) noexcept
{
    m_jsonPointer = jsonPointer;
}


void JsonURI::setFilePath(const std::string& filePath) noexcept
{
    m_filePath = filePath;
}


std::string JsonURI::getFilePath() const noexcept
{
    return m_filePath;
}


json::json_pointer JsonURI::getJsonPointer() const noexcept
{
    return m_jsonPointer;
}


JsonURI::operator std::string() const noexcept
{
    std::string uri = m_filePath;
    if(!m_jsonPointer.empty())
    {
        uri += '#';
        uri += m_jsonPointer.to_string();
    }
    return uri;
}


JsonURI& JsonURI::operator/=(const json::json_pointer& jsonPointer) noexcept
{
    m_jsonPointer /= jsonPointer;
    return *this;
}


JsonURI Data::operator/(JsonURI lhs, const json::json_pointer& rhs) noexcept
{
    lhs /= rhs;
    return lhs;
}

std::ostream& Data::operator<<(std::ostream& os, const JsonURI& jsonURI) noexcept
{
    os << static_cast<std::string>(jsonURI);
    return os;
}