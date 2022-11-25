#include "RestAPI.h"
#include <fstream>

using namespace Connectivity;

RestAPI::RestAPI(AsyncWebServer server, std::string baseUri, bool allowCors) :
    m_server(server),
    m_baseUri(baseUri),
    m_allowCors(allowCors)
{}


// RestAPI::createEndpointRead(std::string endpointUri, std::string filePath, JsonCallback callback)
// {
//     std::string uri = m_baseUri + endpointUri;
    
// }