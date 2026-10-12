/* Standalone reproduction of sljit's test_exec_allocator, test_macros, and
 * test1 through test11 (from sljit's own test_src/sljitTest.c at commit
 * 39c508d1), called in the exact same order sljit_test() calls them, to
 * check whether accumulated state from the preceding tests is what makes
 * test11 crash in the full suite when it didn't crash running alone. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sljitLir.h"

union executable_code {
	void* code;
	sljit_sw (SLJIT_FUNC *func0)(void);
	sljit_sw (SLJIT_FUNC *func1)(sljit_sw a);
	sljit_sw (SLJIT_FUNC *func2)(sljit_sw a, sljit_sw b);
	sljit_sw (SLJIT_FUNC *func3)(sljit_sw a, sljit_sw b, sljit_sw c);
};
typedef union executable_code executable_code;

static sljit_s32 successful_tests = 0;
static sljit_s32 verbose = 1;
static sljit_s32 silent = 0;

#define FAILED(cond, text) \
	if (SLJIT_UNLIKELY(cond)) { \
		printf(text); \
		return; \
	}

#define CHECK(compiler) \
	if (sljit_get_compiler_error(compiler) != SLJIT_ERR_COMPILED) { \
		printf("Compiler error: %d\n", sljit_get_compiler_error(compiler)); \
		sljit_free_compiler(compiler); \
		return; \
	}

#if (defined SLJIT_64BIT_ARCHITECTURE && SLJIT_64BIT_ARCHITECTURE)
#define IS_32BIT 0
#define IS_64BIT 1
#define WCONST(const64, const32) ((sljit_sw)SLJIT_W(const64))
#else /* !SLJIT_64BIT_ARCHITECTURE */
#define IS_32BIT 1
#define IS_64BIT 0
#define WCONST(const64, const32) ((sljit_sw)const32)
#endif /* SLJIT_64BIT_ARCHITECTURE */

static void test21(void)
{
	/* Some complicated addressing modes. */
	executable_code code;
	struct sljit_compiler* compiler = sljit_create_compiler(NULL);
	sljit_sw buf[9];
	sljit_s16 sbuf[5];
	sljit_s8 bbuf[7];

	if (verbose)
		printf("Run test21\n");

	FAILED(!compiler, "cannot create compiler\n");

	buf[0] = 100567;
	buf[1] = 75799;
	buf[2] = 0;
	buf[3] = -8;
	buf[4] = -50;
	buf[5] = 0;
	buf[6] = 0;
	buf[7] = 0;
	buf[8] = 0;

	sbuf[0] = 30000;
	sbuf[1] = 0;
	sbuf[2] = 0;
	sbuf[3] = -12345;
	sbuf[4] = 0;

	bbuf[0] = -128;
	bbuf[1] = 0;
	bbuf[2] = 0;
	bbuf[3] = 99;
	bbuf[4] = 0;
	bbuf[5] = 0;
	bbuf[6] = 0;

	sljit_emit_enter(compiler, 0, SLJIT_ARGS3V(P, P, P), 3, 3, 0);

	/* Nothing should be updated. */
	/* sbuf[1] */
	sljit_emit_op1(compiler, SLJIT_MOV_S16, SLJIT_MEM0(), (sljit_sw)&sbuf[1], SLJIT_MEM0(), (sljit_sw)&sbuf[0]);
	/* bbuf[1] */
	sljit_emit_op1(compiler, SLJIT_MOV_S8, SLJIT_MEM0(), (sljit_sw)&bbuf[1], SLJIT_MEM0(), (sljit_sw)&bbuf[0]);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 2);
	/* sbuf[2] */
	sljit_emit_op1(compiler, SLJIT_MOV_U16, SLJIT_MEM2(SLJIT_S1, SLJIT_R0), 1, SLJIT_MEM0(), (sljit_sw)&sbuf[3]);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, (sljit_sw)&buf[0]);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, sizeof(sljit_sw));
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R2, 0, SLJIT_IMM, 2);
	/* buf[2] */
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_MEM2(SLJIT_R0, SLJIT_R2), SLJIT_WORD_SHIFT, SLJIT_MEM0(), (sljit_sw)&buf[0], SLJIT_MEM2(SLJIT_R1, SLJIT_R0), 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, sizeof(sljit_s8));
	/* bbuf[2] */
	sljit_emit_op1(compiler, SLJIT_MOV_U8, SLJIT_MEM1(SLJIT_R0), (sljit_sw)&bbuf[1], SLJIT_MEM1(SLJIT_R0), (sljit_sw)&bbuf[2]);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, sizeof(sljit_s16));
	/* sbuf[4] */
	sljit_emit_op1(compiler, SLJIT_MOV_S16, SLJIT_MEM1(SLJIT_R1), (sljit_sw)&sbuf[3], SLJIT_R1, 0);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 3);
	/* buf[3] */
	sljit_emit_op2(compiler, SLJIT_MUL, SLJIT_MEM2(SLJIT_S0, SLJIT_R0), SLJIT_WORD_SHIFT, SLJIT_MEM2(SLJIT_S0, SLJIT_R0), SLJIT_WORD_SHIFT, SLJIT_MEM2(SLJIT_S0, SLJIT_R0), SLJIT_WORD_SHIFT);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 4);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_S0, 0);
	/* buf[4] */
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_MEM2(SLJIT_S0, SLJIT_R0), SLJIT_WORD_SHIFT, SLJIT_MEM2(SLJIT_R1, SLJIT_R0), SLJIT_WORD_SHIFT, SLJIT_MEM2(SLJIT_S0, SLJIT_R0), SLJIT_WORD_SHIFT);

	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R0, 0, SLJIT_S0, 0, SLJIT_IMM, 9 * sizeof(sljit_sw));
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_S0, 0, SLJIT_IMM, 4 * sizeof(sljit_sw));
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R2, 0, SLJIT_IMM, -(4 << SLJIT_WORD_SHIFT));
	/* buf[5] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM2(SLJIT_R0, SLJIT_R2), 0, SLJIT_MEM2(SLJIT_R1, SLJIT_R2), 0);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, (sljit_sw)&buf - 0x7fff8000 + 6 * (sljit_sw)sizeof(sljit_sw));
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 952467);
	/* buf[6] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_R0), 0x7fff8000, SLJIT_R1, 0);
	/* buf[7] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_R0), 0x7fff8000 + sizeof(sljit_sw), SLJIT_MEM1(SLJIT_R0), 0x7fff8000);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, (sljit_sw)&buf + 0x7fff7fff + 6 * (sljit_sw)sizeof(sljit_sw));
	/* buf[8] */
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_MEM1(SLJIT_R0), -0x7fff7fff + 2 * (sljit_sw)sizeof(sljit_sw), SLJIT_MEM1(SLJIT_R0), -0x7fff7fff + (sljit_sw)sizeof(sljit_sw), SLJIT_MEM1(SLJIT_R0), -0x7fff7fff);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, (sljit_sw)&bbuf - 0x7fff7ffe + 3 * (sljit_sw)sizeof(sljit_s8));
	/* bbuf[4] */
	sljit_emit_op1(compiler, SLJIT_MOV_S8, SLJIT_MEM1(SLJIT_R0), 0x7fff7fff, SLJIT_MEM1(SLJIT_R0), 0x7fff7ffe);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, (sljit_sw)&bbuf + 0x7fff7fff + 5 * (sljit_sw)sizeof(sljit_s8));
	/* bbuf[5] */
	sljit_emit_op1(compiler, SLJIT_MOV_S8, SLJIT_MEM1(SLJIT_R0), -0x7fff7fff, SLJIT_MEM1(SLJIT_R0), -0x7fff8000);
