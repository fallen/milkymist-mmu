/* MMU test generator
 * Author : Yann Sionneau <yann.sionneau@gmail.com>
 */

#include <stdio.h>

static inline void generate_test(int i, int j) {
	int k;

	puts("asm volatile(");
	puts("\t\"xor r11, r11, r11\\n\\t\"");
	puts("\t\"ori r11, r11, 0x11\\n\\t\"");
	puts("\t\"wcsr tlbctrl, r11\\n\\t\"");
	puts("\t\"xor r0, r0, r0\\n\\t\"");
	puts("\t\"xor r0, r0, r0\\n\\t\"");
	puts("\t\"xor r0, r0, r0\\n\\t\"");
	for (k = 0 ; k < 6 ; k++) {
		if (k == i) {
			printf("\t\"sw (%3+0), %2");
		}
		else if(k == j) {
			printf("\t\"lw %0, (%3+0)");
		}
		else
			printf("\t\"xor r0, r0, r0");
		if (k != 5)
			printf("\\n\\t");
		puts("\"");
	}

	puts(": \"=&r\"(value_verif), \"=&r\"(tlb_lookup) : \"r\"(value), \"r\"(addr) :\"r11\"\n);");

}

int main(void) {

	puts("#include <hal/mmu.h>\n");
	puts("void dtlb_load_test(void) {\n");

	puts(	
		"char a, b, c, d;\n"
		"// map vaddr 0x4400 1000 to paddr 0x4400 0000\n"
		"register unsigned int value, addr, value_verif, stack, tlb_lookup;\n"
		"int success, failure;\n"
//		"mmu_dtlb_map(0x00002000, 0x00001000);\n" // for the test
		"mmu_dtlb_map(0x44002000, 0x44001000);\n" // for the test
		"asm volatile(\"mv %0, sp\" : \"=r\"(stack) :: );\n"
//		"mmu_dtlb_map(stack, stack);\n"
//		"mmu_dtlb_map(stack+0x1000, stack+0x1000);\n"
//		"mmu_dtlb_map(stack-0x1000, stack-0x1000);\n"
		"printf(\"stack == 0x%08X\\n\", stack);"
		"a = 0;\n"
		"b = 1;\n"
		"c = 2;\n"
		"d = 3;\n"
		"addr = 0x44002000;\n"
		"success = 0;\n"
		"failure = 0;"
	);

	int test_num = 0;
	int i, j;

	for (i = 0 ; i < 5 ; i++) {
		for (j = i+1 ; j < 6 ; j++) {

			puts(	"value = a << 24;\n"
				"value |= b << 16;\n"
				"value |= c << 8;\n"
				"value |= d;\n"
				"addr += 4;\n"

				//"enable_dtlb();"

			);


			generate_test(i, j);

			puts("disable_dtlb();");
			puts("printf(\"tlb_lookup = 0x%08X\\n\", tlb_lookup);");
			printf("printf(\"Test n° %02d : \");\n", test_num);
			puts(	"if (value == value_verif) {\n"
					"\tputs(\"PASS\");\n"
					"\tsuccess++;\n"
				"} else {\n"
					"\tputs(\"FAIL\");\n"
					"\tfailure++;\n"
				"}"
			);

			puts(	"a++;\n"
				"b++;\n"
				"c++;\n"
				"d++;\n\n"
			);
			test_num++;
		}
	}

	puts("printf(\"TOTAL : %d/%d successes | %d/%d failures\\n\", success, success + failure, failure, success + failure);");

	puts("}");

	return 0;
}
