/******************************************************************************/
/*!
  \par        Project Octane
  \file       IBehavior.h
  \author     Lowell Novitch
  \date       2020/10/23
  \brief      behavior interface class

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes
#include <OctaneEngine/ComponentSys.h>
#include <OctaneEngine/EntityID.h>

namespace Octane
{
class BehaviorSys;
//class ComponentHandle;

class IBehavior
{
public:
  explicit IBehavior(class BehaviorSys* parent, ComponentHandle handle) : parent_sys {*parent}, handle_(handle) {};

  virtual ~IBehavior() = default;

  virtual void Initialize() = 0;
  virtual void Update(float dt, EntityID myid) = 0;
  virtual void Shutdown() = 0;

  template<class System>
  System* Get()
  {
    static_assert(eastl::is_base_of_v<ISystem, System>);
    return parent_sys.Get<System>();
  }

protected:
  BehaviorSys& parent_sys;
  ComponentHandle handle_;
};

} // namespace Octane
