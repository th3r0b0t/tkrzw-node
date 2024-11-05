#include "tkrzw_dbm_poly.h"
#include <napi.h>
#include "../include/config_parser.hpp"
#include "../include/dbm_async_worker.hpp"


class databaseWrapper : public Napi::ObjectWrap<databaseWrapper>
{
    private:
        tkrzw::PolyDBM dbm;
        //std::map<std::string, std::string> optional_tuning_params;
    
    public:
        static Napi::Object Init(Napi::Env env, Napi::Object exports);          //required by Node!
        databaseWrapper(const Napi::CallbackInfo& info);
        Napi::Value set(const Napi::CallbackInfo& info);                        //async promise
        Napi::Value getSimple(const Napi::CallbackInfo& info);                  //async promise
        Napi::Value shouldBeRebuilt(const Napi::CallbackInfo& info);            //async promise
        Napi::Value rebuild(const Napi::CallbackInfo& info);                    //async promise
        Napi::Value close(const Napi::CallbackInfo& info);
        void Finalize(Napi::BasicEnv env);
};



//====================================================================================================================

databaseWrapper::databaseWrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<databaseWrapper>(info)
{
    Napi::Env env = info.Env();
    std::map<std::string, std::string> optional_tuning_params;
    if(info[0].IsString())
    {
        Napi::String jsonConfigString = info[0].As<Napi::String>();
        optional_tuning_params = parseConfig(env, jsonConfigString);
    }
    else if(info[0].IsObject())
    {
        Napi::Object configObject = info[0].As<Napi::Object>();
        optional_tuning_params = parseConfig(env, configObject);
    }
    std::string dbmPath = info[1].As<Napi::String>().ToString().Utf8Value();

    tkrzw::Status opening_status = dbm.OpenAdvanced(dbmPath, true, tkrzw::File::OPEN_DEFAULT | tkrzw::File::OPEN_SYNC_HARD, optional_tuning_params).OrDie();
    if( opening_status != tkrzw::Status::SUCCESS)
    {
        Napi::TypeError::New(env, opening_status.GetMessage().c_str()).ThrowAsJavaScriptException();
    }
}

Napi::Value databaseWrapper::set(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    std::string key = info[0].As<Napi::String>().ToString().Utf8Value();
    std::string value = info[1].As<Napi::String>().ToString().Utf8Value();

    dbmAsyncWorker* asyncWorker = new dbmAsyncWorker(Env(), dbm, dbmAsyncWorker::SET, key, value);
    asyncWorker->Queue();
    return asyncWorker->deferred_promise.Promise();
    
    /*tkrzw::Status set_status = dbm.Set(key,value);
    if( set_status != tkrzw::Status::SUCCESS)
    {
        Napi::TypeError::New(env, set_status.GetMessage().c_str()).ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }
    else
    { return Napi::Boolean::New(env, true); }*/
}

Napi::Value databaseWrapper::getSimple(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    std::string key = info[0].As<Napi::String>().ToString().Utf8Value();
    std::string default_value = info[1].As<Napi::String>().ToString().Utf8Value();

    dbmAsyncWorker* asyncWorker = new dbmAsyncWorker(Env(), dbm, dbmAsyncWorker::GET_SIMPLE, key, default_value);
    asyncWorker->Queue();
    return asyncWorker->deferred_promise.Promise();
    
    /*std::string getSimple_result = dbm.GetSimple(key,default_value);

    return Napi::String::New(env, getSimple_result);*/
}

Napi::Value databaseWrapper::shouldBeRebuilt(const Napi::CallbackInfo& info)
{
    dbmAsyncWorker* asyncWorker = new dbmAsyncWorker(Env(), dbm, dbmAsyncWorker::SHOULD_BE_REBUILT);
    asyncWorker->Queue();
    return asyncWorker->deferred_promise.Promise();
    
    /*std::string getSimple_result = dbm.GetSimple(key,default_value);

    return Napi::String::New(env, getSimple_result);*/
}

Napi::Value databaseWrapper::rebuild(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    std::map<std::string, std::string> optional_tuning_params;
    if(info[0].IsString())
    {
        Napi::String jsonConfigString = info[0].As<Napi::String>();
        optional_tuning_params = parseConfig(env, jsonConfigString);
    }
    else if(info[0].IsObject())
    {
        Napi::Object configObject = info[0].As<Napi::Object>();
        optional_tuning_params = parseConfig(env, configObject);
    }

    dbmAsyncWorker* asyncWorker = new dbmAsyncWorker(Env(), dbm, dbmAsyncWorker::REBUILD, optional_tuning_params);
    asyncWorker->Queue();
    return asyncWorker->deferred_promise.Promise();
    
    /*std::string getSimple_result = dbm.GetSimple(key,default_value);

    return Napi::String::New(env, getSimple_result);*/
}

Napi::Value databaseWrapper::close(const Napi::CallbackInfo& info)
{
    std::cout << "CLOSE" << std::endl;
    Napi::Env env = info.Env();
    tkrzw::Status close_status = dbm.Close();
    if( close_status != tkrzw::Status::SUCCESS)
    {
        Napi::TypeError::New(env, close_status.GetMessage().c_str()).ThrowAsJavaScriptException();
        return Napi::Boolean::New(env, false);
    }
    else
    { return Napi::Boolean::New(env, true); }
}

//====================================================================================================================

//-----------------JS-Requirements-----------------//
Napi::Object databaseWrapper::Init(Napi::Env env, Napi::Object exports)
{
    // This method is used to hook the accessor and method callbacks
    Napi::Function functionList = DefineClass(env, "tkrzw",
    {
        InstanceMethod<&databaseWrapper::set>("set", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&databaseWrapper::getSimple>("getSimple", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&databaseWrapper::shouldBeRebuilt>("shouldBeRebuilt", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&databaseWrapper::rebuild>("rebuild", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&databaseWrapper::close>("close", static_cast<napi_property_attributes>(napi_writable | napi_configurable))
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(functionList);
    exports.Set("tkrzw", functionList);
    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}

Napi::Object Init (Napi::Env env, Napi::Object exports)
{
    databaseWrapper::Init(env, exports);
    return exports;
}

void databaseWrapper::Finalize(Napi::BasicEnv env)
{
    tkrzw::Status close_status = dbm.Close();
    if( close_status != tkrzw::Status::SUCCESS)
    {
        std::cerr << "Finalize: Failed!" << std::endl;
    }
    else
    { std::cerr << "Finalize: SUCCESS!" << std::endl; }   
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
//-----------------JS-Requirements-----------------//