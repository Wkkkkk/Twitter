#include "cache.h"
#include "singleton.h"
#include "db_manager.h"
#include "twitter_store_service.h"

int64_t TwitterStoreService::store_twitter_post(TwitterPost twitter)
{
    // store into sqlite
    int64_t twitter_id = singleton::get_instance<DBManager>().insert_post(twitter);
    
    return twitter_id;
}

bool TwitterStoreService::store_twitter_post_in_search_index(TwitterPost twitter)
{
    return false;
}

bool TwitterStoreService::store_twitter_post_in_friends_home_timeline(TwitterPost twitter, std::vector<int64_t> friend_ids)
{
    int64_t twitter_id = twitter.twitter_id;
    int64_t user_id = twitter.user_id;

    for (auto friend_id : friend_ids) {
        Node new_node;
        new_node.user_id = friend_id;

        auto node = singleton::get_instance<Cache>(Cache::default_size).get(friend_id);
        if (node.has_value()) {
            new_node.home_timeline = node.value().home_timeline;
        }

        new_node.home_timeline.push_back({ user_id, twitter_id });
        singleton::get_instance<Cache>(Cache::default_size).set(new_node);
    }

    return true;
}

TwitterPost TwitterStoreService::get_twitter_post(int64_t twitter_id)
{
    auto post = singleton::get_instance<DBManager>().get_post(twitter_id);
    if(post.has_value()) return post.value();

    return TwitterPost();
}
