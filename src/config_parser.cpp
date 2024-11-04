#include "../include/config_parser.hpp"
#include<iostream>

std::map<std::string, std::string> parseConfig(const Napi::Env& env, Napi::String jsonConfig)
{
    // Napi::Env env = info.Env();
    // Napi::String json_string = info[0].As<Napi::String>();
    Napi::Object js_json_obj = env.Global().Get("JSON").As<Napi::Object>();
    Napi::Function js_parse_func = js_json_obj.Get("parse").As<Napi::Function>();
    Napi::Object js_parsed_config_obj = js_parse_func.Call(js_json_obj, {jsonConfig}).As<Napi::Object>();

    std::map<std::string, std::string> tkrzw_config_map;

    for (const auto &elem : js_parsed_config_obj)       //std::pair<Napi::Value, Napi::Object::PropertyLValue<?>>
    {
        tkrzw_config_map.emplace(elem.first.ToString().Utf8Value(), static_cast<Napi::Value>(elem.second).ToString().Utf8Value());
    }

    return tkrzw_config_map;
}

std::map<std::string, std::string> parseConfig(const Napi::Env& env, Napi::Object configObject)
{
    std::map<std::string, std::string> tkrzw_config_map;

    for (const auto &elem : configObject)       //std::pair<Napi::Value, Napi::Object::PropertyLValue<?>>
    {
        tkrzw_config_map.emplace(elem.first.ToString().Utf8Value(), static_cast<Napi::Value>(elem.second).ToString().Utf8Value());
    }

    return tkrzw_config_map;
}