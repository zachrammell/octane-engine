/******************************************************************************/
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

// Defines
#define AK_OPTIMIZED

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

  // Banks
  static AkBankID Load_Bank(const char * name);

  // Handle tabbing out
  // True = Partial on; Partial means to keep processing sound events
  // False = Completely suspend all sound
  static void Suspend(bool partial);
  static void Unsuspend();
  static void Restart_Render(); // This is called shortly after Unsuspend
};
}
