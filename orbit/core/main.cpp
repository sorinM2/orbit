#include "platform/platform_common.h"
#include "system.h"


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    if (!orbit::system::initialize(hInst, cmdshow)) return false;
    return orbit::system::run();
}