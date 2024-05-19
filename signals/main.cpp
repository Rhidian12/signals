#include "signals.hpp"

#include <iostream>

void PrintNumber(int const a)
{
	std::cout << "Free function: " << a << "\n";
}

struct Foo
{
	void PrintNumber(const int a)
	{
		std::cout << "Member function: " << a << "\n";
	}

	void PrintNumberConst(const int a) const
	{
		std::cout << "Const Member function: " << a << "\n";
	}
};

void ShowBasicUsage()
{
	// Create a delegate accepting 1 integer as parameter
	Delegate<int> Delegate{};

	// 1. Add a free function
	Delegate.Bind(&PrintNumber);

	// 2. Add a lambda
	Delegate.Bind([](const int a)->void { std::cout << "Lambda: " << a << "\n"; }); // Add a lambda

	// 3. Add a member function
	Foo foo;
	Delegate.Bind(&foo, &Foo::PrintNumber);
	Delegate.Bind(&foo, &Foo::PrintNumberConst);

	Delegate.Invoke(3); // r-values are possible

	const int& Value{ 3 };
	Delegate.Invoke(Value); // const l-value references as well
}

int main()
{
	ShowBasicUsage();
	std::cout << "\n==============================\n";


}