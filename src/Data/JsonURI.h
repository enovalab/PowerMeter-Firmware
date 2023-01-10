#ifndef DATA_JSONURI_H
#define DATA_JSONURI_H

#include <json.hpp>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

namespace Data
{

    class JsonURI
    {
    public:
        JsonURI() {};
        JsonURI(const fs::path& path, const json::json_pointer& jsonPointer = ""_json_pointer);
        JsonURI(const std::string& uri);
        
        json deserialize() const;
        void serialize(const json& data) const;

        void setJsonPointer(const json::json_pointer& jsonPointer);
        void setFilePath(const fs::path& path);
        json::json_pointer getJsonPointer() const;
        fs::path getFilePath() const;
        
        operator std::string() const;
        JsonURI& operator/=(const json::json_pointer& jsonPointer);
        
    private:
        fs::path m_path;
        json::json_pointer m_jsonPointer;
    };

    JsonURI operator/(JsonURI lhs, const json::json_pointer& rhs);
    std::ostream& operator<<(std::ostream& os, const JsonURI& jsonURI);
}

#endif