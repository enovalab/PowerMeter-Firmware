#ifdef ESP32

#include "Connectivity/HTTPSServer.h"
#include "Diagnostics/Log.h"
#include <esp_https_server.h>

using namespace Connectivity;


HTTPSServer::HTTPSServer(
    const std::string& certificate,
    const std::string& privateKey,
    uint16_t port,
    const HTTP::HeaderMap& defaultHeaders
) : 
    m_certificate(certificate),
    m_privateKey(privateKey)
{
    m_port = port;
    m_defaultHeaders = defaultHeaders;
}


void HTTPSServer::start()
{
    httpd_ssl_config_t config = HTTPD_SSL_CONFIG_DEFAULT();
    config.httpd.stack_size = 8192;
    config.httpd.max_uri_handlers = 64;
    config.httpd.max_resp_headers = 32;
    config.port_secure = m_port;
    config.cacert_pem = reinterpret_cast<const uint8_t*>(m_certificate.c_str());
    config.cacert_len = m_certificate.length();
    config.prvtkey_pem = reinterpret_cast<const uint8_t*>(m_privateKey.c_str());
    config.prvtkey_len = m_privateKey.length();
    httpd_ssl_start(&m_server, &config);
}


void HTTPSServer::stop()
{
    httpd_ssl_stop(m_server);
}

#endif