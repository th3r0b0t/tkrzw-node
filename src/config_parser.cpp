#include "../include/config_parser.hpp"
#include<iostream>

std::map<std::string, std::string> parseConfig(const Napi::Env& env, const Napi::Value& jsonOrObjectConfig)
{
    Napi::Object js_config_obj;
    if( jsonOrObjectConfig.IsString() )
    {
        Napi::Object js_json_obj = env.Global().Get("JSON").As<Napi::Object>();
        Napi::Function js_parse_func = js_json_obj.Get("parse").As<Napi::Function>();
        js_config_obj = js_parse_func.Call(js_json_obj, {jsonOrObjectConfig}).As<Napi::Object>();
    }
    else if( jsonOrObjectConfig.IsObject() ) { js_config_obj = jsonOrObjectConfig.As<Napi::Object>(); }

    std::map<std::string, std::string> tkrzw_config_map;

    for (const auto &elem : js_config_obj)       //std::pair<Napi::Value, Napi::Object::PropertyLValue<Napi::Value>>&
    {
        //The operator std::string() is implicitly invoked
        tkrzw_config_map.emplace(elem.first.As<Napi::String>(), static_cast<Napi::Value>(elem.second).As<Napi::String>());
    }

    return tkrzw_config_map;
}