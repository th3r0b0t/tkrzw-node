#ifndef PROCESSOR_JSFUNC_WRAPPER_HPP
#define PROCESSOR_JSFUNC_WRAPPER_HPP

#include <string>
#include <string_view>
#include <future>
#include <iostream>
#include <tkrzw_dbm.h>
#include <tkrzw_dbm_poly.h>
#include <napi.h>
#include "tsfn_types.hpp"

class processor_jsfunc_wrapper : public tkrzw::DBM::RecordProcessor
{
    public:
        explicit processor_jsfunc_wrapper(TSFN tsfn) : tsfn(tsfn)
        {}

        std::string_view ProcessFull(std::string_view key, std::string_view value) override;
        std::string_view ProcessEmpty(std::string_view key) override;

    private:
        std::string new_value_memory;
        TSFN tsfn;
};


#endif //PROCESSOR_JSFUNC_WRAPPER_HPP