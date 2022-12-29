// SmallestBoundingBox.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SandboxFramework.h>
#include <PlayerInput.h>
#include <SandboxGraphics.h>
#include <PlayerInput.h>
void SimAlgorithm();
void CleanAll();

int main(int argc, char* args[])
{
    std::cout << "Bounding Box!\n";
    SandboxFramework::shouldDrawCordinateLines = true;
    SandboxFramework::BindEvents(NULL, NULL, CleanAll);

    SandboxFramework::sandboxInit(1000, 700, "Bounding Box");

    SimAlgorithm();
    //auto line = SandboxGraphics::MakeLine(20, 0, 455, 100);
    while (SandboxFramework::sandboxUpdate())
    {

    }

    SandboxFramework::sandboxCleanup();
    

    int i = getchar();
    return 0;
}
