#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP


#include <map>
#include <napi.h>

std::map<std::string, std::string> parseConfig(const Napi::Env& env, Napi::String jsonConfig);
std::map<std::string, std::string> parseConfig(const Napi::Env& env, Napi::Object configObject);


#endif //CONFIG_PARSER_HPP