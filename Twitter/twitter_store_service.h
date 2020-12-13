#pragma once
#include "twitterpost.h"

class TwitterStoreService
{
public:
	TwitterStoreService() = default;

	int64_t store_twitter_post(TwitterPost twitter);

	bool store_twitter_post_in_search_index(TwitterPost twitter);

	bool store_twitter_post_in_friends_home_timeline(TwitterPost twitter, std::vector<int64_t> friend_ids);

	TwitterPost get_twitter_post(int64_t twitter_id);
private:

};

