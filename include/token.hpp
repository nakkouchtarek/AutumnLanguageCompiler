#pragma once

#include <iostream>

class Token
{
private:
    std::string type;
    std::string subtype = "null";
    std::string value;
public:
    Token();
    Token(const Token& t);
    Token(std::string type, std::string value);
    Token(std::string type, std::string value, std::string subtype);
    void set_type(std::string);
    void set_value(std::string);
    void set_subtype(std::string);
    std::string get_type();
    std::string get_value();
    std::string get_subtype();
    void present();
};