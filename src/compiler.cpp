#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstdio>
#include <algorithm>
#include <assert.h> 
#include <limits>

#include <allocator.hpp>
#include <token.hpp>


std::vector<std::string> nature = {"OPERATOR","VALUE","TYPE","NAME","FUNCTION","PARAMETER"};
std::vector<std::string> control = {"if","for","while","end","endif"};
std::vector<std::string> operators = {"+","=","-","*","/",";","(",")","'","\""};
std::vector<std::string> types = {"int", "string"};
std::vector<std::pair<std::string, int>> functions = {{"write",1},{"read", 2},{"writeln",1},{"exit",1}};

std::vector<Token*> tokens;
std::map<std::string, Token*> mapped_variables;
std::map<std::string, std::string> mapped_functions;

void handle_token(int i);

int token_counter = 0;
int line = 1;

void changeWord(std::string &word1, std::string &word2)
{
    std::string tmp = "";
    int index = -1;

    word1 = word1.substr(1, word1.size() - 2);
    word2 = word2.substr(1, word2.size() - 2);

    for(int i=0;i<word1.size();i++)
    {
        if(word1[i] == word2[0])
        {
            for(int j=i;j<word2.size()+i;j++)
            {
                tmp = tmp + word1[j];
            }

            if(tmp == word2)
            {
                index = i;
                tmp = "";
                break;
            }
        }
    }

    if(index != -1)
    {
        for(int i=0;i<word1.size();i++)
        {
            if(i==index)
            {
                for(int j=0;j<word2.size()-1;j++)
                    i++;
            }
            else
            {
                tmp = tmp + word1[i];
            }
        }

        tmp = '"' + tmp + '"';
        word1 = tmp;
    }
}

void handle_word(std::string &word)
{
    if (word != "")
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

        for(int i=0;i<control.size();i++)
        {
            if ( word == control[i] )
            {
                t1.set_type("CONTROL");
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
            if( tokens.size() > 0 && tokens[tokens.size()-1]->get_type() == "TYPE" ) 
            {
                t1.set_type("NAME");
            }
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
                if( tokens.size() > 0 && tokens[tokens.size()-1]->get_value() == ";" )
                {
                    t1.set_type("NAME");
                }
                else
                {
                    t1.set_type("VALUE");
                }
                
                t1.set_value(word);
            }
            
        }

        Token* t4 = (Token*)allocate(sizeof(Token));
        *t4 = Token(t1);
        tokens.push_back(t4); 

        if(p)
        {
            for(auto token : tmp)
            {
                Token* t5 = (Token*)allocate(sizeof(Token));
                *t5 = Token(token);
                tokens.push_back(t5); 
            }
            
            p=0;
        }
    }
}

void handle_control(int i)
{
    if ( tokens[i]->get_value() == "for" )
    {
        int it;

        if ( tokens[i+3]->get_value() == "inf" )
        {
            it = std::numeric_limits<int>::max();
        }
        else
        {
            it = std::stoi(tokens[i+3]->get_value());
        }

        Token tmp = Token("VALUE", "0", "int");
        mapped_variables[tokens[i+1]->get_value()] = (Token*)allocate(sizeof(Token));
        *mapped_variables[tokens[i+1]->get_value()] = std::move(tmp);

        std::string it_name = tokens[i+1]->get_value();

        i = i + 4;

        int loop_counter = 0;
        int t1 = i;

        for(int j=0;j<it;j++)
        {
            mapped_variables[it_name]->set_value(std::to_string(j));

            do 
            {
                t1++;

                if(tokens[t1]->get_value() == "for")
                {
                    loop_counter++;
                }

                handle_token(t1);

                if ( tokens[t1]->get_value() == "if" )
                {
                    do
                    {
                        t1++;
                    } while ( tokens[t1]->get_value() != "endif" );
                    
                }
                else if(tokens[t1]->get_value() == "end")
                {
                    loop_counter--;
                }
                
            } while ( (loop_counter >= 0) || (tokens[t1]->get_value() != "end") );

            t1 = i;
        }

        do
        {
            if ( tokens[i]->get_value() == "if" )
            {
                do
                {
                    i++;
                } while ( tokens[i]->get_value() != "endif" );
                
            }

            i++;

        } while ( tokens[i]->get_value() != "end" );
        
    }
    else if( tokens[i]->get_value() == "if" )
    {
        std::string cond1, cond2, op;
        int t = i;

        do 
        {
            t++;
            
            if( tokens[t]->get_type() == "VALUE" && tokens[t+1]->get_type() == "OPERATOR" && tokens[t+2]->get_type() == "VALUE" )
            {
                cond1 = tokens[t]->get_value(); 
                cond2 = tokens[t+2]->get_value();

                if (mapped_variables.count(cond1)>0)
                {
                    cond1 = mapped_variables[cond1]->get_value();
                }
                
                if (mapped_variables.count(cond2)>0)
                {
                    cond2 = mapped_variables[cond2]->get_value();
                    
                }

                cond1.erase(remove(cond1.begin(), cond1.end(), '"'), cond1.end());
                cond2.erase(remove(cond2.begin(), cond2.end(), '"'), cond2.end());

                t = t + 3;
            }

        } while ( tokens[t]->get_value() != ";" );

        do 
        {
            t++;

            if(cond1 == cond2)
            {
                handle_token(t);
            }
            
        } while ( tokens[t]->get_value() != "endif" );
        
    }
}

