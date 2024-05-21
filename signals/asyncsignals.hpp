#pragma once

#include <coroutine>
#include <functional>
#include <mutex>
#include <memory>
#include <vector>

namespace asyncdelegateimpl
{
	template<typename T>
	struct StateData final
	{
		std::vector<std::unique_ptr<std::function<void(std::unique_lock<std::mutex>&)>>> Callbacks;
		std::unique_ptr<T> Value;
		std::mutex Mutex;
		bool IsReady;
	};

	template<>
	struct StateData<void> final
	{
		std::vector<std::unique_ptr<std::function<void(std::unique_lock<std::mutex>&)>>> Callbacks;
		std::mutex Mutex;
		bool IsReady;
	};

	inline namespace concepts
	{
		template<typename T>
		concept IsTypeVoid = std::is_void_v<T>;

		template<typename T>
		concept IsTypeNotVoid = !IsTypeVoid<T>;
	}
}

template<typename T>
class AsyncDelegate final
{
public:
	struct awaiter
	{
		AsyncDelegate<T> Delegate;
		
		explicit awaiter(AsyncDelegate<T> InDelegate)
			: Delegate(InDelegate)
		{}

		bool await_ready() const noexcept
		{
			return Delegate.IsReady();
		}

		void await_suspend(std::coroutine_handle<> Handle)
		{
			std::unique_lock<std::mutex> Lock(Delegate.Data.Mutex);

			Delegate.Data.Callbacks.emplace_back(std::make_unique<std::function<void()>>([Handle](std::unique_lock<std::mutex>& Lock)->void
				{
					Lock.unlock();
					Handle.resume();
					Lock.lock();
				}));

			if (Delegate.IsReadyInternal())
			{
				PopAndRunCallbacks(Lock);
			}
		}

		auto await_resume()
		{
			return Delegate.Get();
		}
	};

	awaiter operator co_await() const noexcept
	{
		return awaiter(*this);
	}

	bool IsReady() const
	{
		std::unique_lock<std::mutex> Lock(Data.Mutex);
		return IsReadyInternal();
	}

	template<typename U = T> requires std::convertible_to<U, T>
	bool Set(U Value) const
	{
		std::unique_lock<std::mutex> Lock(Data.Mutex);
		if (IsReadyInternal())
		{
			return false;
		}
		Data.Value = std::make_unique<T>(std::move(Value));
		PopAndRunCallbacks(Lock);
		return true;
	}

	template<asyncdelegateimpl::IsTypeVoid U = T>
	bool Set() const
	{
		std::unique_lock<std::mutex> Lock(Data.Mutex);
		if (IsReadyInternal())
		{
			return false;
		}
		Data.IsReady = true;
		PopAndRunCallbacks(Lock);
		return true;
	}

	template<asyncdelegateimpl::IsTypeNotVoid U = T>
	U& Get() const
	{
		std::unique_lock<std::mutex> Lock(Data.Mutex);
		// [TODO]: ERROR HANDLE IF NOT READY
		return *Data.Value;
	}

	template<asyncdelegateimpl::IsTypeVoid U = T>
	void Get() const
	{
		std::unique_lock<std::mutex> Lock(Data.Mutex);
		// [TODO]: ERROR HANDLE IF NOT READY
		return;
	}

private:
	bool IsReadyInternal() const
	{
		if constexpr (asyncdelegateimpl::IsTypeNotVoid<T>)
		{
			return Data.Value != nullptr;
		}
		else
		{
			return Data.IsReady;
		}
	}

	void PopAndRunCallbacks(std::unique_lock<std::mutex>& Lock) const
	{
		const auto Callbacks{ std::move(Data.Callbacks) };
		for (const auto& Callback : Callbacks)
		{
			(*Callback)(Lock);
		}
	}

	asyncdelegateimpl::StateData<T> Data;
};