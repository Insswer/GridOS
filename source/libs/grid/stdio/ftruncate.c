/**
	The Grid Core Library
 */

/**
	Stream file
	ZhaoYu,Yaosihai
 */
#include "file.h"

#include "sys/file_req.h"


/**
	@brief �����ļ������С

	@param[in] int fd			Ŀ���ļ�������ļ�������
	@param[in] ssize_t length	��Ŀ���ļ����󳤶ȵ���Ϊlength

	@return	�ɹ�����0��ʧ����Ϊ-1;
*/
DLLEXPORT int ftruncate(int fd, off_t length)
{
	int ret;
	
	ret = sys_ftruncate(fd, length);
	if (0 == ret)
	{
#if 0
		struct stdio_file * file = mltt_entry_locate(get_libc_mltt(), fd);
		file->size = length;
#endif
		//TODO
	}
	
	return ret;
}