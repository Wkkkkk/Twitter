#include <iostream>
#include <optional>

#include <absl/strings/str_split.h>
#include <boost/beast/version.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <rpc/client.h>

#include "cache.h"
#include "twitterpost.h"
#include "singleton.h"
#include "request_handler.h"

using namespace Twitter;
using json = nlohmann::json;
using parse_error = nlohmann::detail::parse_error;

int64_t retrive_user_id_from_request_target(const http::request<http::string_body>& request)
{
    int64_t user_id = 0;

    std::string target = request.target().to_string();
    std::vector<std::string> parts = absl::StrSplit(target, "?");
    if (parts.size() == 2) {
        std::vector<std::string> all_parameters = absl::StrSplit(parts[1], "&");

        for (std::string one_parameter : all_parameters) {
            std::vector<std::string> parameter_pair = absl::StrSplit(one_parameter, "=");
            if (parameter_pair.size() == 2) {
                if (parameter_pair[0] == "user_id") {
                    user_id = std::stoi(parameter_pair[1]);

                    break;
                }
            }
        }
    }

    return user_id;
}

std::optional<TwitterPost> retrive_info_from_post_request(const http::request<http::string_body>& request)
{
    json body;

    try
    {
        body = json::parse(request.body());
    }
    catch (parse_error ex)
    {
        return {};
    }

    TwitterPost twitter;

    // special iterator member functions for objects
    for (json::iterator it = body.begin(); it != body.end(); ++it) {

        if (it.key() == "user_id") {
            twitter.user_id = it.value();
        }

        if (it.key() == "auth_token") {
            twitter.auth_token = it.value();
        }

        if (it.key() == "status") {
            twitter.status = it.value();
        }

        if (it.key() == "media_ids") {
            twitter.media_ids = it.value();
        }
    }

    return { twitter };
}
RequestHandler::result_pair
RequestHandler::dispatch(const http::request<http::string_body>& request)
{
    std::cout << "dispatch request: " << request.method() << " "
        << request.target() << " " << request.body() << std::endl;
    result_pair result;

    if (request.method() == http::verb::get) {
        result = handle_read_request(request);
    }
    else if (request.method() == http::verb::post) {
        result = handle_write_request(request);
    }

    return result;
}

RequestHandler::result_pair
RequestHandler::handle_read_request(const http::request<http::string_body>& request)
{
    // Analyze the request target id
    int64_t user_id = retrive_user_id_from_request_target(request);
    if (user_id == 0) {
        return { RequestHandler::STATUS::illegal_request, request.target().to_string() };
    }

    // Get a rpc client for timeline service
    auto& client = singleton::get_instance<rpc::client>("127.0.0.1", 9000);

    // Gets the timeline data stored in the Memory Cache, containing tweet ids and user ids
    Node node = client.call("retrive_home_timeline", user_id).as<Node>();

    std::vector<TwitterPost> posts;
    for (auto cache_pair : node.home_timeline) {
        int64_t user_id = cache_pair.first;
        int64_t twitter_id = cache_pair.second;

        // Queries the Tweet Info Service 
        auto post = client.call("get_twitter_post", twitter_id).as<TwitterPost>();
        if(post.twitter_id > 0) posts.push_back(post);
    }
    // Queries the User Info Service

    // generate response json
    json j;
    for (const auto& post : posts) {
        json doc;
        doc["twitter_id"] = post.twitter_id;
        doc["user_id"] = post.user_id;
        doc["status"] = post.status;

        j.push_back(doc);
    }

    return { RequestHandler::STATUS::ok, j.dump() };
}

RequestHandler::result_pair
RequestHandler::handle_write_request(const http::request<http::string_body>& request)
{
    // retrive input
    auto twitter = retrive_info_from_post_request(request);
    if (!twitter.has_value()) {
        return { RequestHandler::STATUS::illegal_request,
            "Can't parse request" };
    }

    auto post = twitter.value();

    // Get a rpc client for Fan Out Service
    auto& client = singleton::get_instance<rpc::client>("127.0.0.1", 9000);

    // stores the tweet in the user's timeline on a SQL database
    // get the twitter_id
    if(1) {
        int64_t twitter_id = client.call("store_twitter_post", post).as<int64_t>();
        if (twitter_id == 0) {
            return { RequestHandler::STATUS::illegal_request, "Failed to store twitter" };
        }
        post.twitter_id = twitter_id;
    }

    // stores the tweet in the Search Index Service to enable fast searching
    if(0) {
        auto result = client.call("store_twitter_post_in_search_index", post).as<bool>();
        if (!result) {
            return { RequestHandler::STATUS::illegal_request, "Failed to store twitter in search index service" };
        }
    }

    // queries the User Graph Service to find the user's followers
    if(1) {
        auto friend_ids = client.call("usergraph_service_find_friends", post.user_id).as<std::vector<int64_t>>();
        if (friend_ids.empty()) {
            return { RequestHandler::STATUS::not_found, "Failed to find friends" };
        }

        std::cout << "friend_ids: ";
        for (auto id : friend_ids) {
            std::cout << id << " ";
        }
        std::cout << std::endl;

        // stores the twitter in the home timeline of the user's followers
        auto result = client.call("store_twitter_post_in_friends_home_timeline", post, friend_ids).as<bool>();
        if (!result) {
            return { RequestHandler::STATUS::illegal_request, "Failed to store twitter in friends' home timeline" };
        }
    }

    // generate response
    json j;
    j["twitter_id"]   = post.twitter_id;
    j["user_id"]    = post.user_id;
    j["status"]     = post.status;
    j["created_at"] = post.created_at;

    return { RequestHandler::STATUS::ok, j.dump() };
}

RequestHandler::result_pair
RequestHandler::handle_search_request(const http::request<http::string_body>& request)
{
}