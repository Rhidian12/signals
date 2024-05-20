#include "signals.hpp"

#include <iostream>

void PrintNumber(const int a)
{
	std::cout << "Free function: " << a << "\n";
}

void PrintNumbers(const int a, const float b)
{
	std::cout << "Free function: " << a << ", " << b << "\n";
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

	void PrintNumbers(const int a, const float b)
	{
		std::cout << "Member function: " << a << ", " << b << "\n";
	}
};

void ShowBasicUsage()
{
	// Create a delegate accepting 1 integer as parameter
	Delegate<int> Delegate{};

	// 1. Add a free function
	Delegate.Bind(&PrintNumber);

	// 2. Add a lambda
	// Even though the template parameter is only 'int', we can still add 'const'
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

void ShowUnassignUsage()
{
	Delegate<int, float> Delegate{};

	// 1. Assign a free function
	Delegate.Bind(&PrintNumbers);

	// 2. Assign a lambda
	// Since we're not storing the lambda, we cannot unassign it later
	Delegate.Bind([](const int a, const float b) { std::cout << "Lambda: " << a << ", " << b << "\n"; });

	// 3. Assign a member function
	Foo foo;
	Delegate.Bind(&foo, &Foo::PrintNumbers);

	// Now, we unbind our member function which will ensure it is not called when 'Invoke()' is called
	// It is currently not possible to unbind specific member functions from 
	Delegate.Unbind(&foo);
	Delegate.Unbind(&PrintNumbers);

	Delegate.Invoke(5, 10.f);
}

int main()
{
	ShowBasicUsage();
	std::cout << "\n==============================\n";

	ShowUnassignUsage();
	std::cout << "\n==============================\n";
}