#include "lookup_service.h"

LookupService::~LookupService()
{
    std::cout << "~LookupService" << std::endl;
}

void LookupService::add_person_service(std::shared_ptr<PersonService> service)
{
    person_services.push_back(service);
}

std::shared_ptr<PersonService>
LookupService::lookup_person_service(int64_t person_id)
{
    int64_t index = person_id % LookupService::PersonServiceNum;
    auto service = person_services[index];

    return service;
}
