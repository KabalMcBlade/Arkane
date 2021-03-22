// Demo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include "App.h"

#include <iostream>


AK_USING_NAMESPACE

static constexpr uint32_t kWidth = 1024;
static constexpr uint32_t kHeight = 768;


int main(int argc, char** argv)
{
	CommandLineParser* cmdLine = new CommandLineParser;
	cmdLine->AddWithValueAndDefaultUnsigned("-width", false, kWidth);
	cmdLine->AddWithValueAndDefaultUnsigned("-height", false, kHeight);
	cmdLine->AddWithValueAndDefaultBool("-fullscreen", false, false);
	cmdLine->AddWithValueAndDefaultBool("-showcursor", false, true);
	cmdLine->Parse(argc, argv);

	FileSystem* fileSystem = new FileSystem("Assets", "Shaders", "Textures", "Models");

	App demo("Test", VK_MAKE_VERSION(0, 0, 1), cmdLine, fileSystem);

	demo.Execute();

	delete fileSystem;
	delete cmdLine;
	
	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
