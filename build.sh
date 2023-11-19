clang++ -std=c++17 -I include/ src/*.cpp -o bin/interpreter
bin/interpreter "$(pwd)/$1"