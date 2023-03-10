#include "parser.h"
#include <utility>

void SubdivideArrayElements(std::vector<std::string>& container, std::string values) {
    if (std::count(values.begin(), values.end(), '[') != std::count(values.begin(), values.end(), ']')) {
        container.push_back("Fuckup");
        return;
    }
    std::regex array_format_validator("^\\[{1}.*\\]{1}$");
    std::string correction = regex_replace(values, std::regex("s/^\\s+|\\s+$|\\s+(?=\\s)//g"), "1$");
    if (std::regex_match(correction, array_format_validator))
        correction = correction.substr(1, correction.length() - 2);
    std::stringstream ss(correction);
    std::string token;
    while (std::getline(ss, token, ',')) {
        if (std::regex_match(token, array_format_validator)) {
            SubdivideArrayElements(container, token);
        }
        token = regex_replace(token, std::regex("\\[|\\]"), "");
        token = regex_replace(token, std::regex("s/^\\s+|\\s+$|\\s+(?=\\s)//g"), "1$");

        if (token[0] == ' ') {
            token = token.substr(1, token.length() - 1);
        }
        if (token[token.length() - 1] == ' ') {
            token = token.substr(0, token.length() - 1);
        }
        container.push_back(token);
    }
}

void omfl::OMFLParser::SetCurrentSection(std::string current_section) {
    current_section_ = std::move(current_section);
}

void omfl::OMFLParser::AddNewKeyNValue(const std::string& key, const std::string& value) {
    std::string tmp = current_section_.empty() ? key : '.' + key;
    std::map<std::string, std::string>::iterator iter = section_key_n_value_.begin();
    while (iter != section_key_n_value_.end()) {
        if (current_section_+ tmp == iter->first) {
            SetAsInvalid();
        }
        iter++;
    }
    section_key_n_value_.insert(std::make_pair(current_section_ + tmp, value));
}

bool omfl::OMFLParser::valid() {
    return is_valid_;
}

omfl::OMFLParser& omfl::OMFLParser::Get(std::string string) {
    value_getter_.empty() ? value_getter_ += string : value_getter_ += '.' + string;
    return *this;
}

bool omfl::OMFLParser::IsInt() {
    std::regex int_validator("^[+-]?\\d+$");
    bool res = std::regex_match(section_key_n_value_[value_getter_], int_validator);
    value_getter_ = "";
    return res;
}

int omfl::OMFLParser::AsInt() {
    int result = std::stoi(section_key_n_value_[value_getter_]);
    value_getter_ = "";
    return result;
}

int omfl::OMFLParser::AsIntOrDefault(int i) {
    std::regex int_validator("^[+-]?\\d+$");
    int res = std::regex_match(value_getter_, int_validator) ? std::stoi(section_key_n_value_[value_getter_]) : i;
    value_getter_ = "";
    return res;
}

bool omfl::OMFLParser::IsFloat() {
    std::regex float_validator("^[+-]?\\d+[.]?\\d+$");
    bool res = std::regex_match(section_key_n_value_[value_getter_], float_validator);
    value_getter_ = "";
    return res;
}

float omfl::OMFLParser::AsFloat() {
    float res = std::stof(section_key_n_value_[value_getter_]);
    value_getter_ = "";
    return res;
}

float omfl::OMFLParser::AsFloatOrDefault(double d) {
    std::regex float_validator("^[+-]?\\d+[.]?\\d+$");
    float res = std::regex_match(section_key_n_value_[value_getter_], float_validator) ? std::stof(section_key_n_value_[value_getter_]) : d;
    value_getter_ = "";
    return res;
}

bool omfl::OMFLParser::IsString() {
    std::regex string_validator("^[\"]{1}[^\"]*[\"]{1}$");
    bool res =  std::regex_match(section_key_n_value_[value_getter_], string_validator);
    value_getter_ = "";
    return res;
}

std::string omfl::OMFLParser::AsString() {
    std::string res = regex_replace(section_key_n_value_[value_getter_],std::regex("\""), "" );
    value_getter_ = "";
    return res;
}

bool omfl::OMFLParser::IsArray() {
    std::regex array_format_validator("^\\[{1}.*\\]{1}$");
    bool res = std::regex_match(section_key_n_value_[value_getter_], array_format_validator);
}

