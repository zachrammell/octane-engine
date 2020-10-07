/******************************************************************************/
/*!
  \par        Project Octane
  \file       WindowManager.h
  \author     Brayan Lopez
  \date       2020/10/06
  \brief      <WHAT IT DO>

  Copyright � 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

#pragma once

#include <OctaneEngine/ISystem.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <fstream>
#include <iostream>

namespace Octane
{
class SerializerSys : public ISystem
{
public:
  explicit SerializerSys(class Engine* engine);

  virtual ~SerializerSys() = default;

  template<typename Type>
  void Serialize(Type& object, const char* fieldName=nullptr);
  template <typename Type>
  void Serialize(eastl::vector<Type>& object, const char* fieldName = nullptr);
  void SetOutputFile(const eastl::wstring& filePath);
  void FinishSerializing();
  virtual void Load() {};
  virtual void LevelStart() {};
  virtual void Update() {};
  virtual void LevelEnd() {};
  virtual void Unload() {};

  static SystemOrder GetOrder();

private:
  std::ofstream out; //file to serialize to
  //to prevent code duplication
  template<typename Type>
  void SerializeHelper(const Type& object, const char* fieldName);
  template<typename Type>
  void SerializeHelper(eastl::vector<Type>& object, const char* fieldName);

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
}; // namespace Octane
