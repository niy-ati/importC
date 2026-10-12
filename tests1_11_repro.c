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

static void test_exec_allocator(void)
{
	/* This is not an sljit test. */
	void *ptr1;
	void *ptr2;
	void *ptr3;

	if (verbose)
		printf("Run executable allocator test\n");

	MALLOC_EXEC(ptr1, 32);
	MALLOC_EXEC(ptr2, 512);
	MALLOC_EXEC(ptr3, 512);
	FREE_EXEC(ptr2);
	FREE_EXEC(ptr3);
	FREE_EXEC(ptr1);
	MALLOC_EXEC(ptr1, 262104);
	MALLOC_EXEC(ptr2, 32000);
	FREE_EXEC(ptr1);
	MALLOC_EXEC(ptr1, 262104);
	FREE_EXEC(ptr1);
	FREE_EXEC(ptr2);
	MALLOC_EXEC(ptr1, 512);
	MALLOC_EXEC(ptr2, 512);
	MALLOC_EXEC(ptr3, 512);
	FREE_EXEC(ptr2);
	MALLOC_EXEC(ptr2, 512);
#if (defined SLJIT_EXECUTABLE_ALLOCATOR && SLJIT_EXECUTABLE_ALLOCATOR)
	sljit_free_unused_memory_exec();
#endif /* SLJIT_EXECUTABLE_ALLOCATOR */
	FREE_EXEC(ptr3);
	FREE_EXEC(ptr1);
	FREE_EXEC(ptr2);

#if (defined SLJIT_EXECUTABLE_ALLOCATOR && SLJIT_EXECUTABLE_ALLOCATOR)
	sljit_free_unused_memory_exec();
#endif /* SLJIT_EXECUTABLE_ALLOCATOR */
}

#undef MALLOC_EXEC

#endif /* !(defined SLJIT_CONFIG_UNSUPPORTED && SLJIT_CONFIG_UNSUPPORTED) */

static void test_macros(void)
{
	SLJIT_ASSERT(SLJIT_IS_MEM(SLJIT_MEM0()));
	SLJIT_ASSERT(SLJIT_IS_MEM(SLJIT_MEM1(SLJIT_S0)));
	SLJIT_ASSERT(SLJIT_IS_MEM(SLJIT_MEM2(SLJIT_R0, SLJIT_S0)));
	SLJIT_ASSERT(SLJIT_IS_MEM0(SLJIT_MEM0()));
	SLJIT_ASSERT(!SLJIT_IS_MEM0(SLJIT_MEM1(SLJIT_S0)));
	SLJIT_ASSERT(!SLJIT_IS_MEM0(SLJIT_MEM2(SLJIT_R0, SLJIT_S0)));
	SLJIT_ASSERT(!SLJIT_IS_MEM1(SLJIT_MEM0()));
	SLJIT_ASSERT(SLJIT_IS_MEM1(SLJIT_MEM1(SLJIT_S0)));
	SLJIT_ASSERT(!SLJIT_IS_MEM1(SLJIT_MEM2(SLJIT_R0, SLJIT_S0)));
	SLJIT_ASSERT(!SLJIT_IS_MEM2(SLJIT_MEM0()));
	SLJIT_ASSERT(!SLJIT_IS_MEM2(SLJIT_MEM1(SLJIT_R0)));
	SLJIT_ASSERT(SLJIT_IS_MEM2(SLJIT_MEM2(SLJIT_R0, SLJIT_R1)));

	SLJIT_ASSERT(!SLJIT_IS_REG(SLJIT_IMM));
	SLJIT_ASSERT(!SLJIT_IS_MEM(SLJIT_IMM));
	SLJIT_ASSERT(SLJIT_IS_IMM(SLJIT_IMM));
	SLJIT_ASSERT(!SLJIT_IS_REG_PAIR(SLJIT_IMM));
	SLJIT_ASSERT(SLJIT_IS_REG(SLJIT_S0));
	SLJIT_ASSERT(!SLJIT_IS_MEM(SLJIT_S0));
	SLJIT_ASSERT(!SLJIT_IS_IMM(SLJIT_S0));
	SLJIT_ASSERT(!SLJIT_IS_REG_PAIR(SLJIT_S0));
	SLJIT_ASSERT(!SLJIT_IS_REG(SLJIT_REG_PAIR(SLJIT_R0, SLJIT_S0)));
	SLJIT_ASSERT(!SLJIT_IS_MEM(SLJIT_REG_PAIR(SLJIT_R0, SLJIT_S0)));
	SLJIT_ASSERT(!SLJIT_IS_IMM(SLJIT_REG_PAIR(SLJIT_R0, SLJIT_S0)));
	SLJIT_ASSERT(SLJIT_IS_REG_PAIR(SLJIT_REG_PAIR(SLJIT_R0, SLJIT_S0)));

	SLJIT_ASSERT(SLJIT_EXTRACT_REG(SLJIT_R2) == SLJIT_R2);
	SLJIT_ASSERT(SLJIT_EXTRACT_REG(SLJIT_FR1) == SLJIT_FR1);
	SLJIT_ASSERT(SLJIT_EXTRACT_REG(SLJIT_MEM1(SLJIT_S2)) == SLJIT_S2);
	SLJIT_ASSERT(SLJIT_EXTRACT_REG(SLJIT_MEM2(SLJIT_S1, SLJIT_S2)) == SLJIT_S1);
	SLJIT_ASSERT(SLJIT_EXTRACT_REG(SLJIT_REG_PAIR(SLJIT_R3, SLJIT_S3)) == SLJIT_R3);
	SLJIT_ASSERT(SLJIT_EXTRACT_REG(SLJIT_REG_PAIR(SLJIT_FR2, SLJIT_FR4)) == SLJIT_FR2);
	SLJIT_ASSERT(SLJIT_EXTRACT_SECOND_REG(SLJIT_MEM2(SLJIT_S1, SLJIT_S2)) == SLJIT_S2);
	SLJIT_ASSERT(SLJIT_EXTRACT_SECOND_REG(SLJIT_REG_PAIR(SLJIT_R3, SLJIT_S3)) == SLJIT_S3);
	SLJIT_ASSERT(SLJIT_EXTRACT_SECOND_REG(SLJIT_REG_PAIR(SLJIT_FR2, SLJIT_FR4)) == SLJIT_FR4);
}

static void test1(void)
{
	/* Enter and return from an sljit function. */
	executable_code code;
	struct sljit_compiler* compiler = sljit_create_compiler(NULL);

	if (verbose)
		printf("Run test1\n");

	FAILED(!compiler, "cannot create compiler\n");

	/* 3 arguments passed, 3 arguments used. */
	sljit_emit_enter(compiler, 0, SLJIT_ARGS3(W, W, W, W), 3, 3, 0);
	sljit_emit_return(compiler, SLJIT_MOV, SLJIT_S1, 0);

	SLJIT_ASSERT(sljit_get_generated_code_size(compiler) == 0);
	code.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	SLJIT_ASSERT(compiler->error == SLJIT_ERR_COMPILED);
	SLJIT_ASSERT(sljit_get_generated_code_size(compiler) > 0);
	sljit_free_compiler(compiler);

	FAILED(code.func3(3, -21, 86) != -21, "test1 case 1 failed\n");
	FAILED(code.func3(4789, 47890, 997) != 47890, "test1 case 2 failed\n");

	sljit_free_code(code.code, NULL);
	successful_tests++;
}

