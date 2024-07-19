A serializer generator for C++.
It does not serialize pointers.

# Use

To create a serializer for a class or struct:

```
#include "serializer.hpp"

class SomeClass {
public:
    SomeClass();
    ~SomeClass();

    AUTO_SERIALIZE;

private:
    int aNumber;
    float aFloat;
    std::vector<AnotherClass> aVector;
};
```

Where `serializer.hpp` is the generated serializer header file and `AnotherClass` is a class that
can be serialized (but doesn't need to have `AUTO_SERIALIZE`). You can then use the methods
`SomeClass::serialize(std::ostream &os)` and `SomeClass::unserialize(std::istream &is)`.

# Generating serializer

## With cmake

If you use **cmake** you can use the function in `serializer-generator.cmake`:

```
add_serializer_generation(target
                          [COMPILATION_DB compilation_db_folder_path]
                          HEADER generated_header_path
                          CODE generated_code_path
                          FILES cppfile1 [cppfile2 ...])
```

To automatically generate the serializer on build (adds the serializer generation target as a
dependency). `COMPILATION_DB` is set to `PROJECT_SOURCE_DIR` by default.

The `CMakeLists.txt` file should be something like:


```
...
add_subdirectory(lib/serializer-generator) # Or folder where the repo is cloned
include(lib/serializer-generator/serializer-generator.cmake) # To use the function

add_executable(some_executable file1.cpp file2.cpp serializer.cpp)
add_serializer_generation(some_executable
    HEADER serializer.hpp
    CODE serializer.cpp
    FILES file1.cpp file2.cpp
)
...
```


