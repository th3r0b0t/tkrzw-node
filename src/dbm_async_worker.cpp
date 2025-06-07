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
    if(operation == DBM_SET)
    {
        //if(params.size() == 2)
        //{
            tkrzw::Status set_status = dbmReference->get().Set( std::any_cast<std::string>(params[0]), std::any_cast<std::string>(params[1]) );
            
            if( set_status != tkrzw::Status::SUCCESS)
            {
                //Napi::TypeError::New(env, set_status.GetMessage().c_str()).ThrowAsJavaScriptException();
                SetError("Couldn't set!");
            }
        //}
    }
    if(operation == DBM_APPEND)
    {
        tkrzw::Status append_status = dbmReference->get().Append( std::any_cast<std::string>(params[0]), std::any_cast<std::string>(params[1]), std::any_cast<std::string>(params[2]) );
        
        if( append_status != tkrzw::Status::SUCCESS)
        {
            SetError("Couldn't append!");
        }
    }
    else if(operation == DBM_GET_SIMPLE)
    {
        //if(params.size() == 2)
        //{
            any_result = dbmReference->get().GetSimple( std::any_cast<std::string>(params[0]), std::any_cast<std::string>(params[1]) );
            if(std::any_cast<std::string>(any_result) == std::any_cast<std::string>(params[1]))
            {
                SetError("Key \"" + std::any_cast<std::string>(params[0]) + "\" not found!");
            }
        //}
    }
    else if(operation == DBM_SHOULD_BE_REBUILT)
    {
        bool shouldBeRebuilt_result = false;
        tkrzw::Status shouldBeRebuilt_status = dbmReference->get().ShouldBeRebuilt(&shouldBeRebuilt_result);
        if( shouldBeRebuilt_status != tkrzw::Status::SUCCESS)
        {
            SetError("(shouldBeRebuilt) Error checking database status!");
        }
        else if(shouldBeRebuilt_result == false)
        {
            SetError("No need to rebuild the database just yet!");
        }
    }
    else if(operation == DBM_REBUILD)
    {
        //if(params.size() == 1)
        //{
            tkrzw::Status rebuild_status = dbmReference->get().RebuildAdvanced( std::any_cast<std::map<std::string,std::string>>(params[0]) );
            if( rebuild_status != tkrzw::Status::SUCCESS)
            {
                SetError("[" + std::to_string(rebuild_status.GetCode()) + "]: " + rebuild_status.GetMessage());
            }
        //}
    }
    else if(operation == DBM_SYNC)
    {
        tkrzw::Status sync_status = dbmReference->get().Synchronize( std::any_cast<bool>(params[0]) );
        if( sync_status != tkrzw::Status::SUCCESS)
        {
            SetError("[" + std::to_string(sync_status.GetCode()) + "]: " + sync_status.GetMessage());
        }
    }
    else if(operation == DBM_PROCESS)
    {
        std::string key = std::any_cast<std::string>(params[0]);
        bool writable = std::any_cast<bool>(params[1]);
        TSFN tsfn = std::any_cast<TSFN>(params[2]);

        processor_jsfunc_wrapper* processorWrapper = new processor_jsfunc_wrapper(tsfn);
        tkrzw::Status process_status = dbmReference->get().Process(key, processorWrapper, writable);

        tsfn.Release();
    }
    //===============================================INDEX_METHODS
    else if(operation == INDEX_ADD)
    {
        tkrzw::Status add_status = indexReference->get().Add( std::any_cast<std::string>(params[0]), std::any_cast<std::string>(params[1]) );
        if( add_status != tkrzw::Status::SUCCESS)
        {
            SetError("[" + std::to_string(add_status.GetCode()) + "]: " + add_status.GetMessage());
        }
    }
    else if(operation == INDEX_GET_VALUES)
    {
        any_result = indexReference->get().GetValues( std::any_cast<std::string>(params[0]), std::any_cast<std::size_t>(params[1]) );
    }
    else if(operation == INDEX_CHECK)
    {
        bool Check_status = indexReference->get().Check( std::any_cast<std::string>(params[0]), std::any_cast<std::string>(params[1]) );
        if( !Check_status )
        {
            SetError("[\"" + std::any_cast<std::string>(params[0]) + "\": \"" + std::any_cast<std::string>(params[1]) + "\"] doesn't exist!");
        }
    }
    else if(operation == INDEX_REMOVE)
    {
        tkrzw::Status remove_status = indexReference->get().Remove( std::any_cast<std::string>(params[0]), std::any_cast<std::string>(params[1]) );
        if( remove_status != tkrzw::Status::SUCCESS)
        {
            SetError("[" + std::to_string(remove_status.GetCode()) + "]: " + remove_status.GetMessage());
        }
    }
    else if(operation == INDEX_SHOULD_BE_REBUILT)
    {
        bool shouldBeRebuilt_result = false;
        tkrzw::Status shouldBeRebuilt_status = indexReference->get().GetInternalDBM()->ShouldBeRebuilt( &shouldBeRebuilt_result );
        if( shouldBeRebuilt_status != tkrzw::Status::SUCCESS)
        {
            SetError("(shouldBeRebuilt) Error checking index status!");
        }
        else if(shouldBeRebuilt_result == false)
        {
            SetError("No need to rebuild the index just yet!");
        }
    }
    else if(operation == INDEX_REBUILD)
    {
        tkrzw::Status rebuild_status = indexReference->get().Rebuild();
        if( rebuild_status != tkrzw::Status::SUCCESS)
        {
            SetError("[" + std::to_string(rebuild_status.GetCode()) + "]: " + rebuild_status.GetMessage());
        }
    }
    else if(operation == INDEX_SYNC)
    {
        tkrzw::Status sync_status = indexReference->get().Synchronize( std::any_cast<bool>(params[0]) );
        if( sync_status != tkrzw::Status::SUCCESS)
        {
            SetError("(sync) Error dynchronizing index!");
        }
    }
    else if(operation == INDEX_MAKE_JUMP_ITERATOR)
    {
        std::string partialKey = std::any_cast<std::string>(params[0]);
        tkrzw::PolyIndex::Iterator* jump_iter = std::any_cast<tkrzw::PolyIndex::Iterator*>(params[1]);
        jump_iter->Jump(partialKey);    //`tkrzw::PolyIndex::Iterator::Jump` is `void`
    }
    else if(operation == INDEX_GET_ITERATOR_VALUE)
    {
        tkrzw::PolyIndex::Iterator* jump_iter = std::any_cast<tkrzw::PolyIndex::Iterator*>(params[0]);
        std::pair<std::string, std::string> key_value;
        //`tkrzw::PolyIndex::Iterator::Get` returns `true` or `false`!
        if( jump_iter->Get(&key_value.first, &key_value.second) ) { any_result = key_value; }
        else { SetError("(getIteratorValue) No record to fetch!"); }
    }
    else if(operation == INDEX_CONTINUE_ITERATION)
    {
        tkrzw::PolyIndex::Iterator* jump_iter = std::any_cast<tkrzw::PolyIndex::Iterator*>(params[0]);
        jump_iter->Next();  //`tkrzw::PolyIndex::Iterator::Next` is `void`
    }
}