static void test2(void)
{
	/* Test mov. */
	executable_code code;
	struct sljit_compiler* compiler = sljit_create_compiler(NULL);
	sljit_sw buf[12];
	sljit_s32 i;
	static sljit_sw data[2] = { 0, -9876 };

	if (verbose)
		printf("Run test2\n");

	FAILED(!compiler, "cannot create compiler\n");

	for (i = 0; i < 12; i++)
		buf[i] = 0;
	buf[0] = 5678;

	sljit_emit_enter(compiler, 0, SLJIT_ARGS1(W, P), 3, 2, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 9999);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_S1, 0, SLJIT_S0, 0);
	/* buf[1] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S1), sizeof(sljit_sw), SLJIT_R0, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, sizeof(sljit_sw));
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R2, 0, SLJIT_MEM2(SLJIT_S1, SLJIT_R1), 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_S0, 0, SLJIT_IMM, 2);
	/* buf[2] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM2(SLJIT_S1, SLJIT_S0), SLJIT_WORD_SHIFT, SLJIT_MEM2(SLJIT_S1, SLJIT_R1), 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_S0, 0, SLJIT_IMM, 3);
	/* buf[3] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM2(SLJIT_S1, SLJIT_S0), SLJIT_WORD_SHIFT, SLJIT_MEM0(), (sljit_sw)&buf);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, sizeof(sljit_sw));
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_R0), (sljit_sw)&data);
	/* buf[4] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S1), 4 * sizeof(sljit_sw), SLJIT_R0, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, (sljit_sw)&buf - 0x12345678);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_R0), 0x12345678);
	/* buf[5] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S1), 5 * sizeof(sljit_sw), SLJIT_R0, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 3456);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, (sljit_sw)&buf - 0xff890 + 6 * (sljit_sw)sizeof(sljit_sw));
	/* buf[6] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_R1), 0xff890, SLJIT_R0, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, (sljit_sw)&buf + 0xff890 + 7 * (sljit_sw)sizeof(sljit_sw));
	/* buf[7] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_R1), -0xff890, SLJIT_R0, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, (sljit_sw)&buf - 0xfff0ff + 8 * (sljit_sw)sizeof(sljit_sw));
	/* buf[8] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_R0), 0xfff0ff, SLJIT_IMM, 7896);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, (sljit_sw)&buf + 0xfff100 + 9 * (sljit_sw)sizeof(sljit_sw));
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, -2450);
	/* buf[9] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_R0), -0xfff100, SLJIT_R1, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_S1, 0, SLJIT_IMM, (sljit_sw)&buf - 0xffef01 + 10 * (sljit_sw)sizeof(sljit_sw));
	/* buf[10] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S1), 0xffef01, SLJIT_IMM, 8796);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_S1, 0, SLJIT_IMM, (sljit_sw)&buf + 0xffef01 + 11 * (sljit_sw)sizeof(sljit_sw));
	/* buf[11] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S1), -0xffef01, SLJIT_IMM, 5704);

	sljit_emit_return(compiler, SLJIT_MOV, SLJIT_R2, 0);

	code.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	sljit_free_compiler(compiler);

	FAILED(code.func1((sljit_sw)&buf) != 9999, "test2 case 1 failed\n");
	FAILED(buf[1] != 9999, "test2 case 2 failed\n");
	FAILED(buf[2] != 9999, "test2 case 3 failed\n");
	FAILED(buf[3] != 5678, "test2 case 4 failed\n");
	FAILED(buf[4] != -9876, "test2 case 5 failed\n");
	FAILED(buf[5] != 5678, "test2 case 6 failed\n");
	FAILED(buf[6] != 3456, "test2 case 7 failed\n");
	FAILED(buf[7] != 3456, "test2 case 8 failed\n");
	FAILED(buf[8] != 7896, "test2 case 9 failed\n");
	FAILED(buf[9] != -2450, "test2 case 10 failed\n");
	FAILED(buf[10] != 8796, "test2 case 11 failed\n");
	FAILED(buf[11] != 5704, "test2 case 12 failed\n");

	sljit_free_code(code.code, NULL);
	successful_tests++;
}

static void test3(void)
{
	/* Test not. */
	executable_code code;
	struct sljit_compiler* compiler = sljit_create_compiler(NULL);
	sljit_sw buf[5];

	if (verbose)
		printf("Run test3\n");

	FAILED(!compiler, "cannot create compiler\n");
	buf[0] = 1234;
	buf[1] = 0;
	buf[2] = 9876;
	buf[3] = 0;
	buf[4] = 0x12345678;

	sljit_emit_enter(compiler, 0, SLJIT_ARGS1(W, P), 3, 1, 0);
	/* buf[1] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw), SLJIT_MEM1(SLJIT_S0), 0);
	sljit_emit_op2(compiler, SLJIT_XOR, SLJIT_MEM0(), (sljit_sw)&buf[1], SLJIT_MEM0(), (sljit_sw)&buf[1], SLJIT_IMM, -1);
	/* buf[3] */
	sljit_emit_op2(compiler, SLJIT_XOR, SLJIT_RETURN_REG, 0, SLJIT_IMM, -1, SLJIT_MEM1(SLJIT_S0), 0);
	sljit_emit_op2(compiler, SLJIT_XOR, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 3, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 2, SLJIT_IMM, -1);
	/* buf[4] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, (sljit_sw)&buf[4] - 0xff0000 - 0x20);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R2, 0, SLJIT_IMM, (sljit_sw)&buf[4] - 0xff0000);
	sljit_emit_op2(compiler, SLJIT_XOR, SLJIT_MEM1(SLJIT_R1), 0xff0000 + 0x20, SLJIT_IMM, -1, SLJIT_MEM1(SLJIT_R2), 0xff0000);
	sljit_emit_return(compiler, SLJIT_MOV, SLJIT_RETURN_REG, 0);

	code.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	sljit_free_compiler(compiler);

	FAILED(code.func1((sljit_sw)&buf) != ~1234, "test3 case 1 failed\n");
	FAILED(buf[1] != ~1234, "test3 case 2 failed\n");
	FAILED(buf[3] != ~9876, "test3 case 3 failed\n");
	FAILED(buf[4] != ~0x12345678, "test3 case 4 failed\n");

	sljit_free_code(code.code, NULL);
	successful_tests++;
}

static void test4(void)
{
	/* Test negate. */
	executable_code code;
	struct sljit_compiler* compiler = sljit_create_compiler(NULL);
	sljit_sw buf[4];

	if (verbose)
		printf("Run test4\n");

	FAILED(!compiler, "cannot create compiler\n");
	buf[0] = 0;
	buf[1] = 1234;
	buf[2] = 0;
	buf[3] = 0;

	sljit_emit_enter(compiler, 0, SLJIT_ARGS2(W, P, W), 3, 2, 0);
	/* buf[0] */
	sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_MEM0(), (sljit_sw)&buf[0], SLJIT_IMM, 0, SLJIT_MEM0(), (sljit_sw)&buf[1]);
	/* buf[2] */
	sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 2, SLJIT_IMM, 0, SLJIT_S1, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 299);
	/* buf[3] */
	sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 3, SLJIT_IMM, 0, SLJIT_R1, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 2);
	sljit_emit_return(compiler, SLJIT_MOV, SLJIT_RETURN_REG, 0);

	code.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	sljit_free_compiler(compiler);

	FAILED(code.func2((sljit_sw)&buf, 4567) != -4567, "test4 case 1 failed\n");
	FAILED(buf[0] != -1234, "test4 case 2 failed\n");
	FAILED(buf[2] != -4567, "test4 case 3 failed\n");
	FAILED(buf[3] != -299, "test4 case 4 failed\n");

	sljit_free_code(code.code, NULL);
	successful_tests++;
}

