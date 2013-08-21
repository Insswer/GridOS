//#include <compiler.h>

#include <process.h>
#include <thread.h>
#include <section.h>
#include <walk.h>
#include <memory.h>

#include <asm/system.h>
#include <asm/desc.h>
#include <asm/ptrace.h>

extern void *hal_x86_get_init_pgtable();

static bool address_in_user(unsigned long address)
{
	if (address < HAL_GET_BASIC_KADDRESS(0))
		return true;
	return false;
}

/**	
	@brief Page fault handler
*/
asmregparm void do_page_fault(struct pt_regs * regs, long error_code)
{
	unsigned long error_address = read_cr2();	
	bool in_user = address_in_user(error_address);
	struct ko_thread *current;	 
	
	/*
		Access violation.
		����д��u/s�쳣
		������û�ģʽ����ô���ʺ��Ĳ�ռ�ʱ�����������ط������޸�
		�������Ȩģʽ���������޸�
	*/
	if (error_code & PAGE_FAULT_U)
	{
		if (in_user == false)
			goto die_cur;			
		
	}
	//printk("eip %x addr %x code %x\n", regs->ip, error_address, error_code);
	
	current = kt_current();
	if (unlikely(false == ks_exception(current, error_address, error_code)))
		goto die_cur;

	return;	

die_cur:
	printk("TODO: Thread deing :eip %x addr %x code %x\n", regs->ip, error_address, error_code);

	//TODO kill thread
	//fak_arch_x86_dump_register(regs);
	while(1);
}

__init void km_arch_trim()
{
	unsigned long i;
	
	unsigned long *p = hal_x86_get_init_pgtable();
	
	/*
	 Kernel part in 4mb page;
		1,�Ͷˡ�
		2,�߶�
	*/
	for(i = 0; i < HAL_GET_BASIC_KADDRESS(0) / 0x400000; i++)
		p[i] = 0;																//Mask as invalid;
	for(i = (HAL_GET_BASIC_KADDRESS(0) + CONFIG_HAL_KERNEL_MEM_LEN) / 0x400000; i < 1024; i++)
		p[i] = 0;																//Mask as invalid;
	
	write_cr3((unsigned long)hal_x86_get_init_pgtable() - HAL_GET_BASIC_KADDRESS(0));

	/*
	 load tr
	 */
	set_tss_desc(0, &init_tss);
	load_TR_desc();
	
	/*
	 ��WP����
	 */
	do {
		unsigned long i = read_cr0();
		//if(open)
		i |= 1 << 16;
		//else
		//	i &= ~(1 << 16);
		write_cr0(i);
	} while (0);
}

void km_arch_init_for_kernel(struct km *mem)
{
	mem->translation_table = hal_x86_get_init_pgtable();
}