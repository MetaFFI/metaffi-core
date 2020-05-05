#pragma once
#include <functional>
#include <iostream>

class scope_guard
{
private:
	std::function<void()> _f;

public:
	scope_guard(std::function<void()> f):_f(f){}
	~scope_guard()
	{
		try
		{
			_f();
		}
		catch(...)
		{
			std::cerr << "Critical Error: Exceptions must not reach ~scope_guard()" << std::endl;
		}
	}
};