#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "system.h"


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    if (!orbit::system::initialize(hInst, cmdshow)) return false;
    return orbit::system::run();
}