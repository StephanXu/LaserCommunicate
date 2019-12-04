#ifndef UTILS_HPP
#define UTILS_HPP

#include <cpprest/http_client.h>

web::http::http_response MakeResponseWithCORSHeader(web::http::status_code statusCode, const std::string& what);
web::http::http_response MakeSingleValueResponse(web::http::status_code statusCode,
												 const std::string& name,
												 const std::string& content);

web::http::http_response MakeErrorResponse(web::http::status_code statusCode, const std::string& what);
web::http::http_response MakeErrorResponse(web::http::status_code statusCode, const utility::string_t& what);

#endif