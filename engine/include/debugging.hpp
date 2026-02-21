#pragma once

#include "glad.h"

#include <string>
#include <sstream>
#include <fstream>


namespace Nullity
{
	class Debug
	{
	public:
		Debug();

		void Log(const std::ostringstream& oss);
		void Log(const std::string& string);

		static void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam);
	private:
		std::ofstream fout;
	};

	extern Debug debug;
}
