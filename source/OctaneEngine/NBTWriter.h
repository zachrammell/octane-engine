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
  /*!
   * \brief opens and begins writing data to an NBT file
   * \param filepath path of NBT file to create/overwrite and write to
   */
  NBTWriter(string_view filepath);
  ~NBTWriter();

  /*!
   * \brief begins a Compound of other tags.
   * This means that all writes until EndCompound() is called will be written into this compound.
   * Compounds are analogous to structs and contain named tags of any type.
   * Tags inside a compound have the following requirements: they must be named.
   *
   * Usage:
   *
   *  if (BeginCompound("my_compound") {
   *    WriteInt("lives", 3);
   *    WriteFloat("rotation", 45.6666667f);
   *    WriteString("name", "Luigi");
   *    EndCompound();
   *  }
   *
   *  This will create a compound of the following structure (JSON notation):
   *
   *  "my_compound": {
   *    "lives": 3,
   *    "rotation": 45.6666667f,
   *    "name": "Luigi"
   *  }
   *  
   * \param name the name to give this compound tag
   * \return true if compound is successfully opened, false otherwise
   */
  bool BeginCompound(string_view name);
  /*!
   * \brief Ends writing to a previously started compound. Should only be called if BeginCompound() returned true.
   * After calling, writes will no longer be added to the compound.
   */
  void EndCompound();

  /*!
   * \brief begins a List of other tags.
   *  This means that all writes until EndList() is called will be written into this list.
   *  Lists are analogous to arrays, and are intended for grouping data that has an obvious structure or otherwise does not need a name.
   *  Tags inside a list have the following requirements: they must all have the same type, and they cannot have names.
   *
   *  Usage:
   *
   *    if (BeginList("my_list") {
   *      WriteInt("", 1);
   *      WriteInt("", 2);
   *      WriteInt("", 3);
   *      EndList();
   *    }
   *
   *  This will create a List with 3 ints: {1, 2, 3}.
   *  Order is relevant: the order that tags are written to the list will be the order they are read out/accessed.
   *
   * \param name the name to give this list object
   * \return true if list is opened for writing, false otherwise
   */
  bool BeginList(string_view name);
  /*!
   * \brief Ends writing to a previously started list. Should only be called if BeginList() returned true.
   * After calling, writes will no longer be added to the list.
   */
  void EndList();

  void WriteByte(string_view name, int8_t b);
  void WriteShort(string_view name, int16_t s);
  void WriteInt(string_view name, int32_t i);
  void WriteLong(string_view name, int64_t l);
  void WriteFloat(string_view name, float f);
  void WriteDouble(string_view name, double d);
  void WriteByteArray(string_view name, int8_t const* array, int32_t length);
  void WriteString(string_view name, string_view str);

  /*!
   * \brief This function is not implemented, only specialized! Specialize it on your own type to enable serialization.
   * It's a generic writer function. for the basic NBT types, it acts exactly like calling the explicit function.
   * For other types, the behavior is user-defined.
   * \tparam T type of data to write (prefer to explicitly specialize when calling)
   * \param name name of value to write
   * \param value value to write
   */
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
