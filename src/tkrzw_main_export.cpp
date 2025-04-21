#include "../include/polyDBM_wrapper.hpp"
#include "../include/polyIndex_wrapper.hpp"

Napi::Object InitAll (Napi::Env env, Napi::Object exports)
{
    polyDBM_wrapper::Init(env, exports);
    polyIndex_wrapper::Init(env, exports);
    return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)