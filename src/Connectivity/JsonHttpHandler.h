#ifndef HTTPHANDLER_H
#define HTTPHANDLER_H

#include <ESPAsyncWebServer.h>

namespace Connectivity
{
    struct JsonHttpHandler
    {
        template<WebRequestMethod method>
        constexpr static JsonHttpHandler makeDefaultHandler();

        ArRequestHandlerFunction requestHandler;
        ArUploadHandlerFunction uploadHandler;
        ArBodyHandlerFunction bodyHandler;
    };
}

#endif