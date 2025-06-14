#ifndef DBM_ASYNC_WORKER_HPP
#define DBM_ASYNC_WORKER_HPP

#include <any>
#include <optional>
#include <functional>       //For std::reference_wrapper
#include <vector>
#include <iostream>
#include <tkrzw_dbm_poly.h>
#include <tkrzw_index.h>

#include <memory>           //For std::unique_ptr
#include <utility>          //For std::pair
#include <napi.h>
#include "utils/processor_jsfunc_wrapper.hpp"
#include "utils/tsfn_types.hpp"

class dbmAsyncWorker : public Napi::AsyncWorker
{
    public:
        enum OPERATION_TYPE
        {
            DBM_SET,
            DBM_APPEND,
            DBM_GET_SIMPLE,
            DBM_SHOULD_BE_REBUILT,
            DBM_REBUILD,
            DBM_SYNC,
            DBM_PROCESS,

            INDEX_ADD,
            INDEX_GET_VALUES,
            INDEX_CHECK,
            INDEX_REMOVE,
            INDEX_SHOULD_BE_REBUILT,
            INDEX_REBUILD,
            INDEX_SYNC,
            INDEX_MAKE_JUMP_ITERATOR,
            INDEX_GET_ITERATOR_VALUE,
            INDEX_CONTINUE_ITERATION
        };

        /*dbmAsyncWorker(const Napi::Env& env, tkrzw::PolyDBM& dbmReference, OPERATION_TYPE operation, std::string param1, std::string param2);
        dbmAsyncWorker(const Napi::Env& env, tkrzw::PolyDBM& dbmReference, OPERATION_TYPE operation);*/
        /*template <typename... argTypes>
        dbmAsyncWorker(const Napi::Env& env, tkrzw::PolyDBM& dbmReference, OPERATION_TYPE operation, argTypes... paramPack);*/
        template <typename... argTypes>
        dbmAsyncWorker(const Napi::Env& env, tkrzw::PolyDBM& dbmReference, OPERATION_TYPE operation, argTypes... paramPack):
        Napi::AsyncWorker(env),
        dbmReference(dbmReference),
        operation(operation),
        deferred_promise{Env()}
        {
            (params.emplace_back(std::any(paramPack)),...);
        };

        template <typename... argTypes>
        dbmAsyncWorker(const Napi::Env& env, tkrzw::PolyIndex& indexReference, OPERATION_TYPE operation, argTypes... paramPack):
        Napi::AsyncWorker(env),
        indexReference(indexReference),
        operation(operation),
        deferred_promise{Env()}
        {
            (params.emplace_back(std::any(paramPack)),...);
        };

        void Execute() override;
        void OnOK() override;
        void OnError(const Napi::Error& err) override;
        
        Napi::Promise::Deferred deferred_promise;

    private:
        //std::string param1;
        //std::string param2;
        std::vector<std::any> params;
        std::any any_result;
        //std::string result;
        OPERATION_TYPE operation;
        std::optional<std::reference_wrapper<tkrzw::PolyDBM>> dbmReference;
        std::optional<std::reference_wrapper<tkrzw::PolyIndex>> indexReference;
        //tkrzw::PolyDBM& dbmReference;
        //tkrzw::PolyIndex& indexReference;
};

#endif //DBM_ASYNC_WORKER_HPP