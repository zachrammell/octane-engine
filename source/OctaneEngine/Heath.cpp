/******************************************************************************/
/*!
  \par        Project Octane
  \file       Health.cpp
  \author     Lowell Novitch
  \date       2020/10/20
  \brief      Class for HP for entities

  Copyright � 2020 DigiPen, All rights reserved.
*/
/******************************************************************************/

// Main include
#include <OctaneEngine/Heath.h>
namespace Octane
{
Health::Health(int maxHP) :maxHP_(maxHP), currentHP_(maxHP)
{}

int Health::GetCurrentHP() const 
{
  return currentHP_;
}

int Health::GetMaxHP() const
{
  return maxHP_;
}

bool Health::is_dead() const 
{
  return currentHP_ == 0;
}

//sets the value of maxHp
//brings down currentHP if greater than newMax
void Health::set_maxHP(int newMax) 
{
  maxHP_ = newMax;

  if (currentHP_> newMax)
  {
    currentHP_ = newMax;
  }
}

//change the currentHP by given value,
//maxes at maxHP
//returns true when currentHP is 0
bool Health::ChangeCurrentHPby(int changeOfHP) 
{
  currentHP_ -= changeOfHP;

  if (currentHP_ > maxHP_)
  {
    currentHP_ = maxHP_;
  }

  if (currentHP_ <= 0)
  {
    currentHP_ = 0;
  }

  return is_dead();
}

 //sets currentHP to given value and put in the range [0,maxHP]
void Health::setCurrentHP(int newCurrent) 
{
  currentHP_ = newCurrent;

  if (currentHP_ > maxHP_)
  {
    currentHP_ = maxHP_;
  }

  if (currentHP_ <= 0)
  {
    currentHP_ = 0;
  }
}
}
