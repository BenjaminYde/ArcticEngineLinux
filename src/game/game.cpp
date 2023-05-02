#include "engine/arctic_engine.h"

int main()
{
    ArcticEngine engine;
    engine.Initialize();
    engine.Run();
    engine.Cleanup();

    return 0;
}