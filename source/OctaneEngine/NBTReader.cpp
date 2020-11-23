/******************************************************************************/
/*!
  \par        Project Octane
  \file       NBTReader.cpp
  \author     Zach Rammell
  \date       2020/10/23
  \brief      Implementation for NBT deserialization

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/NBTReader.h>

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>

#include <cassert>

#include <OctaneEngine/Trace.h>

namespace Octane
{

uint64_t htonll(uint64_t x);
uint64_t ntohll(uint64_t x);
float htonf(float value);
double htond(double value);

NBTReader::NBTReader(string_view filepath) : infile_ {nullptr}
{
  errno_t err;
  int const w_filepath_length = MultiByteToWideChar(CP_UTF8, 0, filepath.data(), -1, NULL, 0);
  wchar_t* w_filepath = static_cast<wchar_t*>(_malloca(sizeof(wchar_t) * w_filepath_length));
  MultiByteToWideChar(CP_UTF8, 0, filepath.data(), -1, w_filepath, w_filepath_length);
#ifdef _DEBUG
file_open:
#endif
  err = _wfopen_s(&infile_, w_filepath, L"rb");
  if (err)
  {
#ifdef _DEBUG
    // the file is open, close it and continue debugging.
    eastl::wstring file_open_msg;
    file_open_msg.append_sprintf(L"File already open in another program: %s\nClose it and press OK.", w_filepath);
    MessageBoxW(nullptr, file_open_msg.c_str(), L"File already open", MB_OK);
    goto file_open;
#endif
    // TODO: actual error handling
  }
  _freea(w_filepath);

  ParseDataTree();

  EnterRoot();
}

NBTReader::~NBTReader()
{
  CloseCompound();
  fclose(infile_);
}

void NBTReader::EnterRoot()
{
  AddToCurrentName(root_name);
  auto const found = named_tags_.find(current_name_.c_str());
  if (found != named_tags_.end())
  {
    nesting_info_.emplace_back(NestingInfo {TAG::End, NestingInfo::ContainerType::Compound, 0, 0});
    return;
  }
  PopLatestName();
}

bool NBTReader::OpenCompound(string_view name)
{
  if (!HandleNesting(name, TAG::Compound))
  {
    return false;
  }
  auto const found = named_tags_.find(current_name_.c_str());
  if (found != named_tags_.end())
  {
    nesting_info_.emplace_back(NestingInfo {TAG::End, NestingInfo::ContainerType::Compound, 0, 0});
    return true;
  }
  PopLatestName();
  return false;
}

void NBTReader::CloseCompound()
{
  NestingInfo& nesting = nesting_info_.top();
  if (nesting.container_type == NestingInfo::ContainerType::Compound)
  {
    nesting_info_.pop();
    PopLatestName();
    return;
  }
  Trace::Error("NBTReader : Compound Close Mismatch - Attempted to close a compound when a list was not open.");
}

bool NBTReader::OpenList(string_view name)
{
  if (!HandleNesting(name, TAG::List))
  {
    return false;
  }
  auto const found = named_tags_.find(current_name_.c_str());
  if (found != named_tags_.end())
  {
    nesting_info_.emplace_back(
      NestingInfo {found->second.list_type_, NestingInfo::ContainerType::List, found->second.list_length_, 0});
    return true;
  }
  PopLatestName();
  return false;
}

int32_t NBTReader::ListSize()
{
  NestingInfo& nesting = nesting_info_.top();
  if (nesting.container_type == NestingInfo::ContainerType::List)
  {
    return nesting.length;
  }
  Trace::Error("NBTReader : Invalid List Size Read - Attempted to read list size when a list was not open.");
  return -1;
}

void NBTReader::CloseList()
{
  NestingInfo& nesting = nesting_info_.top();
  if (nesting.container_type == NestingInfo::ContainerType::List)
  {
    nesting_info_.pop();
    PopLatestName();
    return;
  }
  Trace::Error("NBTReader : List Close Mismatch - Attempted to close a list when a list was not open.");
}

int8_t NBTReader::ReadByte(string_view name)
{
  HandleNesting(name, TAG::Byte);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  assert(found->second.type_ == TAG::Byte);
  return found->second.byte_;
}

int16_t NBTReader::ReadShort(string_view name)
{
  HandleNesting(name, TAG::Short);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  assert(found->second.type_ == TAG::Short);
  return found->second.short_;
}

int32_t NBTReader::ReadInt(string_view name)
{
  HandleNesting(name, TAG::Int);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  assert(found->second.type_ == TAG::Int);
  return found->second.int_;
}

int64_t NBTReader::ReadLong(string_view name)
{
  HandleNesting(name, TAG::Long);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  assert(found->second.type_ == TAG::Long);
  return found->second.long_;
}

float NBTReader::ReadFloat(string_view name)
{
  HandleNesting(name, TAG::Float);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  assert(found->second.type_ == TAG::Float);
  return found->second.float_;
}

double NBTReader::ReadDouble(string_view name)
{
  HandleNesting(name, TAG::Double);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  assert(found->second.type_ == TAG::Double);
  return found->second.double_;
}

eastl::vector<int8_t> NBTReader::ReadByteArray(string_view name)
{
  HandleNesting(name, TAG::Byte_Array);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  assert(found->second.type_ == TAG::Byte_Array);
  eastl::vector<int8_t> ret {
    byte_array_pool_.begin() + found->second.byte_array_pool_index,
    byte_array_pool_.begin() + found->second.byte_array_length_};
  return ret;
}

string_view NBTReader::ReadString(string_view name)
{
  HandleNesting(name, TAG::String);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  assert(found->second.type_ == TAG::String);
  return string_view {
    string_pool_.begin() + found->second.string_pool_index,
    static_cast<size_t>(found->second.string_length_)};
}

eastl::optional<int8_t> NBTReader::MaybeReadByte(string_view name)
{
  HandleNesting(name, TAG::Byte);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  if (found != named_tags_.end())
  {
    assert(found->second.type_ == TAG::Byte);
    return eastl::make_optional(found->second.byte_);
  }
  return eastl::nullopt;
}

eastl::optional<int16_t> NBTReader::MaybeReadShort(string_view name)
{
  HandleNesting(name, TAG::Short);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  if (found != named_tags_.end())
  {
    assert(found->second.type_ == TAG::Short);
    return eastl::make_optional(found->second.short_);
  }
  return eastl::nullopt;
}

eastl::optional<int32_t> NBTReader::MaybeReadInt(string_view name)
{
  HandleNesting(name, TAG::Int);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  if (found != named_tags_.end())
  {
    assert(found->second.type_ == TAG::Int);
    return eastl::make_optional(found->second.int_);
  }
  return eastl::nullopt;
}

eastl::optional<int64_t> NBTReader::MaybeReadLong(string_view name)
{
  HandleNesting(name, TAG::Long);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  if (found != named_tags_.end())
  {
    assert(found->second.type_ == TAG::Long);
    return eastl::make_optional(found->second.long_);
  }
  return eastl::nullopt;
}

eastl::optional<float> NBTReader::MaybeReadFloat(string_view name)
{
  HandleNesting(name, TAG::Float);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  if (found != named_tags_.end())
  {
    assert(found->second.type_ == TAG::Float);
    return eastl::make_optional(found->second.float_);
  }
  return eastl::nullopt;
}

eastl::optional<double> NBTReader::MaybeReadDouble(string_view name)
{
  HandleNesting(name, TAG::Double);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  if (found != named_tags_.end())
  {
    assert(found->second.type_ == TAG::Double);
    return eastl::make_optional(found->second.double_);
  }
  return eastl::nullopt;
}

eastl::optional<eastl::vector<int8_t>> NBTReader::MaybeReadByteArray(string_view name)
{
  HandleNesting(name, TAG::Byte_Array);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  if (found != named_tags_.end())
  {
    assert(found->second.type_ == TAG::Byte_Array);
    eastl::vector<int8_t> ret {
      byte_array_pool_.begin() + found->second.byte_array_pool_index,
      byte_array_pool_.begin() + found->second.byte_array_length_};
    return eastl::make_optional(ret);
  }
  return eastl::nullopt;
}

eastl::optional<string_view> NBTReader::MaybeReadString(string_view name)
{
  HandleNesting(name, TAG::String);
  auto const found = named_tags_.find(current_name_.c_str());
  PopLatestName();
  if (found != named_tags_.end())
  {
    assert(found->second.type_ == TAG::String);
    return eastl::make_optional<string_view>(
      string_pool_.begin() + found->second.string_pool_index,
      static_cast<size_t>(found->second.string_length_));
  }
  return eastl::nullopt;
}

template<>
int8_t NBTReader::Read(string_view name)
{
  return ReadByte(name);
}
template<>
int16_t NBTReader::Read(string_view name)
{
  return ReadShort(name);
}
template<>
int32_t NBTReader::Read(string_view name)
{
  return ReadInt(name);
}
template<>
int64_t NBTReader::Read(string_view name)
{
  return ReadLong(name);
}
template<>
float NBTReader::Read(string_view name)
{
  return ReadFloat(name);
}
template<>
double NBTReader::Read(string_view name)
{
  return ReadDouble(name);
}
template<>
eastl::vector<int8_t> NBTReader::Read(string_view name)
{
  return ReadByteArray(name);
}
template<>
eastl::string_view NBTReader::Read(string_view name)
{
  return ReadString(name);
}

template<>
eastl::optional<int8_t> NBTReader::MaybeRead(string_view name)
{
  return MaybeReadByte(name);
}
template<>
eastl::optional<int16_t> NBTReader::MaybeRead(string_view name)
{
  return MaybeReadShort(name);
}
template<>
eastl::optional<int32_t> NBTReader::MaybeRead(string_view name)
{
  return MaybeReadInt(name);
}
template<>
eastl::optional<int64_t> NBTReader::MaybeRead(string_view name)
{
  return MaybeReadLong(name);
}
template<>
eastl::optional<float> NBTReader::MaybeRead(string_view name)
{
  return MaybeReadFloat(name);
}
template<>
eastl::optional<double> NBTReader::MaybeRead(string_view name)
{
  return MaybeReadDouble(name);
}

bool NBTReader::HandleNesting(string_view name, TAG t)
{
  NestingInfo& nesting = nesting_info_.top();
  // Lists have strict requirements
  if (nesting.container_type == NestingInfo::ContainerType::List)
  {
    if (!name.empty())
    {
      // problems, lists cannot have named tags
      assert(0);
    }
    if (nesting.data_type != t)
    {
      if (nesting.length == 0)
      {
      }
      else
      {
      }
    }
    else
    {
      if (nesting.list_index >= nesting.length)
      {
        // read too many from list
        return false;
      }
      AddIndexToCurrentName(nesting.list_index);
      ++(nesting.list_index);
    }
  }
  else if (nesting.container_type == NestingInfo::ContainerType::Compound)
  {
    if (name.empty())
    {
      // bad, compound tags must have names
      assert(0);
    }
    AddToCurrentName(name);
  }
  return true;
}

void NBTReader::ParseDataTree()
{
  // parse root tag
  TAG const type = ReadTag();
  Trace::Assert(type == TAG::Compound, SERIALIZATION, "root compound tag exists.");
  root_name = "root:" + ReadName();

  AddToCurrentName(root_name);

  ParseDataTagUnnamed(type);

  PopLatestName();

  current_name_.clear();
}

NBTReader::DataTag& NBTReader::ParseDataTag()
{
  eastl::string name;

  TAG const type = ReadTag();
  if (type != TAG::End)
  {
    name = ReadName();
  }
  AddToCurrentName(name);
  DataTag& tag = ParseDataTagUnnamed(type);
  PopLatestName();

  return tag;
}

NBTReader::DataTag& NBTReader::ParseDataTagUnnamed(TAG type)
{
  DataTag data_tag {type};
  switch (data_tag.type_)
  {
  case TAG::List:
  {
    ++parsing_nesting_depth_;
    data_tag.list_type_ = ReadTag();
    data_tag.list_length_ = ReadArrayLen();
    // read len unnamed elements
    for (int i = 0; i < data_tag.list_length_; ++i)
    {
      AddIndexToCurrentName(i);
      named_tags_.insert(current_name_.c_str(), ParseDataTagUnnamed(data_tag.list_type_));
      PopLatestName();
    }
    --parsing_nesting_depth_;
  }
  break;
  case TAG::Compound:
  {
    ++parsing_nesting_depth_;
    do
    {
    } while (ParseDataTag().type_ != TAG::End);
  }
  break;
  case TAG::End:
  {
    --parsing_nesting_depth_;
  }
  break;
  case TAG::Byte:
  {
    int8_t val;
    fread_s(&val, sizeof(val), sizeof(val), 1, infile_);
    data_tag.byte_ = val;
  }
  break;
  case TAG::Short:
  {
    int16_t val;
    fread_s(&val, sizeof(val), sizeof(val), 1, infile_);
    data_tag.short_ = htons(val);
  }
  break;
  case TAG::Int:
  {
    int32_t val;
    fread_s(&val, sizeof(val), sizeof(val), 1, infile_);
    data_tag.int_ = htonl(val);
  }
  break;
  case TAG::Long:
  {
    int64_t val;
    fread_s(&val, sizeof(val), sizeof(val), 1, infile_);
    data_tag.long_ = htonll(val);
  }
  break;
  case TAG::Float:
  {
    float val;
    fread_s(&val, sizeof(val), sizeof(val), 1, infile_);
    data_tag.float_ = htonf(val);
  }
  break;
  case TAG::Double:
  {
    double val;
    fread_s(&val, sizeof(val), sizeof(val), 1, infile_);
    data_tag.double_ = htond(val);
  }
  break;
  case TAG::String:
  {
    int16_t const length = ReadStrLen();
    size_t const insertion_point = string_pool_.size();
    string_pool_.resize(insertion_point + length + 1, '\0');
    fread_s(string_pool_.data() + insertion_point, length, sizeof(char), length, infile_);
    data_tag.string_pool_index = insertion_point;
    data_tag.string_length_ = length;
  }
  break;
  case TAG::Byte_Array:
  {
    int32_t const length = ReadArrayLen();
    size_t const insertion_point = byte_array_pool_.size();
    byte_array_pool_.resize(insertion_point + length + 1, 0);
    fread_s(byte_array_pool_.data() + insertion_point, length, sizeof(char), length, infile_);
    data_tag.byte_array_pool_index = insertion_point;
    data_tag.byte_array_length_ = length;
  }
  break;
  default:
    __debugbreak(); // ask Zach to stop being lazy and implement deserialization for the right primitive type
    break;
  }

  auto placed = named_tags_.insert(current_name_.c_str(), data_tag);

  return placed.first->second;
}

NBTReader::TAG NBTReader::ReadTag()
{
  TAG t;
  fread_s(&t, sizeof(t), sizeof(t), 1, infile_);
  return t;
}

eastl::string NBTReader::ReadName()
{
  int16_t const length = ReadStrLen();
  if (length != 0)
  {
    eastl::string name(static_cast<size_t>(length) + 1, '\0');
    fread_s(name.data(), length, sizeof(eastl::string::value_type), length, infile_);
    return name;
  }
  return "";
}

void NBTReader::AddToCurrentName(string_view name)
{
  if (!(current_name_.empty()))
  {
    current_name_ += '.';
  }
  current_name_ += name.data();
}

void NBTReader::AddIndexToCurrentName(int32_t index)
{
  eastl::string indexed;
  indexed.append_sprintf("[%d]", index);
  AddToCurrentName(indexed);
}

void NBTReader::PopLatestName()
{
  size_t const last_dot = current_name_.rfind(".");
  if (last_dot != eastl::string::npos)
  {
    current_name_.resize(last_dot);
  }
  else
  {
    current_name_.clear();
  }
}

int16_t NBTReader::ReadStrLen()
{
  int16_t len;
  fread_s(&len, sizeof(len), sizeof(len), 1, infile_);
  return htons(len);
}

int32_t NBTReader::ReadArrayLen()
{
  int32_t len;
  fread_s(&len, sizeof(len), sizeof(len), 1, infile_);
  return htonl(len);
}

} // namespace Octane
