# RoombaCpp
Like [RoombaSim](https://github.com/jarlold/RoombaSim) but in C++ with Raylib. To be honest, I'm mostly writing this to
try out Raylib because it looks cool.

## Rules For C++
I will continue my efforts to make C++ a nice language, by following these rules:
- Headers are still annoying and I will instead group reflective types together in one file. Which if you think about it while drunk, is organic.
- I will continue to pretend I am writing C code, with the exception of memory-safe versions of structures like std::array and std::vector.
- I will not write global variables. Code will be written functionally and the state passed. This is to ensure things are allocated
  on the stack, and avoid possible memory leaks.
- I will not write recursive functions. Every time I see a recursive function it informs me the author knew mathematics or computer programming, but
  not both.
- I will prefer structures over classes.
- I will not use inheritance, except for the sparing use of interfaces, if absolutely required.
- I will not use templates.
- I will avoid `malloc` and his friends.
- I will not define functions inside of structures.
- I will never use the "new" keyword. 

The rules for this project are an experiment, so they might be awful. They might also be very enlightening. 

## Credit To Raylib
I don't want to have to fish out the right version of raylib later to compile this, so I will include the entire library.
Raylib can be found [here](https://github.com/raysan5/raylib) and is under the Zlib license. 
