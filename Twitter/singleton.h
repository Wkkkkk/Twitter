#pragma once

class singleton
{
private:
	singleton() = default;
	~singleton() = default;

	singleton(const singleton&) = delete;
	singleton& operator=(const singleton&) = delete;
public:
	template<typename T, typename... Args>
	static T& get_instance(const Args... args) {

		static T instance(args...);
		return instance;
	}
};

