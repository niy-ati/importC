/* Minimal, standalone reproduction of sljit's real test11() (rewritable
 * constants test, from sljit's own test_src/sljitTest.c at commit 39c508d1),
 * stripped of everything except that one test's own logic, to check whether
 * it alone reproduces the ImportC codegen crash without the other 137 tests. */

#include <stdio.h>
#include <string.h>
#include "sljitLir.h"

union executable_code {
	void* code;
	sljit_sw (SLJIT_FUNC *func1)(sljit_sw a);
};
typedef union executable_code executable_code;

#define WCONST(const64, const32) ((sljit_sw)SLJIT_W(const64))

static int failures = 0;

#define CHECK_EQ(actual, expected, msg) \
	if ((actual) != (expected)) { \
		printf("FAIL: %s (got %lld, expected %lld)\n", msg, (long long)(actual), (long long)(expected)); \
		failures++; \
	}

static void test11(void)
{
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

	printf("Run test11\n");

	if (!compiler) { printf("cannot create compiler\n"); failures++; return; }
	buf[0] = 0;
	buf[1] = 0;
	buf[2] = 0;

	sljit_emit_enter(compiler, 0, SLJIT_ARGS1(W, P), 3, 1, 0);

	value = sljit_alloc_memory(compiler, 0);
	value = sljit_alloc_memory(compiler, 16 * (int)sizeof(sljit_sw) + 1);

	/* buf[0] */
	const1 = sljit_emit_const(compiler, SLJIT_MOV, SLJIT_MEM0(), (sljit_sw)&buf[0], -0x81b9);

	value = sljit_alloc_memory(compiler, 16 * sizeof(sljit_sw));
	if (value != NULL)
		memset(value, 255, 16 * sizeof(sljit_sw));

	/* buf[1] */
	sljit_emit_op1(compiler, SLJIT_MOV, SLJIT_R0, 0, SLJIT_IMM, 2);
	const2 = sljit_emit_const(compiler, SLJIT_MOV, SLJIT_MEM2(SLJIT_S0, SLJIT_R0), SLJIT_WORD_SHIFT - 1, -65535);
	/* buf[2] */
	sljit_emit_op2(compiler, SLJIT_ADD, SLJIT_R0, 0, SLJIT_R0, 0, SLJIT_IMM, (sljit_sw)&buf[0] + 2 * (sljit_sw)sizeof(sljit_sw) - 2);
	const3 = sljit_emit_const(compiler, SLJIT_MOV, SLJIT_MEM1(SLJIT_R0), 0, word_value1);

	value = sljit_alloc_memory(compiler, 17);
	if (value != NULL)
		memset(value, 255, 16);

	/* Return value */
	const4 = sljit_emit_const(compiler, SLJIT_MOV, SLJIT_RETURN_REG, 0, (sljit_sw)0xf7afcdb7);

	sljit_emit_return(compiler, SLJIT_MOV, SLJIT_RETURN_REG, 0);

	code.code = sljit_generate_code(compiler, 0, NULL);
	if (sljit_get_compiler_error(compiler) != SLJIT_ERR_COMPILED) {
		printf("Compiler error: %d\n", sljit_get_compiler_error(compiler));
		failures++;
		sljit_free_compiler(compiler);
		return;
	}
	executable_offset = sljit_get_executable_offset(compiler);
	const1_addr = sljit_get_const_addr(const1);
	const2_addr = sljit_get_const_addr(const2);
	const3_addr = sljit_get_const_addr(const3);
	const4_addr = sljit_get_const_addr(const4);
	sljit_free_compiler(compiler);

	CHECK_EQ(code.func1((sljit_sw)&buf), (sljit_sw)0xf7afcdb7, "test11 case 1");
	CHECK_EQ(buf[0], -0x81b9, "test11 case 2");
	CHECK_EQ(buf[1], -65535, "test11 case 3");
	CHECK_EQ(buf[2], word_value1, "test11 case 4");

	/* buf[0] */
	sljit_set_const(const1_addr, SLJIT_MOV, -1, executable_offset);
	/* buf[1] */
	sljit_set_const(const2_addr, SLJIT_MOV, word_value2, executable_offset);
	/* buf[2] */
	sljit_set_const(const3_addr, SLJIT_MOV, (sljit_sw)0xbab0fea1, executable_offset);
	/* Return value */
	sljit_set_const(const4_addr, SLJIT_MOV, -60089, executable_offset);

	CHECK_EQ(code.func1((sljit_sw)&buf), -60089, "test11 case 5");
	CHECK_EQ(buf[0], -1, "test11 case 6");
	CHECK_EQ(buf[1], word_value2, "test11 case 7");
	CHECK_EQ(buf[2], (sljit_sw)0xbab0fea1, "test11 case 8");

	sljit_free_code(code.code, NULL);
}

int main(void)
{
	test11();
	if (failures == 0)
		printf("TEST11_REPRO_PASSED\n");
	else
		printf("TEST11_REPRO_FAILED (%d checks)\n", failures);
	return failures;
}
