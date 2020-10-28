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

  // This function is not implemented, only specialized! Specialize it on your own type to enable serialization
  template<typename T>
  void Write(string_view name, T value);

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
  void WriteTag(TAG t);
  /* The following functions are for raw writing without considering names, types, or nesting */

  void WriteStr(string_view name);
  void WriteStrLen(int16_t len);
  void WriteArrayLen(int32_t len);

  void BeginRoot();

  void HandleNesting(string_view name, TAG t);

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