#if IS_64BIT
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, (sljit_sw)&bbuf - SLJIT_W(0x123456123456));
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, (sljit_sw)&bbuf - SLJIT_W(0x123456123456));
	/* bbuf[6] */
	sljit_emit_op1(compiler, SLJIT_MOV_S8, SLJIT_MEM1(SLJIT_R0), SLJIT_W(0x123456123456) + 6 * sizeof(sljit_s8), SLJIT_MEM1(SLJIT_R1), SLJIT_W(0x123456123456));
#endif /* IS_64BIT */

	sljit_emit_return_void(compiler);

	code.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	sljit_free_compiler(compiler);

	code.func3((sljit_sw)&buf, (sljit_sw)&sbuf, (sljit_sw)&bbuf);
	FAILED(buf[2] != 176366, "test21 case 1 failed\n");
	FAILED(buf[3] != 64, "test21 case 2 failed\n");
	FAILED(buf[4] != -100, "test21 case 3 failed\n");
	FAILED(buf[5] != 100567, "test21 case 4 failed\n");
	FAILED(buf[6] != 952467, "test21 case 5 failed\n");
	FAILED(buf[7] != 952467, "test21 case 6 failed\n");
	FAILED(buf[8] != 952467 * 2, "test21 case 7 failed\n");

	FAILED(sbuf[1] != 30000, "test21 case 8 failed\n");
	FAILED(sbuf[2] != -12345, "test21 case 9 failed\n");
	FAILED(sbuf[4] != sizeof(sljit_s16), "test21 case 10 failed\n");

	FAILED(bbuf[1] != -128, "test21 case 11 failed\n");
	FAILED(bbuf[2] != 99, "test21 case 12 failed\n");
	FAILED(bbuf[4] != 99, "test21 case 13 failed\n");
	FAILED(bbuf[5] != 99, "test21 case 14 failed\n");
#if IS_64BIT
	FAILED(bbuf[6] != -128, "test21 case 15 failed\n");
#endif /* IS_64BIT */

	sljit_free_code(code.code, NULL);
	successful_tests++;
}


int main(void)
{
	test21();
	printf("TEST21_REPRO_DONE successful_tests=%d\n", successful_tests);
	return 0;
}