void dbmAsyncWorker::OnOK()
{
    if(operation == INDEX_GET_VALUES)
    {
        std::vector<std::string>& casted_vector = std::any_cast<std::vector<std::string>&>(any_result);
        Napi::Array res_arr = Napi::Array::New(Env(), casted_vector.size());
        size_t i = 0;
        for (auto &&record : casted_vector)
        {
            res_arr[i++] = record;
        }
        deferred_promise.Resolve( res_arr );
    }
    else if(operation == DBM_GET_SIMPLE)
    {
        deferred_promise.Resolve( Napi::String::New( Env(), std::any_cast<std::string>(any_result) ) );
    }
    else if(operation == INDEX_GET_ITERATOR_VALUE)
    {
        std::pair<std::string, std::string>& casted_key_value = std::any_cast<std::pair<std::string, std::string>&>(any_result);
        Napi::Object key_value_obj = Napi::Object::New(Env());
        key_value_obj.Set("key", casted_key_value.first);
        key_value_obj.Set("value", casted_key_value.second);
        deferred_promise.Resolve( key_value_obj );
    }
    else { deferred_promise.Resolve( Env().Undefined() ); }
}

void dbmAsyncWorker::OnError(const Napi::Error& err)
{
    deferred_promise.Reject(err.Value());
}