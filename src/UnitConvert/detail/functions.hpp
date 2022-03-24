#ifndef UnitConvert_detail_functions_hpp
#define UnitConvert_detail_functions_hpp

/** @file functions.hpp
  * @brief 
  * @author C.D. Clark III
  * @date 09/06/18
  */

#include <sstream>
#include <boost/units/io.hpp>

namespace unit_convert {
namespace detail {

template <typename T>
inline std::string
str(const T& t)
{
  std::ostringstream s;
  s << t;
  return s.str();
}

template <typename T>
inline std::string
str(const T& t, boost::units::format_mode mode)
{
  std::ostringstream s;
  boost::units::set_format(s, mode);
  s << t;
  return s.str();
}

  // remove whitespace from beginning of string
  inline std::string &ltrim(std::string &s) {
	  s.erase(s.begin(), std::find_if(s.begin(), s.end(), 
		  [](int c) {return !std::isspace(c);}));
	  return s;
  }

  // remove whitespace from end of string
	inline
	std::string&
	rtrim(std::string& s)
	{
	s.erase(std::find_if(s.rbegin(), s.rend(),
		[](int c) {return !std::isspace(c);})
				.base(),
			s.end());
	return s;
	}

  // remove whitespace from both ends of string
  inline
  std::string&
  trim(std::string& s)
  {
    return ltrim(rtrim(s));
  }
}

}
namespace UnitConvert {
  namespace detail {
    using namespace unit_convert::detail;
  }
}


#endif // include protector
