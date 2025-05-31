#include "../../include/utils/tsfn_types.hpp"
#include "../../include/utils/globals.hpp"
#include <iostream>

void CallJS(Napi::Env env, Napi::Function jsCallback, ContextType* context, DataType* data)
{
    // Retrieve associated static properties of add-on instance
    /*
    Napi::FunctionReference* constructor = env.GetInstanceData<Napi::FunctionReference>();
    Napi::Function polyDBMConstructor = constructor->Value();

    std::cerr << "###############################" << std::endl;
    std::cerr << "-> napi_undefined " << napi_valuetype::napi_undefined << std::endl;
    std::cerr << "-> napi_null " << napi_valuetype::napi_null << std::endl;
    std::cerr << "-> napi_symbol " << napi_valuetype::napi_symbol << std::endl;
    Napi::Value noopSym = env.GetInstanceData<Napi::FunctionReference>()->Value().Get("NOOP");
    Napi::Value removeSym = env.GetInstanceData<Napi::FunctionReference>()->Value().Get("REMOVE");
    std::cerr << "noopSym.IsSymbol(): " << noopSym.IsSymbol() << "noopSym.Type(): " << noopSym.Type() << std::endl;
    std::cerr << "removeSym.IsSymbol(): " << removeSym.IsSymbol() << "removeSym.Type(): " << removeSym.Type() << std::endl;
    */

    Napi::Boolean keyExists = Napi::Boolean::New(env, data->processFull);
    Napi::String key = Napi::String::New(env, std::string(data->key));
    Napi::String value = Napi::String::New(env, std::string(data->value));
    std::promise<CallJSPromiseType>* cppPromise = data->result_promise;

    Napi::Value jsRes = jsCallback.Call({keyExists, key, value});
    if( jsRes.IsPromise() )
    {
        Napi::Object promiseObj = jsRes.As<Napi::Object>();
        Napi::Function promise_then = promiseObj.Get("then").As<Napi::Function>();
        promise_then.Call(jsRes,
            {
                Napi::Function::New(env, [cppPromise](const Napi::CallbackInfo& info)
                {
                    CallJSPromiseType res;
                    // On resolve
                    if( std::string(info[0].As<Napi::String>()) == "___TKRZW_NOOP___" ) { res = CallJSPromiseType{CALLJS_PROMISE_RETURNED_VALUE_TYPE::OPERATION, "NOOP"}; }
                    else if( std::string(info[0].As<Napi::String>()) == "___TKRZW_REMOVE___" ) { res = CallJSPromiseType{CALLJS_PROMISE_RETURNED_VALUE_TYPE::OPERATION, "REMOVE"}; }
                    else { res = CallJSPromiseType{CALLJS_PROMISE_RETURNED_VALUE_TYPE::STRING, info[0].As<Napi::String>()}; }
                    cppPromise->set_value(res);
                }),
                Napi::Function::New(env, [cppPromise](const Napi::CallbackInfo& info)
                {
                    // On reject
                    cppPromise->set_value( CallJSPromiseType{CALLJS_PROMISE_RETURNED_VALUE_TYPE::OPERATION, "NOOP"} );
                })
            }
        );
    }
    else
    {
        //std::cerr << "Js function didn't return promise ------: " << (std::string(jsRes.As<Napi::String>()) == "___TKRZW_NOOP___") << std::endl;
        //if( jsRes.IsSymbol() ) { std::cerr << "Confirmed to be a symbol ------\t" << jsRes.As<Napi::Symbol>().StrictEquals(noopSym) << std::endl; }
        CallJSPromiseType res;
        if( std::string(jsRes.As<Napi::String>()) == "___TKRZW_NOOP___" ) { res = CallJSPromiseType{CALLJS_PROMISE_RETURNED_VALUE_TYPE::OPERATION, "NOOP"}; }
        else if( std::string(jsRes.As<Napi::String>()) == "___TKRZW_REMOVE___" ) { res = CallJSPromiseType{CALLJS_PROMISE_RETURNED_VALUE_TYPE::OPERATION, "REMOVE"}; }
        else { res = CallJSPromiseType{CALLJS_PROMISE_RETURNED_VALUE_TYPE::STRING, jsRes.As<Napi::String>()}; }

        cppPromise->set_value(res);
    }
}

//TODO: Using Napi::String and comparing against string is obviously subject to string spoofing but couldn't get `Napi::Symbol` to work because `StrictEquals` doesn't work! Even with `Napi::String`!