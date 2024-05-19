#pragma once

#include <functional>
#include <vector>

template<typename ... Ts>
class Delegate final
{
public:
	using Type = std::function<void(Ts...)>;

private:
	struct CallbackInfo final
	{
		const void* Target;
		std::function<void(Ts...)> Callback;

		void operator()(Ts... args) const
		{
			Callback(std::forward<Ts>(args)...);
		}
	};

public:
	void Bind(void(*Function)(Ts...))
	{
		Callbacks.emplace_back(nullptr, Function);
	}
	template<typename T>
	void Bind(T* const Target, void(T::* Function)(Ts...))
	{
		Callbacks.emplace_back(Target, [Target, Function](Ts... args) { (Target->*Function)(std::forward<Ts>(args)...);  });
	}

	void Invoke(Ts... args)
	{
		for (const CallbackInfo& callback : Callbacks)
		{
			callback(std::forward<Ts>(args)...);
		}
	}

private:
	std::vector<CallbackInfo> Callbacks;
};