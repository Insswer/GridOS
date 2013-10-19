/**
	The Grid Core Library
 */

/**
	Posix file
	ZhaoYu,Yaosihai
 */
#include <compiler.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include "file.h"

#define FLAG_LENGTH 3

/*
	���ڼ���ַ������Ƿ����ظ����ַ�
	����ֵΪtrue˵�����ظ���Ϊfalse˵�����ظ�
 
	���㷨�ÿռ任ʱ��,TODO:�д��Ż�
 */
static bool check_repeat(const char *str)
{
	/* ASCII�����ֵΪ255�����㷨�����������ռ任ȡ�㷨Ч�� */
	unsigned char ascii_array[256] = {0};
	
	while (*str != '\0')
	{
		/* �ַ�ASCII���Ӧ������Ԫ�ؼ�1 */
		ascii_array[*str]++;
		
		/* ͬһ�ַ�ASCII���Ӧ������Ԫ��������ڵ���2��˵�����ظ� */
		if (ascii_array[*str] >= 2) return true;
		str++;
	}
	
	return false;
}

/*
	@brief Ϊstdio_file���þ����flags
 
	r	��ֻ���ļ������ļ�������ڣ�
	r+	�򿪿ɶ�д�ļ������ļ�������ڣ�
	w	��ֻд�ļ������ļ�����������ļ����ݣ����ļ�����Ϊ0��
		���ļ��������򴴽��ļ���
	w+	�򿪿ɶ�д�ļ������ļ�����������ļ����ݣ����ļ�����Ϊ0��
		���ļ��������򴴽��ļ���
	a	���ӷ�ʽ��ֻд�ļ������ļ�������д������ֻ��׷�����ļ�β���ļ�ԭ�����޷��޸ģ�
		���ļ��������򴴽��ļ���
	a+	���ӷ�ʽ�򿪿ɶ�д�ļ������ļ�������д������ֻ��׷�����ļ�β���ļ�ԭ�����޷��޸ģ�
		���ļ��������򴴽��ļ���
 
	b	��������
 
	@return �ɹ�����true��ʧ���򷵻�false
 */
static bool set_file_flags(struct stdio_file *file, const char *type)
{
	bool ret = true;
	int	access_modes, status_flags, count;
	
	if (strlen(type) > FLAG_LENGTH) return false;
	
	/* ���flags���Ƿ����ظ����� */
	if (check_repeat(type)) return false;
	
	/* r/w/aΪ��һ���ַ� */
	switch (type[0])
	{
		case 'r':
			access_modes = O_RDONLY;
			status_flags = 0;
			break;
		case 'w':
			access_modes = O_WRONLY;
			status_flags = O_CREAT | O_TRUNC;
			break;
		case 'a':
			access_modes = O_WRONLY;
			status_flags = O_CREAT | O_APPEND;
			break;
			
		default:
			return false;
	}
	
	/* 'b'��'+'λ��/r/w/a����˳��ɲ������ƣ���/r/w/ab+��/r/w/a+b��Ч */
	for (type++; *type != '\0'; type++)
	{
		switch (*type)
		{
			case 'b':
				break;
			case '+':
				access_modes = O_RDWR;
				break;
			default:
				return false;
		}
	}
	
	file->flags = access_modes | status_flags;
	return ret;
}

static bool set_file_fd(struct stdio_file *file, const char *path)
{
	int fd;
	bool ret = true;
	
	// TODO LOCK fd
	
	fd = sys_open(path, S_IFREG);
	
	if (POSIX_INVALID_FD == fd)
	{
		if (file->flags & O_CREAT)
		{
			/* ��������Ϊ0���ļ� */
			fd = sys_mkfile(path);
		}
		else
		{
			/* ���ܴ򿪲����ڵ��ļ� */
			return false;
		}
	}
	/* �ļ����ڣ���Ҫ���ݽ�һ������ */
	else if (file->flags & O_TRUNC)
	{
		/* �ļ����Ƚض�Ϊ0 */
		if (-1 == sys_ftruncate(fd, 0)) return false;
	}
	
	/* Ϊ�ļ���������fd */
	file->fd = fd;
	// TODO UNLOCK fd
	
	return ret;
}

static struct stdio_file *open_stream_file(const char *path, const char *type)
{
	struct stdio_file *file;
	
	file = malloc(sizeof(struct stdio_file));
	if (!file)	goto end_exit;
	memset(file, 0, sizeof(struct stdio_file));
	
	/*  */
	if (false == set_file_flags(file, type)) goto err;
	
	/* �ļ������� */
	if (false == set_file_fd(file, path)) goto err;
	
	stream_file_init_ops(file);
	
	return file;
	
err:
	free(file);
	
	file = NULL;
	
end_exit:
	return file;
}

DLLEXPORT FILE *fopen(const char *path, const char *type)
{
	return (FILE *)open_stream_file(path, type);
}