static void test5(void)
{
	/* Test add. */
	executable_code code;
	struct sljit_compiler* compiler = sljit_create_compiler(NULL);
	sljit_s32 i;
	sljit_sw buf[10];

	if (verbose)
		printf("Run test5\n");

	FAILED(!compiler, "cannot create compiler\n");

	for (i = 0; i < 10; i++)
		buf[i] = 0;

	buf[0] = 100;
	buf[1] = 200;
	buf[2] = 300;
	buf[8] = 313;
	buf[9] = 856;

	sljit_emit_enter(compiler, 0, SLJIT_ARGS1(W, P), 3, 2, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, sizeof(sljit_sw));
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 50);
	/* buf[2] */
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_MEM2(SLJIT_S0, SLJIT_R0), 1, SLJIT_MEM2(SLJIT_S0, SLJIT_R0), 1, SLJIT_MEM2(SLJIT_S0, SLJIT_R0), 0);
	/* buf[0] */
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, sizeof(sljit_sw) + 2);
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, 50);
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), 0);
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_MEM1(SLJIT_S0), 0, SLJIT_MEM1(SLJIT_S0), 0, SLJIT_R0, 0);
	/* buf[5] */
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R0, 0, SLJIT_IMM, 4, SLJIT_R0, 0);
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_IMM, 50, SLJIT_R1, 0);
	/* buf[4] */
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_MEM1(SLJIT_S0), 5 * sizeof(sljit_sw), SLJIT_IMM, 50, SLJIT_MEM1(SLJIT_S0), 5 * sizeof(sljit_sw));
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), 5 * sizeof(sljit_sw), SLJIT_R1, 0);
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_MEM1(SLJIT_S0), 4 * sizeof(sljit_sw), SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), 4 * sizeof(sljit_sw));
	/* buf[5] */
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_MEM1(SLJIT_S0), 5 * sizeof(sljit_sw), SLJIT_MEM1(SLJIT_S0), 4 * sizeof(sljit_sw), SLJIT_MEM1(SLJIT_S0), 5 * sizeof(sljit_sw));
	/* buf[3] */
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_MEM1(SLJIT_S0), 3 * sizeof(sljit_sw), SLJIT_MEM1(SLJIT_S0), 4 * sizeof(sljit_sw), SLJIT_R1, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 0x1e7d39f2);
	/* buf[6] */
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_MEM1(SLJIT_S0), 6 * sizeof(sljit_sw), SLJIT_R1, 0, SLJIT_IMM, 0x23de7c06);
	/* buf[7] */
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_MEM1(SLJIT_S0), 7 * sizeof(sljit_sw), SLJIT_IMM, 0x3d72e452, SLJIT_R1, 0);
	/* buf[8] */
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_MEM1(SLJIT_S0), 8 * sizeof(sljit_sw), SLJIT_IMM, -43, SLJIT_MEM1(SLJIT_S0), 8 * sizeof(sljit_sw));
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R2, 0, SLJIT_IMM, (sljit_sw)(buf + 9) >> 1);
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_R1), (sljit_sw)(buf + 1), SLJIT_MEM1(SLJIT_R2), (sljit_sw)(buf + 9) >> 1);
	/* buf[9] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), 9 * sizeof(sljit_sw), SLJIT_R1, 0);

	/* Return value */
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R0, 0, SLJIT_IMM, 1000, SLJIT_R0, 0);
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, 1430);
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R0, 0, SLJIT_IMM, -99, SLJIT_R0, 0);

	sljit_emit_return(compiler, SLJIT_MOV, SLJIT_R0, 0);

	code.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	sljit_free_compiler(compiler);

	FAILED(code.func1((sljit_sw)&buf) != 2437 + 2 * sizeof(sljit_sw), "test5 case 1 failed\n");
	FAILED(buf[0] != 202 + 2 * sizeof(sljit_sw), "test5 case 2 failed\n");
	FAILED(buf[2] != 500, "test5 case 3 failed\n");
	FAILED(buf[3] != 400, "test5 case 4 failed\n");
	FAILED(buf[4] != 200, "test5 case 5 failed\n");
	FAILED(buf[5] != 250, "test5 case 6 failed\n");
	FAILED(buf[6] != 0x425bb5f8, "test5 case 7 failed\n");
	FAILED(buf[7] != 0x5bf01e44, "test5 case 8 failed\n");
	FAILED(buf[8] != 270, "test5 case 9 failed\n");
	FAILED(buf[9] != 1056, "test5 case 10 failed\n");

	sljit_free_code(code.code, NULL);
	successful_tests++;
}

