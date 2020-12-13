#include "twitters.h"
#include "db_manager.h"

using namespace Twitter;

sql::connection_config DBManager::get_config() {
	sql::connection_config config;
	config.path_to_database = ":memory:";
	config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	config.debug = false;

	return config;
}

DBManager::DBManager() : db(get_config()) {
	db.execute(R"(CREATE TABLE IF NOT EXISTS twitters (
		twitter_id INTEGER PRIMARY KEY AUTOINCREMENT,
		user_id integer,
		auth_token TEXT,
		status TEXT,
		media_ids TEXT,
		creation_at TEXT DEFAULT CURRENT_TIMESTAMP
      ))");
}

int64_t DBManager::insert_post(const TwitterPost& post)
{
	const auto twitter = Twitter::Twitters{};

	db(insert_into(twitter).set(
		twitter.twitterId = db.last_insert_id() + 1,
		twitter.userId = post.user_id,
		twitter.authToken = post.auth_token,
		twitter.status = post.status,
		twitter.mediaIds = post.media_ids,
		twitter.creationAt = post.created_at));

	int64_t twitter_id = db.last_insert_id();
	return twitter_id;
}

std::optional<TwitterPost> DBManager::get_post(int64_t twitter_id)
{
	const auto twitter = Twitter::Twitters{};

	for (const auto& row : db(select(all_of(twitter)).from(twitter).where(twitter.twitterId == twitter_id)))
	{
		TwitterPost post;
		post.twitter_id = row.twitterId;
		post.user_id = row.userId;
		post.auth_token = row.authToken;
		post.status = row.status;
		post.media_ids = row.mediaIds;
		post.created_at = row.creationAt;

		return { post };
	}

	return {};
}
