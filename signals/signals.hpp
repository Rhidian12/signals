#pragma once

#include <functional>
#include <vector>

template<typename ... Ts>
class Delegate final
{
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
		BindInternal(Target, Function, std::make_index_sequence<sizeof ... (Ts)>{});
	}
	template<typename T>
	void Bind(const T* const Target, void(T::* Function)(Ts...) const)
	{
		BindInternal(Target, Function, std::make_index_sequence<sizeof ... (Ts)>{});
	}

	void Unbind(void(*Function)(Ts...))
	{
		std::erase_if(Callbacks, [Function](const CallbackInfo& cInfo)->bool
			{
				return GetFunctionAddress(cInfo.Callback) == reinterpret_cast<uint64_t>(Function);
			});
	}
	template<typename T>
	void Unbind(T* const Target)
	{
		std::erase_if(Callbacks, [Target](const CallbackInfo& cInfo)->bool
			{
				return cInfo.Target == Target;
			});
	}

	template<typename ... Us>
	void Invoke(Us&&... args)
	{
		for (const CallbackInfo& callback : Callbacks)
		{
			callback(std::forward<Us>(args)...);
		}
	}

private:
	template<typename T, typename U, size_t ... Is>
	void BindInternal(T* const Target, U Function, const std::index_sequence<Is...>)
	{
		if constexpr (sizeof ... (Ts) == 0)
		{
			Callbacks.emplace_back(Target, std::bind(Function, Target));
		}
		else
		{
			Callbacks.emplace_back(Target, std::bind(Function, Target, std::_Ph<Is + 1>{}...));
		}
	}

	uint64_t GetFunctionAddress(const std::function<void(Ts...)>& function) const
	{
		typedef void(FunctionType)(Ts...);
		FunctionType* const* functionPointer{ function.template target<FunctionType*>() };

		return reinterpret_cast<uint64_t>(functionPointer);
	}

	std::vector<CallbackInfo> Callbacks;
};