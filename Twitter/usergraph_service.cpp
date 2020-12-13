#include <queue>

#include "usergraph_service.h"

UserGraphService::UserGraphService(LookupService lookup_service)
    : lookup_service_(lookup_service)
{
}

std::optional<Person> UserGraphService::find_person(int64_t id)
{
    auto person_service = lookup_service_.lookup_person_service(id);

    return person_service->find_person(id);
}

std::vector<Person> UserGraphService::find_people(std::vector<int64_t> ids)
{
    std::vector<Person> people;

    for (auto id : ids) {
        auto person = find_person(id);
        if (person.has_value()) {
            people.push_back(person.value());
        }
    }

    return people;
}

std::vector<int64_t> UserGraphService::shortest_path(int64_t source_id, int64_t dest_id)
{
    if (source_id < 0 || dest_id < 0) return {};

    if (source_id == dest_id) return { source_id };

    auto prev_node_keys = _shortest_path(source_id, dest_id);
    if (prev_node_keys.empty()) return {};

    // Iterate through the path_ids backwards, starting at dest_id
    std::vector<int64_t> path = { dest_id };
    auto prev_node_key = prev_node_keys[dest_id];
    while (prev_node_key != source_id) {
        path.push_back(prev_node_key);
        prev_node_key = prev_node_keys[prev_node_key];
    }
    path.push_back(source_id);

    // Reverse the list since we iterated backwards
    return std::vector<int64_t>(path.rbegin(), path.rend());
}

std::map<int64_t, int64_t> UserGraphService::_shortest_path(int64_t source_id, int64_t dest_id)
{
    // Use the id to get the Person
    auto source = find_person(source_id);
    if (!source.has_value()) return std::map<int64_t, int64_t>();
    
    // Update our bfs queue
    std::queue<Person> queue;
    queue.push(source.value());
    
    // prev_node_keys keeps track of each hop from
    // the source_key to the dest_key
    std::map<int64_t, int64_t> prev_node_keys;
    prev_node_keys[source_id] = -1;

    // We'll use visited_ids to keep track of which nodes we've
    // visited, which can be different from a typical bfs where
    // this can be stored in the node itself
    std::set<int64_t> visited_ids;
    visited_ids.insert(source_id);

    while (!queue.empty()) {
        Person node = queue.front();
        queue.pop();
        // print_person(node);

        if (node.id == dest_id) return prev_node_keys;
        Person prev_node = node;
        for (auto friend_id : node.friend_ids) {
            if (visited_ids.find(friend_id) == visited_ids.end()) {
                auto friend_node = find_person(friend_id);
                if (friend_node.has_value()) {
                    queue.push(friend_node.value());
                    prev_node_keys[friend_id] = prev_node.id;
                    visited_ids.insert(friend_id);
                }
            }
        }
    }
    
    return std::map<int64_t, int64_t>();
}
