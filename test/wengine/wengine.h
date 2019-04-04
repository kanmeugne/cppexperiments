#ifndef WENGINE_H
#define WENGINE_H
#include <exception>
#include <sstream>

#ifdef WENGINE_EXPORTS
#define WENGINE_API __declspec(dllexport)
#else
#define WENGINE_API __declspec(dllimport)
#endif


namespace engine
{
	class WENGINE_API DivisionByZeroException : public std::exception
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
		WENGINE_API int my_addition_func(int, int);
		WENGINE_API int my_multiplication_func(int, int);
		WENGINE_API int my_division_func(int, int);
		WENGINE_API int my_soustraction_func(int, int);
		WENGINE_API int my_power_func(int, int);
	}; // entier

	namespace numeric
	{
		WENGINE_API double my_addition_func(double, double);
		WENGINE_API double my_multiplication_func(double, double);
		WENGINE_API double my_division_func(double, double);
		WENGINE_API double my_soustraction_func(double, double);
		WENGINE_API double my_power_func(double, double);
	}; // numeric
}; // engine

#endif // WENGINE_H