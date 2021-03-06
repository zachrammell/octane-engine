﻿/******************************************************************************/
/*!
  \par        Project Octane
  \file       Audio.h
  \author     William Fang
  \date       2020/10/03
  \brief      Audio implementation

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once
// Includes
#include <AK/SoundEngine/Common/AkSoundEngine.h>                           // Core Engine
#include <AK/IBytes.h>
#include <AK/SoundEngine/Common/AkMemoryMgr.h>                             // Memory Manager interface
#include <AK/SoundEngine/Common/AkModule.h>                                // Default memory manager
#include <AK/SoundEngine/Common/IAkStreamMgr.h>                            // Streaming Manager
#include <AK/Tools/Common/AkPlatformFuncs.h>                               // Thread defines
#include <SampleSoundEngine/Win32/AkFilePackageLowLevelIOBlocking.h>       // Sample low-level I/O implementation
#include <AK/MusicEngine/Common/AkMusicEngine.h>                           // Music Engine
#include <AK/SpatialAudio/Common/AkSpatialAudio.h>                         // Spatial Audio
#include <OctaneEngine/ISystem.h>
#include "Wwise_IDs.h"



// Defines
#ifndef _DEBUG
#define AK_OPTIMIZED
#endif

#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h> // Communications, not for release!
#endif // AK_OPTIMIZED



namespace Octane
{

class Audio : public ISystem
{
public:

  explicit Audio(class Engine* parent_engine);
  ~Audio();

  void Load() override;
  void LevelStart() override;
  void Update() override;
  void LevelEnd() override;
  void Unload() override;

  static SystemOrder GetOrder();

  void music_play();
  void music_stop();
  void ambience_start();
  void ambience_stop();

private:
  // Variables
  AkMemSettings memSettings;
  AkStreamMgrSettings stmSettings;
  AkDeviceSettings deviceSettings;
  AkInitSettings initSettings;
  AkPlatformInitSettings platformInitSettings;
  AkMusicSettings musicInit;

#ifndef AK_OPTIMIZED
  AkCommSettings commSettings;
#endif // AK_OPTIMIZED
  
  // Possibly temporary
  AkBankID init;
  AkBankID main;

  // Test object
  AkGameObjectID Player;
  AkGameObjectID test_emitter;
  AkGameObjectID test_listener;

  bool AudioInit();
  void AudioUpdate();
  void AudioShutdown();

  // Setters
  void Set_Bank_Path(const AkOSChar*);
  void Set_Language(const AkOSChar*); // PASS IN "English(US)"

  // Getters
  const AkOSChar* Get_Language(); // Not sure if the return value is convertible to const char * yet, but we'll see
  AkTimeMs Get_Position(AkPlayingID);

  // Banks
  AkBankID Load_Bank(const wchar_t*);
  void Unload_Bank(const char*);

  // Events
  void Play_Event(AkUniqueID, AkGameObjectID);
  AkPlayingID Play_Event_RI(AkUniqueID, AkGameObjectID);

  // Game Objects
  // Every Game Object must have a AkGameObjectID if sound wants to be associated to the object
  void Register_Object(AkGameObjectID, const char*);
  void Unregister_Object(AkGameObjectID);
  void Unregister_All_Objects();

  // Position
  // AkSoundPosition needs to be changed to whatever vector format we're using
  void Set_Position(AkGameObjectID);
  
  void Set_Multiple_Positions(AkGameObjectID, const AkSoundPosition*, int);

  // Listeners
  void Set_Default_Listener(const AkGameObjectID*, AkUInt32);

  // Handle tabbing out
  // True = Partial on; Partial means to keep processing sound events
  // False = Completely suspend all sound
  void Suspend(bool partial);
  void Unsuspend();
  void Restart_Render(); // This is called shortly after Unsuspend, may not be necessary since it's normally called every frame
};


}


// Current to do:
// Move static class into its own file