static void test6(void)
{
	/* Test addc, sub, subc. */
	executable_code code;
	struct sljit_compiler* compiler = sljit_create_compiler(NULL);
	sljit_sw buf[23];
	sljit_s32 i;

	if (verbose)
		printf("Run test6\n");

	FAILED(!compiler, "cannot create compiler\n");

	for (i = 0; i < 23; i++)
		buf[i] = 0;
	buf[10] = 4000;

	sljit_emit_enter(compiler, 0, SLJIT_ARGS1(W, P), 3, 1, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, -1);
	sljit_emit_op2(compiler, SLJIT_ADD | SLJIT_SET_CARRY, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, -1);
	/* buf[0] */
	sljit_emit_op2(compiler, SLJIT_ADDC, SLJIT_MEM1(SLJIT_S0), 0, SLJIT_IMM, 0, SLJIT_IMM, 0);
	sljit_emit_op2(compiler, SLJIT_ADD | SLJIT_SET_CARRY, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_R0, 0);
	/* buf[1] */
	sljit_emit_op2(compiler, SLJIT_ADDC, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw), SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw), SLJIT_IMM, 4);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 100);
	/* buf[2] */
	sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 2, SLJIT_R0, 0, SLJIT_IMM, 50);
	sljit_emit_op2(compiler, SLJIT_SUB | SLJIT_SET_CARRY, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, 6000);
	/* buf[3] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 3, SLJIT_IMM, 10);
	sljit_emit_op2(compiler, SLJIT_SUBC, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 3, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 3, SLJIT_IMM, 5);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 100);
	sljit_emit_op2(compiler, SLJIT_SUB | SLJIT_SET_CARRY, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 2);
	/* buf[4] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 4, SLJIT_R0, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 5000);
	sljit_emit_op2(compiler, SLJIT_SUBC, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 4, SLJIT_R0, 0);
	sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 4, SLJIT_R0, 0);
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R0, 0, SLJIT_R1, 0);
	/* buf[5] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 5, SLJIT_R1, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 5000);
	sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_R0, 0, SLJIT_IMM, 6000, SLJIT_R0, 0);
	/* buf[6] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 6, SLJIT_R0, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 100);
	sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_R1, 0, SLJIT_R0, 0, SLJIT_IMM, 32768);
	/* buf[7] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 7, SLJIT_R1, 0);
	sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_R1, 0, SLJIT_R0, 0, SLJIT_IMM, -32767);
	/* buf[8] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 8, SLJIT_R1, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 0x52cd3bf4);
	/* buf[9] */
	sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 9, SLJIT_R0, 0, SLJIT_IMM, 0x3da297c6);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 6000);
	/* buf[10] */
	sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 10, SLJIT_R1, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 10, SLJIT_R1, 0);
	/* buf[11] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, -1);
	sljit_emit_op2(compiler, SLJIT_ADD | SLJIT_SET_CARRY, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, 1);
	sljit_emit_op2(compiler, SLJIT_ADDC, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 11, SLJIT_IMM, 0xff00ff, SLJIT_R1, 0);
	/* buf[12] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 0);
	sljit_emit_op2(compiler, SLJIT_SUB | SLJIT_SET_CARRY, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, 1);
	sljit_emit_op2(compiler, SLJIT_SUBC, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 12, SLJIT_R1, 0, SLJIT_IMM, 0xff00ff);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 5);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 4);
	/* buf[13] */
	sljit_emit_op2(compiler, SLJIT_ADD | SLJIT_SET_CARRY, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 13, SLJIT_R0, 0, SLJIT_IMM, -2);
	/* buf[14] */
	sljit_emit_op2(compiler, SLJIT_ADDC, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 14, SLJIT_R1, 0, SLJIT_IMM, -2);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, -5);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, -4);
	sljit_emit_op2(compiler, SLJIT_SUB | SLJIT_SET_CARRY, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, -2);
	sljit_emit_op2(compiler, SLJIT_SUBC, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, -2);
	/* buf[15] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 15, SLJIT_R0, 0);
	/* buf[16] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 16, SLJIT_R1, 0);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 3);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R2, 0, SLJIT_IMM, 4);
	sljit_emit_op2(compiler, SLJIT_ADD | SLJIT_SET_CARRY, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, -4);
	sljit_emit_op2(compiler, SLJIT_ADDC, SLJIT_R2, 0, SLJIT_R2, 0, SLJIT_IMM, -4);
	/* buf[17] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 17, SLJIT_R1, 0);
	/* buf[18] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 18, SLJIT_R2, 0);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, -3);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R2, 0, SLJIT_IMM, -4);
	/* buf[19] */
	sljit_emit_op2(compiler, SLJIT_SUB | SLJIT_SET_CARRY, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 19, SLJIT_R1, 0, SLJIT_IMM, -4);
	/* buf[20] */
	sljit_emit_op2(compiler, SLJIT_SUBC, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 20, SLJIT_R2, 0, SLJIT_IMM, -4);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, -100);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, -200);
	sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_R0, 0);
	/* buf[21] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 21, SLJIT_R1, 0);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 1);
	sljit_emit_op2(compiler, SLJIT_SUB | SLJIT_SET_CARRY, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, 1);
	sljit_emit_op2(compiler, SLJIT_SUBC, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_R1, 0);
	/* buf[22] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 22, SLJIT_R0, 0);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_RETURN_REG, 0, SLJIT_IMM, 10);
	sljit_emit_op2(compiler, SLJIT_SUB | SLJIT_SET_CARRY, SLJIT_RETURN_REG, 0, SLJIT_RETURN_REG, 0, SLJIT_IMM, 5);
	sljit_emit_op2(compiler, SLJIT_SUBC, SLJIT_RETURN_REG, 0, SLJIT_RETURN_REG, 0, SLJIT_IMM, 2);
	sljit_emit_op2(compiler, SLJIT_SUB, SLJIT_RETURN_REG, 0, SLJIT_RETURN_REG, 0, SLJIT_IMM, -2220);
	sljit_emit_return(compiler, SLJIT_MOV, SLJIT_RETURN_REG, 0);

	code.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	sljit_free_compiler(compiler);

	FAILED(code.func1((sljit_sw)&buf) != 2223, "test6 case 1 failed\n");
	FAILED(buf[0] != 1, "test6 case 2 failed\n");
	FAILED(buf[1] != 5, "test6 case 3 failed\n");
	FAILED(buf[2] != 50, "test6 case 4 failed\n");
	FAILED(buf[3] != 4, "test6 case 5 failed\n");
	FAILED(buf[4] != 50, "test6 case 6 failed\n");
	FAILED(buf[5] != 50, "test6 case 7 failed\n");
	FAILED(buf[6] != 1000, "test6 case 8 failed\n");
	FAILED(buf[7] != 100 - 32768, "test6 case 9 failed\n");
	FAILED(buf[8] != 100 + 32767, "test6 case 10 failed\n");
	FAILED(buf[9] != 0x152aa42e, "test6 case 11 failed\n");
	FAILED(buf[10] != -2000, "test6 case 12 failed\n");
	FAILED(buf[11] != 0xff0100, "test6 case 13 failed\n");
	FAILED(buf[12] != -0xff0101, "test6 case 14 failed\n");
	FAILED(buf[13] != 3, "test6 case 15 failed\n");
	FAILED(buf[14] != 3, "test6 case 16 failed\n");
	FAILED(buf[15] != -3, "test6 case 17 failed\n");
	FAILED(buf[16] != -3, "test6 case 18 failed\n");
	FAILED(buf[17] != -1, "test6 case 19 failed\n");
	FAILED(buf[18] != 0, "test6 case 20 failed\n");
	FAILED(buf[19] != 1, "test6 case 21 failed\n");
	FAILED(buf[20] != 0, "test6 case 22 failed\n");
	FAILED(buf[21] != 100, "test6 case 23 failed\n");
	FAILED(buf[22] != -3, "test6 case 24 failed\n");

	sljit_free_code(code.code, NULL);
	successful_tests++;
}

static void test7(void)
{
	/* Test logical operators. */
	executable_code code;
	struct sljit_compiler* compiler = sljit_create_compiler(NULL);
	sljit_s32 i;
	sljit_sw buf[9];

	if (verbose)
		printf("Run test7\n");

	FAILED(!compiler, "cannot create compiler\n");
	for (i = 0; i < 9; i++)
		buf[i] = 0;

	buf[0] = 0xff80;
	buf[1] = 0x0f808080;
	buf[3] = 0xaaaaaa;
	buf[5] = 0x4040;
	buf[7] = (sljit_sw)0xc43a7f95;

	sljit_emit_enter(compiler, 0, SLJIT_ARGS1(W, P), 3, 1, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 0xf0C000);
	/* buf[2] */
	sljit_emit_op2(compiler, SLJIT_OR, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 2, SLJIT_R0, 0, SLJIT_IMM, 0x308f);
	/* buf[0] */
	sljit_emit_op2(compiler, SLJIT_XOR, SLJIT_MEM1(SLJIT_S0), 0, SLJIT_MEM1(SLJIT_S0), 0, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw));
	/* buf[3] */
	sljit_emit_op2(compiler, SLJIT_AND, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 3, SLJIT_IMM, 0xf0f0f0, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 3);
	/* buf[4] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 0xC0F0);
	sljit_emit_op2(compiler, SLJIT_XOR, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 5);
	sljit_emit_op2(compiler, SLJIT_OR, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, 0xff0000);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 4, SLJIT_R0, 0);
	/* buf[5] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R2, 0, SLJIT_IMM, 0xC0F0);
	sljit_emit_op2(compiler, SLJIT_AND, SLJIT_R2, 0, SLJIT_R2, 0, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 5);
	sljit_emit_op2(compiler, SLJIT_OR, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 5, SLJIT_R2, 0, SLJIT_IMM, 0xff0000);
	/* buf[1] */
	sljit_emit_op2(compiler, SLJIT_XOR, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw), SLJIT_IMM, 0xFFFFFF, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw));
	/* buf[6] */
	sljit_emit_op2(compiler, SLJIT_OR, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 6, SLJIT_IMM, (sljit_sw)0xa56c82c0, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 6);
	/* buf[7] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 7);
	sljit_emit_op2(compiler, SLJIT_XOR, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 7, SLJIT_IMM, (sljit_sw)0xff00ff00, SLJIT_R0, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 0x76);
	sljit_emit_op2(compiler, SLJIT_AND, SLJIT_R1, 0, SLJIT_IMM, 0x1c, SLJIT_R1, 0);
	/* buf[8] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 8, SLJIT_R1, 0);
	/* Return value */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, (sljit_sw)0xff00ff00);
	sljit_emit_op2(compiler, SLJIT_OR, SLJIT_R1, 0, SLJIT_R0, 0, SLJIT_IMM, 0x0f);
	sljit_emit_op2(compiler, SLJIT_AND, SLJIT_RETURN_REG, 0, SLJIT_IMM, 0x888888, SLJIT_R1, 0);

	sljit_emit_return(compiler, SLJIT_MOV, SLJIT_RETURN_REG, 0);

	code.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	sljit_free_compiler(compiler);

	FAILED(code.func1((sljit_sw)&buf) != 0x8808, "test7 case 1 failed\n");
	FAILED(buf[0] != 0x0F807F00, "test7 case 2 failed\n");
	FAILED(buf[1] != 0x0F7F7F7F, "test7 case 3 failed\n");
	FAILED(buf[2] != 0x00F0F08F, "test7 case 4 failed\n");
	FAILED(buf[3] != 0x00A0A0A0, "test7 case 5 failed\n");
	FAILED(buf[4] != 0x00FF80B0, "test7 case 6 failed\n");
	FAILED(buf[5] != 0x00FF4040, "test7 case 7 failed\n");
	FAILED(buf[6] != (sljit_sw)0xa56c82c0, "test7 case 8 failed\n");
	FAILED(buf[7] != 0x3b3a8095, "test7 case 9 failed\n");
	FAILED(buf[8] != 0x14, "test7 case 10 failed\n");

	sljit_free_code(code.code, NULL);
	successful_tests++;
}

