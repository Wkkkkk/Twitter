#pragma once

#include <string>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace beast = boost::beast;     // from <boost/beast.hpp>
namespace http = beast::http;       // from <boost/beast/http.hpp>

// Business logic
namespace Twitter {

	class RequestHandler
	{
	public:
		enum class STATUS {
			ok,
			not_found,
			illegal_request
		};

		using result_pair = std::pair<STATUS, std::string /* response_body */>;

		RequestHandler() = default;

		result_pair dispatch(const http::request<http::string_body>& request);

	private:
		result_pair handle_read_request(const http::request<http::string_body>& request);

		result_pair handle_write_request(const http::request<http::string_body>& request);

		result_pair handle_search_request(const http::request<http::string_body>& request);

	};

}