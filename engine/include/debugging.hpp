#pragma once

#include "glad.h"

#include <string>
#include <sstream>


namespace Nullity
{
	namespace Debug
	{
		void DebugInit();

		void Log(const std::ostringstream& oss);
		void Log(const std::string& string);
	};
}