void handle_type(int i)
{
    if ( tokens[i]->get_type() == "TYPE" && tokens[i+1]->get_type() == "NAME" && tokens[i+2]->get_value() == ";" )
    {
        Token tmp;

        if ( tokens[i]->get_value() == "int" )
            tmp = Token("VALUE", "0", "int");
        else if ( tokens[i]->get_value() == "string" )
            tmp = Token("VALUE", " ", "string");

        mapped_variables[tokens[i+1]->get_value()] = (Token*)allocate(sizeof(Token));
        *mapped_variables[tokens[i+1]->get_value()] = std::move(tmp);
    }
    else
    {
        int c = 3;
        Token* last = tokens[i+c];

        last->set_subtype(tokens[i]->get_value());

        if(last->get_subtype() == "int")
        {
            int res, snd;

            if (mapped_variables.count(last->get_value())>0)
            {
                res = std::stoi( mapped_variables[last->get_value()]->get_value() );
            }
            else
            { 
                res = std::stoi( last->get_value() );
            }

            do 
            {
                c++;

                if ( tokens[i+c]->get_type() == "VALUE" )
                {
                    if (mapped_variables.count(tokens[i+c]->get_value())>0)
                    {
                        snd = std::stoi( mapped_variables[tokens[i+c]->get_value()]->get_value() );
                    }
                    else
                    { 
                        snd = std::stoi( tokens[i+c]->get_value() );
                    }

                    if ( tokens[i+c-1]->get_value() == "+" )
                        res = res + snd ;
                    if ( tokens[i+c-1]->get_value() == "-" )
                        res = res - snd ;
                    if ( tokens[i+c-1]->get_value() == "/" )
                        res = res / snd ;
                    if ( tokens[i+c-1]->get_value() == "*" )
                        res = res * snd ;
                }
                    

            } while ( tokens[i+c]->get_value() != ";" );

            Token tmp = Token("VALUE", std::to_string(res), tokens[i]->get_value());
            mapped_variables[tokens[i+1]->get_value()] = (Token*)allocate(sizeof(Token));
            *mapped_variables[tokens[i+1]->get_value()] = std::move(tmp);
        }
        else if(last->get_subtype() == "string")
        {
            std::string res;
            std::string snd;

            if (mapped_variables.count(last->get_value())>0)
            {
                res = mapped_variables[last->get_value()]->get_value();
            }
            else
            { 
                res = tokens[i+c]->get_value();
            }

            do 
            {
                c++;

                if ( tokens[i+c]->get_type() == "VALUE" )
                {
                    if (mapped_variables.count(tokens[i+c]->get_value())>0)
                    {
                        snd = mapped_variables[tokens[i+c]->get_value()]->get_value();
                    }
                    else
                    { 
                        snd = tokens[i+c]->get_value();
                    }

                    if ( tokens[i+c-1]->get_value() == "+" )
                        res.append(snd);
                    else if ( tokens[i+c-1]->get_value() == "-" )
                        changeWord(res, snd);
                }
                    

            } while ( tokens[i+c]->get_value() != ";" );

            Token tmp = Token("VALUE", res, tokens[i]->get_value());
            mapped_variables[tokens[i+1]->get_value()] = (Token*)allocate(sizeof(Token));
            *mapped_variables[tokens[i+1]->get_value()] = std::move(tmp);
        }
    }
}

void handle_func(int i)
{
    if ( tokens[i]->get_value() == "write" )
    {
        if( tokens[i+1]->get_value()[0] != '"' )
        {
            std::string my_str = mapped_variables[tokens[i+1]->get_value()]->get_value();
            my_str.erase(remove(my_str.begin(), my_str.end(), '"'), my_str.end());
            std::cout << my_str;
        }
        else
        {
            std::cout << tokens[i+1]->get_value().substr(1, tokens[i+1]->get_value().length()-2 );
        } 
    }
    else if ( tokens[i]->get_value() == "writeln" )
    {
        if( tokens[i+1]->get_value()[0] != '"' )
        {
            std::string my_str = mapped_variables[tokens[i+1]->get_value()]->get_value();
            my_str.erase(remove(my_str.begin(), my_str.end(), '"'), my_str.end());
            std::cout << my_str << std::endl;
        }
        else
        {
            std::cout << tokens[i+1]->get_value().substr(1, tokens[i+1]->get_value().length()-2 ) << std::endl;
        } 
    }
    else if ( tokens[i]->get_value() == "read" )
    {
        std::string t1;
        std::cout << tokens[i+1]->get_value().substr(1, tokens[i+1]->get_value().length()-2 );
        getline( std::cin, t1 );

        if (mapped_variables.count(tokens[i+2]->get_value())>0)
        {
            mapped_variables[tokens[i+2]->get_value()]->set_value(t1);
        }
        else
        {
            Token tmp = Token("VALUE", tokens[i+2]->get_value(), "string");
            mapped_variables[tokens[i+2]->get_value()] = (Token*)allocate(sizeof(Token));
            *mapped_variables[tokens[i+2]->get_value()] = std::move(tmp);
        }
        
    }
    else if ( tokens[i]->get_value() == "exit" )
    {
        int code = std::stoi(tokens[i+1]->get_value());
        garbage_collector();
        exit(code);
    }
}

