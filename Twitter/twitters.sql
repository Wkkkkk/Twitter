CREATE TABLE IF NOT EXISTS twitters (
twitter_id integer AUTOINCREMENT,
user_id integer,
auth_token TEXT,
status TEXT,
media_ids TEXT,
creation_at TEXT DEFAULT CURRENT_TIMESTAMP,
PRIMARY KEY(twitter_id)
);
