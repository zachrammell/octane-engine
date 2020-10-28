/******************************************************************************/
/*!
  \par        Project Octane
  \file       NBTReader.h
  \author     Zach Rammell
  \date       2020/10/23
  \brief      Class for deserializing NBT data

  Copyright � 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <cstdio>
#include <EASTL/string_view.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/stack.h>
#include <EASTL/string_hash_map.h>

namespace Octane
{
  using eastl::string_view;
class NBTReader
{
public:
  NBTReader(string_view filepath);
  ~NBTReader();

  bool OpenCompound(string_view name);
  void CloseCompound();

  bool OpenList(string_view name);
  int32_t ListSize();
  void CloseList();

  int8_t ReadByte(string_view name);
  int16_t ReadShort(string_view name);
  int32_t ReadInt(string_view name);
  int64_t ReadLong(string_view name);
  float ReadFloat(string_view name);
  double ReadDouble(string_view name);
  eastl::vector<int8_t> ReadByteArray(string_view name);
  eastl::string ReadString(string_view name);

  // This function is not implemented, only specialized! Specialize it on your own type to enable deserialization
  template<typename T>
  T Read(string_view name);

private:
  enum class TAG : uint8_t
  {
    End = 0,
    Byte = 1,
    Short = 2,
    Int = 3,
    Long = 4,
    Float = 5,
    Double = 6,
    Byte_Array = 7,
    String = 8,
    List = 9,
    Compound = 10,
    INVALID = 0xCC
  };

  struct DataTag
  {
    TAG type_;
    union
    {
      int8_t byte_;
      int16_t short_;
      int32_t int_;
      int64_t long_;
      float float_;
      double double_;
      struct
      {
        int32_t byte_array_length_;
        uint8_t* byte_array_;
      };
      struct
      {
        int16_t string_length_;
        char const* string_;
      };
      struct
      {
        TAG list_type_;
        int32_t list_length_;
      };
    };
  };

  eastl::string_hash_map<DataTag> named_tags_;
  eastl::string current_name_ = "";
  eastl::vector<eastl::string> string_pool_;
  eastl::vector<uint8_t> byte_array_pool_;
  size_t parsing_nesting_depth_ = 0;
  struct NestingInfo
  {
    TAG data_type;
    enum class ContainerType
    {
      List,
      Compound
    } container_type;
    int32_t length;
    int32_t list_index;
  };

  eastl::stack<NestingInfo> nesting_info_;

  FILE* infile_;

  /* READING HELPER FUNCTIONS */

  bool HandleNesting(string_view name, TAG t);
  void EnterRoot();

  /* PARSING FUNCTIONS */

  void ParseDataTree();
  DataTag& ParseDataTag();
  DataTag& ParseDataTagUnnamed(TAG type);

  TAG ReadTag();
  eastl::string ReadName();

  void AddToCurrentName(string_view name);
  void PopLatestName();

  int16_t ReadStrLen();
  int32_t ReadArrayLen();
};

}
