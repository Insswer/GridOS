/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *
 *   ϵͳ��ʼ������
 */

#include <stdio.h>

int test_bss_data;

void main()
{
	FILE *fp;
	int r;
	char str[32] = {0};
	
	printf("��ʼ������������(stack = %p)...\n", &fp);
	

	y_process_create("NES", "0:\\os\\i386\\nes.exe 0:\\os\\i386\\90.nes");
}