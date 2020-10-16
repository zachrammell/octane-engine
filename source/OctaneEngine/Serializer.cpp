/******************************************************************************/
/*!
  \par        Project Octane
  \file       WindowManager.h
  \author     Brayan Lopez
  \date       2020/10/06
  \brief      Serializes data into NBT format

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#include <OctaneEngine/Serializer.h>

#ifndef DEPRECATED

#include <OctaneEngine/SystemOrder.h>
#include <cstddef>



//NBT FORMAT:
//First byte in file is TAG_Compound
//Next 2 bytes are length of compound name
//Next bytes are compound name
//
//First byte is TAG
//Next 2 bytes are length of name
//Then name
//if array, next 4 bytes are length of array, else skip this part
//Then payload
//
//Last byte in file is TAG_END

namespace Octane
{

template<>
void Serializer::Serialize<char>(const char& object, const eastl::string& fieldName)
{
  out << TAG_Byte;
  SerializeHelper(object, fieldName);
}

template<>
void Serializer::Serialize<short>(const short& object, const eastl::string& fieldName)
{
  out << TAG_Short;
  SerializeHelper(object, fieldName);
}

template<>
void Serializer::Serialize<int>(const int& object, const eastl::string& fieldName)
{
  out << Serializer::TAG_Int;
  SerializeHelper(object, fieldName);
}

template<>
void Serializer::Serialize<long>(const long& object, const eastl::string& fieldName)
{
  out << TAG_Long;
  SerializeHelper(object, fieldName);
}

template<>
void Serializer::Serialize<float>(const float& object, const eastl::string& fieldName)
{
  out << TAG_Float;
  SerializeHelper(object, fieldName);
}

template<>
void Serializer::Serialize<double>(const double& object, const eastl::string& fieldName)
{
  out << TAG_Double;
  SerializeHelper(object, fieldName);
}

template <>
void Serializer::Serialize(const eastl::string& object, const eastl::string& fieldName)
{
  out << TAG_String;
  eastl::vector<char> strVec{object.begin(),object.end()};
  
  SerializeHelper(strVec, fieldName);
}

void Serializer::SetOutputFile(const eastl::wstring& filePath)
{
  out.open(filePath.c_str(), std::ios_base::binary | std::ios_base::out);
  if (out.is_open())
  {
    const eastl::string topCompound {"Default"};
    out << TAG_Compound;
    char fieldLenBuff[2] {static_cast<unsigned short>(topCompound.size())};
    out.write(fieldLenBuff, 2);
    out.write(topCompound.c_str(), topCompound.size());
  }
}

void Serializer::FinishSerializing()
{
  out << TAG_End;
  out.close();
}

}; // namespace Octane

#endif
