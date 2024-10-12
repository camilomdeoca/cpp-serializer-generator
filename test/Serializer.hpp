#pragma once

// This Header was generated with serializer-generator

#include <type_traits>
#include <concepts>
#include <iostream>

#ifdef LOG_EVERY_SERIALIZATION
    #define LOG_SERIALIZATION(variable) { std::cout << variable << std::endl; }
#else
    #define LOG_SERIALIZATION(variable)
#endif

/// @brief This macro should only be used in header files because the record
///        needs to be included in the .cpp file of the serializer.
#define AUTO_SERIALIZE                                                                             \
    using enable_auto_serialize = void;                                                            \
    friend class Serializer;                                                                       \
    friend class Unserializer;

template<typename T>
concept dynamic_array_type = requires(T t)
{
    t.size();
    t.data();
    requires std::is_pointer_v<decltype(t.data())>;
    t[0];
    requires std::is_reference_v<decltype(t[0])>;
    t.resize(t.size());
};

template<typename T>
concept trivially_copyable = std::is_trivially_copyable_v<T>;

struct AnotherStructThatWillBeField;
struct Another;
namespace what::whatagain {
    struct TestStruct;
}
struct StructInOtherFile;

class Serializer
{
public:
    Serializer(std::ostream &os)
        : m_os(os)
    {}

    void operator()(const dynamic_array_type auto &field)
    {
        using size_type = decltype(field.size());
        const size_type size = field.size();

        LOG_SERIALIZATION("Dynamic aggregate of size: " << size);
        m_os.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (size_type i = 0; i < size; i++)
        {
            (*this)(field[i]);
        }
    }

    template <class ElementType, typename SizeType, SizeType size>
    void operator()(const std::array<ElementType, size> &field)
    {
        LOG_SERIALIZATION("Serializing std::array of size: " << size);
        for (SizeType i = 0; i < size; i++)
        {
            (*this)(field[i]);
        }
    }

    void operator()(const trivially_copyable auto &field)
    {
        LOG_SERIALIZATION(field);
        m_os.write(reinterpret_cast<const char*>(&field), sizeof(field));
    }

    void operator()(const auto &object) { static_assert(false, "The Serializer class needs regeneration."); }

    void operator()(const AnotherStructThatWillBeField &object);
    void operator()(const Another &object);
    void operator()(const what::whatagain::TestStruct &object);
    void operator()(const StructInOtherFile &object);

private:
    std::ostream &m_os;
};

class Unserializer
{
public:
    Unserializer(std::istream &is)
        : m_is(is)
    {}

    void operator()(dynamic_array_type auto &field)
    {
        using size_type = decltype(field.size());
        size_type size;
        m_is.read(reinterpret_cast<char*>(&size), sizeof(size));
        LOG_SERIALIZATION("Dynamic aggregate of size: " << size);
        field.resize(size);
        for (size_type i = 0; i < size; i++)
        {
            (*this)(field[i]);
        }
    }

    template <class ElementType, typename SizeType, SizeType size>
    void operator()(std::array<ElementType, size> &field)
    {
        LOG_SERIALIZATION("Unserializing std::array of size: " << size);
        for (SizeType i = 0; i < size; i++)
        {
            (*this)(field[i]);
        }
    }

    void operator()(trivially_copyable auto &field)
    {
        m_is.read(reinterpret_cast<char*>(&field), sizeof(field));
        LOG_SERIALIZATION(field);
    }

    void operator()(auto &object) { static_assert(false, "The Unserializer class needs regeneration."); }

    void operator()(AnotherStructThatWillBeField &object);
    void operator()(Another &object);
    void operator()(what::whatagain::TestStruct &object);
    void operator()(StructInOtherFile &object);

private:
    std::istream &m_is;
};

