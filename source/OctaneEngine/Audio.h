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
#include "Wwise_IDs.h"


// Defines
//#define AK_OPTIMIZED

#ifndef AK_OPTIMIZED
#include <AK/Comm/AkCommunication.h> // Communications, not for release!
#endif // AK_OPTIMIZED

namespace Octane
{


static class Audio
{
public:
  static bool Audio_Init();
  static void Audio_Update();
  static void Audio_Shutdown();
  
private:
  // Setters
  static void Set_Bank_Path(const AkOSChar*);
  static void Set_Language(const AkOSChar*); // PASS IN "English(US)"

  // Getters
  static const AkOSChar* Get_Language(); // Not sure if the return value is convertible to const char * yet, but we'll see
  static AkTimeMs Get_Position(AkPlayingID);

  // Banks
  static AkBankID Load_Bank(const char*);
  static void Unload_Bank(const char*);

  // Events
  static void Play_Event(AkUniqueID, AkGameObjectID);
  static AkPlayingID Play_Event_RI(AkUniqueID, AkGameObjectID);

  // Game Objects
  // Every Game Object must have a AkGameObjectID if sound wants to be associated to the object
  static void Register_Object(AkGameObjectID, const char*);
  static void Unregister_Object(AkGameObjectID);
  static void Unregister_All_Objects();

  // Position
  // AkSoundPosition needs to be changed to whatever vector format we're using
  static void Set_Position(AkGameObjectID, const AkSoundPosition&);
  static void Set_Multiple_Positions(AkGameObjectID, const AkSoundPosition*, int);

  // Handle tabbing out
  // True = Partial on; Partial means to keep processing sound events
  // False = Completely suspend all sound
  static void Suspend(bool partial);
  static void Unsuspend();
  static void Restart_Render(); // This is called shortly after Unsuspend, may not be necessary since it's normally called every frame
};
}

// Current to do:
// Put Init, Update, and Shutdown into game loop
// register a game object
// use Wwise IDs and event playing and game object to play a test sound
// Implement the rest of the functions for now
