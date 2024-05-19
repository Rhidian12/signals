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
};

int main()
{
	Delegate<int> signal{};

	// 1. Add a free function
	signal.Bind(&PrintNumber);

	// 2. Add a lambda
	signal.Bind([](const int a)->void { std::cout << "Lambda: " << a << "\n"; }); // Add a lambda

	// 3. Add a member function
	Foo foo;
	signal.Bind(&foo, &Foo::PrintNumber);

	signal.Invoke(3);

	std::cout << "\n\n==============================\n\n";


}