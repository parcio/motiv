#include <iostream>
#include <functional>

int globVal = 12;

auto wrapper_function = [](int n)
{
	std::function<int(int)> fac = [&](int n) { return (n < 2) ? 1 : n * fac(n - 1); };
	return fac(n);
};

int main()
{
    std::cout << "the factorial of " << globVal << " is " << wrapper_function(globVal) << "\n";
    return 0;
}

