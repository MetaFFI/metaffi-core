#include <iostream>

template<typename _T>
class scope_guard
{
private:
	_T _f;

public:
	scope_guard(_T&& f):_f(f){}
	~scope_guard()
	{
		try
		{
			_f();
		}
		catch(...)
		{
			std::cerr << "Error caught during scope_guard destructor! Exceptions must not be thrown outside scope_guard function!" << std::endl;
		}
	}
};