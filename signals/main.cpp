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

	// You can invoke with any type, as long as it is convertible to the original template parameter given to 'Delegate'
	// in this case, a normal 'int' was passed, so (const) r-value integers and (const) l-value integers can all bind to it
	// if an 'int&' had been used instead, only l- and r-value references would be allowed
	// Note that this does mean that parameters will eventually be copied when the call is invoked (if the template parameter is not
	// a reference)

	Delegate.Invoke(3); // r-value references are possible

	const int& Value{ 3 };
	Delegate.Invoke(Value); // const l-value references as well
}

int main()
{
	ShowBasicUsage();
	std::cout << "\n==============================\n";


}