#include "backend.h"
#include "utils.hpp"

#include <cpprest/http_listener.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

void ReplySingleValue(web::http::http_request request,
					  web::http::status_code statusCode, 
					  const std::string& name,
					  const std::string& content)
{
	boost::property_tree::ptree root;
	root.put(name, content);
	std::ostringstream ss;
	boost::property_tree::write_json(ss, root);
	request.reply(statusCode, ss.str(), "application/json; charset=utf-8");
}

void ReplyError(web::http::http_request request, web::http::status_code statusCode, const std::string& what)
{
	spdlog::error(what);
	ReplySingleValue(request, statusCode, "error", what);
}
void ReplyError(web::http::http_request request, web::http::status_code statusCode, const utility::string_t& what)
{
	ReplyError(request, statusCode, utility::conversions::to_utf8string(what));
}
