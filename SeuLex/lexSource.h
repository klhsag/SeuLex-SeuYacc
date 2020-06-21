#pragma once

#include <string>

namespace lexSource {

extern std::string program1;

extern std::string prepared_program1;

extern std::string program2;

extern std::string prepared_program2;

extern std::string auto_program;

extern std::string executer;

}

void yieldLexYyC(std::string filename, bool hasExec = true);
