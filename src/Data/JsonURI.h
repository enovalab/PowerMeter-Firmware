#ifndef DATA_JSONURI_H
#define DATA_JSONURI_H

#include <json.hpp>

namespace Data
{

    class JsonURI
    {
    public:
        JsonURI() {};
        JsonURI(const std::string& path, const json::json_pointer& jsonPointer);
        JsonURI(const std::string& uri);
        
        json deserialize() const;
        void serialize(const json& data) const;

        void setJsonPointer(const json::json_pointer& jsonPointer);
        void setFilePath(const std::string& path);
        json::json_pointer getJsonPointer() const;
        std::string getFilePath() const;
        
        operator std::string() const;
        JsonURI& operator/=(const json::json_pointer& jsonPointer);
        
    private:
        std::string m_filePath;
        json::json_pointer m_jsonPointer;
    };

    JsonURI operator/(JsonURI lhs, const json::json_pointer& rhs);
    std::ostream& operator<<(std::ostream& os, const JsonURI& jsonURI);
}

#endif