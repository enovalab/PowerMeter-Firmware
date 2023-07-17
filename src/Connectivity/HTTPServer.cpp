#include "Connectivity/HTTPServer.h"
#include "Diagnostics/ExceptionTrace.h"
#include "Diagnostics/Log.h"

#include <sstream>

using namespace Connectivity;


HTTPServer::HTTPServer(uint16_t port, HTTP::HeaderMap defaultHeaders) : m_defaultHeaders(defaultHeaders)
{}


void HTTPServer::start()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // config.server_port = m_port;
    config.max_uri_handlers = 30;
    config.max_resp_headers = 15;
    httpd_start(&m_server, &config);
}


void HTTPServer::stop()
{
    httpd_stop(&m_server);
}


void HTTPServer::handle(const std::string& uri, HTTP::Method method, const RequestHandler& handlerCallback) noexcept
{
    struct UserContext
    {
        HTTPServer::RequestHandler handlerCallback;
        HTTP::HeaderMap defaultHeaders;
    };

    httpd_uri_t endpoint = {
        .uri = uri.c_str(),
        .method = static_cast<httpd_method_t>(method),
        .handler = [](httpd_req_t* request){
            try
            {
                Diagnostics::Logger[Level::Debug] << "Handling Reqeust" << std::endl;
                std::string body;
                size_t maxBodyLength = 1000;
                body.reserve(maxBodyLength);
                httpd_req_recv(request, const_cast<char*>(body.c_str()), maxBodyLength);
                
                UserContext* userContext = static_cast<UserContext*>(request->user_ctx);
                Response response = userContext->handlerCallback(
                    Request(
                        std::string(request->uri), 
                        static_cast<HTTP::Method>(request->method),
                        body
                    )
                );
                
                std::stringstream status;
                status << response.status << ' ' << response.status.getDescription();
                httpd_resp_set_status(request, status.str().c_str());

                response.headers.insert(userContext->defaultHeaders.begin(), userContext->defaultHeaders.end());
                for(const auto& header : response.headers)
                    httpd_resp_set_hdr(request, header.first.c_str(), header.second.c_str());

                if(response.body != "")
                    httpd_resp_set_type(request, response.contentType.c_str());
                
                httpd_resp_send(request, response.body.c_str(), response.body.length());

                request->free_ctx = [](void* userContext){
                    delete static_cast<UserContext*>(userContext);
                };
            }
            catch(...)
            {
                httpd_resp_set_status(request, "500 Internal Server Error");
                httpd_resp_set_type(request, "text/plain");
                std::string errorMessage = Diagnostics::ExceptionTrace::what();
                httpd_resp_send(request, errorMessage.c_str(), errorMessage.length());
                Diagnostics::Logger[Level::Error] 
                    << SOURCE_LOCATION << "An Exception occured, here is what happened:\n" 
                    << errorMessage << std::endl;
            }

            return ESP_OK;
        },
        .user_ctx = new UserContext {
            .handlerCallback = handlerCallback,
            .defaultHeaders = m_defaultHeaders
        }
    };
    httpd_register_uri_handler(m_server, &endpoint);
}


HTTPServer::~HTTPServer()
{
    stop();
}