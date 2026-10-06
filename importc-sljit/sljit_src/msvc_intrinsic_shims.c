/* Real cl.exe never emits actual calls to __arch_inverted, __arch_avx10ver or
 * RtlSetVolatileMemory - immintrin.h and winnt.h declare them as plain
 * extern functions (no WINBASEAPI/dllimport), but cl.exe's front end
 * recognizes the names and expands them inline, the same way it does for
 * __cpuidex. dmd's ImportC doesn't have redirects for these three yet (they
 * are not in druntime's __builtins_msvc.d - confirmed by grepping the whole
 * dmd repo), so real out-of-line bodies are provided here instead, matching
 * the Windows SDK's documented behaviour, until dmd grows native support.
 */

unsigned __arch_inverted(void)
{
    /* Cleared bit means that vector width is supported. Reporting no
     * special /arch support (all bits set) matches the conservative
     * baseline codegen sljit already falls back to when SIMD. */
    return 0xFFFFFFFFu;
}

unsigned __arch_avx10ver(void)
{
    return 0;
}

volatile void *RtlSetVolatileMemory(volatile void *Destination, int Fill, unsigned long long Length)
{
    volatile unsigned char *p = (volatile unsigned char *)Destination;
    unsigned long long i;
    for (i = 0; i < Length; i++)
        p[i] = (unsigned char)Fill;
    return Destination;
}
