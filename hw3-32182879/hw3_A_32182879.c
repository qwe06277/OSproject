
// virtual memory simulation
// gets input from input_vm
// prints output va -> pa

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "page.h"

// assume Pmem 1MB
int PMem[0x100000];

void init_pmem()
{
	int i;
	for (i = 0 ; i < 0x100000 ; i++) {
		PMem[i] = i;
	}
}
static struct frame *free_list;

int access_pa(int pid, int va)
{
int pa = 0;
//1. pg number pg offset separate
int pg_number = va/0x1000; //in front of number of va
int pg_offset = va%0x1000; //behind number of va
//2. find page frame number

if(pg_table[0][pg_number] == -1){//if non-pfn
free_list = get_free_frame();
pg_table[0][pg_number] = free_list->pfn*0x1000;//mapping
}
pa = pg_table[0][pg_number]*0x1000 + pg_offset;
return pa;
}

void main(int argc, char *argv[])
{
	char *input_file_name = "input_vm";;
	FILE *fp; //input file ptr
	char *line = NULL;
	char *saveptr;

	char *pid_str;
	char *va_str;
	int pid;
	int va;
	int pa;
	char *e;

	int pg;
	int pg_offset;
	size_t len;

	if (argc == 2) {
		input_file_name = argv[1];
	}
	fp = fopen(input_file_name, "r");

	page_init();
	init_pmem();

	// get a line from input
	while (getline(&line, &len, fp) != -1) {
		pid_str = strtok_r(line, " \n", &saveptr);
		va_str = strtok_r(NULL, " \n", &saveptr);

		pid = strtol(pid_str, &e, 16);
		va = strtol(va_str, &e, 16);
		pa = access_pa(pid,va); // needs to be translated from va
		printf("pid: %d, va: 0x%08x pa: [0x%08x] = 0x%08X\n", pid, va, pa, PMem[pa]);
	}

	fclose(fp);
}
