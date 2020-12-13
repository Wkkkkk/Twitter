# Twitter timeline

Design the Twitter timeline

## Use case
- User posts a tweet
  - Service pushes tweets to followers
```
$ curl -X POST --data '{ "user_id": "123", "auth_token": "ABC123", \
    "status": "hello world!", "media_ids": "ABC987" }' \
    https://localhost:8080/tweet
```
Response
```
{
    "created_at": "Wed Sep 05 00:37:15 +0000 2012",
    "status": "hello world!",
    "tweet_id": "987",
    "user_id": "123",
    ...
}
```

- User views the home timeline
```
$ curl https://localhost:8080/twitter?user_id=123
```
Response
```
{
    "user_id": "456",
    "tweet_id": "123",
    "status": "foo"
},
{
    "user_id": "789",
    "tweet_id": "456",
    "status": "bar"
},
{
    "user_id": "789",
    "tweet_id": "579",
    "status": "baz"
},
```
## Design
![system design](res/system.png)


## Rationale

We could store the user's own tweets to populate the user timeline (activity from the user) in a relational database. 

Delivering tweets and building the home timeline (activity from people the user is following) is trickier. Fanning out tweets to all followers (60 thousand tweets delivered on fanout per second) will overload a traditional relational database. We'll probably want to choose a data store with fast writes such as a NoSQL database or Memory Cache. 

We could store media such as photos or videos on an Object Store.

### Use case
Use case: User posts a tweet

- The Client posts a tweet to the Web Server, running as a reverse proxy
- The Web Server forwards the request to the Write API server
- The Write API stores the tweet in the user's timeline on a SQL database
- The Write API contacts the Fan Out Service, which does the following:
  - Queries the User Graph Service to find the user's followers stored in the Memory Cache
  - Stores the tweet in the home timeline of the user's followers in a Memory Cache
    - O(n) operation: 1,000 followers = 1,000 lookups and inserts

Use case: User views the home timeline

- The Client posts a home timeline request to the Web Server
- The Web Server forwards the request to the Read API server
- The Read API server contacts the Timeline Service, which does the following:
  - Gets the timeline data stored in the Memory Cache, containing tweet ids and user ids - O(1)
  - Queries the Tweet Info Service with a multiget to obtain additional info about the tweet ids - O(n)
  - Queries the User Info Service with a multiget to obtain additional info about the user ids - O(n)

## Compile
### third party

```
vcpkg install sqlpp11-connector-sqlite3:x64-windows mongo-cxx-driver:x64-windows nlohmann-json:x64-windows boost-beast:x64-windows abseil:x64-windows rpclib:x64-windows
```

### compiler
VS2019 or above(C++17)

## Bottleneck
The Fanout Service is a potential bottleneck. Twitter users with millions of followers could take several minutes to have their tweets go through the fanout process. 
This could lead to race conditions with @replies to the tweet, which we could mitigate by re-ordering the tweets at serve time.

We could also avoid fanning out tweets from highly-followed users. Instead, we could search to find tweets for highly-followed users, merge the search results with the user's home timeline results, then re-order the tweets at serve time.

Another bottleneck is the user store service. Since we used mongoDB cloud to store all users, we could have partitioned users with their ids.
But the free version allows only one cluster at a time most, so all test users are stored in one database.

## Extension
- Distributed autoincremental twitter id
- Keep only several hundred tweets for each home timeline in the Memory Cache
- Keep only active users' home timeline info in the Memory Cache
  - If a user was not previously active in the past 30 days, we could rebuild the timeline from the SQL Database
    - Query the User Graph Service to determine who the user is following
    - Get the tweets from the SQL Database and add them to the Memory Cache
- Store only a month of tweets in the Tweet Info Service
- Store only active users in the User Info Service