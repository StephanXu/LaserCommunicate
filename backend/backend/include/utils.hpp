#ifndef UTILS_HPP
#define UTILS_HPP

#include <cpprest/http_client.h>

void ReplySingleValue(web::http::http_request request,
					  web::http::status_code statusCode,
					  const std::string& name,
					  const std::string& content);

void ReplyError(web::http::http_request request, web::http::status_code statusCode, const std::string& what);
void ReplyError(web::http::http_request request, web::http::status_code statusCode, const utility::string_t& what);


#endif