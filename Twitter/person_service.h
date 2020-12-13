#pragma once
#include <optional>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include "person.h"

namespace mongo = mongocxx;

class PersonService
{
public:
	PersonService();
	~PersonService() = default;

	void add_person(Person person);

	std::optional<Person> find_person(int64_t id);

private:
	mongo::instance inst;
	mongo::client conn;
	mongo::collection user_collection;
};

