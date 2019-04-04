#include "stdafx.h"

int main(int argc, char* argv[])
{
	int a = 0, b = 0;
	if (argc == 3)
	{
		a = atoi(argv[1]);
		b = atoi(argv[2]);
	}

	// addition 
	std::cout << "Shared Lib (default)!" << std::endl;
	std::cout << "a = " << a << "; b = " << b << "; a+b = [" << engine::entier::my_addition_func(a, b) << "]" << std::endl;

	// division
	std::cout << "Shared Lib (default)!" << std::endl;
	try
	{
		std::cout << "a = " << a << "; b = " << b  << "; a/b = [" << engine::numeric::my_division_func(a, b) << "]" << std::endl;
	}
	catch (engine::DivisionByZeroException const& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}