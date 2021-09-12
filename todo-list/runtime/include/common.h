#pragma once

#include <map>
#include <string>
#include <vector>

// http headers
typedef std::map<std::string, std::vector<std::string>> Header;

// http query
typedef std::map<std::string, std::vector<std::string>> Values;