static void test8(void)
{
	/* Test flags (neg, cmp, test). */
	executable_code code;
	struct sljit_compiler* compiler = sljit_create_compiler(NULL);
	sljit_sw buf[27];
	sljit_s32 i;

	if (verbose)
		printf("Run test8\n");

	FAILED(!compiler, "cannot create compiler\n");
	buf[0] = 100;
	for (i = 1; i < 27; i++)
		buf[i] = 3;

	sljit_emit_enter(compiler, 0, SLJIT_ARGS1V(P), 3, 2, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 20);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 10);
	sljit_emit_op2u(compiler, SLJIT_SUB | SLJIT_SET_Z, SLJIT_IMM, 6, SLJIT_IMM, 5);
	/* buf[1] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw), SLJIT_NOT_EQUAL);
	/* buf[2] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 2, SLJIT_EQUAL);
	sljit_emit_op2u(compiler, SLJIT_SUB | SLJIT_SET_GREATER, SLJIT_R0, 0, SLJIT_IMM, 3000);
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_S1, 0, SLJIT_GREATER);
	sljit_emit_op2u(compiler, SLJIT_SUB | SLJIT_SET_LESS, SLJIT_R0, 0, SLJIT_IMM, 3000);
	/* buf[3] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 3, SLJIT_S1, 0);
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_R2, 0, SLJIT_LESS);
	/* buf[4] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 4, SLJIT_R2, 0);
	sljit_emit_op2u(compiler, SLJIT_SUB | SLJIT_SET_SIG_GREATER, SLJIT_R0, 0, SLJIT_IMM, -15);
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_R2, 0, SLJIT_SIG_GREATER);
	/* buf[5] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 5, SLJIT_R2, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, -(sljit_sw)(~(sljit_uw)0 >> 1) - 1);
	sljit_emit_op2u(compiler, SLJIT_SUB | SLJIT_SET_Z | SLJIT_SET_OVERFLOW, SLJIT_R0, 0, SLJIT_R1, 0);
	sljit_emit_op2u(compiler, SLJIT_SUB | SLJIT_SET_Z | SLJIT_SET_OVERFLOW, SLJIT_MEM1(SLJIT_S0), 0, SLJIT_R0, 0);
	sljit_emit_op2(compiler, SLJIT_SUB | SLJIT_SET_Z | SLJIT_SET_OVERFLOW, SLJIT_R1, 0, SLJIT_IMM, 0, SLJIT_R0, 0);
	/* buf[6] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 6, SLJIT_OVERFLOW);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, -1);
	sljit_emit_op2(compiler, SLJIT_XOR | SLJIT_SET_Z, SLJIT_R1, 0, SLJIT_R0, 0, SLJIT_IMM, -1);
	/* buf[7] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 7, SLJIT_ZERO);
	sljit_emit_op2(compiler, SLJIT_XOR | SLJIT_SET_Z, SLJIT_R0, 0, SLJIT_IMM, -1, SLJIT_R1, 0);
	/* buf[8] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 8, SLJIT_ZERO);
	sljit_emit_op2u(compiler, SLJIT_AND | SLJIT_SET_Z, SLJIT_IMM, 0xffff, SLJIT_R0, 0);
	sljit_emit_op2u(compiler, SLJIT_AND | SLJIT_SET_Z, SLJIT_R0, 0, SLJIT_IMM, 0xffff);
	/* buf[9] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 9, SLJIT_NOT_ZERO);
	sljit_emit_op2u(compiler, SLJIT_AND | SLJIT_SET_Z, SLJIT_IMM, 0xffff, SLJIT_R1, 0);
	sljit_emit_op2u(compiler, SLJIT_AND | SLJIT_SET_Z, SLJIT_R1, 0, SLJIT_IMM, 0xffff);
	sljit_emit_op2u(compiler, SLJIT_AND | SLJIT_SET_Z, SLJIT_MEM1(SLJIT_S0), 0, SLJIT_R1, 0);
	sljit_emit_op2u(compiler, SLJIT_AND | SLJIT_SET_Z, SLJIT_R1, 0, SLJIT_MEM1(SLJIT_S0), 0);
	sljit_emit_op2u(compiler, SLJIT_AND | SLJIT_SET_Z, SLJIT_MEM1(SLJIT_S0), 0, SLJIT_MEM1(SLJIT_S0), 0);
	sljit_emit_op2u(compiler, SLJIT_AND | SLJIT_SET_Z, SLJIT_MEM1(SLJIT_S0), 0, SLJIT_IMM, 0x1);
	/* buf[10] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 10, SLJIT_NOT_ZERO);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, -(sljit_sw)(~(sljit_uw)0 >> 1) - 1);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 0);
	sljit_emit_op2u(compiler, SLJIT_SUB | SLJIT_SET_OVERFLOW, SLJIT_R1, 0, SLJIT_R0, 0);
	/* buf[11] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 11, SLJIT_OVERFLOW);
	sljit_emit_op2u(compiler, SLJIT_ADD | SLJIT_SET_OVERFLOW, SLJIT_R1, 0, SLJIT_R0, 0);
	/* buf[12] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 12, SLJIT_OVERFLOW);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 9);
	sljit_emit_op2(compiler, SLJIT_SUB | SLJIT_SET_Z | SLJIT_SET_LESS, SLJIT_R0, 0, SLJIT_IMM, 0, SLJIT_R0, 0);
	/* buf[13] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 13, SLJIT_LESS);
	/* buf[14] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 14, SLJIT_ZERO);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 0);
	sljit_emit_op2(compiler, SLJIT_SUB | SLJIT_SET_Z | SLJIT_SET_LESS, SLJIT_R0, 0, SLJIT_IMM, 0, SLJIT_R0, 0);
	/* buf[15] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 15, SLJIT_LESS);
	/* buf[16] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 16, SLJIT_ZERO);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, -9);
	sljit_emit_op2(compiler, SLJIT_SUB | SLJIT_SET_SIG_LESS, SLJIT_R0, 0, SLJIT_IMM, 0, SLJIT_R0, 0);
	/* buf[17] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 17, SLJIT_SIG_LESS);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, -9);
	sljit_emit_op2(compiler, SLJIT_SUB | SLJIT_SET_SIG_GREATER, SLJIT_R0, 0, SLJIT_IMM, 0, SLJIT_R0, 0);
	/* buf[18] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 18, SLJIT_SIG_GREATER);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 0);
	sljit_emit_op2(compiler, SLJIT_OR | SLJIT_SET_Z, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, 0);
	/* buf[19] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 19, SLJIT_ZERO);
	/* buf[20] */
	sljit_emit_op2u(compiler, SLJIT_OR | SLJIT_SET_Z, SLJIT_R0, 0, SLJIT_IMM, 1);
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 20, SLJIT_ZERO);
	sljit_emit_op2(compiler, SLJIT_XOR | SLJIT_SET_Z, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, 0);
	/* buf[21] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 21, SLJIT_ZERO);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_S1, 0, SLJIT_IMM, (sljit_sw)(~(sljit_uw)0 >> 1));
	sljit_emit_op2(compiler, SLJIT_ADD | SLJIT_SET_OVERFLOW, SLJIT_S1, 0, SLJIT_S1, 0, SLJIT_IMM, 1);
	/* buf[22] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 22, SLJIT_OVERFLOW);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R2, 0, SLJIT_IMM, (sljit_sw)((~(sljit_uw)0 >> 1) - 500));
	sljit_emit_op2u(compiler, SLJIT_ADD | SLJIT_SET_OVERFLOW, SLJIT_R2, 0, SLJIT_IMM, 500);
	/* buf[23] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 23, SLJIT_OVERFLOW);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, (sljit_sw)(~(~(sljit_uw)0 >> 1)));
	sljit_emit_op2u(compiler, SLJIT_SUB | SLJIT_SET_OVERFLOW, SLJIT_R1, 0, SLJIT_IMM, 1);
	/* buf[24] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 24, SLJIT_OVERFLOW);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_S1, 0, SLJIT_IMM, 3);
	sljit_emit_op2(compiler, SLJIT_SUB | SLJIT_SET_LESS, SLJIT_S1, 0, SLJIT_S1, 0, SLJIT_IMM, 10);
	/* buf[25] */
	sljit_emit_op_flags(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 25, SLJIT_LESS);
	/* buf[26] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 26, SLJIT_S1, 0);

	sljit_emit_return_void(compiler);

	code.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	sljit_free_compiler(compiler);

	code.func1((sljit_sw)&buf);
	FAILED(buf[1] != 1, "test8 case 1 failed\n");
	FAILED(buf[2] != 0, "test8 case 2 failed\n");
	FAILED(buf[3] != 0, "test8 case 3 failed\n");
	FAILED(buf[4] != 1, "test8 case 4 failed\n");
	FAILED(buf[5] != 1, "test8 case 5 failed\n");
	FAILED(buf[6] != 1, "test8 case 6 failed\n");
	FAILED(buf[7] != 1, "test8 case 7 failed\n");
	FAILED(buf[8] != 0, "test8 case 8 failed\n");
	FAILED(buf[9] != 1, "test8 case 9 failed\n");
	FAILED(buf[10] != 0, "test8 case 10 failed\n");
	FAILED(buf[11] != 1, "test8 case 11 failed\n");
	FAILED(buf[12] != 0, "test8 case 12 failed\n");
	FAILED(buf[13] != 1, "test8 case 13 failed\n");
	FAILED(buf[14] != 0, "test8 case 14 failed\n");
	FAILED(buf[15] != 0, "test8 case 15 failed\n");
	FAILED(buf[16] != 1, "test8 case 16 failed\n");
	FAILED(buf[17] != 0, "test8 case 17 failed\n");
	FAILED(buf[18] != 1, "test8 case 18 failed\n");
	FAILED(buf[19] != 1, "test8 case 19 failed\n");
	FAILED(buf[20] != 0, "test8 case 20 failed\n");
	FAILED(buf[21] != 1, "test8 case 21 failed\n");
	FAILED(buf[22] != 1, "test8 case 22 failed\n");
	FAILED(buf[23] != 0, "test8 case 23 failed\n");
	FAILED(buf[24] != 1, "test8 case 24 failed\n");
	FAILED(buf[25] != 1, "test8 case 25 failed\n");
	FAILED(buf[26] != -7, "test8 case 26 failed\n");

	sljit_free_code(code.code, NULL);
	successful_tests++;
}

static void test9(void)
{
	/* Test shift. */
	executable_code code;
	struct sljit_compiler* compiler = sljit_create_compiler(NULL);
	sljit_sw buf[18];
	sljit_s32 i;
