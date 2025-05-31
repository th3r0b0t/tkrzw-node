#include "../../include/utils/processor_jsfunc_wrapper.hpp"

std::string_view processor_jsfunc_wrapper::ProcessFull(std::string_view key, std::string_view value)
{
    std::promise<CallJSPromiseType>* result_promise = new std::promise<CallJSPromiseType>();
    std::future<CallJSPromiseType> result_future = result_promise->get_future();
    DataType* data = new callJSData{true, result_promise, key, value};
    
    tsfn.BlockingCall(data);

    CallJSPromiseType JSres = result_future.get();
    if(JSres.type == CALLJS_PROMISE_RETURNED_VALUE_TYPE::OPERATION )
    {
        if(JSres.result == "NOOP") { return NOOP; }
        else if(JSres.result == "REMOVE") { return REMOVE; }
    }
    else if(JSres.type == CALLJS_PROMISE_RETURNED_VALUE_TYPE::STRING )
    {
        new_value_memory = JSres.result;
        return new_value_memory;
    }

}

std::string_view processor_jsfunc_wrapper::ProcessEmpty(std::string_view key){}