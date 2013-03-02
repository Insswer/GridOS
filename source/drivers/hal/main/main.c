/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL Main
*/

#include <arch/arch.h>

void __init __noreturn hal_main()
{
	/* ���ٺ���,˭Ϊ���֣������һ��*/
	hal_arch_init(HAL_ARCH_INIT_PHASE_EARLY);
	hal_malloc_init();

	/* IRQ,ƽ̨�ĳ�ʼ������ƽ̨���жϣ�����������Ϣ */
	hal_irq_init();
	hal_arch_init(HAL_ARCH_INIT_PHASE_MIDDLE);

	/* DPC */
	hal_dpc_init();
	hal_timer_init();
	hal_hardware_init();
	hal_console_init();

	/* ƽ̨���ڳ�ʼ��,�����������������������������ж�*/
	hal_arch_init(HAL_ARCH_INIT_PHASE_LATE);

	/* Manager */
	kma_setup();
	kmm_init();
	kmp_init();
	ko_init();
	kp_init();
	ks_init();	

	while(1)
	{
	}
}