#ifdef SLJIT_PREF_SHIFT_REG
	sljit_s32 shift_reg = SLJIT_PREF_SHIFT_REG;
#else /* !SLJIT_PREF_SHIFT_REG */
	sljit_s32 shift_reg = SLJIT_R2;
#endif /* SLJIT_PREF_SHIFT_REG */

	SLJIT_ASSERT(shift_reg >= SLJIT_R2 && shift_reg <= SLJIT_R3);

	if (verbose)
		printf("Run test9\n");

	FAILED(!compiler, "cannot create compiler\n");

	for (i = 0; i < 18; i++)
		buf[i] = -1;

	buf[4] = 1 << 10;
	buf[9] = 3;

	sljit_emit_enter(compiler, 0, SLJIT_ARGS1V(P), 4, 2, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 0xf);
	sljit_emit_op2(compiler, SLJIT_SHL, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, 3);
	sljit_emit_op2(compiler, SLJIT_LSHR, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, 1);
	/* buf[0] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), 0, SLJIT_R0, 0);
	sljit_emit_op2(compiler, SLJIT_SHL, SLJIT_R1, 0, SLJIT_R0, 0, SLJIT_IMM, 1);
	/* buf[1] */
	sljit_emit_op2(compiler, SLJIT_SHL, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw), SLJIT_R1, 0, SLJIT_IMM, 1);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, -64);
	sljit_emit_op1(compiler, SLJIT_MOV, shift_reg, 0, SLJIT_IMM, 2);
	/* buf[2] */
	sljit_emit_op2(compiler, SLJIT_ASHR, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 2, SLJIT_R0, 0, shift_reg, 0);

	sljit_emit_op1(compiler, SLJIT_MOV, shift_reg, 0, SLJIT_IMM, 0xff);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 4);
	sljit_emit_op2(compiler, SLJIT_SHL, shift_reg, 0, shift_reg, 0, SLJIT_R0, 0);
	/* buf[3] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 3, shift_reg, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, shift_reg, 0, SLJIT_IMM, 0xff);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 8);
	/* buf[4] */
	sljit_emit_op2(compiler, SLJIT_LSHR, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 4, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 4, SLJIT_R0, 0);
	/* buf[5] */
	sljit_emit_op2(compiler, SLJIT_SHL, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 5, shift_reg, 0, SLJIT_R0, 0);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_S1, 0, SLJIT_IMM, 0xf);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 2);
	sljit_emit_op2(compiler, SLJIT_SHL, SLJIT_S1, 0, SLJIT_S1, 0, SLJIT_R0, 0);
	/* buf[6] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 6, SLJIT_S1, 0);
	sljit_emit_op2(compiler, SLJIT_SHL, SLJIT_R0, 0, SLJIT_S1, 0, SLJIT_R0, 0);
	/* buf[7] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 7, SLJIT_R0, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R2, 0, SLJIT_IMM, 0xf00);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 4);
	sljit_emit_op2(compiler, SLJIT_LSHR, SLJIT_R1, 0, SLJIT_R2, 0, SLJIT_R0, 0);
	/* buf[8] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 8, SLJIT_R1, 0);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, (sljit_sw)buf);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 9);
	/* buf[9] */
	sljit_emit_op2(compiler, SLJIT_SHL, SLJIT_MEM2(SLJIT_R0, SLJIT_R1), SLJIT_WORD_SHIFT, SLJIT_MEM2(SLJIT_R0, SLJIT_R1), SLJIT_WORD_SHIFT, SLJIT_MEM2(SLJIT_R0, SLJIT_R1), SLJIT_WORD_SHIFT);

	sljit_emit_op1(compiler, SLJIT_MOV, shift_reg, 0, SLJIT_IMM, 4);
	sljit_emit_op2(compiler, SLJIT_SHL, shift_reg, 0, SLJIT_IMM, 2, shift_reg, 0);
	/* buf[10] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 10, shift_reg, 0);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 0xa9);
	sljit_emit_op2(compiler, SLJIT_SHL, SLJIT_R1, 0, SLJIT_R0, 0, SLJIT_IMM, 0);
	sljit_emit_op1(compiler, SLJIT_MOV32, SLJIT_R0, 0, SLJIT_IMM, 0x7d00);
	sljit_emit_op2(compiler, SLJIT_LSHR32, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, 32);
#if IS_64BIT
	sljit_emit_op1(compiler, SLJIT_MOV_U32, SLJIT_R0, 0, SLJIT_R0, 0);
#endif /* IS_64BIT */
	sljit_emit_op2(compiler, SLJIT_OR, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_R0, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 0xe30000);
