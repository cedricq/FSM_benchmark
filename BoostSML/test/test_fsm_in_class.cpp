#include "gtest/gtest.h"

#include "sml.hpp"

#include <fstream>
#include <memory>
#include "sml_logging.hpp"

namespace sml = boost::sml;
using namespace sml;

namespace {
    
class Truc
{
public:
    Truc();
    
    ~Truc() = default;
    struct startController{};
    
    struct StartUpFSM {
        auto operator()() noexcept {
            using namespace sml;
            return make_transition_table(
                "CalibrationCheck"_s    <= *"Ready"_s           + event<startController>
            );
        }
    };
    
    my_logger logger;
    using fsm_t = sml::sm<StartUpFSM, sml::logger<my_logger>>;
    std::unique_ptr<fsm_t> sm_;
};

Truc::Truc()
{
    sm_ =  std::make_unique<fsm_t>(logger);
}

TEST(SML_FSMInClass, test)
{
    Truc truc;
    truc.sm_->process_event(Truc::startController());
}

}  // namespace

