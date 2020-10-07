/******************************************************************************/
/*!
  \par        Project Octane
  \file       WindowManager.h
  \author     Brayan Lopez
  \date       2020/10/06
  \brief      <WHAT DOES IT DO>

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#include <OctaneEngine/SerializerSys.h>
#include <OctaneEngine/SystemOrder.h>
#include <cstddef>
//NBT FORMAT:
//First byte is TAG
//Next 2 bytes are length of name
//Then name
//if array, next 4 bytes are length of array, else skip this part
//Then payload

namespace Octane
{

SerializerSys::SerializerSys(Engine* engine)
  : ISystem(engine)
{
}

template<typename Type>
void SerializerSys::Serialize(Type& object, const char* fieldName)
{
  //if its some sort of structure (struct class) do: out << TAG_Compound
  //if its some sort of array do: out << TAG_List
  return;
}

template<>
void SerializerSys::Serialize<char>(char& object, const char* fieldName)
{
  out << TAG_Byte;
  SerializeHelper(object, fieldName);
}

template<>
void SerializerSys::Serialize<short>(short& object, const char* fieldName)
{
  out << TAG_Short;
  SerializeHelper(object, fieldName);
}

template<>
void SerializerSys::Serialize<int>(int& object, const char* fieldName)
{
  out << TAG_Int;
  SerializeHelper(object, fieldName);
}

template<>
void SerializerSys::Serialize<long>(long& object, const char* fieldName)
{
  out << TAG_Long;
  SerializeHelper(object, fieldName);
}

template<>
void SerializerSys::Serialize<float>(float& object, const char* fieldName)
{
  out << TAG_Float;
  SerializeHelper(object, fieldName);
}

template<>
void SerializerSys::Serialize<double>(double& object, const char* fieldName)
{
  out << TAG_Double;
  SerializeHelper(object, fieldName);
}

template<typename Type>
void SerializerSys::Serialize(eastl::vector<Type>& object, const char* fieldName)
{
  out << TAG_List;
  SerializeHelper(object, fieldName);
}

template <>
void SerializerSys::Serialize(eastl::string& object, const char* fieldName)
{
  out << TAG_String;
  eastl::vector<char> strVec{object.begin(),object.end()};
  
  SerializeHelper(strVec, fieldName);
}

void SerializerSys::SetOutputFile(const eastl::wstring& filePath)
{
  out.open(filePath.c_str(), std::ios_base::binary | std::ios_base::out);
}

void SerializerSys::FinishSerializing()
{
  out.close();
}

SystemOrder SerializerSys::GetOrder()
{
  return SystemOrder::Serializer;
}

  //to prevent code duplication
template<typename Type>
void SerializerSys::SerializeHelper(const Type& object, const char* fieldName)
{
  out << static_cast<unsigned short>(strlen(fieldName));
  out << fieldName;
  out << object;
}

template<typename Type>
  void SerializerSys::SerializeHelper(eastl::vector<Type>& object,
  const char* fieldName)
{
  //2 bytes for length of field name
  char fieldLenBuff[2] {static_cast<unsigned short>(strlen(fieldName))};
  out.write(fieldLenBuff, 2);
  //write field name to file
  out << fieldName;
  //write data type used inside vector
  if(std::is_same<Type, char>::value)
  {
    out << TAG_Byte;
  }
  else if (std::is_same<Type, short>::value)
  {
    out << TAG_Short;
  }
  else if (std::is_same<Type, int>::value)
  {
    out << TAG_Int;
  }
  else if (std::is_same<Type, long>::value)
  {
    out << TAG_Long;
  }
  else if (std::is_same<Type, float>::value)
  {
    out << TAG_Float;
  }
  else if (std::is_same<Type, double>::value)
  {
    out << TAG_Double;
  }
  //4 bytes for size of array
  char sizeBuff[4] {static_cast<int>(object.size())};
  out.write(sizeBuff, 4);
  //next bytes for data of array
  out.write(reinterpret_cast<char*>(object.data()), object.size() * sizeof(Type));
}




};



