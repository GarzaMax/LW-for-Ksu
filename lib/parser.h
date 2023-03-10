#pragma once

#include <filesystem>
#include <istream>
#include <string>
#include <map>
#include <regex>

namespace omfl {

class OMFLParser {
 private:
  std::map<std::string, std::string> section_key_n_value_;
  std::string current_section_;
  std::string value_getter_;
  bool is_valid_ = true;

 public:
  void SetAsInvalid() {
    is_valid_ = false;
  }

  bool valid();

  void SetCurrentSection(std::string current_section);
  void AddNewKeyNValue(const std::string& key, const std::string& value);
  OMFLParser& Get(std::string string);
  bool IsInt();
  int AsInt();
  int AsIntOrDefault(int i);
  bool IsFloat();
  float AsFloat();
  float AsFloatOrDefault(double d);
  bool IsString();
  std::string AsString();
  bool IsArray();
  bool AsBool();
  std::string AsStringOrDefault(std::string string);

};


}// namespace
omfl::OMFLParser parse(std::string str);