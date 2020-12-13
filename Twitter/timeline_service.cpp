#include "cache.h"
#include "singleton.h"
#include "timeline_service.h"

Node TimelineService::search_for_timeline(int64_t user_id)
{
    auto node = singleton::get_instance<Cache>(Cache::default_size).get(user_id);
    if (node.has_value()) return node.value();

    return Node();
}
