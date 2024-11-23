clang++ -std=c++23 FileFinder.cppm --precompile -o FileFinder.pcm
clang++ -std=c++23 FileFinder.pcm -fprebuilt-module-path=. -c -o FileFinder.o
clang++ -std=c++23 main.cpp -fprebuilt-module-path=. -c -o main.o
clang++ main.o FileFinder.o -o bin.out
