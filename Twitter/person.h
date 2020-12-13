#pragma once
#include <set>
#include <string>
#include <iostream>

#include <rpc/msgpack.hpp>


struct Person
{
	int64_t id;
	std::string name;
	std::set<int64_t> friend_ids;

	MSGPACK_DEFINE_ARRAY(id, name, friend_ids)
};

inline bool operator==(const Person& lhs, const Person& rhs) {
	return lhs.id == rhs.id;
}

inline bool operator<(const Person& lhs, const Person& rhs) {
	return lhs.id < rhs.id;
}

Person inline generate_random_person(int id) {
    Person person;
    person.id = id;
    person.name = "user" + std::to_string(id);
    for (int i = 0; i < 10; i++) {
        person.friend_ids.insert(rand() % 10);
    }

    return person;
}

inline void print_person(const Person& person) {
    std::cout << "Person:" << person.id << " " << person.name << " ";
    for (auto id : person.friend_ids) {
        std::cout << id << " ";
    }
    std::cout << std::endl;
}
