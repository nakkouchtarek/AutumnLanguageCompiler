#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstdio>
#include <assert.h> 

#include <allocator.hpp>
#include <token.hpp>

std::vector<std::string> nature = {"OPERATOR","VALUE","TYPE","NAME","FUNCTION","PARAMETER"};
std::vector<std::string> operators = {"+","=","-","*","/",";","(",")","'","\""};
std::vector<std::string> types = {"int", "string"};
std::vector<std::pair<std::string, int>> functions = {{"write",1},{"read", 2},{"writeln",1}};

std::vector<Token*> tokens;
std::map<std::string, Token*> mapped_variables;
std::map<std::string, std::string> mapped_functions;

int token_counter = 0;

void handle_word(std::string &word, int line)
{
    Token t1 = Token("null", word);
    int p = 0;
    std::vector<Token> tmp;
    
    for(int i=0;i<operators.size();i++)
    {
        if ( word == operators[i] )
        {
            t1.set_type("OPERATOR");
            t1.set_value(word);
        }
    }

    if( t1.get_type() == "null" )
    {
        for(int i=0;i<types.size();i++)
        {
            if ( word == types[i] )
            {
                t1.set_type("TYPE");
                t1.set_value(word);
            }
        }
    }

    if( t1.get_type() == "null" )
    {
        if(tokens[tokens.size()-1]->get_type() == "TYPE")
            t1.set_type("NAME");
        else
        {
            for(int i=0;i<functions.size();i++)
            {
                if (word.find(functions[i].first) != std::string::npos) {
                    t1.set_type("FUNCTION");
                    t1.set_value(functions[i].first);
                    
                    int f1 = word.find("(")+1;
                    int f2 = word.find(")");
                    word = word.substr(0, f2);

                    std::string pm;
                    int c=0;
                    int p_c = 1;

                    for( auto x : word.substr(f1, f2) )
                    {
                        if (word.substr(f1, f2).find("\"") != std::string::npos)
                        {
                            if ( x == '"' )
                            {
                                pm = pm + x;
                                c++;
                            }
                            else if ( c == 1 )
                                pm = pm + x;
                            else if ( x != ',' && x != ' ' )
                            {
                                pm = pm + x;
                            }

                            if ( c == 2 )
                            {
                                if( p_c <= functions[i].second )
                                {
                                    tmp.push_back(Token("PARAMETER",pm));
                                    p_c++;
                                    pm = "";
                                    c = 0;
                                }
                                else
                                {
                                    std::cout << "Invalid number of arguments for function: " << functions[i].first << " at line " << line << std::endl;
                                    exit(-1);
                                }
                            }
                        }
                        else
                        {
                            pm = pm + x;
                        }                        
                    }

                    if(pm != "" || tmp.empty() )
                        tmp.push_back(Token("PARAMETER",pm));
                    

                    p=1;
                }
            }
        }

        if ( t1.get_type() == "null" )
        {
            t1.set_type("VALUE");
            t1.set_value(word);
        }
        
    }

    Token* t4 = (Token*)allocate(sizeof(Token));
    *t4 = std::move(t1);
    tokens.push_back(t4); 

    if(p)
    {
        for(auto token : tmp)
        {
            Token* t5 = (Token*)allocate(sizeof(Token));
            *t5 = std::move(token);
            tokens.push_back(t5); 
        }
        
        p=0;
    }
}

