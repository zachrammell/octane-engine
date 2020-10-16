/******************************************************************************/
/*!
  \par        Project Octane
  \file       Serializer.h
  \author     Brayan Lopez
  \date       2020/10/06
  \brief      Serializes and Deserializes binary files in NBT format

  Copyright � 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

#pragma once

#define DEPRECATED
#ifndef DEPRECATED
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <fstream>
#include <iostream>

namespace Octane
{
class Serializer
{
public:
  template<typename Type>
  void Serialize(const Type& object, const eastl::string& fieldName = "");

  template <typename Type>
  void Serialize(const eastl::vector<Type>& object, const eastl::string& fieldName = "");

  //void StartCompound(const char* compoundName);
  void SetOutputFile(const eastl::wstring& filePath);
  void FinishSerializing();
private:
  std::ofstream out; //file to serialize to
  //to prevent code duplication
  template<typename Type>
  void SerializeHelper(const Type& object, const eastl::string& fieldName = "");

  template<typename Type>
  void SerializeHelper(eastl::vector<Type>& object, const eastl::string& fieldName = "");

  enum NBTTags
  {
    TAG_End,
    TAG_Byte,
    TAG_Short,
    TAG_Int,
    TAG_Long,
    TAG_Float,
    TAG_Double,
    TAG_Byte_Array,
    TAG_String,
    TAG_List,
    TAG_Compound
  };

};

template <typename Type>
void Serializer::SerializeHelper(const Type& object, const eastl::string& fieldName)
{
  char fieldLenBuff[2]{static_cast<unsigned short>(eastl::string(fieldName).size())};
  out << fieldLenBuff;
  out.write(fieldName.c_str(), fieldName.size());
  out << object;
}

template <typename Type>
void Serializer::SerializeHelper(eastl::vector<Type>& object, const eastl::string& fieldName)
{
  //2 bytes for size of array if string, 4 otherwise
  constexpr int lengthBytes = std::is_same<Type, char>::value ? 2 : 4;
  Serializer::NBTTags tag;
  //get data type used inside vector
  if (std::is_same<Type, char>::value)
  {
    tag = TAG_Byte;
  }
  else if (std::is_same<Type, short>::value)
  {
    tag = TAG_Short;
  }
  else if (std::is_same<Type, int>::value)
  {
    tag = TAG_Int;
  }
  else if (std::is_same<Type, long>::value)
  {
    tag = TAG_Long;
  }
  else if (std::is_same<Type, float>::value)
  {
    tag = TAG_Float;
  }
  else if (std::is_same<Type, double>::value)
  {
    tag = TAG_Double;
  }
  //2 bytes for length of field name
  char fieldLenBuff[2]{static_cast<short>(fieldName.size())};
  out.write(fieldLenBuff, 2);
  out.write(fieldName.c_str(), fieldName.size());
  out << tag;
  //size of array bytes
  char sizeBuff[lengthBytes]{static_cast<int>(object.size())};
  out.write(sizeBuff, lengthBytes);
  //next bytes for data of array
  out.write(reinterpret_cast<char*>(object.data()), object.size() * sizeof(Type));
}

template <typename Type>
void Serializer::Serialize(const Type& object, const eastl::string& fieldName)
{
  //if its some sort of structure (struct class) do: out << TAG_Compound
  //if its some sort of array do: out << TAG_List
}

template <typename Type>
void Serializer::Serialize(const eastl::vector<Type>& object, const eastl::string& fieldName)
{
  out << TAG_List;
  SerializeHelper(object, fieldName);
}
}; // namespace Octane

#endif