#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP


#include <map>
#include <napi.h>

std::map<std::string, std::string> parseConfig(const Napi::Env& env, const Napi::Value& jsonOrObjectConfig);


#endif //CONFIG_PARSER_HPP