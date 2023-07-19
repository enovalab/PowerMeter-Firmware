#ifndef POWERMETER_HTTPSSERVER_H
#define POWERMETER_HTTPSSERVER_H

#include "Connectivity/HTTPServer.h"

namespace Connectivity
{
    class HTTPSServer : virtual public HTTPServer
    {
    public:
        HTTPSServer(
            const std::string& certificate,
            const std::string& privateKey,
            uint16_t port = 443,
            const HTTP::HeaderMap& defaultHeaders = {}
        );
        void start() override;
        void stop() override;

    private:
        std::string m_certificate;
        std::string m_privateKey;
    };
}

#endif