void handle_token(int i)
{
    if(tokens[i]->get_value() != "" )
    {
        if ( i > 0 && tokens[i-1]->get_value() == ";" && tokens[i]->get_type() == "NAME" )
        {
            int c = 2;
            Token* last = tokens[i];
            last->set_subtype(mapped_variables[tokens[i]->get_value()]->get_subtype());

            if(last->get_subtype() == "int")
            {
                int res, snd=0;
                
                if (mapped_variables.count(tokens[i+c]->get_value())>0)
                {
                    res = std::stoi( mapped_variables[tokens[i+c]->get_value()]->get_value() );
                }
                else
                { 
                    res = std::stoi( tokens[i+c]->get_value() );
                }
                
                do 
                {
                    c++;

                    if ( tokens[i+c]->get_type() == "VALUE" )
                    {
                        if (mapped_variables.count(tokens[i+c]->get_value())>0)
                        {
                            snd = std::stoi( mapped_variables[tokens[i+c]->get_value()]->get_value() );
                        }
                        else
                        { 
                            snd = std::stoi( tokens[i+c]->get_value() );
                        }

                        if ( tokens[i+c-1]->get_value() == "+" )
                            res = res + snd ;
                        if ( tokens[i+c-1]->get_value() == "-" )
                            res = res - snd ;
                        if ( tokens[i+c-1]->get_value() == "/" )
                            res = res / snd ;
                        if ( tokens[i+c-1]->get_value() == "*" )
                            res = res * snd ;
                    }
                        
                } while ( tokens[i+c]->get_value() != ";" );

                mapped_variables[tokens[i]->get_value()]->set_value(std::to_string(res));
            }
            else if(last->get_subtype() == "string")
            {
                std::string res;
                std::string snd;

                res = tokens[i+c]->get_value();

                do 
                {
                    c++;

                    if ( tokens[i+c]->get_type() == "VALUE" )
                    {
                        if (mapped_variables.count(tokens[i+c]->get_value())>0)
                        {
                            snd = mapped_variables[tokens[i+c]->get_value()]->get_value();
                        }
                        else
                        {
                            snd = tokens[i+c]->get_value();
                        }

                        if ( tokens[i+c-1]->get_value() == "+" )
                            res.append(snd);
                        else if ( tokens[i+c-1]->get_value() == "-" )
                            changeWord(res, snd);
                    }
                        

                } while ( tokens[i+c]->get_value() != ";" );

                mapped_variables[tokens[i+1]->get_value()]->set_value(res);
            }
        }
        else if ( tokens[i]->get_type() == "CONTROL" )
        {
            handle_control(i);
        }
        else if ( tokens[i]->get_type() == "TYPE" )
        {
            handle_type(i);
        }
        else if ( tokens[i]->get_type() == "FUNCTION" )
        {
            handle_func(i);        
        }
    }
}

void show_variables()
{
    for (auto& [k, v] : mapped_variables) {
        std::cout << "name: " << k << " type: " << v->get_type() << " subtype: " << v->get_subtype() << " value: " << v->get_value() << std::endl;
    }
}

void show_tokens()
{
    for(int i=0;i<tokens.size();i++)
        tokens[i]->present();
}

void handle_tokens()
{
    //show_tokens();

    for(int i=0;i<tokens.size();i++)
    {
        handle_token(i); 

        if ( tokens[i]->get_value() == "if" )
        {
            do
            {
                i++;
            } while ( tokens[i]->get_value() != "endif" );
            
        }
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

    int c = 0;

    while (std::getline(file, word))
    {
        for (auto x : word)
        {
            if ( x == '"' && c == 1 )
            {
                tmp = tmp + x;
                handle_word(tmp);
                tmp = "";
                c = 0;
            }
            else if ( x == ';' )
            {
                handle_word(tmp);
                tmp = ";";
                handle_word(tmp);
                tmp = "";
            }
            else if ( x == '"' )
            {
                tmp = tmp + x;
                c++;
            }
            else if (  x == ' ' && (tmp.find('"') == std::string::npos) )
            {
                handle_word(tmp);
                tmp = "";
            }
            else if ( check_func(tmp) )
            {
                handle_word(word);
                tmp = "";
                break;
            }
            else
            {
                tmp = tmp + x;
            }   
        }

        line++;

    }

    handle_tokens();

    //show_variables();
}

int main()
{
    readFile();

    //show_memory();
    //show_memory_blocks();

    garbage_collector();

    return 0;
}