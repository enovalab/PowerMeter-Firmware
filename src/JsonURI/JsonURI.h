#ifndef JSONURI_H
#define JSONURI_H

#include <json.hpp>

class JsonURI
{
public:
    JsonURI() noexcept {};
    JsonURI(const std::string& path, const json::json_pointer& jsonPointer) noexcept;
    JsonURI(const std::string& uri);
    
    json deserialize() const;
    void serialize(const json& data) const;
    void erase() const;

    void setJsonPointer(const json::json_pointer& jsonPointer) noexcept;
    void setFilePath(const std::string& path) noexcept;
    json::json_pointer getJsonPointer() const noexcept;
    std::string getFilePath() const noexcept;
    
    operator std::string() const noexcept;
    JsonURI& operator/=(const json::json_pointer& jsonPointer) noexcept; 
    
private:
    std::string m_filePath;
    json::json_pointer m_jsonPointer;
};

JsonURI operator/(JsonURI lhs, const json::json_pointer& rhs) noexcept;
std::ostream& operator<<(std::ostream& os, const JsonURI& jsonURI) noexcept;

#endif