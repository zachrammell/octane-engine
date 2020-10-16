#pragma once
#include <OctaneEngine/Scenes/TestScene.h>
#include <OctaneEngine/Scenes/MenuScene.h>


namespace Octane
{
	//must be in same order as added to the scene holder in scenesys
enum class SceneE : unsigned int
{
	MenuScene,
	TestScene,

	COUNT
};
}