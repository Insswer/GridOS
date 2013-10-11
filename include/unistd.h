/**
*  @defgroup posix_unistd
*  @ingroup ��׼C���̽ӿ�
*
*  posix unistd
*
*  @{
*/
#ifndef _UNISTD_H
#define _UNISTD_H

#include <types.h>

BEGIN_C_DECLS;

int ftruncate(int fd, off_t length);

/**
	@brief Sleep for the specified number of seconds

	Makes the calling process sleep until seconds seconds have elapsed or a signal arrives which is not ignored.

	@return
		Zero if the requested time has elapsed, or the number of seconds left to sleep, if the call was interrupted by a signal handler.
*/
unsigned int sleep(unsigned int seconds);

/** 
	@brief �л���ǰ�̹߳���Ŀ¼

	�л�Ŀ¼�Ĳ���ֻӰ�쵱ǰ�̣߳���������ͬһ�����̲�Ӱ�������̡߳�

	@param[in] path Ҫ�л���Ŀ¼�������������·������Ե�ǰ�̹߳�����·������Ҳ���Ծ���·������..����ʾ��һ��·����

	@return
		�ɹ�����0��ʧ�ܷ���-1��
*/
int chdir(const char *path);

END_C_DECLS;

#endif

/** @} */