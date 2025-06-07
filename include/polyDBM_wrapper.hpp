#ifndef POLYDBM_WRAPPER_HPP
#define POLYDBM_WRAPPER_HPP

#include <tkrzw_dbm_poly.h>
#include "config_parser.hpp"
#include "dbm_async_worker.hpp"
#include <napi.h>
#include "utils/globals.hpp"
#include <iostream>

class polyDBM_wrapper : public Napi::ObjectWrap<polyDBM_wrapper>
{
    private:
        tkrzw::PolyDBM dbm;
        //std::map<std::string, std::string> optional_tuning_params;
    
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);          //required by Node!
        polyDBM_wrapper(const Napi::CallbackInfo& info);
        Napi::Value set(const Napi::CallbackInfo& info);                        //async promise
        Napi::Value append(const Napi::CallbackInfo& info);                     //async promise
        Napi::Value getSimple(const Napi::CallbackInfo& info);                  //async promise
        Napi::Value shouldBeRebuilt(const Napi::CallbackInfo& info);            //async promise
        Napi::Value rebuild(const Napi::CallbackInfo& info);                    //async promise
        Napi::Value sync(const Napi::CallbackInfo& info);                       //async promise
        Napi::Value process(const Napi::CallbackInfo& info);                    //async promise
        Napi::Value close(const Napi::CallbackInfo& info);
        void Finalize(Napi::BasicEnv env);
};


#endif //POLYDBM_WRAPPER_HPP