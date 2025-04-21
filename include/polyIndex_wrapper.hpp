#ifndef POLYINDEX_WRAPPER_HPP
#define POLYINDEX_WRAPPER_HPP

#include <tkrzw_index.h>
#include "config_parser.hpp"
#include "dbm_async_worker.hpp"
#include <napi.h>

class polyIndex_wrapper : public Napi::ObjectWrap<polyIndex_wrapper>
{
    private:
        tkrzw::PolyIndex index;

    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);          //required by Node!
        polyIndex_wrapper(const Napi::CallbackInfo& info);
        Napi::Value add(const Napi::CallbackInfo& info);
        Napi::Value getValues(const Napi::CallbackInfo& info);
        Napi::Value check(const Napi::CallbackInfo& info);
        Napi::Value remove(const Napi::CallbackInfo& info);
        Napi::Value shouldBeRebuilt(const Napi::CallbackInfo& info);
        Napi::Value rebuild(const Napi::CallbackInfo& info);
        Napi::Value sync(const Napi::CallbackInfo& info);
        Napi::Value close(const Napi::CallbackInfo& info);
        void Finalize(Napi::BasicEnv env);
};


#endif //POLYINDEX_WRAPPER_HPP