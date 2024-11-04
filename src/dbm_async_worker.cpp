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

template <typename... argTypes>
dbmAsyncWorker::dbmAsyncWorker(const Napi::Env& env, tkrzw::PolyDBM& dbmReference, OPERATION_TYPE operation, argTypes... paramPack):
Napi::AsyncWorker(env),
dbmReference(dbmReference),
operation(operation),
deferred_promise{Env()},
params(std::make_tuple(paramPack...))
{};

void dbmAsyncWorker::Execute()
{
    if(operation == SET)
    {
        if constexpr (std::tuple_size<decltype(params)>::value == 2)
        {
            std::string& p1 = std::get<0>(params);
            tkrzw::Status set_status = dbmReference.Set( "", "" );
            
            if( set_status != tkrzw::Status::SUCCESS)
            {
                //Napi::TypeError::New(env, set_status.GetMessage().c_str()).ThrowAsJavaScriptException();
                SetError("Couldn't set!");
            }
        }
    }
    else if(operation == GET_SIMPLE)
    {
        if constexpr (std::tuple_size<decltype(params)>::value == 2)
        {
            result = dbmReference.GetSimple( "", "" );
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
        if constexpr (std::tuple_size<decltype(params)>::value == 1)
        {
            tkrzw::Status rebuild_status = dbmReference.RebuildAdvanced( std::map<std::string,std::string>() );
            if( rebuild_status != tkrzw::Status::SUCCESS)
            {
                SetError("(rebuild) Error rebuilding database!");
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