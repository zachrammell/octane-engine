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
  bool AudioPlayer::Hover_Bool = true;

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
      // Playing works. Disabling this debug for now as to not clog the output
      /*
      Trace::Log(DEBUG) << "Playing audio event #"
        << Set(ColorANSI::Yellow) << UniqueID << Set() << " at object ID #"
        << Set(ColorANSI::Lime) << 100 << Set() << std::endl;*/
    }
  }
  void AudioPlayer::Play_Event(AkUniqueID UniqueID, AkGameObjectID object)
  {
    using FormattedOutput::Set;
    using FormattedOutput::ColorANSI;
    if (AK::SoundEngine::PostEvent(UniqueID, object) == AK_INVALID_PLAYING_ID)
    {
      Trace::Log(ERROR) << "Event posting '" << UniqueID << "' failed!" << std::endl;
    }
    else
    {
      /*
      Trace::Log(DEBUG) << "Playing audio event #"
        << Set(ColorANSI::Yellow) << UniqueID << Set() << " at object ID #"
        << Set(ColorANSI::Lime) << object << Set() << std::endl; */
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
  void AudioPlayer::Play_Once(AkUniqueID UniqueID)
  {
    if (Hover_Bool)
    {
      using FormattedOutput::Set;
      using FormattedOutput::ColorANSI;
      if (AK::SoundEngine::PostEvent(UniqueID, 100) == AK_INVALID_PLAYING_ID)
      {
        Trace::Log(ERROR) << "Event posting '" << UniqueID << "' failed!" << std::endl;
      }
      else
      {
        // Playing works. Disabling this debug for now as to not clog the output
        /*
        Trace::Log(DEBUG) << "Playing audio event #"
          << Set(ColorANSI::Yellow) << UniqueID << Set() << " at object ID #"
          << Set(ColorANSI::Lime) << 100 << Set() << std::endl;*/
      }
      Hover_Bool = false;
    }
  }
  void AudioPlayer::Reset_Hover()
  {
    Hover_Bool = true;
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
    position.SetPosition(0, 0, 0);
    position.SetOrientation(0, 0, -1, 0, 1, 0);

    if (AK::SoundEngine::SetPosition(object, position) != AK_Success)
    {
      Trace::Log(ERROR) << "Position setting failed!" << std::endl;
    }
  }

  void AudioPlayer::Set_Position(AkGameObjectID object, float x, float y, float z)
  {
    AkSoundPosition position;
    position.SetPosition(x, y, z);
    position.SetOrientation(0, 0, -1, 0, 1, 0);

    if (AK::SoundEngine::SetPosition(object, position) != AK_Success)
    {
      Trace::Log(ERROR) << "Position setting failed!" << std::endl;
    }
  }

  void AudioPlayer::Set_Position(AkGameObjectID object, DirectX::XMFLOAT3 pos)
  {
    AkSoundPosition position;
    position.SetPosition(pos.x, pos.y, pos.z);
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

  void AudioPlayer::Set_Listener(AkGameObjectID emitter, const AkGameObjectID* listeners, AkUInt32 count)
  {
    if (AK::SoundEngine::SetListeners(emitter, listeners, count))
    {
      Trace::Log(ERROR) << "Listener " << *listeners << " setting failed with emitter: " << emitter << std::endl;
    }
  }

  void AudioPlayer::Set_Value(AkRtpcID RTCP, AkRtpcValue value, AkGameObjectID object = AK_INVALID_GAME_OBJECT)
  {
    if (AK::SoundEngine::SetRTPCValue(RTCP, value, object))
    {
      Trace::Log(ERROR) << "Setting " << RTCP << " to " << value << "failed!" << std::endl;
    }
  }

  void AudioPlayer::Set_Value(const char* name, AkRtpcValue value, AkGameObjectID object = AK_INVALID_GAME_OBJECT)
  {
    if (AK::SoundEngine::SetRTPCValue(name, value, object))
    {
      Trace::Log(ERROR) << "Setting " << name << " to " << value << "failed!" << std::endl;
    }
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

  void AudioPlayer::Load_Player()
  {
    Register_Object(Player, "Player");
    Set_Default_Listener(&Player, 1);
    Set_Position(Player);
  }

  void AudioPlayer::Unload_Player()
  {
    Unregister_Object(Player);
  }

  void AudioPlayer::Update_Player(DirectX::XMFLOAT3 pos)
  {
    Set_Position(Player, pos);
  }
}
