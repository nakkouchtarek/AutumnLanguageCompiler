#include <iostream>
#include <token.hpp>

Token::Token(){}

Token::Token(std::string t, std::string v)
{
    value = v;
    type = t;
}

void Token::set_type(std::string t)
{
    type = t;
}

void Token::set_value(std::string v)
{
    value = v;
}

void Token::present()
{
    std::cout << "My type is " << type << " My value is " << value << std::endl;
}

std::string Token::get_type()
{
    return type;
}

std::string Token::get_value()
{
    return value;
}