#pragma once


#include "glad.h"

#include <string>
#include <sstream>

namespace Nullity 
{
	void DebugLog(std::ostringstream& oss);
	void DebugLog(std::string& string);

	void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam);
}
