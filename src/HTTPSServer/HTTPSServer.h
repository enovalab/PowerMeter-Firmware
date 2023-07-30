#ifndef HTTPSSERVER_H
#define HTTPSSERVER_H

#include "HTTPServer/HTTPServer.h"


class HTTPSServer : public HTTPServer
{
public:
    HTTPSServer(
        const uint8_t* certificate,
        size_t certificateLength,
        const uint8_t* privateKey,
        size_t privateKeyLength,
        uint16_t port = 443,
        const HTTP::HeaderMap& defaultHeaders = {}
    );
    void start() override;
    void stop() override;

private:
    const uint8_t* m_certificate;
    size_t m_certificateLength;
    const uint8_t* m_privateKey;
    size_t m_privateKeyLength;
};

#endif