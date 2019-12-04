#include "backend.h"
#include "utils.hpp"

#include <cpprest/http_listener.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

web::http::http_response MakeSingleValueResponse(web::http::status_code statusCode,
												 const std::string& name,
												 const std::string& content)
{
	boost::property_tree::ptree root;
	root.put(name, content);
	std::ostringstream ss;
	boost::property_tree::write_json(ss, root);
	return MakeResponseWithCORSHeader(statusCode, ss.str());
}

web::http::http_response MakeErrorResponse(web::http::status_code statusCode, const std::string& what)
{
	spdlog::error(what);
	return MakeSingleValueResponse(statusCode, "error", what);
}
web::http::http_response MakeErrorResponse(web::http::status_code statusCode, const utility::string_t& what)
{
	return MakeErrorResponse(statusCode, utility::conversions::to_utf8string(what));
}

web::http::http_response MakeResponseWithCORSHeader(web::http::status_code statusCode, const std::string& what)
{
	web::http::http_response rep;
	rep.headers().add(L"Content-Type", L"application/json; charset=utf-8");
	rep.headers().add(L"Access-Control-Allow-Origin", L"*");
	rep.headers().add(L"Access-Control-Request-Method", L"GET,POST,OPTIONS");
	rep.headers().add(L"Access-Control-Allow-Credentials", L"true");
	rep.headers().add(L"Access-Control-Allow-Headers", L"Content-Type,Access-Token,x-requested-with,Authorization");
	rep.set_body(what);
	rep.set_status_code(statusCode);
	return rep;

}