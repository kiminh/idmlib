///
/// @file idm_term.h
/// @brief indicate the interfaces of input class for keyphrase extraction algorithm.
/// @author Jia Guo <guojia@gmail.com>
/// @date Created 2010-04-08
/// @date Updated 2010-04-08
///

#ifndef IDM_KPETERM_H_
#define IDM_KPETERM_H_

#include <string>
#include <vector>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <util/ustring/UString.h>
#include "../idm_types.h"


NS_IDMLIB_UTIL_BEGIN


struct IDMTerm
{
  izenelib::util::UString text;
  uint32_t id;
  char tag;
  uint32_t position;
};


NS_IDMLIB_UTIL_END

#endif 