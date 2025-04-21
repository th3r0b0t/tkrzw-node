#include "../include/polyDBM_wrapper.hpp"


polyDBM_wrapper::polyDBM_wrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<polyDBM_wrapper>(info)
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

Napi::Value polyDBM_wrapper::set(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    std::string key = info[0].As<Napi::String>().ToString().Utf8Value();
    std::string value = info[1].As<Napi::String>().ToString().Utf8Value();

    dbmAsyncWorker* asyncWorker = new dbmAsyncWorker(Env(), dbm, dbmAsyncWorker::DBM_SET, key, value);
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

Napi::Value polyDBM_wrapper::getSimple(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    std::string key = info[0].As<Napi::String>().ToString().Utf8Value();
    std::string default_value = info[1].As<Napi::String>().ToString().Utf8Value();

    dbmAsyncWorker* asyncWorker = new dbmAsyncWorker(Env(), dbm, dbmAsyncWorker::DBM_GET_SIMPLE, key, default_value);
    asyncWorker->Queue();
    return asyncWorker->deferred_promise.Promise();
    
    /*std::string getSimple_result = dbm.GetSimple(key,default_value);

    return Napi::String::New(env, getSimple_result);*/
}

Napi::Value polyDBM_wrapper::shouldBeRebuilt(const Napi::CallbackInfo& info)
{
    dbmAsyncWorker* asyncWorker = new dbmAsyncWorker(Env(), dbm, dbmAsyncWorker::DBM_SHOULD_BE_REBUILT);
    asyncWorker->Queue();
    return asyncWorker->deferred_promise.Promise();
    
    /*std::string getSimple_result = dbm.GetSimple(key,default_value);

    return Napi::String::New(env, getSimple_result);*/
}

Napi::Value polyDBM_wrapper::rebuild(const Napi::CallbackInfo& info)
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

    dbmAsyncWorker* asyncWorker = new dbmAsyncWorker(Env(), dbm, dbmAsyncWorker::DBM_REBUILD, optional_tuning_params);
    asyncWorker->Queue();
    return asyncWorker->deferred_promise.Promise();
    
    /*std::string getSimple_result = dbm.GetSimple(key,default_value);

    return Napi::String::New(env, getSimple_result);*/
}

Napi::Value polyDBM_wrapper::sync(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    bool sync_hard = info[0].As<Napi::Boolean>().ToBoolean();

    dbmAsyncWorker* asyncWorker = new dbmAsyncWorker(Env(), dbm, dbmAsyncWorker::DBM_SYNC, sync_hard);
    asyncWorker->Queue();
    return asyncWorker->deferred_promise.Promise();
}

Napi::Value polyDBM_wrapper::close(const Napi::CallbackInfo& info)
{
    std::cout << "CLOSE DBM" << std::endl;
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
Napi::Object polyDBM_wrapper::Init(Napi::Env env, Napi::Object exports)
{
    // This method is used to hook the accessor and method callbacks
    Napi::Function functionList = DefineClass(env, "polyDBM",
    {
        InstanceMethod<&polyDBM_wrapper::set>("set", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&polyDBM_wrapper::getSimple>("getSimple", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&polyDBM_wrapper::shouldBeRebuilt>("shouldBeRebuilt", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&polyDBM_wrapper::rebuild>("rebuild", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&polyDBM_wrapper::sync>("sync", static_cast<napi_property_attributes>(napi_writable | napi_configurable)),
        InstanceMethod<&polyDBM_wrapper::close>("close", static_cast<napi_property_attributes>(napi_writable | napi_configurable))
    });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();

    *constructor = Napi::Persistent(functionList);
    exports.Set("polyDBM", functionList);
    env.SetInstanceData<Napi::FunctionReference>(constructor);

    return exports;
}

void polyDBM_wrapper::Finalize(Napi::BasicEnv env)
{
    tkrzw::Status close_status = dbm.Close();
    if( close_status != tkrzw::Status::SUCCESS)
    {
        std::cerr << "DBM finalize: Failed!" << std::endl;
    }
    else
    { std::cerr << "DBM finalize: SUCCESS!" << std::endl; }   
}
//-----------------JS-Requirements-----------------//