void handle_tokens()
{
    for(int i=0;i<tokens.size();i++)
    {
        //tokens[i].present();

        if ( tokens[i]->get_type() == "TYPE" && tokens[i+1]->get_type() == "NAME" && tokens[i+2]->get_type() == "OPERATOR")
        {
            int c = 3;

            Token* last = tokens[i+c];

            int res = std::stoi( last->get_value() );
            
            do 
            {
                c++;

                if ( tokens[i+c]->get_type() == "VALUE" )
                {
                    if ( tokens[i+c-1]->get_value() == "+" )
                        res = res + std::stoi( tokens[i+c]->get_value() );
                    if ( tokens[i+c-1]->get_value() == "-" )
                        res = res - std::stoi( tokens[i+c]->get_value() );
                    if ( tokens[i+c-1]->get_value() == "/" )
                        res = res / std::stoi( tokens[i+c]->get_value() );
                    if ( tokens[i+c-1]->get_value() == "*" )
                        res = res * std::stoi( tokens[i+c]->get_value() );
                }
                    

            } while ( tokens[i+c]->get_value() != ";" );


            Token tmp = Token("VALUE", std::to_string(res));
            mapped_variables[tokens[i+1]->get_value()] = (Token*)allocate(sizeof(Token));
            *mapped_variables[tokens[i+1]->get_value()] = std::move(tmp);
        }
        
        if ( tokens[i]->get_type() == "FUNCTION" && tokens[i+1]->get_type() == "PARAMETER" )
        {
            if ( tokens[i]->get_value() == "write" )
            {
                
                if( tokens[i+1]->get_value()[0] != '"' )
                {
                    std::cout << mapped_variables[tokens[i+1]->get_value()]->get_value();
                    //std::cout << "print(" <<  tokens[i+1].get_value() << ")" << std::endl;
                }
                else
                {
                    std::cout << tokens[i+1]->get_value().substr(1, tokens[i+1]->get_value().length()-2 );
                    //std::cout << "print(" <<  tokens[i+1].get_value() << ")" << std::endl;
                } 
            }
            else if ( tokens[i]->get_value() == "writeln" )
            {
                if( tokens[i+1]->get_value()[0] != '"' )
                {
                    std::cout << mapped_variables[tokens[i+1]->get_value()]->get_value() << std::endl;
                    //std::cout << "print(" <<  tokens[i+1].get_value() << ")" << std::endl;
                }
                else
                {
                    std::cout << tokens[i+1]->get_value().substr(1, tokens[i+1]->get_value().length()-2 ) << std::endl;
                    //std::cout << "print(" <<  tokens[i+1].get_value() << ")" << std::endl;
                } 
            }
            else if ( tokens[i]->get_value() == "read" )
            {
                std::string tmp;
                std::cout << tokens[i+1]->get_value().substr(1, tokens[i+1]->get_value().length()-2 );
                getline( std::cin, tmp );
                mapped_variables[tokens[i+2]->get_value()]->set_value(tmp);
            }
        }
    }
    
   for (auto& [k, v] : mapped_variables) {
        std::cout << "name: " << k << " value: " << v->get_value() << std::endl;
    }
}

bool check_func(std::string &word)
{
    for(auto func : functions)
    {
        if ( word == func.first )
            return true;
    }

    return false;
}

void readFile()
{
    std::ifstream file;

    file.open ("../file.aut");

    if (!file.is_open()){
        std::cout << "Error while opening file" << std::endl;
        exit(-1);
    };

    std::string word;
    std::string target;
    std::string tmp;

    int counter = 1;

    while (std::getline(file, word))
    {
        for (auto x : word)
        {
            if ( x == ';' )
            {
                handle_word(tmp, counter);
                tmp = ";";
                handle_word(tmp, counter);
                tmp = "";
            }
            else if ( x == ' ' )
            {
                handle_word(tmp,counter);
                tmp = "";
            }
            else if ( check_func(tmp) )
            {
                handle_word(word,counter);
                tmp = "";
                break;
            }
            else
            {
                tmp = tmp + x;
            }   
        }

        counter++;

    }

    handle_tokens();
}

int main()
{
    /*
    int* t = (int*)allocate(sizeof(int));
    int* t2 = (int*)allocate(sizeof(int));
    int* t3 = (int*)allocate(sizeof(int));
    String* x = (String*)allocate(sizeof(char)*50);
    *t = 124213;
    *t2 = 403211;
    *t3 = 321312;
    *x = "this is a random string testing buds test";
    std::cout << "My sentence is: " << *x << std::endl;
    *x = "this is a random strinewqewqewqt";
    std::cout << "My sentence is: " << *x << std::endl;
    */

    readFile();

    //show_memory();
    //show_memory_blocks();

    garbage_collector();

    return 0;
}