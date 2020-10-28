/******************************************************************************/
/*!
  \par        Project Octane
  \file       NBTReader.cpp
  \author     Zach Rammell
  \date       2020/10/23
  \brief      Implementation for NBT deserialization

  Copyright � 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/NBTReader.h>

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>

#include <cassert>

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
    __debugbreak();
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
  ExitCompound();
  fclose(infile_);
}

void NBTReader::EnterRoot()
{
  AddToCurrentName("root");
  auto const found = named_tags_.find(current_name_.c_str());
  if (found != named_tags_.end())
  {
    nesting_info_.emplace_back(NestingInfo {TAG::End, NestingInfo::ContainerType::Compound, 0, 0});
    return;
  }
  PopLatestName();
}

bool NBTReader::EnterCompound(string_view name)
{
  HandleNesting(name, TAG::Compound);
  auto const found = named_tags_.find(current_name_.c_str());
  if (found != named_tags_.end())
  {
    nesting_info_.emplace_back(NestingInfo {TAG::End, NestingInfo::ContainerType::Compound, 0, 0});
    return true;
  }
  PopLatestName();
  return false;
}

void NBTReader::ExitCompound()
{
  PopLatestName();
}

bool NBTReader::EnterList(string_view name)
{
  HandleNesting(name, TAG::List);
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
  return named_tags_.find(current_name_.c_str())->second.list_length_;
}

void NBTReader::ExitList()
{
  nesting_info_.pop();
  PopLatestName();
}

float NBTReader::ReadFloat(string_view name)
{
  HandleNesting(name, TAG::Float);
  float const ret = named_tags_.find(current_name_.c_str())->second.float_;
  PopLatestName();
  return ret;
}

int32_t NBTReader::ReadInt(string_view name)
{
  HandleNesting(name, TAG::Int);
  int32_t const ret = named_tags_.find(current_name_.c_str())->second.int_;
  PopLatestName();
  return ret;
}

void NBTReader::HandleNesting(string_view name, TAG t)
{
  NestingInfo& nesting = nesting_info_.top();
  // Lists have strict requirements
  if (nesting.container_type == NestingInfo::ContainerType::List)
  {
    if (!name.empty())
    {
      // problems
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
      AddToCurrentName(eastl::to_string(nesting.list_index));
      ++(nesting.list_index);
    }
  }
  else if (nesting.container_type == NestingInfo::ContainerType::Compound)
  {
    if (name.empty())
    {
      // bad
      assert(0);
    }
    AddToCurrentName(name);
  }
}

void NBTReader::ParseDataTree()
{
  do
  {
    ParseDataTag();
  } while (parsing_nesting_depth_ > 0);

  current_name_.clear();
}

NBTReader::DataTag& NBTReader::ParseDataTag()
{
  eastl::string name = "";

  TAG type = ReadTag();
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
      AddToCurrentName(eastl::to_string(i));
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
  case TAG::Int:
  {
    int32_t val;
    fread_s(&val, sizeof(val), sizeof(val), 1, infile_);
    data_tag.int_ = htonl(val);
  }
  break;
  case TAG::Float:
  {
    float val;
    fread_s(&val, sizeof(val), sizeof(val), 1, infile_);
    data_tag.float_ = htonf(val);
  }
  break;
  default:
    __debugbreak(); // uh oh
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
  eastl::string name(static_cast<size_t>(length) + 1, '\0');
  fread_s(name.data(), length, sizeof(eastl::string::value_type), length, infile_);
  return name;
}

void NBTReader::AddToCurrentName(string_view name)
{
  if (!(current_name_.empty()))
  {
    current_name_ += '.';
  }
  current_name_ += name.data();
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