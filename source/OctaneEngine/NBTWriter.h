#pragma once

#include <cstdio>
#include <EASTL/string_view.h>
#include <EASTL/stack.h>

namespace Octane
{

using eastl::string_view;

class NBTWriter
{
public:
  NBTWriter(string_view filepath);
  ~NBTWriter();
  bool BeginCompound(string_view name);
  void EndCompound();
  bool BeginList(string_view name);
  void EndList();
  void WriteByte(string_view name, int8_t b);
  void WriteShort(string_view name, int16_t s);
  void WriteInt(string_view name, int32_t i);
  void WriteLong(string_view name, int64_t l);
  void WriteFloat(string_view name, float f);
  void WriteDouble(string_view name, double d);
  void WriteByteArray(string_view name, int8_t const* array, int32_t length);
  void WriteString(string_view name, string_view str);

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
    Compound = 10
  };
  void WriteTag(TAG t);

  FILE* outfile_;

  struct NestingInfo
  {
    TAG data_type;
    enum class ContainerType
    {
      List,
      Compound
    } container_type;
    int32_t length;
    fpos_t file_pos;
  };

  eastl::stack<NestingInfo> nesting_info_;
};

}
