// wengine.cpp : Définit les fonctions exportées pour l'application DLL.
//

#include "stdafx.h"

using namespace std;

// my_addition_func
int engine::entier::my_addition_func(int a, int b)
{
	return a + b;
}

// my_multiplication_func
int engine::entier::my_multiplication_func(int a, int b)
{
	return a * b;
}

// my_division_func
int engine::entier::my_division_func(int a, int b)
{
	if (b == 0)
	{
		throw DivisionByZeroException(a, b);
	}
	return a / b;
}

// my_soustraction_func
int engine::entier::my_soustraction_func(int a, int b)
{
	return a - b;
}

// my_power_func
int engine::entier::my_power_func(int a, int b)
{
	if (b == 0)
	{
		throw DivisionByZeroException(a, b);
	}
	return (int)std::pow(a, b);
}

// my_addition_func
double engine::numeric::my_addition_func(double a, double b)
{
	return a + b;
}

// my_multiplication_func
double engine::numeric::my_multiplication_func(double a, double b)
{
	return a * b;
}

// my_division_func
double engine::numeric::my_division_func(double a, double b)
{
	if (b == 0)
	{
		throw DivisionByZeroException(a, b);
	}
	return a / b;
}

// my_soustraction_func
double engine::numeric::my_soustraction_func(double a, double b)
{
	return a - b;
}

// my_power_func
double engine::numeric::my_power_func(double a, double b)
{
	if (b == 0)
	{
		throw DivisionByZeroException(a, b);
	}
	return std::pow(a, b);
}

// DivisionByZeroException::build_phrase
template <typename T>
void engine::DivisionByZeroException::build_phrase(T a, T b) throw()
{
	ostringstream format;
	format << "Division by zero error : left operand = " << a << "; right operand = " << b;
	m_phrase = format.str();
}

// DivisionByZeroException
engine::DivisionByZeroException::DivisionByZeroException(double a, double b) throw()
{
	build_phrase<double>(a, b);
}

// DivisionByZeroException
engine::DivisionByZeroException::DivisionByZeroException(int a, int b) throw()
{
	build_phrase<int>(a, b);
}

// DivisionByZeroException::what
const char * engine::DivisionByZeroException::what() const throw()
{
	return m_phrase.c_str();
}

// DivisionByZeroException::~DivisionByZeroException
engine::DivisionByZeroException::~DivisionByZeroException() throw()
{
}
