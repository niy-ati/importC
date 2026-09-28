import sljitLir;
import core.stdc.stdio : printf;

alias AddFn = extern(C) sljit_sw function(sljit_sw, sljit_sw);

void main()
{
    sljit_compiler* c = sljit_create_compiler(null);
    assert(c !is null, "sljit_create_compiler failed");

    // SLJIT_ARGS2(W, W, W) is not usable here: it needs SLJIT_ARG_TO_TYPE's
    // "##" token-pasting, which ImportC currently drops. Built by hand from
    // the same underlying (non-pasted) constants until that's fixed.
    enum arg_types = SLJIT_ARG_RETURN(SLJIT_ARG_TYPE_W)
        | SLJIT_ARG_VALUE(SLJIT_ARG_TYPE_W, 1)
        | SLJIT_ARG_VALUE(SLJIT_ARG_TYPE_W, 2);

    int rc = sljit_emit_enter(c, 0, arg_types, 2, 2, 0);
    assert(rc == SLJIT_SUCCESS);

    rc = sljit_emit_op2(c, SLJIT_ADD, SLJIT_R0, 0, SLJIT_S0, 0, SLJIT_S1, 0);
    assert(rc == SLJIT_SUCCESS);

    rc = sljit_emit_return(c, SLJIT_MOV, SLJIT_R0, 0);
    assert(rc == SLJIT_SUCCESS);

    void* code = sljit_generate_code(c, 0, null);
    assert(code !is null, "sljit_generate_code failed");

    auto add = cast(AddFn) code;
    sljit_sw result = add(2, 3);
    printf("2 + 3 = %ld\n", cast(long) result);
    assert(result == 5, "JIT-compiled add() gave the wrong answer");

    sljit_free_code(code, null);
    sljit_free_compiler(c);

    printf("SLJIT ZERO-WRAPPER SMOKE TEST PASSED\n");
}
