#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "person_service.h"

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;

using nlohmann::json;

PersonService::PersonService()
	: inst()
	, conn(mongo::uri("mongodb+srv://Wkkkkk:wkkkkk@cluster0.zjfeg.mongodb.net/Twitter?retryWrites=true&w=majority"))
	, user_collection(conn["Twitter"]["users"])
{
	//std::vector<int64_t> ids{ 1, 3, 5 };

	//for (auto id : ids) {
	//	bsoncxx::builder::stream::document document{};
	//	document << "id" << id;
	//	auto result = user_collection.find_one(document.view());
	//	if (result) {
	//		std::string result_str = bsoncxx::to_json(*result);
	//		json body = json::parse(result_str);

	//		Person person;
	//		person.id = body["id"];
	//		person.name = body["name"];
	//		std::vector<int> friend_ids = body["friend_ids"];
	//		std::set<int64_t> tmp(friend_ids.begin(), friend_ids.end());
	//		person.friend_ids = tmp;

	//		print_person(person);
	//	}
	//}
}

void PersonService::add_person(Person person)
{
	bsoncxx::builder::stream::document document{};
	auto Array = document << "id" << person.id
		<< "name" << person.name
		<< "friend_ids" << open_array;
	for (auto id : person.friend_ids) {
		Array << id;
	}
	Array << close_array;

	user_collection.insert_one(document.view());
	std::cout << "add_person: " << person.name << std::endl;
}

std::optional<Person> PersonService::find_person(int64_t id)
{
	bsoncxx::builder::stream::document document{};
	document << "id" << id;

	auto result = user_collection.find_one(document.view());
	if (result) {
		std::string result_str = bsoncxx::to_json(*result);
		json body = json::parse(result_str);

		Person user;
		user.id = body["id"];
		user.name = body["name"];
		std::vector<int> friend_ids = body["friend_ids"];
		std::set<int64_t> tmp(friend_ids.begin(), friend_ids.end());
		user.friend_ids = tmp;

		return { user };
	}

	return {};
}
