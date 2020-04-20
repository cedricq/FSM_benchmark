#include "gtest/gtest.h"

#include "sml.hpp"

#include <fstream>
#include "sml_logging.hpp"

namespace sml = boost::sml;
using namespace sml;

namespace {
    
class Truc
{
    Truc() = default;
    ~Truc() = default;
    
    struct startController{};
    
    struct StartUpFSM {
        auto operator()() noexcept {
            using namespace sml;
            return make_transition_table(
                "CalibrationCheck"_s    <= *"Ready"_s           + event<startController>
            ) ;
        }
    };


    //sml_wdc_logger::sml_logger logger_;
    //decltype(StartUpFSM()) sm_{};
    //sml::sm<typename decltype(StartUpFSM()), sml::logger<sml_wdc_logger::sml_logger>> sm_{logger_};


    //sml::sm<decltype(), sml::logger<sml_wdc_logger::sml_logger>> sm_{logger_};

    sml::sm<StartUpFSM> sm_{};
}

TEST(SML_FSMInClass, test)
{
  
}



}  // namespace

