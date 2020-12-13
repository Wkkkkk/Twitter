// SocialGraph.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>

#include <rpc/server.h>

#include "person_service.h"
#include "usergraph_service.h"
#include "twitter_store_service.h"
#include "timeline_service.h"

int main()
{
    // Creating a server that listens on port 8080
    rpc::server srv(9000);

    // Our service
    LookupService lookup_service;
    for (int i = 0; i < LookupService::PersonServiceNum; i++) {
        auto person_service = std::make_shared<PersonService>();
        //for (int j = 0; j < 100; j++) {
        //    person_service->add_person(generate_random_person(i * 10 + j));
        //}
        lookup_service.add_person_service(person_service);
    }

    UserGraphService usergraph_service(lookup_service);

    srv.bind("usergraph_service_find_friends",
        [&usergraph_service](int64_t id) -> std::vector<int64_t> {
            std::vector<int64_t> friends;
            auto person = usergraph_service.find_person(id);
            if (person.has_value()) {
                auto friends_set = person.value().friend_ids;
                friends.insert(friends.end(), friends_set.begin(), friends_set.end());
            }
            std::cout << "friends: ";
            for (auto id : friends) {
                std::cout << id << " ";
            }
            std::cout << std::endl;

            return friends;
        });

    srv.bind("usergraph_service_find_people", 
        [&usergraph_service](std::vector<int64_t> ids) -> std::vector<Person> {
            return usergraph_service.find_people(ids);
        });

    srv.bind("usergraph_service_shortest_path", 
        [&usergraph_service](int64_t source_id, int64_t dest_id) -> std::vector<int64_t> {
        return usergraph_service.shortest_path(source_id, dest_id);
        });

    TwitterStoreService twitter_store_service;
    srv.bind("store_twitter_post",
        [&twitter_store_service](TwitterPost post) -> int64_t {
            return twitter_store_service.store_twitter_post(post);
        });

    srv.bind("store_twitter_post_in_search_index",
        [&twitter_store_service](TwitterPost post) -> bool {
            return twitter_store_service.store_twitter_post_in_search_index(post);
        });

    srv.bind("store_twitter_post_in_friends_home_timeline",
        [&twitter_store_service](TwitterPost post, std::vector<int64_t> friend_ids) -> bool {
            return twitter_store_service.store_twitter_post_in_friends_home_timeline(post, friend_ids);
        });

    srv.bind("get_twitter_post",
        [&twitter_store_service](int64_t twitter_id) -> TwitterPost {
            return twitter_store_service.get_twitter_post(twitter_id);
        });

    TimelineService timeline_service;
    srv.bind("retrive_home_timeline",
        [&timeline_service](int64_t user_id) -> Node {
            return timeline_service.search_for_timeline(user_id);
        });

    // Run the server loop.
    srv.run();

}