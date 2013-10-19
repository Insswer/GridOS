/**
	The Grid Core Library
 */

/**
	Posix file
	ZhaoYu,Yaosihai
 */

#ifndef __CRT_FILE_H__
#define __CRT_FILE_H__

#include <types.h>
#include "posix.h"

/**
	@brief ��ȡ����
 
	@param[in] void *ptr	Դ��ַ
	@param[in] void *buf	Ŀ�ĵ�ַ���õ�ַָʾ�Ŀռ����ڴ��Դ��ַ��ȡ������
	@param[in] ssize_t n_bytes	��Ҫ��ȡ���ֽ���
 
	@return �ɹ�����ʵ�ʶ�ȡ�����ֽ�����ʧ����Ϊ����;
 */
typedef ssize_t (*stdio_read)	(void *ptr, void *buf, ssize_t n_bytes);

/**
	@brief д������
 
	@param[in] void *ptr	Ŀ�ĵ�ַ
	@param[in] void *buf	Դ��ַ���õ�ַָʾ�Ŀռ����ڴ����Ҫд��Ŀ�ĵ�ַ������
	@param[in] ssize_t n_bytes	��Ҫд����ֽ���
 
	@return �ɹ�����ʵ��д����ֽ�����ʧ����Ϊ����;
 */
typedef	ssize_t (*stdio_write)	(void *ptr, void *buf, ssize_t n_bytes);

/**
	@brief ����ƫ����
 
	@param[in] void *ptr
	@param[in] long offset	�����whence��ƫ����
	@param[in] int whence
 
	@return	�ɹ������µ�ƫ������ʧ����Ϊ����;
 */
typedef int (*stdio_seek)	(void *ptr, loff_t offset, int whence);

/**
	@brief �ر��ļ�
 
	@param[in] void *ptr
	
	@return	�ɹ�����0��ʧ����Ϊ����;
 */
typedef	ssize_t	(*stdio_close)	(void *ptr);

struct file_operations
{
	stdio_read		read;
	stdio_write		write;
	stdio_seek		seek;
	stdio_close		close;
};

/* Real file */
#include "stream_file.h"

#endif