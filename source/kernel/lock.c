/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   ������
 */

#include <kernel/ke_lock.h>
#include <kernel/ke_rwlock.h>

#include "spinlock.h"

#define __KE_SPIN_LOCK_UNLOCKED(lockname)	\
	(struct ke_spinlock) __RAW_SPIN_LOCK_INITIALIZER(lockname)
# define __spin_lock_init(lock)				\
	do { *(lock) = __KE_SPIN_LOCK_UNLOCKED(lock); } while (0)

DLLEXPORT void ke_spin_init(struct ke_spinlock * lock)
{
	__spin_lock_init(lock);
}

DLLEXPORT void ke_spin_lock(struct ke_spinlock * lock)
{
	arch_spin_lock((arch_spinlock_t*)lock);
}

DLLEXPORT void ke_spin_unlock(struct ke_spinlock * lock)
{
	arch_spin_unlock((arch_spinlock_t*)lock);
}

DLLEXPORT bool ke_spin_lock_try(struct ke_spinlock * lock)
{
	arch_spin_trylock((arch_spinlock_t*)lock);
}

/**
 @brief ���������������ظ�IRQ״̬
 */
DLLEXPORT void ke_spin_unlock_irqrestore(struct ke_spinlock * lock, unsigned long flags)
{
	_raw_spin_unlock_irqrestore((arch_spinlock_t*)lock, flags);
}

/**
 @brief �������������ر�IRQ�������عر�IRQǰ��״̬
 */
DLLEXPORT unsigned long ke_spin_lock_irqsave(struct ke_spinlock * lock)
{
	unsigned long flags;
	_raw_spin_lock_irqsave((arch_spinlock_t*)lock, flags);
	return flags;
}

DLLEXPORT void ke_spin_lock_irq(struct ke_spinlock * lock)
{
	local_irq_disable();
	ke_spin_lock(lock);
}

DLLEXPORT void ke_spin_unlock_irq(struct ke_spinlock * lock)
{
	ke_spin_unlock(lock);
	local_irq_enable();
}

DLLEXPORT void ke_spin_lock_may_be_long(struct ke_spinlock * lock)
{
	//TODO
}

/************************************************************************/
/* About rwlock                                                         */
/************************************************************************/

/**
	@brief ��ʼ����д��
*/
void ke_rwlock_init(struct ke_rwlock * rwlock)
{
	struct __rwlock_t
	{
		arch_rwlock_t raw_lock;
	} * wrap = (struct __rwlock_t*)rwlock;
	
	*wrap = (struct __rwlock_t)	{.raw_lock = __ARCH_RW_LOCK_UNLOCKED};
}

/**
	@brief ��д��
*/
void ke_rwlock_write_lock(struct ke_rwlock * rwlock)
{
	arch_write_lock((arch_rwlock_t*)rwlock);
}

void ke_rwlock_read_lock(struct ke_rwlock * rwlock)
{
	arch_read_lock((arch_rwlock_t*)rwlock);
} 

/**
	@brief ��д������д��
*/
void ke_rwlock_write_unlock(struct ke_rwlock * rwlock)
{
	arch_write_unlock((arch_rwlock_t*)rwlock);
}

void ke_rwlock_read_unlock(struct ke_rwlock * rwlock)
{
	arch_read_unlock((arch_rwlock_t*)rwlock);
}
