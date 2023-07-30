#include "JsonURI.h"
#include "Log/Log.h"
#include "ExceptionTrace/ExceptionTrace.h"

#include <fstream>
#include <exception>

#ifdef ESP32
#include <LittleFS.h>
#include <sys/stat.h>
#include <regex>
#endif


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
        ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


void JsonURI::serialize(const json& data) const
{
    try
    {

#ifdef ESP32
        std::regex directoryRegex("\\/[^\\/]+(?=\\/)");
        std::string currentDirectory = "";
        for(std::sregex_iterator i(m_filePath.begin(), m_filePath.end(), directoryRegex); i != std::sregex_iterator(); i++)
        {
            std::smatch match = *i;
            currentDirectory += match.str();
            LittleFS.mkdir(currentDirectory.c_str());
        }
#endif
        std::ofstream file;
        if(m_jsonPointer.empty())
        {
            file.open(m_filePath);
            file << data.dump(1, '\t') << std::flush;
        }
        else
        {
            json fileData;
            try
            {
                fileData = JsonURI(m_filePath, json::json_pointer()).deserialize();
            }
            catch(json::exception)
            {
                ExceptionTrace::clear();
            }
            catch(std::runtime_error)
            {
                ExceptionTrace::clear();
            }
            file.open(m_filePath);
            fileData[m_jsonPointer].merge_patch(data);
            file << fileData.dump(1, '\t') << std::flush;
        }
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to serialize \"" << data.dump() << "\" to \"" << *this << "\"";
        ExceptionTrace::trace(errorMessage.str());
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
        return json::parse(file).at(m_jsonPointer);
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to deserialize \"" << *this << "\"";
        ExceptionTrace::trace(errorMessage.str());
        throw;
    }
}


void JsonURI::erase() const
{
    try
    {
        std::string filePath = m_filePath;
        JsonURI fileURI(filePath);

        // Erase at json level
        if(!m_jsonPointer.empty())
        {
            json data = fileURI.deserialize();
            json patch;
            patch["/0/op"_json_pointer] = "remove";
            patch["/0/path"_json_pointer] = m_jsonPointer.to_string();
            try
            {
                data.patch_inplace(patch);
            }
            catch(json::out_of_range)
            {
                ExceptionTrace::clear();
            }
            json flattenedData = data.flatten();
            for(const auto& flattenedDataElement : flattenedData)
            {
                if(!flattenedDataElement.empty())
                {
                    json data = flattenedData.unflatten();
                    fileURI.serialize(data);
                    return;
                }
            }
        }

        // Erase at file level
#ifdef ESP32
        auto popBackPath = [](std::string& path, char seperator = '/'){
            size_t seperatorIndex = path.rfind('/');
            if(seperatorIndex == std::string::npos)
            path = "";
            path.erase(seperatorIndex);
        };
        
        bool success = LittleFS.remove(m_filePath.c_str());
        popBackPath(filePath);
        while(!filePath.empty())
        {
            bool success = LittleFS.rmdir(filePath.c_str());
            popBackPath(filePath);
        }
#endif
    }
    catch(...)
    {
        std::stringstream errorMessage;
        errorMessage << SOURCE_LOCATION << "Failed to erase\"" << *this << "\"";
        ExceptionTrace::trace(errorMessage.str());
        throw;
    }
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


JsonURI operator/(JsonURI lhs, const json::json_pointer& rhs) noexcept
{
    lhs /= rhs;
    return lhs;
}

std::ostream& operator<<(std::ostream& os, const JsonURI& JsonURI) noexcept
{
    os << static_cast<std::string>(JsonURI);
    return os;
}