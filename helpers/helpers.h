#pragma once

#include <sstream>

#define STR(...) static_cast<std::stringstream &&>(std::stringstream() << __VA_ARGS__).str()

/** Marks given argument as unused so that the compiler will stop giving warnings about it when extra warnings are enabled. 
 */
#define UNUSED(ARG_NAME) (void)(ARG_NAME)

#define UNIMPLEMENTED 

#define UNREACHABLE 