/******************************************************************************/
/*!
  \par        Project Octane
  \file       Audio.cpp
  \author     William Fang
  \date       2020/10/03
  \brief      Audio implementation

  Copyright � 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/Audio.h>
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/Trace.h>
#include <cassert>
#include <iostream>

// defines
#define AKTEXT


// Wwise's default implementation
CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

namespace Octane
{

bool Audio::AudioInit()
{
  // Initialize memory manager
  AK::MemoryMgr::GetDefaultSettings(memSettings);
  if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
  {
    assert(!"Could not create the memory manager.");
    return false;
  }

  // Initialize streaming manager
  AK::StreamMgr::GetDefaultSettings(stmSettings);
  // Customize the Stream Manager settings here.
  if (!AK::StreamMgr::Create(stmSettings))
  {
    assert(!"Could not create the Streaming Manager");
    return false;
  }

  // Create a streaming device with blocking low-level I/O handshaking.
  AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);
  // Customize the streaming device settings here.
  // CAkFilePackageLowLevelIOBlocking::Init() creates a streaming device
  // in the Stream Manager, and registers itself as the File Location Resolver.
  if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
  {
    assert(!"Could not create the streaming device and Low-Level I/O system");
    return false;
  }

  // Create the Sound Engine
  // Using default initialization parameters
  AK::SoundEngine::GetDefaultInitSettings(initSettings);
  AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);
  if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
  {
    assert(!"Could not initialize the Sound Engine.");
    return false;
  }

  // Initialize the music engine
  // Using default initialization parameters
  AK::MusicEngine::GetDefaultInitSettings(musicInit);
  if (AK::MusicEngine::Init(&musicInit) != AK_Success)
  {
    assert(!"Could not initialize the Music Engine.");
    return false;
  }

  // Initialize Spatial Audio
  // Using default initialization parameters
  AkSpatialAudioInitSettings settings; // The constructor fills AkSpatialAudioInitSettings with the recommended default settings. 
  if (AK::SpatialAudio::Init(settings) != AK_Success)
  {
    assert(!"Could not initialize the Spatial Audio.");
    return false;
  }

#ifndef AK_OPTIMIZED
  // Initialize communications (not in release build!)
  AK::Comm::GetDefaultInitSettings(commSettings);
  if (AK::Comm::Init(commSettings) != AK_Success)
  {
    assert(!"Could not initialize communication.");
    return false;
  }
#endif // AK_OPTIMIZED

  return true;
}

void Audio::AudioUpdate()
{
  AK::SoundEngine::RenderAudio();
}

void Audio::AudioShutdown()
{
  // Shutting down Wwise will be implemented here
// Start from last initialized to first
#ifndef AK_OPTIMIZED
  // Terminate Communication Services
  AK::Comm::Term();
#endif // AK_OPTIMIZED

  // Terminate Spatial Audio
  //AK::SpatialAudio::Term();
  // SPATIAL AUDIO FOR SOME REASON DOES NOT HAVE TERMINATE FUNCTION?

  // Terminate Music Engine
  AK::MusicEngine::Term();

  // Terminate the Sound Engine
  AK::SoundEngine::Term();

  // Terminate the Streaming Device and Streaming Manager
  // CAkFilePackageLowLevelIOBlocking::Term() destroys its associated streaming device 
  // that lives in the Stream Manager, and unregisters itself as the File Location Resolver.
  g_lowLevelIO.Term();

  if (AK::IAkStreamMgr::Get())
    AK::IAkStreamMgr::Get()->Destroy();

  // Terminate the Memory Manager
  AK::MemoryMgr::Term();
}

void Audio::Set_Bank_Path(const AkOSChar * path)
{
  // Why is this needed?
  //int wchars_num = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
  //wchar_t* wstr = new wchar_t[wchars_num];
  // MultiByteToWideChar(CP_UTF8, 0, path, -1, wstr, wchars_num);
  
  //delete[] wstr;

  g_lowLevelIO.SetBasePath(path);
}

void Audio::Set_Language(const AkOSChar* language)
{
  // This too
  //int wchars_num = MultiByteToWideChar(CP_UTF8, 0, language, -1, NULL, 0);
  //wchar_t* wstr = new wchar_t[wchars_num];
  //MultiByteToWideChar(CP_UTF8, 0, language, -1, wstr, wchars_num);
  
  //delete[] wstr;

  AK::StreamMgr::SetCurrentLanguage(language);
}

const AkOSChar* Audio::Get_Language()
{
  return AK::StreamMgr::GetCurrentLanguage();
}

AkTimeMs Audio::Get_Position(AkPlayingID event)
{
  AkTimeMs position;
  if (AK::SoundEngine::GetSourcePlayPosition(event, &position) != AK_Success)
  {
    std::cerr << "Position acquisition failed!" << std::endl;
    return NULL;
  }
  return position;
}

// Before anyone asks, cerr is temporary. Can be changed to however we handle errors
AkBankID Audio::Load_Bank(const char* name)
{
  AkBankID bankID;
  
  if (AK::SoundEngine::LoadBank(name, bankID) != AK_Success)
  {
    Trace::Log(ERROR) << "Bank load '" << name << "' failed!" << std::endl;
  }

  return bankID;
}

void Audio::Unload_Bank(const char* name)
{
  if (AK::SoundEngine::UnloadBank(name, NULL) != AK_Success)
  {
    Trace::Log(ERROR) << "Bank unload '" << name << "' failed!" << std::endl;
  }
}

void Audio::Play_Event(AkUniqueID UniqueID, AkGameObjectID GameObjectID)
{
  if (AK::SoundEngine::PostEvent(UniqueID, GameObjectID) != AK_Success)
  {
    Trace::Log(ERROR) << "Event posting '" << UniqueID << "' failed!" << std::endl;
  }
}

AkPlayingID Audio::Play_Event_RI(AkUniqueID UniqueID, AkGameObjectID GameObjectID)
{
  AkPlayingID PlayingID;
  PlayingID = AK::SoundEngine::PostEvent(UniqueID, GameObjectID, AK_EnableGetSourcePlayPosition, NULL, NULL);

  if (PlayingID == NULL)
  {
    Trace::Log(ERROR) << "Event posting '" << UniqueID << "' failed!" << std::endl;
  }

  return PlayingID;
}

void Audio::Register_Object(AkGameObjectID id, const char* name)
{
  if (AK::SoundEngine::RegisterGameObj(id, name) != AK_Success)
  {
    Trace::Log(ERROR) << "Object registration '" << name << "' failed!" << std::endl;
  }
}

void Audio::Unregister_Object(AkGameObjectID id)
{
  if (AK::SoundEngine::UnregisterGameObj(id) != AK_Success)
  {
    Trace::Log(ERROR) << "Unregistering object '" << id << "' failed!" << std::endl;
  }
}

void Audio::Unregister_All_Objects()
{
  if (AK::SoundEngine::UnregisterAllGameObj() != AK_Success)
  {
    Trace::Log(ERROR) << "Unregistering all objects failed!" << std::endl;
  }
}

void Audio::Set_Position(AkGameObjectID object, const AkSoundPosition& position_)
{
  AkSoundPosition position;
  // These things don't work? Need to figure out how new version works
  /*
  position.Position.X = position_.Position.X;
  position.Position.Y = position_.Position.Y;
  position.Position.Z = position_.Position.Z;
  position.OrientationFront = -1; // Not sure if this is correct yet, requires testing!
  position.OrientationTop = 0;
  */
  
  if (AK::SoundEngine::SetPosition(object, position) != AK_Success)
  {
    Trace::Log(ERROR) << "Position setting failed!" << std::endl;
  }
}

void Audio::Set_Multiple_Positions(AkGameObjectID object, const AkSoundPosition* position_, int NumPositions)
{
  if (AK::SoundEngine::SetMultiplePositions(object, position_, NumPositions) != AK_Success)
  {
    Trace::Log(ERROR) << "Multiple position setting failed!" << std::endl;
  }
}

void Audio::Suspend(bool partial)
{
  AK::SoundEngine::Suspend(partial);
}

void Audio::Unsuspend()
{
  AK::SoundEngine::WakeupFromSuspend();
}

void Audio::Restart_Render()
{
  AK::SoundEngine::RenderAudio();
}


Audio::Audio(Engine* parent_engine) : ISystem(parent_engine)
{
  if (!AudioInit())
  {
    Trace::Log(ERROR) << "Audio Engine failed initializing!" << std::endl;
  }
}

void Audio::Load()
{
}

void Audio::LevelStart()
{
}

void Audio::Update()
{
  AudioUpdate();
}

void Audio::LevelEnd()
{
}

void Audio::Unload()
{
  AudioShutdown();
}

SystemOrder Audio::GetOrder()
{
  return SystemOrder::Audio;
}

}
