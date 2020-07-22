#include <boost/dll.hpp>

template<typename T>
std::shared_ptr<typename boost::dll::detail::import_type<T>::type> load_func(const std::string& fullpath, const std::string& funcname, boost::dll::load_mode::type mode )
{
	return std::make_shared<typename boost::dll::detail::import_type<T>::type>(
			boost::dll::import<T>(fullpath, funcname, mode)
	);
}

template<typename T>
std::shared_ptr<typename boost::dll::detail::import_type<T>::type> load_func(const boost::dll::shared_library& so, const std::string& funcname)
{
	return std::make_shared<typename boost::dll::detail::import_type<T>::type>(
			boost::dll::import<T>(so, funcname)
	);
}