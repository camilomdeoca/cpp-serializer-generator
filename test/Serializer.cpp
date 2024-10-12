#include "Serializer.hpp"

#include "test.h"
#include "anotherfile.hpp"

void Serializer::operator()(const AnotherStructThatWillBeField &object)
{
    (*this)(object.i);
    (*this)(object.j);
}

void Unserializer::operator()(AnotherStructThatWillBeField &object)
{
    (*this)(object.i);
    (*this)(object.j);
}

void Serializer::operator()(const Another &object)
{
    (*this)(object.name);
    (*this)(object.numbers);
    (*this)(object.structInside);
}

void Unserializer::operator()(Another &object)
{
    (*this)(object.name);
    (*this)(object.numbers);
    (*this)(object.structInside);
}

void Serializer::operator()(const what::whatagain::TestStruct &object)
{
    (*this)(static_cast<const Another&>(object));
    (*this)(object.volume);
    (*this)(object.count);
}

void Unserializer::operator()(what::whatagain::TestStruct &object)
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

