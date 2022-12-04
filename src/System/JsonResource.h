#ifndef SYSTEM_JSONRESOURCE_H
#define SYSTEM_JSONRESOURCE_H

#include <json.hpp>

namespace System
{
    class JsonResource
    {
    public:
        JsonResource(const std::string& path, const json::json_pointer& jsonPointer);
        JsonResource(const std::string& uri);

        json deserialize() const;
        void serialize(const json& data) const;
    
    private:
        std::string m_path;
        json::json_pointer m_jsonPointer;
    };

}

#endif