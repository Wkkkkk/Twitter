#pragma once

#include <map>
#include <list>
#include <vector>
#include <string>
#include <optional>
#include <iostream>

#include <rpc/msgpack.hpp>

struct Node {
	using post = std::pair<int64_t /* user_id */, int64_t /* twitter_id*/>;
	int64_t user_id;
	std::vector<post> home_timeline;

	Node() {}

	Node(int64_t user_id, std::vector<post> home_timeline) {
		this->user_id = user_id;
		this->home_timeline = home_timeline;
	}

	MSGPACK_DEFINE_ARRAY(user_id, home_timeline)
};

inline void print_node(const Node& node) {
	std::cout << "Node: " << node.user_id << std::endl;
	for (const auto& post : node.home_timeline) {
		std::cout << post.first << " " << post.second << std::endl;
	}
}

inline bool operator==(const Node& lhs, const Node& rhs) {
	return lhs.user_id == rhs.user_id;
}

class Cache
{
	size_t max_size_;
	size_t size_;
	std::list<Node> nodes_;
	std::map<int64_t /* user_id */, Node /* home timeline*/> map_;

public:
	const static size_t default_size = 1000;

	Cache(size_t max_size)
		: max_size_(max_size)
		, size_(0) {
		std::cout << "Cache with max size: " << max_size << " is created." << std::endl;
	}

	void set(Node node) {
		auto iter = map_.find(node.user_id);

		if (iter != map_.end()) {
			// Key exists in cache, update the content
			Node& n = iter->second;
			n.home_timeline = node.home_timeline;

			// move the node to the front of the list
			move_to_front(node);
		}
		else {
			// Key does not exist in cache
			if (size_ == max_size_) {
				// Remove the oldest entry from the linked list and map
				auto oldest_node_it = nodes_.back();
				auto oldest_query = oldest_node_it.user_id;
				// std::cout << "remove cache: " << oldest_node_it.query << " " << oldest_node_it.content << std::endl;

				map_.erase(oldest_query);	// erasing by key
				nodes_.pop_back();
			}
			else {
				size_++;
			}

			// Add the new key and value
			nodes_.insert(nodes_.begin(), node);
			map_[node.user_id] = node;
		}
	}

	std::optional<Node> get(int64_t user_id) {
		auto iter = map_.find(user_id);
		if (iter == map_.end()) return {};

		Node node = iter->second;
		move_to_front(node);

		return node;
	}

	void print() {
		std::cout << "all nodes: " << std::endl;
		for (const auto& node : nodes_) {
			print_node(node);
		}
	}

private:
	// move the node to the front of the list
	void move_to_front(const Node& node) {
		auto it = std::find(nodes_.begin(), nodes_.end(), node);
		nodes_.splice(nodes_.begin(), nodes_, it); // O(1)
	}

};

