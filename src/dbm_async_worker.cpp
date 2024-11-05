#include "../include/dbm_async_worker.hpp"

/*dbmAsyncWorker::dbmAsyncWorker(const Napi::Env& env, tkrzw::PolyDBM& dbmReference, OPERATION_TYPE operation, std::string param1, std::string param2):
Napi::AsyncWorker(env),
dbmReference(dbmReference),
operation(operation),
param1(param1),
param2(param2),
deferred_promise{Env()}
{};

dbmAsyncWorker::dbmAsyncWorker(const Napi::Env& env, tkrzw::PolyDBM& dbmReference, OPERATION_TYPE operation):
Napi::AsyncWorker(env),
dbmReference(dbmReference),
operation(operation),
deferred_promise{Env()}
{};*/

/*template <typename... argTypes>
dbmAsyncWorker::dbmAsyncWorker(const Napi::Env& env, tkrzw::PolyDBM& dbmReference, OPERATION_TYPE operation, argTypes... paramPack):
Napi::AsyncWorker(env),
dbmReference(dbmReference),
operation(operation),
deferred_promise{Env()}
{
    (params.emplace_back(std::any(paramPack)),...);
};*/

void dbmAsyncWorker::Execute()
{
    if(operation == SET)
    {
        if(params.size() == 2)
        {
            tkrzw::Status set_status = dbmReference.Set( std::any_cast<std::string>(params[0]), std::any_cast<std::string>(params[1]) );
            
            if( set_status != tkrzw::Status::SUCCESS)
            {
                //Napi::TypeError::New(env, set_status.GetMessage().c_str()).ThrowAsJavaScriptException();
                SetError("Couldn't set!");
            }
        }
    }
    else if(operation == GET_SIMPLE)
    {
        if(params.size() == 2)
        {
            result = dbmReference.GetSimple( std::any_cast<std::string>(params[0]), std::any_cast<std::string>(params[1]) );
            if(result == "PARAM2 HERE")
            {
                std::string x = "PARAM1 HERE";
                SetError("Key \"" + x + "\" not found!");
            }
        }
    }
    else if(operation == SHOULD_BE_REBUILT)
    {
        bool shouldBeRebuilt_result = false;
        tkrzw::Status shouldBeRebuilt_status = dbmReference.ShouldBeRebuilt(&shouldBeRebuilt_result);
        if( shouldBeRebuilt_status != tkrzw::Status::SUCCESS)
        {
            SetError("(shouldBeRebuilt) Error checking database status!");
        }
        else if(shouldBeRebuilt_result == false)
        {
            SetError("No need to rebuild the database just yet!");
        }
    }
    else if(operation == REBUILD)
    {
        if(params.size() == 1)
        {
            tkrzw::Status rebuild_status = dbmReference.RebuildAdvanced( std::any_cast<std::map<std::string,std::string>>(params[0]) );
            if( rebuild_status != tkrzw::Status::SUCCESS)
            {
                SetError("[" + std::to_string(rebuild_status.GetCode()) + "]: " + rebuild_status.GetMessage());
            }
        }
    }
}

void dbmAsyncWorker::OnOK()
{
    deferred_promise.Resolve( Napi::String::New(Env(), result) );
}

void dbmAsyncWorker::OnError(const Napi::Error& err)
{
    deferred_promise.Reject(err.Value());
}