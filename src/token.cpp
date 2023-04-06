#include <iostream>
#include <token.hpp>

Token::Token(){}

Token::Token(const Token &t)
{
    type = t.type;
    value = t.value;
    subtype = t.subtype;
};

Token::Token(std::string t, std::string v)
{
    value = v;
    type = t;
}

Token::Token(std::string t, std::string v, std::string s)
{
    value = v;
    type = t;
    subtype = s;
}

void Token::set_type(std::string t)
{
    type = t;
}

void Token::set_value(std::string v)
{
    value = v;
}

void Token::set_subtype(std::string s)
{
    subtype = s;
}

void Token::present()
{
    std::cout << "My type is " << type << " My value is " << value << " My subtype is " << subtype << std::endl;
}

std::string Token::get_type()
{
    return type;
}

std::string Token::get_value()
{
    return value;
}

std::string Token::get_subtype()
{
    return subtype;
}