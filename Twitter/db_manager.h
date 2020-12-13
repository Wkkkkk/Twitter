#pragma once
#include <tuple>
#include <optional>

#include <sqlpp11/sqlite3/sqlite3.h>
#include <sqlpp11/sqlpp11.h>

#include "twitterpost.h"

namespace sql = sqlpp::sqlite3;

class DBManager
{
private:
	sql::connection_config get_config();
	sql::connection db;

public:
	DBManager();

	int64_t insert_post(const TwitterPost& post);
	std::optional<TwitterPost> get_post(int64_t twitter_id);
};
