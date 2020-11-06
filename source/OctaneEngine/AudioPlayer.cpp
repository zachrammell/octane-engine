/******************************************************************************/
/*!
  \par        Project Octane
  \file       AudioPlayer.cpp
  \author     William Fang
  \date       YYYY/MM/DD
  \brief      <WHAT DOES IT DO>

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include "AudioPlayer.h"
#include <OctaneEngine/SystemOrder.h>
#include <OctaneEngine/Trace.h>
#include <OctaneEngine/FormattedOutput.h>
#include <cassert>
#include <iostream>

namespace Octane
{
  AkBankID AudioPlayer::Load_Bank(const wchar_t* name)
  {
    AkBankID bankID;

    if (AK::SoundEngine::LoadBank(name, bankID) != AK_Success)
    {
      Trace::Log(ERROR) << "Bank load '" << name << "' failed!" << std::endl;
    }

    return bankID;
  }
  void AudioPlayer::Unload_Bank(const char* name)
  {
    if (AK::SoundEngine::UnloadBank(name, NULL) != AK_Success)
    {
      Trace::Log(ERROR) << "Bank unload '" << name << "' failed!" << std::endl;
    }
  }
  void AudioPlayer::Play_Event(AkUniqueID UniqueID)
  {
    using FormattedOutput::Set;
    using FormattedOutput::ColorANSI;
    if (AK::SoundEngine::PostEvent(UniqueID, 100) == AK_INVALID_PLAYING_ID)
    {
      Trace::Log(ERROR) << "Event posting '" << UniqueID << "' failed!" << std::endl;
    }
    else
    {
      Trace::Log(DEBUG) << "Playing audio event #"
        << Set(ColorANSI::Yellow) << UniqueID << Set() << " at object ID #"
        << Set(ColorANSI::Lime) << 100 << Set() << std::endl;
    }
  }
  AkPlayingID AudioPlayer::Play_Event_RI(AkUniqueID UniqueID, AkGameObjectID GameObjectID)
  {
    AkPlayingID PlayingID;
    PlayingID = AK::SoundEngine::PostEvent(UniqueID, GameObjectID, AK_EnableGetSourcePlayPosition, NULL, NULL);

    if (PlayingID == AK_INVALID_PLAYING_ID)
    {
      Trace::Log(ERROR) << "Event posting '" << UniqueID << "' failed!" << std::endl;
    }

    return PlayingID;
  }
  void AudioPlayer::Register_Object(AkGameObjectID id, const char* name)
  {
    if (AK::SoundEngine::RegisterGameObj(id, name) != AK_Success)
    {
      Trace::Log(ERROR) << "Object registration '" << name << "' failed!" << std::endl;
    }
  }
  void AudioPlayer::Unregister_Object(AkGameObjectID id)
  {
    if (AK::SoundEngine::UnregisterGameObj(id) != AK_Success)
    {
      Trace::Log(ERROR) << "Unregistering object '" << id << "' failed!" << std::endl;
    }
  }
  void AudioPlayer::Unregister_All_Objects()
  {
    if (AK::SoundEngine::UnregisterAllGameObj() != AK_Success)
    {
      Trace::Log(ERROR) << "Unregistering all objects failed!" << std::endl;
    }
  }
  void AudioPlayer::Set_Position(AkGameObjectID object)
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

  void AudioPlayer::Set_Multiple_Positions(AkGameObjectID object, const AkSoundPosition* position_, int NumPositions)
  {
    if (AK::SoundEngine::SetMultiplePositions(object, position_, NumPositions) != AK_Success)
    {
      Trace::Log(ERROR) << "Multiple position setting failed!" << std::endl;
    }
  }

  void AudioPlayer::Set_Default_Listener(const AkGameObjectID* id, AkUInt32 count)
  {
    if (AK::SoundEngine::SetDefaultListeners(id, count) != AK_Success)
    {
      Trace::Log(ERROR) << "Default Listener " << *id << " setting failed!" << std::endl;
    }
  }

  void AudioPlayer::Set_Listener(AkGameObjectID, const AkGameObjectID*, AkUInt32)
  {

  }

  void AudioPlayer::Suspend(bool partial)
  {
    AK::SoundEngine::Suspend(partial);
  }

  void AudioPlayer::Unsuspend()
  {
    AK::SoundEngine::WakeupFromSuspend();
  }

  void AudioPlayer::Restart_Render()
  {
    AK::SoundEngine::RenderAudio();
  }
}
