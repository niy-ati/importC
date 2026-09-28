import sdl;
import core.stdc.stdio : printf;
import core.stdc.string : strcmp;

extern(C) void main()
{
    int ver = SDL_GetVersion();
    printf("SDL_GetVersion(): %d.%d.%d\n",
        SDL_VERSIONNUM_MAJOR(ver), SDL_VERSIONNUM_MINOR(ver), SDL_VERSIONNUM_MICRO(ver));
    assert(ver > 0, "SDL_GetVersion() returned nothing");

    // No subsystem needs a display for these: real SDL3 calls through the
    // zero-wrapper import, exercising memory, string and timer utilities.
    void* p = SDL_malloc(64);
    assert(p !is null, "SDL_malloc failed");
    SDL_memset(p, 0x42, 64);
    assert((cast(ubyte*) p)[0] == 0x42 && (cast(ubyte*) p)[63] == 0x42, "SDL_memset didn't fill the buffer");
    SDL_free(p);

    char[32] buf;
    SDL_snprintf(buf.ptr, buf.length, "%d + %d = %d".ptr, 2, 3, 5);
    assert(strcmp(buf.ptr, "2 + 3 = 5") == 0, "SDL_snprintf produced the wrong string");

    ulong ticks = SDL_GetTicks();
    printf("SDL_GetTicks(): %llu\n", ticks);

    printf("SDL ZERO-WRAPPER SMOKE TEST PASSED\n");
}
