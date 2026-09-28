importC - Test suite for the feasibility of importC with usable C libraries

Zero-wrapper packages, each library imported directly via ImportC with no hand-written D bindings:

- `importc-sljit` - stack-less JIT compiler. Windows only for now: `sljit_create_compiler` itself calls cpuid detection through GNU inline asm on Linux, which ImportC turns into a runtime assert, so nothing that creates a compiler can run there yet, not just its bundled test suite.
- `importc-zlib` - compression, round-tripped compress/decompress.
- `importc-curl` - libcurl, version check and option set/readback.
- `importc-sdl` - SDL3 (built from source; the existing `importc/` package here is SDL2 via bindbc-sdl).
- `importc-sqlite` - blocked on Milestone 2 (Windows SEH/dllimport, Linux `__int128`), see its README.

`.github/workflows/zero-wrapper-libs.yml` builds dmd from the fix branches these packages need and runs each one's example against it, separate from the existing `e2e.yml` (SDL2/bindbc, released compilers).

