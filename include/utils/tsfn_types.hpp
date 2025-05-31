#ifndef TSFN_TYPES_HPP
#define TSFN_TYPES_HPP

#include <cstddef>
#include <string>
#include <string_view>
#include <future>
#include <napi.h>

enum CALLJS_PROMISE_RETURNED_VALUE_TYPE
{
    STRING,
    OPERATION
};

//if type == STRING result is the actual string value
//if type == OPERATION result should be treated as either "NOOP" or "REMOVE" as tkrzw::RecordProcessor::NOOP/REMOVE
struct CallJSPromiseType
{
    CALLJS_PROMISE_RETURNED_VALUE_TYPE type;
    std::string result;
};


struct callJSData
{
    bool processFull;
    std::promise<CallJSPromiseType>* result_promise;
    std::string_view key;
    std::string_view value;
};


using ContextType = std::nullptr_t;
using DataType = callJSData;

void CallJS(Napi::Env env, Napi::Function jsCallback, ContextType* context, DataType* data);

using TSFN = Napi::TypedThreadSafeFunction<ContextType, DataType, CallJS>;


#endif //TSFN_TYPES_HPP