/******************************************************************************/
/*!
  \par        Project Octane
  \file       Audio.cpp
  \author     William Fang
  \date       2020/10/03
  \brief      Audio implementation

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/Audio.h>
#include <cassert>

// defines
#define AKTEXT


// Wwise's default implementation
CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

namespace Octane
{
bool Audio::Audio_Init()
{
  // Initialize memory manager
  AkMemSettings memSettings;
  AK::MemoryMgr::GetDefaultSettings(memSettings);
  if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
  {
    assert(!"Could not create the memory manager.");
    return false;
  }

  // Initialize streaming manager
  AkStreamMgrSettings stmSettings;
  AK::StreamMgr::GetDefaultSettings(stmSettings);
  // Customize the Stream Manager settings here.
  if (!AK::StreamMgr::Create(stmSettings))
  {
    assert(!"Could not create the Streaming Manager");
    return false;
  }

  // Create a streaming device with blocking low-level I/O handshaking.
  AkDeviceSettings deviceSettings;
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
  AkInitSettings initSettings;
  AkPlatformInitSettings platformInitSettings;
  AK::SoundEngine::GetDefaultInitSettings(initSettings);
  AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);
  if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
  {
    assert(!"Could not initialize the Sound Engine.");
    return false;
  }

  // Initialize the music engine
  // Using default initialization parameters
  AkMusicSettings musicInit;
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
  AkCommSettings commSettings;
  AK::Comm::GetDefaultInitSettings(commSettings);
  if (AK::Comm::Init(commSettings) != AK_Success)
  {
    assert(!"Could not initialize communication.");
    return false;
  }
#endif // AK_OPTIMIZED

  return true;
}

void Audio::Audio_Update()
{
  // Recurring call to process everything
  AK::SoundEngine::RenderAudio();
}

void Audio::Audio_Shutdown()
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
  g_lowLevelIO.SetBasePath(AKTEXT(path));
}

void Audio::Set_Language(const AkOSChar* language)
{
  AK::StreamMgr::SetCurrentLanguage(AKTEXT(language));
}

const AkOSChar* Audio::Get_Language()
{
  return AK::StreamMgr::GetCurrentLanguage();
}

AkBankID Audio::Load_Bank(const char* name)
{
  AkBankID bankID; // Supposedly this is not used, but we will put this here in case
  AKRESULT eResult = AK::SoundEngine::LoadBank(name, bankID);
  assert(eResult == AK_Success);

  return bankID;
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


}