#if IS_64BIT
	sljit_emit_op2(compiler, SLJIT_ASHR, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, 0xffc0);
#else /* !IS_64BIT */
	sljit_emit_op2(compiler, SLJIT_ASHR, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, 0xffe0);
#endif /* IS_64BIT */
	sljit_emit_op2(compiler, SLJIT_OR, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_R0, 0);
	sljit_emit_op1(compiler, SLJIT_MOV32, SLJIT_R0, 0, SLJIT_IMM, 0x25000000);
	sljit_emit_op2(compiler, SLJIT_SHL32, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, 0xfffe1);
#if IS_64BIT
	sljit_emit_op1(compiler, SLJIT_MOV_U32, SLJIT_R0, 0, SLJIT_R0, 0);
#endif /* IS_64BIT */
	/* buf[11] */
	sljit_emit_op2(compiler, SLJIT_OR, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 11, SLJIT_R1, 0, SLJIT_R0, 0);

	sljit_emit_op1(compiler, SLJIT_MOV, shift_reg, 0, SLJIT_IMM, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 0x5c);
	sljit_emit_op2(compiler, SLJIT_SHL, SLJIT_R1, 0, SLJIT_R0, 0, shift_reg, 0);
	sljit_emit_op1(compiler, SLJIT_MOV32, SLJIT_R0, 0, SLJIT_IMM, 0xf600);
	sljit_emit_op2(compiler, SLJIT_LSHR32, SLJIT_R0, 0, SLJIT_R0, 0, shift_reg, 0);
#if IS_64BIT
	/* Alternative form of uint32 type cast. */
	sljit_emit_op2(compiler, SLJIT_AND, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, 0xffffffff);
#endif /* IS_64BIT */
	sljit_emit_op2(compiler, SLJIT_OR, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_R0, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 0x630000);
	sljit_emit_op2(compiler, SLJIT_ASHR, SLJIT_R0, 0, SLJIT_R0, 0, shift_reg, 0);
	/* buf[12] */
	sljit_emit_op2(compiler, SLJIT_OR, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 12, SLJIT_R1, 0, SLJIT_R0, 0);

	/* Test shift_reg keeps 64 bit value after 32 bit operation. */
	sljit_emit_op1(compiler, SLJIT_MOV32, SLJIT_R0, 0, SLJIT_IMM, 0);
	sljit_emit_op1(compiler, SLJIT_MOV32, SLJIT_R1, 0, SLJIT_IMM, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, shift_reg, 0, SLJIT_IMM, -3062);
	sljit_emit_op2(compiler, SLJIT_ASHR32, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_R1, 0);
	/* buf[13] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 13, shift_reg, 0);

	sljit_emit_op1(compiler, SLJIT_MOV, shift_reg, 0, SLJIT_IMM, -4691);
	sljit_emit_op2(compiler, SLJIT_LSHR32, SLJIT_R0, 0, SLJIT_R1, 0, SLJIT_R0, 0);
	/* buf[14] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 14, shift_reg, 0);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, -0x3b7a81);
	sljit_emit_op2(compiler, SLJIT_ASHR, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, 2);
	/* buf[15] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 15, SLJIT_R0, 0);

#if IS_64BIT && (defined SLJIT_UPPER_BITS_IGNORED && SLJIT_UPPER_BITS_IGNORED)
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, SLJIT_W(0xf7c8e5aef));
	sljit_emit_op2(compiler, SLJIT_LSHR32, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, 4);
	/* buf[16] */
	sljit_emit_op1(compiler, SLJIT_MOV32, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 16, SLJIT_R1, 0);

	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_S1, 0, SLJIT_IMM, SLJIT_W(0xff7e824baf));
	sljit_emit_op2(compiler, SLJIT_ASHR32, SLJIT_S1, 0, SLJIT_S1, 0, SLJIT_IMM, 8);
	/* buf[17] */
	sljit_emit_op1(compiler, SLJIT_MOV32, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 17, SLJIT_S1, 0);
#endif /* IS_64BIT && SLJIT_UPPER_BITS_IGNORED */

	sljit_emit_return_void(compiler);

	code.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	sljit_free_compiler(compiler);

	code.func1((sljit_sw)&buf);
	FAILED(buf[0] != 0x3c, "test9 case 1 failed\n");
	FAILED(buf[1] != 0xf0, "test9 case 2 failed\n");
	FAILED(buf[2] != -16, "test9 case 3 failed\n");
	FAILED(buf[3] != 0xff0, "test9 case 4 failed\n");
	FAILED(buf[4] != 4, "test9 case 5 failed\n");
	FAILED(buf[5] != 0xff00, "test9 case 6 failed\n");
	FAILED(buf[6] != 0x3c, "test9 case 7 failed\n");
	FAILED(buf[7] != 0xf0, "test9 case 8 failed\n");
	FAILED(buf[8] != 0xf0, "test9 case 9 failed\n");
	FAILED(buf[9] != 0x18, "test9 case 10 failed\n");
	FAILED(buf[10] != 32, "test9 case 11 failed\n");
	FAILED(buf[11] != 0x4ae37da9, "test9 case 12 failed\n");
	FAILED(buf[12] != 0x63f65c, "test9 case 13 failed\n");
	FAILED(buf[13] != -3062, "test9 case 14 failed\n");
	FAILED(buf[14] != -4691, "test9 case 15 failed\n");
	FAILED(buf[15] != -0xedea1, "test9 case 16 failed\n");
#if IS_64BIT && (defined SLJIT_UPPER_BITS_IGNORED && SLJIT_UPPER_BITS_IGNORED)
	FAILED(*(sljit_s32*)(buf + 16) != 0x7c8e5ae, "test9 case 17 failed\n");
	FAILED(*(sljit_s32*)(buf + 17) != 0x7e824b, "test9 case 18 failed\n");
#endif /* IS_64BIT && SLJIT_UPPER_BITS_IGNORED */

	sljit_free_code(code.code, NULL);
	successful_tests++;
}

