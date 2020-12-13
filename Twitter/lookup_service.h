#pragma once
#include <vector>
#include <memory>

#include "person_service.h"

class LookupService
{
public:
	const static int64_t PersonServiceNum = 1;

	LookupService() = default;
	~LookupService();

	void add_person_service(std::shared_ptr<PersonService> service);

	std::shared_ptr<PersonService> lookup_person_service(int64_t person_id);

private:
	std::vector<std::shared_ptr<PersonService>> person_services;
};

