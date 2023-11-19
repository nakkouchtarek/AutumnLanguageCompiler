# CustomLanguageInterpreter

Made a simple interpreter written in C++ for a custom language I created, made a Parser and Tokenizer, still has some bugs and could be optimized, but is functional for basic tasks!

# Sample Code
```
for i = 5;
    for j = 2;
        writeln(j);
    endif;
end;

int x = 2;
int c = 5;
int res = x + c;

writeln(res);

string password = "";

for i = inf;
    read("Password: ", password, string);

    if password = "password";
        system("id");
    endif;

    if password = "exit";
        writeln("stuff");
    endif;

end;
```
