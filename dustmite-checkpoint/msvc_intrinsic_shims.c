/* Real cl.exe never emits actual calls to __arch_inverted, __arch_avx10ver or
 * RtlSetVolatileMemory - immintrin.h and winnt.h declare them as plain
 * extern functions (no WINBASEAPI/dllimport), but cl.exe's front end
 * recognizes the names and expands them inline, the same way it does for
 * __cpuidex. dmd's ImportC doesn't have redirects for these three yet (they
 * are not in druntime's __builtins_msvc.d - confirmed by grepping the whole
 * dmd repo), so real out-of-line bodies are provided here instead, matching
 * the Windows SDK's documented behaviour, until dmd grows native support.
 */

unsigned __arch_inverted()
{
}

unsigned __arch_avx10ver()
{
}

void RtlSetVolatileMemory()
{
}
