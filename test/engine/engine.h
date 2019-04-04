#ifndef ENGINE_H
#define ENGINE_H
#include <exception>
#include <sstream>


namespace engine
{
	class DivisionByZeroException : public std::exception
	{
	public:
		DivisionByZeroException(int, int) throw();
		DivisionByZeroException(double, double) throw();
		virtual const char* what() const throw();
		virtual ~DivisionByZeroException() throw();
	private:
		std::string m_phrase;
		template<typename T> void build_phrase(T a, T b) throw();
	}; // DivisionByZeroError

	namespace entier
	{
		int my_addition_func(int, int);
		int my_multiplication_func(int, int);
		int my_division_func(int, int);
		int my_soustraction_func(int, int);
		int my_power_func(int, int);
	}; // entier

	namespace numeric
	{
		double my_addition_func(double, double);
		double my_multiplication_func(double, double);
		double my_division_func(double, double);
		double my_soustraction_func(double, double);
		double my_power_func(double, double);
	}; // numeric
}; // engine

#endif // ENGINE_H