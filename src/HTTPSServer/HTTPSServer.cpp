#ifdef ESP32

#include "HTTPSServer.h"
#include <esp_https_server.h>


HTTPSServer::HTTPSServer(
    const uint8_t* certificate,
    size_t certificateLength,
    const uint8_t* privateKey,
    size_t privateKeyLength,
    uint16_t port,
    const HTTP::HeaderMap& defaultHeaders
) : 
    m_certificate(certificate),
    m_certificateLength(certificateLength),
    m_privateKey(privateKey),
    m_privateKeyLength(privateKeyLength)
{
    m_port = port;
    m_defaultHeaders = defaultHeaders;
}


void HTTPSServer::start()
{
    httpd_ssl_config_t config = HTTPD_SSL_CONFIG_DEFAULT();
    config.httpd.stack_size = 8192;
    config.httpd.max_uri_handlers = 256;
    config.httpd.max_resp_headers = 32;
    config.httpd.core_id = 0;
    config.port_secure = m_port;
    config.cacert_pem = m_certificate;
    config.cacert_len = m_certificateLength;
    config.prvtkey_pem = m_privateKey;
    config.prvtkey_len = m_privateKeyLength;
    httpd_ssl_start(&m_server, &config);
}


void HTTPSServer::stop()
{
    httpd_ssl_stop(m_server);
}

#endif