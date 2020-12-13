#pragma once
#include <map>

#include <optional>

#include "lookup_service.h"

class UserGraphService
{
public:
	UserGraphService(LookupService lookup_service);

	std::optional<Person> find_person(int64_t id);
	std::vector<Person> find_people(std::vector<int64_t> ids);
	std::vector<int64_t> shortest_path(int64_t source_id, int64_t dest_id);

private:
	std::map<int64_t, int64_t> _shortest_path(int64_t source_id, int64_t dest_id);

	LookupService lookup_service_;
};

