#ifndef USING_VARS_H
#define USING_VARS_H

#include <map>
#include <vector>
#include <string>

//using JSONObject = std::map<*char, *JSONNode>;
using JSONObject = std::map<std::string, *JSONNode>;
using JSONList = std::vector<*JSONNode>; 

#endif
