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
#include <OctaneEngine/FormattedOutput.h>
#include <cassert>
#include <iostream>

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

  // Set bank path
  Set_Bank_Path(AKTEXT("assets/soundbanks/"));
  Set_Language(AKTEXT("English(US)")); // Remember, we have to wrap all text in AKTEXT

  // Test but load our two banks
  init = Load_Bank(AKTEXT("Init.bnk"));
  main = Load_Bank(AKTEXT("Main.bnk"));

  // Register our test objects
  test_object = 100;
  test_listener = 0;
  test_emitter = 1;
  Register_Object(test_object, "test");
  Register_Object(test_listener, "Listener");
  Register_Object(test_emitter, "Emitter");

  // Set listener and emitter
  Set_Default_Listener(&test_listener, 1);

  Set_Position(test_listener);
  Set_Position(test_object);

  return true;
}

void Audio::AudioUpdate()
{
  if (AK::SoundEngine::RenderAudio() != AK_Success)
  {
    Trace::Log(ERROR) << "Audio Rendering failed!" << std::endl;
  }
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

void Audio::Set_Bank_Path(const AkOSChar* path)
{
  g_lowLevelIO.SetBasePath(path);
}

void Audio::Set_Language(const AkOSChar* language)
{
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
    Trace::Log(ERROR) << "Position acquisition failed!" << std::endl;
    return NULL;
  }
  return position;
}

AkBankID Audio::Load_Bank(const wchar_t* name)
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
  using FormattedOutput::Set;
  using FormattedOutput::ColorANSI;
  if (AK::SoundEngine::PostEvent(UniqueID, GameObjectID) == AK_INVALID_PLAYING_ID)
  {
    Trace::Log(ERROR) << "Event posting '" << UniqueID << "' failed!" << std::endl;
  }
  else
  {
    Trace::Log(DEBUG) << "Playing audio event #"
                      << Set(ColorANSI::Yellow) << UniqueID << Set() << " at object ID #"
                      << Set(ColorANSI::Lime) << GameObjectID << Set() << std::endl;
  }
}

AkPlayingID Audio::Play_Event_RI(AkUniqueID UniqueID, AkGameObjectID GameObjectID)
{
  AkPlayingID PlayingID;
  PlayingID = AK::SoundEngine::PostEvent(UniqueID, GameObjectID, AK_EnableGetSourcePlayPosition, NULL, NULL);

  if (PlayingID == AK_INVALID_PLAYING_ID)
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

void Audio::Set_Position(AkGameObjectID object)
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

  // Set location ontop of origin
  position.SetPosition(0, 0, 0);
  position.SetOrientation(0, 0, -1, 0, 1, 0);
  
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

void Audio::Set_Default_Listener(const AkGameObjectID* id, AkUInt32 count)
{
  if (AK::SoundEngine::SetDefaultListeners(id, count) != AK_Success)
  {
    Trace::Log(ERROR) << "Default Listener " << *id << " setting failed!" << std::endl;
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

Audio::~Audio()
{
  AudioShutdown();
}

void Audio::Load()
{
  
}

void Audio::LevelStart()
{
  ambience_start();
  //button_sound();
  music_play();
}

void Audio::Update()
{
  AudioUpdate();
}

void Audio::LevelEnd()
{
  ambience_stop();
  //back_sound();
  music_stop();
}

void Audio::Unload()
{
}

SystemOrder Audio::GetOrder()
{
  return SystemOrder::Audio;
}

void Audio::button_sound()
{
  Play_Event(AK::EVENTS::PLAY_BUTTONSELECT, test_object);
}

void Audio::back_sound()
{
  Play_Event(AK::EVENTS::PLAY_BUTTONBACK, test_object);
}
void Audio::music_play()
{
  Play_Event(AK::EVENTS::TESTMUSIC, test_object);
}
void Audio::music_stop()
{
  Play_Event(AK::EVENTS::STOP_TESTMUSIC, test_object);
}
void Audio::ambience_start()
{
  Play_Event(AK::EVENTS::PLAY_AMBIENCE, test_object);
}
void Audio::ambience_stop()
{
  Play_Event(AK::EVENTS::STOP_AMBIENCE, test_object);
}


}
