#ifndef BOOSTSML_SML_2_UML_HPP_
#define BOOSTSML_SML_2_UML_HPP_

#include "sml.hpp"

#include <iostream>
#include <string>
#include <typeinfo>
#include <regex>
#include <fstream>

// This is to output state machine in Plant UML format
template <class T>
void dump_transition(std::ofstream& o) noexcept {
  std::string line;

  auto src_state = std::string{sml::aux::string<typename T::src_state>{}.c_str()};
  auto dst_state = std::string{sml::aux::string<typename T::dst_state>{}.c_str()};
  if (dst_state == "X") {
    dst_state = "[*]";
  }

  if (T::initial) {
      line += "[*] --> ";
      line += src_state;
      line += "\n";
  }

  line += src_state;
  line += " --> ";
  line += dst_state;

  const auto has_event = !sml::aux::is_same<typename T::event, sml::anonymous>::value;
  const auto has_guard = !sml::aux::is_same<typename T::guard, sml::front::always>::value;
  const auto has_action = !sml::aux::is_same<typename T::action, sml::front::none>::value;

  if (has_event || has_guard || has_action) {
      line += " :";
  }

  if (has_event) {
      line += " ";
      line += boost::sml::aux::get_type_name<typename T::event>();
  }

  if (has_guard) {
      line += " [";
      line += boost::sml::aux::get_type_name<typename T::guard::type>();
      line += "]";
  }

  if (has_action) {
      line += " / ";
      line += boost::sml::aux::get_type_name<typename T::action::type>();
  }

  try
  {
      std::smatch m;

      std::regex re1("\\{anonymous\\}::");
      line = std::regex_replace(line, re1, "");
      std::regex re2("/ [a-zA-Z0-9_]*::operator\\(\\)\\(\\) const::<lambda\\((.*)\\)>");
      line = std::regex_replace(line, re2, "");

      std::regex re3("boost[a-zA-Z0-9_:]*<boost[a-zA-Z0-9_:]*<([a-zA-Z0-9_: ]*)>[a-zA-Z0-9_: ]*>");
      std::string state ="";
      std::regex_search (line,m,re3);
      for (auto x : m) state = x;
      line = std::regex_replace(line, re3, state);

      std::regex re4("\\/ [ a-zA-Z0-9_&:<>\\(\\)]*");
      line = std::regex_replace(line, re4, "");

      std::regex re5(" \\[[, a-zA-Z0-9_&:<>\\(\\)]*\\]");
      line = std::regex_replace(line, re5, "");
  }
  catch ( const std::exception& e )
  {
      std::cout<<"regex_error exception: " <<e.what() <<std::endl;
  }
  catch ( ... )
  {
      std::cout<<"regex_error exception: ..." <<std::endl;
  }

  std::cout<<line.c_str() <<std::endl;
  o<<line.c_str() <<"\n";
}

template <template <class...> class T, class... Ts>
void dump_transitions(const T<Ts...>&, std::ofstream& o) noexcept {
  int _[]{0, (dump_transition<Ts>(o), 0)...};
  (void)_;
}

template <class SM>
void dump(const SM&, std::ofstream& o) noexcept {
  std::cout << "@startuml" << std::endl << std::endl;
  dump_transitions(typename SM::transitions{}, o);
  std::cout << std::endl << "@enduml" << std::endl;
}



#endif /* BOOSTSML_SML_2_UML_HPP_ */
