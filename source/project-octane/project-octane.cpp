#include "project-octane/project-octane.h"

#include <iostream>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <EASTL/vector.h>

// EASTL expects user-defined new[] operators that it will use for memory allocation.
// TODO: make these return already-allocated memory from our own memory allocator.
void* operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
	return new uint8_t[size];
}

void* operator new[](size_t size, size_t alignment, size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line)
{
	return new uint8_t[size];
}

int main(int argc, char* argv[])
{
	// must be the first thing, for SDL2 initialization
	SDL_SetMainReady();

	std::cout << "[== Project Octane ==]\n";

	eastl::vector<int> some_ints{ 0, 1, 42, 69, 420, 666 };

	// the EASTL does not throw exceptions - this will trigger a breakpoint but it will not crash the program
	int garbage = some_ints.at(7);

	std::cout << "Garbage, for your viewing pleasure: " << garbage << "\n";

	std::cout << "Some numbers: ";
	for (int element : some_ints)
	{
		std::cout << element << " ";
	}
	std::cout << "\n";

	return 0;
}
