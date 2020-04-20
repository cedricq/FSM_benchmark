#include "gtest/gtest.h"

#include "sml.hpp"

#include <iostream>
#include <memory>
#include "sml_logging.hpp"

namespace sml = boost::sml;
using namespace sml;


template <class R, class... Ts>
auto call_impl(R (*f)(Ts...)) {
  return [f](Ts... args) { return f(args...); };
}
template <class T, class R, class... Ts>
auto call_impl(T* self, R (T::*f)(Ts...)) {
  return [self, f](Ts... args) { return (self->*f)(args...); };
}
template <class T, class R, class... Ts>
auto call_impl(const T* self, R (T::*f)(Ts...) const) {
  return [self, f](Ts... args) { return (self->*f)(args...); };
}
template <class T, class R, class... Ts>
auto call_impl(const T* self, R (T::*f)(Ts...)) {
  return [self, f](Ts... args) { return (self->*f)(args...); };
}

auto call = [](auto... args) { return call_impl(args...); };


template<typename>
class BiduleT
{
public:
    BiduleT() = default;
    ~BiduleT() = default;
    
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
    fsm_t sm_{logger};
};
using Bidule = BiduleT<int>;
    

void action() {std::cout <<"action" <<std::endl;}
    
    
class Truc
{
public:
    Truc();
    ~Truc() = default;
    
    struct startController{};
    
    // Cannot call a non satic method !! 
    static void action2() {std::cout <<"action2" <<std::endl;}
    
    struct StartUpFSM {
        auto operator()() noexcept {
            using namespace sml;
            return make_transition_table(
                "CalibrationCheck"_s    <= *"Ready"_s           + event<startController> / (call(action), call(Truc::action2))
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
    
    Bidule bidule;
    bidule.sm_.process_event(Bidule::startController());
}


