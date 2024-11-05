#include <any>
#include <vector>
#include "tkrzw_dbm_poly.h"
#include <napi.h>

class dbmAsyncWorker : public Napi::AsyncWorker
{
    public:
        enum OPERATION_TYPE
        {
            SET,
            GET_SIMPLE,
            SHOULD_BE_REBUILT,
            REBUILD
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

        void Execute() override;
        void OnOK() override;
        void OnError(const Napi::Error& err) override;
        
        Napi::Promise::Deferred deferred_promise;

    private:
        //std::string param1;
        //std::string param2;
        std::vector<std::any> params;
        std::string result;
        OPERATION_TYPE operation;
        tkrzw::PolyDBM& dbmReference;
};