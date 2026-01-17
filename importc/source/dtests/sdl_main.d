// https://github.com/dlang/dmd/issues/21932

import std.stdio;
import clibs.sdl; // your SDL2 C bindings

void main()
{
    // Initialize video + events subsystems
    uint flags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;

    if (SDL_Init(flags) != 0)
    {
        writeln("Failed to initialize SDL2: ", SDL_GetError());
        return;
    }
    writeln("SDL2 successfully initialized.");

    // Create a window (SDL2 signature)
    SDL_Window* window = SDL_CreateWindow(
        "SDL2 D Example".ptr,       // needs .ptr for C compatibility
        SDL_WINDOWPOS_CENTERED,    // SDL2 constant
        SDL_WINDOWPOS_CENTERED,    // SDL2 constant
        640,                       // width
        480,                       // height
        0                          // window flags
    );

    if (window is null)
    {
        writeln("Failed to create SDL2 window: ", SDL_GetError());
        SDL_Quit();
        return;
    }

    // Wait 3 seconds
    SDL_Delay(3000);

    // Cleanup
    SDL_DestroyWindow(window);
    writeln("Shutting down SDL2.");
    SDL_Quit(); // quit
}
