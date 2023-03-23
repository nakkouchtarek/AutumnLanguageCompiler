#pragma once

#include <iostream>

class Token
{
private:
    std::string type;
    std::string value;
public:
    Token();
    Token(std::string type, std::string value);
    void set_type(std::string);
    void set_value(std::string);
    std::string get_type();
    std::string get_value();
    void present();
};