bool omfl::OMFLParser::AsBool() {
    bool res = section_key_n_value_[value_getter_] == "true" || section_key_n_value_[value_getter_] == "fasle";
    value_getter_ = "";
    return res;
}

std::string omfl::OMFLParser::AsStringOrDefault(std::string string) {
    std::string res = IsString() ? section_key_n_value_[value_getter_] : string;
    value_getter_ = "";
    return res;
}




omfl::OMFLParser parse(std::string str) {
    omfl::OMFLParser newParser;

    std::vector<std::string> tokens;

    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, '\n')) {
        tokens.push_back(token);
    }

    for (int i = 0; i < tokens.size(); i++) {
        std::string current_str = regex_replace(tokens[i], std::regex("^ +| +$|( ) +"),
                                                "$1"); // Сокращаю все пробелы до одного через регулярки
        if (current_str.find('#') != std::string::npos)
            current_str.erase(current_str.find('#'));                  // Игнорирование комментария;

        if (current_str.find('=') == std::string::npos) {                // Валидиация секции, т.к. нет знака равно
            if (current_str.find('[') != std::string::npos
                && current_str.find(']') != std::string::npos) {           // Если секция указана корректно
                std::string section;
                section = current_str.substr(current_str.find('[') + 1,
                                             current_str.find(']') - current_str.find('[') - 1);
                if (section.empty()) {                                         // Если секция пустая, то весь файл невалиден
                    newParser.SetAsInvalid();
                    break;
                } else {                                                       // Если секция не пустая, то необходимо проверить корректность задания подсекций
                    if (section[0] == '.' || section[section.length() - 1] == '.') {
                        newParser.SetAsInvalid();
                        break;
                    } else {
                        newParser.SetCurrentSection(section);
                    }
                }
            } else {                                                        // Если не нашлось скобок, то либо они заданы неправильно, либо строка была забита комментарием
                if (current_str.empty()) {                                    // Если строка оказалась пустой, то строка была комментарием
                    continue;
                } else {                                                      // Если не пустая, то либо невалидная секция, либо просто буквы в строке
                    newParser.SetAsInvalid();
                }
            }
        } else {                                                                                            // Если знак равно всё же присутствует
            std::string key;
            std::string value;

            std::regex key_validator("[A-Za-z0-9-_$]*");

            key = current_str.substr(0, current_str.find('='));
            value = current_str.substr(current_str.find('=') + 1, current_str.length() - current_str.find('=') - 1);
            if (key[0] == ' ') {
                key = key.substr(1, key.length() - 1);
            }
            if (key[key.length() - 1] == ' ') {
                key = key.substr(0, key.length() - 1);
            }
            if (value[0] == ' ') {
                value = value.substr(1, value.length() - 1);
            }
            if (value[value.length() - 1] == ' ') {
                value = value.substr(0, value.length() - 1);
            }

            if (!std::regex_match(key.data(), key_validator) || key.empty()) {
                newParser.SetAsInvalid();
                break;
            }
            if (value.empty()) {
                newParser.SetAsInvalid();
                break;
            }

            std::regex int_validator("^[+-]?\\d+$");
            std::regex string_validator("^[\"]{1}[^\"]*[\"]{1}$");
            std::regex float_validator("^[+-]?\\d+[.]?\\d+$");
            std::regex array_format_validator("^\\[{1}.*\\]{1}$");


            if (value == "true" || value == "false") {
                newParser.AddNewKeyNValue(key, value);
            } else if (std::regex_match(value, int_validator)) {
                newParser.AddNewKeyNValue(key, value);
            } else if (std::regex_match(value, string_validator)) {
                newParser.AddNewKeyNValue(key, value);
            } else if (std::regex_match(value, float_validator)) {
                newParser.AddNewKeyNValue(key, value);
            } else if (std::regex_match(value, array_format_validator)) {
                std::vector<std::string> container;
                SubdivideArrayElements(container, value);
                if (container[0] == "Fuckup") {
                    newParser.SetAsInvalid();
                }
                for (int i = 0; i < container.size(); i++) {
                    if (!(std::regex_match(container[i], int_validator) ||
                          std::regex_match(container[i], string_validator) ||
                          std::regex_match(container[i], float_validator) ||
                          container[i].find("true") != std::string::npos ||
                          container[i].find("false") != std::string::npos)) {
                        newParser.SetAsInvalid();
                    }
                }
            } else {
                newParser.SetAsInvalid();
            }
        }
    }

    return newParser;
}