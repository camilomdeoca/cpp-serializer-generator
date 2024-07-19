A serializer generator for C++.
It does not serialize pointers.

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
can be serialized (but doesn't need to have `AUTO_SERIALIZE`).
You then can use the methods `SomeClass::serialize(std::ostream &os)` and
`SomeClass::unserialize(std::istream &is)`.

If you use **cmake** you can use the function in `serializer-generator.cmake`:

```
add_serializer_generation(target
                          [COMPILATION_DB compilation_db_folder_path]
                          HEADER generated_header_path
                          CODE generated_code_path
                          FILES cppfile1 [cppfile2 ...])
```

To automatically generate the serializer on build (adds the serializer generation target as a dependency).

