#pragma once
#include <string>
#include <iostream>

#include <rpc/msgpack.hpp>

inline std::string generate_time()
{
    time_t now_time = time(NULL);
    char str[26];
    ctime_s(str, sizeof str, &now_time);
    std::string time_str(str);

    return time_str;
}

struct TwitterPost {
    int64_t twitter_id;
    int64_t user_id;
    std::string auth_token;
    std::string status;
    std::string media_ids;
    std::string created_at;

    TwitterPost() {
        created_at = generate_time();
    }

    MSGPACK_DEFINE_ARRAY(twitter_id, user_id, auth_token, status, media_ids, created_at)
};

inline bool operator==(const TwitterPost& lhs, const TwitterPost& rhs) {
	return lhs.twitter_id == rhs.twitter_id;
}

inline bool operator<(const TwitterPost& lhs, const TwitterPost& rhs) {
	return lhs.twitter_id < rhs.twitter_id;
}

inline void print_twitter(const TwitterPost& twitter) {
    std::cout << "Twitter:" << twitter.twitter_id << " "
        << twitter.user_id << " " << twitter.status << std::endl;
}
