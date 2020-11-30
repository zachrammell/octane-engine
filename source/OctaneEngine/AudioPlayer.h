/******************************************************************************/
/*!
  \par        Project Octane
  \file       AudioPlayer.h
  \author     William Fang
  \date       2020/11/06
  \brief      Static class that scenes can access

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <AK/SoundEngine/Common/AkSoundEngine.h>                           // Core Engine
#include <DirectXMath.h>
#include "Wwise_IDs.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h> // Core Engine

namespace Octane
{
  class AudioPlayer
  {
  public:
    // Banks
    static AkBankID Load_Bank(const wchar_t*);
    static void Unload_Bank(const char*);

    // Events
    static void Play_Event(AkUniqueID);
    static void Play_Event(AkUniqueID, AkGameObjectID);
    static AkPlayingID Play_Event_RI(AkUniqueID, AkGameObjectID);
    static void Play_Once(AkUniqueID);

    static void Reset_Hover();

  // Game Objects
  // Every Game Object must have a AkGameObjectID if sound wants to be associated to the object
  static void Register_Object(AkGameObjectID, const char*);
  static void Unregister_Object(AkGameObjectID);
  static void Unregister_All_Objects();

    // Position
    // AkSoundPosition needs to be changed to whatever vector format we're using
    static void Set_Position(AkGameObjectID);
    static void Set_Position(AkGameObjectID, DirectX::XMFLOAT3);
    static void Set_Position(AkGameObjectID, float x, float y, float z);
    
    static void Set_Multiple_Positions(AkGameObjectID, const AkSoundPosition*, int);

    // Listeners
    static void Set_Default_Listener(const AkGameObjectID*, AkUInt32);
    static void Set_Listener(AkGameObjectID, const AkGameObjectID*, AkUInt32);

    // RTCP functions for later use
    static void Set_Value(AkRtpcID, AkRtpcValue, AkGameObjectID);
    static void Set_Value(const char*, AkRtpcValue, AkGameObjectID);

    // I need to change how this is done later
    // Handle tabbing out
    // True = Partial on; Partial means to keep processing sound events
    // False = Completely suspend all sound
    static void Suspend(bool partial);
    static void Unsuspend();
    static void Restart_Render(); // This is called shortly after Unsuspend, may not be necessary since it's normally called every frame

    // To handle players
    static void Load_Player();
    static void Unload_Player();
    static void Update_Player(DirectX::XMFLOAT3);

    // Audio Setting
    static void Set_Music();
    static void Set_SFX();

    static bool Hover_Bool;
  private:
    static const AkGameObjectID Player = 100;
    
  };
}

// need to make a player object and constantly set this to where the player is every frame
// should this be done in audio or audioplayer?
// afterward make it so spawner 