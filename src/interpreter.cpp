#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstdio>
#include <algorithm>
#include <assert.h> 
#include <limits>

#include <token.hpp>

std::vector<std::string> nature = {"OPERATOR","VALUE","TYPE","NAME","FUNCTION","PARAMETER"};
std::vector<std::string> control = {"if","for","end","endif","func","endfunc"};
std::vector<std::string> operators = {"+","=","-","*","/",";","(",")","'","\""};
std::vector<std::string> types = {"int", "string"};
// vector of function pairs ( function name, number of args )
std::vector<std::pair<std::string, int>> functions = {{"write",1},{"read", 3},{"writeln",1},{"exit",1},{"system",1}};

std::vector<Token> tokens;
std::map<std::string, Token> mapped_variables;
std::map<std::string, std::string> mapped_functions;

int token_counter = 0;
int line = 1;

void handle_token(int i);
void substringWord(std::string &word1, std::string &word2);
void tokenize(std::string &word);
void handle_control_statements_statements(int i);
void handle_type(int i);
void handle_func(int i);
void handle_token(int i);
void show_variables();
void show_tokens();
bool check_func(std::string &word);

void show_variables()
{
    for (auto& [k, v] : mapped_variables) {
        std::cout << "name: " << k << " type: " << v.get_type() << " subtype: " << v.get_subtype() << " value: " << v.get_value() << std::endl;
    }
}

