/******************************************************************************/
/*!
  \par        Project Octane
  \file       Health.h
  \author     Lowell Novitch
  \date       2020/10/20
  \brief      Class for HP for entities

  Copyright © 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/
#pragma once

// Includes

namespace Octane
{

class Health
{
public:
  Health(int maxHP);

  int GetCurrentHP() const;
  int GetMaxHP() const;

  bool is_dead() const;

  //sets the value of maxHp 
  //brings down currentHP if greater than newMax
  void set_maxHP(int newMax);

  //change the currentHP by given value,
  //maxes at maxHP
  //returns true when currentHP is 0
  bool ChangeCurrentHPby(int changeOfHP);

  //sets currentHP to given value and put in the range [0,maxHP]
  void setCurrentHP(int newCurrent);

private:
  int maxHP_;
  int currentHP_;


};
}
