# sqlite (blocked - Milestone 2)

Not usable yet. Left in the CI matrix on purpose so its failure stays visible
instead of silently skipped.

- **Windows:** with `-DSQLITE_OMIT_SEH`, compiling still fails on
  `cannot take address of imported symbol` for ~62 Windows API functions
  taken by address in a static initializer (`dinterpret.d:1999`).
- **Linux:** fails on `__int128 not supported` (dmd#21543), pulled in even
  with `-DSQLITE_DISABLE_INTRINSIC` because glibc's own headers use it.

Both are Milestone 2 tasks (system header / intrinsic compatibility), not
this milestone's scope.