void show_tokens()
{
    for(int i=0;i<tokens.size();i++)
        tokens[i].present();
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

void substringWord(std::string &word1, std::string &word2)
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

bool checkType(std::string &word, std::vector<std::string> &param)
{
    for(int i=0;i<param.size();i++)
    {
        if ( word == param[i] )
        {
            return true;
        }
    }

    return false;
}

void tokenize(std::string &word)
{
    // check word in not empty
    if (word != "")
    {
        // create initial token with word as its value but no type
        Token parent_token = Token("null", word);

        // secondary token
        Token child_token = Token("null","null");

        // temporary token
        std::vector<Token> token_queue;
        
        // check if function has parameters
        bool has_parameters = false;

        // whether im making a new function or not
        bool create=false;
        
        // check whether its an operator
        if (checkType(word, operators))
        {
            parent_token.set_type("OPERATOR");
            parent_token.set_value(word);
        }

        // check whether its a control statetement
        if (checkType(word, control))
        {
            parent_token.set_type("CONTROL");
            parent_token.set_value(word);
        }

        // check whether its type statement
        if (checkType(word, types))
        {
            parent_token.set_type("TYPE");
            parent_token.set_value(word);
        }

        // check whether we are defining a new function
        if ( word.substr(0,4) == "func" )
        {
            parent_token.set_type("CONTROL");
            parent_token.set_value("func");

            // set flag create to true
            create=true;

            // get function name
            word = word.substr(5, word.size());
            std::string name = word.substr(0, word.find('('));
            
            // set second token for function name
            child_token.set_type("FUNCTION");
            child_token.set_value(name);

            int i ,param_counter = 0;

            std::string args = word.substr(word.find('('), word.size());
            std::string param_buffer, param_type;

            if( args.substr(1, args.size()-2) != "" )
            {
                do
                {
                    if ( args[i] == ',' || args[i] == ')' )
                    {
                        // add one to parameter counter
                        param_counter++;

                        // get type of parameter
                        param_type = param_buffer.substr(1, param_buffer.find(' '));
                        token_queue.push_back(Token("TYPE",param_type));

                        // get name of parameter
                        param_buffer = param_buffer.substr(param_buffer.find(' ')-1, param_buffer.size());
                        param_buffer.erase(remove(param_buffer.begin(), param_buffer.end(), ' '), param_buffer.end());
                        token_queue.push_back(Token("PARAMETER",param_buffer.substr(1,param_buffer.size())));

                        // add our parameter to our variables
                        mapped_variables[param_buffer.substr(1,param_buffer.size())] = Token("VALUE", "null", param_type);

                        // clear buffer
                        param_buffer.clear();
                    }

                    // go to next letter
                    param_buffer += args[i++];

                } while (args[i]);
            }
            
            if (param_counter > 0) has_parameters = true;

            // add our function to our function vector
            functions.push_back({name, param_counter});
        }

        if( parent_token.get_type() == "null" )
        {
            // check whether our past token if its exists was a type token so we set our current to name type
            if( tokens.size() > 0 && tokens[tokens.size()-1].get_type() == "TYPE" ) 
            {
                parent_token.set_type("NAME");
            }
            else
            {
                // check if our word is a pre-existing function

                for(int i=0;i<functions.size();i++)
                {
                    if (word.find(functions[i].first) != std::string::npos) {
                        
                        // set our parent token to function type and give it its name
                        parent_token.set_type("FUNCTION");
                        parent_token.set_value(functions[i].first);

                        // set our parameters flag
                        if(functions[i].second>0) has_parameters = true;
                        
                        // start and end of args
                        int start_param = word.find("(")+1;
                        int end_param = word.find(")");

                        // remove last two chars so last paranthesis and colon
                        word = word.substr(0, end_param);

                        std::string word_buffer;
                        int parenthesis_counter, parameter_counter = 0;

                        for( auto x : word.substr(start_param, end_param) )
                        {
                            if (word.substr(start_param, end_param).find("\"") != std::string::npos)
                            {
                                // append letter to our buffer
                                if ( parenthesis_counter == 1 || (x != ',' && x != ' ') || x == '"')
                                {
                                    word_buffer = word_buffer + x;
                                }
                                // completed parameter
                                else if(( x == ',' && word_buffer != "" ) || ( parenthesis_counter == 2 ))
                                {
                                    token_queue.push_back(Token("PARAMETER",word_buffer));
                                    parameter_counter++;

                                    word_buffer.clear();

                                    parenthesis_counter = 0;
                                }

                                // if we have new double quote, add 
                                if (  x == '"' ) parenthesis_counter++;
                            }
                            else
                            {
                                word_buffer = word_buffer + x;
                            }                        
                        }

                        // check for leftover parameter
                        if(word_buffer != "" || token_queue.empty() ) token_queue.push_back(Token("PARAMETER",word_buffer));
                        
                    }
                }
            }

            if ( parent_token.get_type() == "null" )
            {
                if( tokens.size() > 0 && tokens[tokens.size()-1].get_value() == ";" )
                {
                    parent_token.set_type("NAME");
                }
                else
                {
                    parent_token.set_type("VALUE");
                }
                
                parent_token.set_value(word);
            }
            
        }

        // Final step, pushing our tokens to our token vector

        tokens.push_back(parent_token);

        if(child_token.get_type() != "null")
        {
            tokens.push_back(Token(child_token)); 
        }

        if(has_parameters)
        {
            for(auto &token : token_queue)
            {
                tokens.push_back(Token(token)); 
            }
        }

        if( create && tokens.size() > 2 && tokens[tokens.size()-2].get_type() == "PARAMETER" && tokens[tokens.size()-1].get_type() != "PARAMETER" )
        {
            tokens.push_back(Token("OPERATOR",";"));
        }
    }
}

void handle_control_statements(int i)
{
    if ( tokens[i].get_value() == "for" )
    {
        // set iterator in for loop
        int it;

        // set to max INT for inf
        if ( tokens[i+3].get_value() == "inf" )
        {
            it = std::numeric_limits<int>::max();
        }
        else
        {
            // check if its a preexisting variable
            if ( mapped_variables.count(tokens[i+3].get_value()) > 0 )
            {
                it = std::stoi(mapped_variables[tokens[i+3].get_value()].get_value()) - 1;
            }
            // else get value of actual number specified
            else
            {
                it = std::stoi(tokens[i+3].get_value()) - 1;
            }
        }

        // put our iterator as a variable inside vector
        mapped_variables[tokens[i+1].get_value()] = Token("VALUE", "0", "int");

        // get iterator name
        std::string it_name = tokens[i+1].get_value();

        i = i + 4;

        int loop_counter = 0;
        int t = i;

        for(int j=0;j<it;j++)
        {
            do 
            {
                t++;

                handle_token(t);

                if ( tokens[t].get_value() == "if" )
                {
                    do
                    {
                        t++;
                    } while ( tokens[t].get_value() != "endif" );
                    
                }
                
            } while ( tokens[t].get_value() != "end" );

            t = i;
            mapped_variables[it_name].set_value(std::to_string(j+1));
        }

        do
        {
            if ( tokens[i].get_value() == "if" )
            {
                do
                {
                    i++;
                } while ( tokens[i].get_value() != "endif" );
                
            }

            i++;

        } while ( tokens[i].get_value() != "end" );
        
    }
    else if( tokens[i].get_value() == "if" )
    {
        std::string cond1, cond2, op;
        int t = i;

        do 
        {
            t++;
            
            if( tokens[t].get_type() == "VALUE" && tokens[t+1].get_type() == "OPERATOR" && tokens[t+2].get_type() == "VALUE" )
            {
                cond1 = tokens[t].get_value(); 
                cond2 = tokens[t+2].get_value();

                if (mapped_variables.count(cond1)>0)
                {
                    cond1 = mapped_variables[cond1].get_value();
                }
                
                if (mapped_variables.count(cond2)>0)
                {
                    cond2 = mapped_variables[cond2].get_value();
                    
                }

                cond1.erase(remove(cond1.begin(), cond1.end(), '"'), cond1.end());
                cond2.erase(remove(cond2.begin(), cond2.end(), '"'), cond2.end());

                t = t + 3;
            }

        } while ( tokens[t].get_value() != ";" );

        do 
        {
            t++;

            if(cond1 == cond2)
            {
                
                if ( tokens[t].get_value() == "if" )
                {
                    do
                    {
                        t++;
                    } while ( tokens[t].get_value() != "endif" );
                    
                }

                handle_token(t);
                
            }
            
        } while ( tokens[t].get_value() != "endif" );
        
    }
}

void handle_type(int i)
{
    if ( tokens[i].get_type() == "TYPE" && tokens[i+1].get_type() == "NAME" && tokens[i+2].get_value() == ";" )
    {
        Token tmp;

        if ( tokens[i].get_value() == "int" )
            tmp = Token("VALUE", "0", "int");
        else if ( tokens[i].get_value() == "string" )
            tmp = Token("VALUE", " ", "string");

        mapped_variables[tokens[i+1].get_value()] = std::move(tmp);
    }
    else
    {
        int c = 3;
        Token last = tokens[i+c];

        last.set_subtype(tokens[i].get_value());

        if(last.get_subtype() == "int")
        {
            int res, snd;

            if (mapped_variables.count(last.get_value())>0)
            {
                res = std::stoi( mapped_variables[last.get_value()].get_value() );
            }
            else
            { 
                res = std::stoi( last.get_value() );
            }

            do 
            {
                c++;

                if ( tokens[i+c].get_type() == "VALUE" )
                {
                    if (mapped_variables.count(tokens[i+c].get_value())>0)
                    {
                        snd = std::stoi( mapped_variables[tokens[i+c].get_value()].get_value() );
                    }
                    else
                    { 
                        snd = std::stoi( tokens[i+c].get_value() );
                    }

                    if ( tokens[i+c-1].get_value() == "+" )
                        res = res + snd ;
                    else if ( tokens[i+c-1].get_value() == "-" )
                        res = res - snd ;
                    else if ( tokens[i+c-1].get_value() == "/" )
                        res = res / snd ;
                    else if ( tokens[i+c-1].get_value() == "*" )
                        res = res * snd ;
                }
                    

            } while ( tokens[i+c].get_value() != ";" );

            Token tmp = Token("VALUE", std::to_string(res), tokens[i].get_value());
            mapped_variables[tokens[i+1].get_value()] = std::move(tmp);
        }
        else if(last.get_subtype() == "string")
        {
            std::string res;
            std::string snd;

            if (mapped_variables.count(last.get_value())>0)
            {
                res = mapped_variables[last.get_value()].get_value();
            }
            else
            { 
                res = tokens[i+c].get_value();
            }

            do 
            {
                c++;

                if ( tokens[i+c].get_type() == "VALUE" )
                {
                    if (mapped_variables.count(tokens[i+c].get_value())>0)
                    {
                        snd = mapped_variables[tokens[i+c].get_value()].get_value();
                    }
                    else
                    { 
                        snd = tokens[i+c].get_value();
                    }

                    if ( tokens[i+c-1].get_value() == "+" )
                        res.append(snd);
                    else if ( tokens[i+c-1].get_value() == "-" )
                        substringWord(res, snd);
                }
                    

            } while ( tokens[i+c].get_value() != ";" );

            Token tmp = Token("VALUE", res, tokens[i].get_value());
            mapped_variables[tokens[i+1].get_value()] = std::move(tmp);
        }
    }
}

void handle_func(int i)
{
    if ( tokens[i].get_value() == "write" )
    {
        if( tokens[i+1].get_value()[0] != '"' )
        {
            std::string my_str = mapped_variables[tokens[i+1].get_value()].get_value();
            my_str.erase(remove(my_str.begin(), my_str.end(), '"'), my_str.end());
            std::cout << my_str;
        }
        else
        {
            std::cout << tokens[i+1].get_value().substr(1, tokens[i+1].get_value().length()-2 );
        } 
    }
    else if ( tokens[i].get_value() == "writeln" )
    {
        if( tokens[i+1].get_value()[0] != '"' )
        {
            std::string my_str = mapped_variables[tokens[i+1].get_value()].get_value();
            my_str.erase(remove(my_str.begin(), my_str.end(), '"'), my_str.end());
            std::cout << my_str << std::endl;
        }
        else
        {
            std::cout << tokens[i+1].get_value().substr(1, tokens[i+1].get_value().length()-2 ) << std::endl;
        } 
    }
    else if ( tokens[i].get_value() == "read" )
    {
        std::string t1;
        std::cout << tokens[i+1].get_value().substr(1, tokens[i+1].get_value().length()-2 );
        getline( std::cin, t1 );

        std::string func_type = tokens[i+3].get_value();

        if (mapped_variables.count(tokens[i+2].get_value())>0)
        {
            mapped_variables[tokens[i+2].get_value()].set_type("NAME");
            mapped_variables[tokens[i+2].get_value()].set_subtype(func_type);
            mapped_variables[tokens[i+2].get_value()].set_value(t1);
        }
        else
        {
            Token tmp = Token("NAME", t1, func_type);
            mapped_variables[tokens[i+2].get_value()] = std::move(tmp);
        }
        
    }
    else if ( tokens[i].get_value() == "exit" )
    {
        int code = std::stoi(tokens[i+1].get_value());
        exit(code);
    }
    else if ( tokens[i].get_value() == "system" )
    {
        system(tokens[i+1].get_value().data());
    }
    else
    {
        std::string orig = tokens[i].get_value();
        std::vector<std::string> params;
        
        for(int j=i;j<tokens.size();j++)
        {
            if(tokens[j].get_type() == "PARAMETER")
            {
                if(tokens[j].get_value().find(",") != std::string::npos)
                {
                    std::string h;
                    int i = 0;
                    
                    for(char x : tokens[j].get_value())
                    {
                        if(x == ',')
                        {
                            params.push_back(h);
                            h = "";
                        }

                        if( x != ',' && x != ' ')
                            h = h + x;
                    }

                    params.push_back(h);
                }
                else
                {
                    params.push_back(tokens[j].get_value());
                }
            }
        }

        int x = 0;

        do
        {
            i++;
        } while (tokens[x].get_value() != "func" && tokens[x+1].get_value() != orig);

        x = x + 3;
        int c = 0;

        if (tokens[x].get_type() == "PARAMETER")
        {
            do
            {
                mapped_variables[tokens[x].get_value()].set_value(params[c]);
                x = x + 2;
                c++;
            } while (tokens[x].get_type() == "PARAMETER");
        }

        x = 0;

        while (tokens[x].get_value() != "func" && tokens[x+1].get_value() != orig)
        {
            x++;
        }

        while (tokens[x].get_value() != ";" )
        {
            x++;
        }

        x++;

        do
        {
            handle_token(x);

            if ( tokens[x].get_value() == "if" )
            {
                do
                {
                    x++;
                } while ( tokens[x].get_value() != "endif" );
            }

            x++;

        } while (tokens[x].get_value() != "endfunc");
        
    }
}

void handle_token(int i)
{
    if(tokens[i].get_value() != "" )
    {
        // handle : ;x = t + t1...;
        if ( i > 0 && ( tokens[i-1].get_value() == ";" && tokens[i].get_type() == "VALUE" || tokens[i-1].get_value() == ";" && tokens[i].get_type() == "NAME" ) )
        {
            tokens[i].set_type("NAME");

            int c = 2;
            Token last = tokens[i];

            last.set_subtype(mapped_variables[tokens[i].get_value()].get_subtype());

            if(last.get_subtype().find("int") != std::string::npos)
            {
                int res, snd=0;
                
                if (mapped_variables.count(tokens[i+c].get_value())>0)
                {
                    res = std::stoi( mapped_variables[tokens[i+c].get_value()].get_value() );
                }
                else
                { 
                    res = std::stoi( tokens[i+c].get_value() );
                }
                
                do 
                {
                    c++;

                    if ( tokens[i+c].get_type() == "VALUE" )
                    {
                        if (mapped_variables.count(tokens[i+c].get_value())>0)
                        {
                            snd = std::stoi( mapped_variables[tokens[i+c].get_value()].get_value() );
                        }
                        else
                        { 
                            snd = std::stoi( tokens[i+c].get_value() );
                        }

                        if ( tokens[i+c-1].get_value() == "+" )
                            res = res + snd ;
                        else if ( tokens[i+c-1].get_value() == "-" )
                            res = res - snd ;
                        else if ( tokens[i+c-1].get_value() == "/" )
                            res = res / snd ;
                        else if ( tokens[i+c-1].get_value() == "*" )
                            res = res * snd ;
                    }
                        
                } while ( tokens[i+c].get_value() != ";" );

                mapped_variables[tokens[i].get_value()].set_value(std::to_string(res));
            }
            else if(last.get_subtype().find("string") != std::string::npos)
            {
                std::string res;
                std::string snd;

                if (mapped_variables.count(tokens[i+c].get_value())>0)
                {
                    res = mapped_variables[tokens[i+c].get_value()].get_value();
                }
                else
                { 
                    res = tokens[i+c].get_value();
                }

                do 
                {
                    c++;

                    if ( tokens[i+c].get_type() == "VALUE" )
                    {
                        if (mapped_variables.count(tokens[i+c].get_value())>0)
                        {
                            snd = mapped_variables[tokens[i+c].get_value()].get_value();
                        }
                        else
                        {
                            snd = tokens[i+c].get_value();
                        }

                        if ( tokens[i+c-1].get_value() == "+" )
                            res.append(snd);
                        else if ( tokens[i+c-1].get_value() == "-" )
                            substringWord(res, snd);
                    }
                        

                } while ( tokens[i+c].get_value() != ";" );

                mapped_variables[tokens[i].get_value()].set_value(res);
            }
        }
        else if ( tokens[i].get_type() == "CONTROL" )
        {
            handle_control_statements(i);
        }
        else if ( tokens[i].get_type() == "TYPE" )
        {
            handle_type(i);
        }
        else if ( tokens[i].get_type() == "FUNCTION" )
        {
            handle_func(i);        
        }
    }
}

void handle_tokens()
{
    //show_tokens();
    //show_variables();

    for(int i=0;i<tokens.size();i++)
    {
        handle_token(i);

        if ( tokens[i].get_value() == "if" )
        {
            do
            {
                i++;
            } while ( tokens[i].get_value() != "endif" );
        }
        else if ( tokens[i].get_value() == "func" )
        {
            do
            {
                i++;
            } while ( tokens[i].get_value() != "endfunc" );
            
        }
    }
}

void readFile(std::string filename)
{
    std::ifstream file;

    file.open(filename);

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
                tokenize(tmp);
                tmp = "";
                c = 0;
            }
            else if ( x == ';' )
            {
                tokenize(tmp);
                tmp = ";";
                tokenize(tmp);
                tmp = "";
            }
            else if ( x == '"' )
            {
                tmp = tmp + x;
                c++;
            }
            else if (  x == ' ' && (tmp.find('"') == std::string::npos) && (tmp.find("func") == std::string::npos))
            {
                tokenize(tmp);
                tmp = "";
            }
            else if ( check_func(tmp) )
            {
                tokenize(word);
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

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("Usage: interpreter [filename.aut]\n");
        exit(-1);
    }

    readFile(argv[1]);
    return 0;
}