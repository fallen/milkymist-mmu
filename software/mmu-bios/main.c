void puts(char *str)
{

	while (*str != '\0')
	{
//		*((unsigned int *)0xe0000000) = *str;
		str++;
	}

}

void printf(char *str)
{
	puts(str);
}

int main(int argc, char **argv)
{

	asm volatile("wcsr IE, r0");
	dtlb_load_test();

	while (1)
	{
		asm volatile("xor r0, r0, r0");
	}

	return 0;
}
