#include "Serializer.hpp"

#include "test.h"
#include "anotherfile.hpp"

void Serializer::operator()(const Another &object)
{
    (*this)(object.name);
    (*this)(object.numbers);
}

void Unserializer::operator()(Another &object)
{
    (*this)(object.name);
    (*this)(object.numbers);
}

void Serializer::operator()(const TestStruct &object)
{
    (*this)(static_cast<const Another&>(object));
    (*this)(object.volume);
    (*this)(object.count);
}

void Unserializer::operator()(TestStruct &object)
{
    (*this)(static_cast<Another&>(object));
    (*this)(object.volume);
    (*this)(object.count);
}

void Serializer::operator()(const StructInOtherFile &object)
{
    (*this)(object.key);
    (*this)(object.values);
}

void Unserializer::operator()(StructInOtherFile &object)
{
    (*this)(object.key);
    (*this)(object.values);
}

