#include <OctaneEngine/NBTWriter.h>

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>
// Load-time link with the networking library
#pragma comment(lib, "ws2_32.lib")

#include <OctaneEngine/Helper.h>
#include <cassert>

namespace Octane
{

uint64_t htonll(uint64_t x)
{
  return (static_cast<uint64_t>(htonl((x)&0xFFFFFFFFLL)) << 32) | htonl((x) >> 32);
}

uint64_t ntohll(uint64_t x)
{
  return (static_cast<uint64_t>(ntohl((x)&0xFFFFFFFFLL)) << 32) | ntohl((x) >> 32);
}

float htonf(float value)
{
  uint32_t const temp = htonl(*reinterpret_cast<uint32_t*>(&value));
  return *reinterpret_cast<float const*>(&temp);
}

double htond(double value)
{
  uint64_t const temp = htonll(*reinterpret_cast<uint64_t*>(&value));
  return *reinterpret_cast<double const*>(&temp);
}

NBTWriter::NBTWriter(string_view filepath) : outfile_ {nullptr}
{
  errno_t err;
  int const w_filepath_length = MultiByteToWideChar(CP_UTF8, 0, filepath.data(), -1, NULL, 0);
  wchar_t* w_filepath = static_cast<wchar_t*>(_malloca(sizeof(wchar_t) * w_filepath_length));
  MultiByteToWideChar(CP_UTF8, 0, filepath.data(), -1, w_filepath, w_filepath_length);
  if ((err = _wfopen_s(&outfile_, w_filepath, L"wb")))
  {
    __debugbreak();
  }
  _freea(w_filepath);
  BeginCompound("root");
}

NBTWriter::~NBTWriter()
{
  EndCompound();
  if (!nesting_info_.empty())
  {
    assert(!"[ERROR] NBTWriter: Mismatched Compound Begin/End - A Compound Tag was not closed.");
  }
}

bool NBTWriter::BeginCompound(string_view name)
{
  nesting_info_.emplace(NestingInfo {TAG::End, NestingInfo::ContainerType::Compound, 0, 0});
  if (!name.empty())
  {
    WriteTag(TAG::Compound);
    WriteString("", name);
  }
  return true;
}

void NBTWriter::EndCompound()
{
  NestingInfo& nesting = nesting_info_.top();
  if (nesting.container_type != NestingInfo::ContainerType::Compound)
  {
    assert(!"[ERROR] NBTWriter: Mismatched Compound Begin/End - Compound Tag closed without being opened.");
  }
  if (nesting.length == 0)
  {
    /* [WARNING] NBTWriter: Empty Compound - Compound Tag with no values. */
  }
  nesting_info_.pop();
  WriteTag(TAG::End);
}

bool NBTWriter::BeginList(string_view name)
{
  NestingInfo& nesting = nesting_info_.top();
  if (!name.empty())
  {
    WriteTag(TAG::List);
    WriteString("", name);
  }
  // Store the file offset corresponding to the length, so that when ending the list we can write the length in here
  fpos_t current_file_pos;
  fgetpos(outfile_, &current_file_pos);
  // The following two writes are not accurate, they are simply filling space to be written in when the list is finished
  WriteTag({});
  WriteInt("", {});
  nesting_info_.emplace(NestingInfo {TAG::End, NestingInfo::ContainerType::List, 0, current_file_pos});
  return true;
}

void NBTWriter::EndList()
{
  NestingInfo& nesting = nesting_info_.top();
  if (nesting.container_type != NestingInfo::ContainerType::List)
  {
    assert(!"[ERROR] NBTWriter: Mismatched List Begin/End - List Tag closed without being opened.");
  }
  if (nesting.length == 0)
  {
    /* [WARNING] NBTWriter: Empty List - List Tag with no values. */
  }
  fpos_t current_file_pos;
  fgetpos(outfile_, &current_file_pos);

  fsetpos(outfile_, &(nesting.file_pos));
  WriteTag(nesting.data_type);
  WriteInt("", nesting.length);

  fsetpos(outfile_, &current_file_pos);
  nesting_info_.pop();
}

void NBTWriter::WriteString(string_view name, string_view str)
{
  NestingInfo& nesting = nesting_info_.top();
  // Lists have strict requirements
  if (nesting.container_type == NestingInfo::ContainerType::List)
  {
    if (!name.empty())
    {
      assert(
        !"[ERROR] NBTWriter: Name Error - Attempted to add a named string to a List. Lists cannot contain named tags.");
    }
    // The list is not exclusively strings
    if (nesting.data_type != TAG::String)
    {
      // If the list is currently empty, make it into a string list
      if (nesting.length == 0)
      {
        nesting.data_type = TAG::String;
      }
      else
      {
        assert(
          !"[ERROR] NBTWriter: Type Error - Attempted to add a string to a list of non-strings. All tags in a list must be of the same type.");
      }
    }
  }
  ++(nesting.length);

  // Tag and name information is only written for named tags
  if (!name.empty())
  {
    WriteTag(TAG::String);
    WriteString("", name);
  }
  WriteShort("", static_cast<uint16_t>(str.size()));
  fwrite(str.data(), sizeof(string_view::value_type), str.size(), outfile_);
}

void NBTWriter::WriteByte(string_view name, int8_t b)
{
  if (!name.empty())
  {
    WriteTag(TAG::Byte);
    WriteString("", name);
  }
  fwrite(&b, sizeof(b), 1, outfile_);
}

void NBTWriter::WriteShort(string_view name, int16_t s)
{
  if (!name.empty())
  {
    WriteTag(TAG::Short);
    WriteString("", name);
  }
  uint16_t const s_big_endian = htons(s);
  fwrite(&s_big_endian, sizeof(s_big_endian), 1, outfile_);
}

void NBTWriter::WriteInt(string_view name, int32_t i)
{
  if (!name.empty())
  {
    WriteTag(TAG::Int);
    WriteString("", name);
  }
  uint32_t const i_big_endian = htonl(i);
  fwrite(&i_big_endian, sizeof(i_big_endian), 1, outfile_);
}

void NBTWriter::WriteLong(string_view name, int64_t l)
{
  if (!name.empty())
  {
    WriteTag(TAG::Long);
    WriteString("", name);
  }
  uint32_t const l_big_endian = htonll(l);
  fwrite(&l_big_endian, sizeof(l_big_endian), 1, outfile_);
}

void NBTWriter::WriteFloat(string_view name, float f)
{
  if (!name.empty())
  {
    WriteTag(TAG::Float);
    WriteString("", name);
  }
  float const f_big_endian = htonf(f);
  fwrite(&f_big_endian, sizeof(f_big_endian), 1, outfile_);
}

void NBTWriter::WriteDouble(string_view name, double d)
{
  if (!name.empty())
  {
    WriteTag(TAG::Float);
    WriteString("", name);
  }
  double const d_big_endian = htond(d);
  fwrite(&d_big_endian, sizeof(d_big_endian), 1, outfile_);
}

void NBTWriter::WriteByteArray(string_view name, int8_t const* array, int32_t length)
{
  if (!name.empty())
  {
    WriteTag(TAG::Byte_Array);
    WriteString("", name);
  }
  WriteInt("", length);
  fwrite(&array, sizeof(array[0]), length, outfile_);
}

void NBTWriter::WriteTag(TAG t)
{
  fwrite(&t, sizeof(t), 1, outfile_);
}

} // namespace Octane