static void test10(void)
{
	/* Test multiplications. */
	executable_code code;
	struct sljit_compiler* compiler = sljit_create_compiler(NULL);
	sljit_sw buf[7];

	if (verbose)
		printf("Run test10\n");

	FAILED(!compiler, "cannot create compiler\n");
	buf[0] = 3;
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 6;
	buf[4] = -10;
	buf[5] = 0;
	buf[6] = 0;

	sljit_emit_enter(compiler, 0, SLJIT_ARGS1(W, P), 3, 1, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 5);
	sljit_emit_op2(compiler, SLJIT_MUL, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), 0, SLJIT_R0, 0);
	/* buf[0] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), 0, SLJIT_R0, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R2, 0, SLJIT_IMM, 7);
	sljit_emit_op2(compiler, SLJIT_MUL, SLJIT_R0, 0, SLJIT_R2, 0, SLJIT_IMM, 8);
	/* buf[1] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw), SLJIT_R0, 0);
	sljit_emit_op2(compiler, SLJIT_MUL, SLJIT_R0, 0, SLJIT_IMM, -3, SLJIT_IMM, -4);
	/* buf[2] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 2, SLJIT_R0, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, -2);
	/* buf[3] */
	sljit_emit_op2(compiler, SLJIT_MUL, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 3, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 3, SLJIT_R0, 0);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, sizeof(sljit_sw) / 2);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, (sljit_sw)&buf[3]);
	/* buf[4] */
	sljit_emit_op2(compiler, SLJIT_MUL, SLJIT_MEM2(SLJIT_R1, SLJIT_R0), 1, SLJIT_MEM2(SLJIT_R1, SLJIT_R0), 1, SLJIT_MEM2(SLJIT_R1, SLJIT_R0), 1);
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 9);
	sljit_emit_op2(compiler, SLJIT_MUL, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_R0, 0);
	/* buf[5] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 5, SLJIT_R0, 0);
#if IS_64BIT
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 3);
	sljit_emit_op2(compiler, SLJIT_MUL, SLJIT_R0, 0, SLJIT_R1, 0, SLJIT_IMM, SLJIT_W(0x123456789));
	/* buf[6] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), sizeof(sljit_sw) * 6, SLJIT_R0, 0);
#endif /* IS_64BIT */
	sljit_emit_op2(compiler, SLJIT_MUL, SLJIT_RETURN_REG, 0, SLJIT_IMM, 11, SLJIT_IMM, 10);
	sljit_emit_return(compiler, SLJIT_MOV, SLJIT_RETURN_REG, 0);

	code.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	sljit_free_compiler(compiler);

	FAILED(code.func1((sljit_sw)&buf) != 110, "test10 case 1 failed\n");
	FAILED(buf[0] != 15, "test10 case 2 failed\n");
	FAILED(buf[1] != 56, "test10 case 3 failed\n");
	FAILED(buf[2] != 12, "test10 case 4 failed\n");
	FAILED(buf[3] != -12, "test10 case 5 failed\n");
	FAILED(buf[4] != 100, "test10 case 6 failed\n");
	FAILED(buf[5] != 81, "test10 case 7 failed\n");
#if IS_64BIT
	FAILED(buf[6] != SLJIT_W(0x123456789) * 3, "test10 case 8 failed\n");
#endif /* IS_64BIT */

	sljit_free_code(code.code, NULL);
	successful_tests++;
}

static void test11(void)
{
	/* Test rewritable constants. */
	executable_code code;
	struct sljit_compiler* compiler = sljit_create_compiler(NULL);
	struct sljit_const* const1;
	struct sljit_const* const2;
	struct sljit_const* const3;
	struct sljit_const* const4;
	void* value;
	sljit_sw executable_offset;
	sljit_uw const1_addr;
	sljit_uw const2_addr;
	sljit_uw const3_addr;
	sljit_uw const4_addr;
	sljit_sw word_value1 = WCONST(0xaaaaaaaaaaaaaaaa, 0xaaaaaaaa);
	sljit_sw word_value2 = WCONST(0xfee1deadfbadf00d, 0xfbadf00d);
	sljit_sw buf[3];

	if (verbose)
		printf("Run test11\n");

	FAILED(!compiler, "cannot create compiler\n");
	buf[0] = 0;
	buf[1] = 0;
	buf[2] = 0;

	sljit_emit_enter(compiler, 0, SLJIT_ARGS1(W, P), 3, 1, 0);

	SLJIT_ASSERT(!sljit_alloc_memory(compiler, 0));
	SLJIT_ASSERT(!sljit_alloc_memory(compiler, 16 * sizeof(sljit_sw) + 1));

	/* buf[0] */
	const1 = sljit_emit_const(compiler, SLJIT_MOV, SLJIT_MEM0(), (sljit_sw)&buf[0], -0x81b9);

	value = sljit_alloc_memory(compiler, 16 * sizeof(sljit_sw));
	if (value != NULL)
	{
		SLJIT_ASSERT(!((sljit_sw)value & ((sljit_sw)sizeof(sljit_sw) - 1)));
		memset(value, 255, 16 * sizeof(sljit_sw));
	}

	/* buf[1] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 2);
	const2 = sljit_emit_const(compiler, SLJIT_MOV, SLJIT_MEM2(SLJIT_S0, SLJIT_R0), SLJIT_WORD_SHIFT - 1, -65535);
	/* buf[2] */
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, (sljit_sw)&buf[0] + 2 * (sljit_sw)sizeof(sljit_sw) - 2);
	const3 = sljit_emit_const(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_R0), 0, word_value1);

	value = sljit_alloc_memory(compiler, 17);
	if (value != NULL)
	{
		SLJIT_ASSERT(!((sljit_sw)value & ((sljit_sw)sizeof(sljit_sw) - 1)));
		memset(value, 255, 16);
	}

	/* Return value */
	const4 = sljit_emit_const(compiler, SLJIT_MOV, SLJIT_RETURN_REG, 0, (sljit_sw)0xf7afcdb7);

	sljit_emit_return(compiler, SLJIT_MOV, SLJIT_RETURN_REG, 0);

	code.code = sljit_generate_code(compiler, 0, NULL);
	CHECK(compiler);
	executable_offset = sljit_get_executable_offset(compiler);
	const1_addr = sljit_get_const_addr(const1);
	const2_addr = sljit_get_const_addr(const2);
	const3_addr = sljit_get_const_addr(const3);
	const4_addr = sljit_get_const_addr(const4);
	sljit_free_compiler(compiler);

	FAILED(code.func1((sljit_sw)&buf) != (sljit_sw)0xf7afcdb7, "test11 case 1 failed\n");
	FAILED(buf[0] != -0x81b9, "test11 case 2 failed\n");
	FAILED(buf[1] != -65535, "test11 case 3 failed\n");
	FAILED(buf[2] != word_value1, "test11 case 4 failed\n");

	/* buf[0] */
	sljit_set_const(const1_addr, SLJIT_MOV, -1, executable_offset);
	/* buf[1] */
	sljit_set_const(const2_addr, SLJIT_MOV, word_value2, executable_offset);
	/* buf[2] */
	sljit_set_const(const3_addr, SLJIT_MOV, (sljit_sw)0xbab0fea1, executable_offset);
	/* Return vaue */
	sljit_set_const(const4_addr, SLJIT_MOV, -60089, executable_offset);

	FAILED(code.func1((sljit_sw)&buf) != -60089, "test11 case 5 failed\n");
	FAILED(buf[0] != -1, "test11 case 6 failed\n");
	FAILED(buf[1] != word_value2, "test11 case 7 failed\n");
	FAILED(buf[2] != (sljit_sw)0xbab0fea1, "test11 case 8 failed\n");

	sljit_free_code(code.code, NULL);
	successful_tests++;
}


int main(void)
{
	test_exec_allocator();
	test_macros();
	test1();
	test2();
	test3();
	test4();
	test5();
	test6();
	test7();
	test8();
	test9();
	test10();
	test11();
	printf("TESTS1_11_REPRO_DONE successful_tests=%d\n", successful_tests);
	return 0;
}
