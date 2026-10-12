import sljitLir;
import core.stdc.stdio : printf;

alias ConstFn = extern(C) sljit_sw function();

void main()
{
    sljit_compiler* c = sljit_create_compiler(null);
    assert(c !is null, "sljit_create_compiler failed");

    enum arg_types = SLJIT_ARG_RETURN(SLJIT_ARG_TYPE_W);

    int rc = sljit_emit_enter(c, 0, arg_types, 1, 1, 0);
    assert(rc == SLJIT_SUCCESS);

    sljit_const* cnst = sljit_emit_const(c, SLJIT_MOV, SLJIT_R0, 0, cast(sljit_sw) 0x7777777777777777L);
    assert(cnst !is null, "sljit_emit_const failed");

    rc = sljit_emit_return(c, SLJIT_MOV, SLJIT_R0, 0);
    assert(rc == SLJIT_SUCCESS);

    void* code = sljit_generate_code(c, 0, null);
    assert(code !is null, "sljit_generate_code failed");

    auto fn = cast(ConstFn) code;
    sljit_sw before = fn();
    printf("before patch: 0x%llx (expect 0x7777777777777777)\n", cast(long) before);

    sljit_uw addr = sljit_get_const_addr(cnst);
    printf("const addr = 0x%llx\n", cast(long) addr);

    ubyte* p = cast(ubyte*) addr;
    printf("bytes before patch:");
    for (int i = -16; i < 8; i++) printf(" %02x", p[i]);
    printf("\n");

    sljit_sw exec_offset = sljit_get_executable_offset(c);
    printf("executable_offset = %lld\n", cast(long) exec_offset);

    sljit_set_const(addr, SLJIT_MOV, cast(sljit_sw) 0x1234567890ABCDEFL, exec_offset);

    printf("bytes after patch: ");
    for (int i = -16; i < 8; i++) printf(" %02x", p[i]);
    printf("\n");

    sljit_sw after = fn();
    printf("after patch: 0x%llx (expect 0x1234567890abcdef)\n", cast(long) after);

    if (after == cast(sljit_sw) 0x1234567890ABCDEFL)
        printf("PATCH_OK\n");
    else
        printf("PATCH_FAILED\n");

    sljit_free_code(code, null);
    sljit_free_compiler(c);
}
