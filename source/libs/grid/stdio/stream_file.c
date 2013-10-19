/**
	The Grid Core Library
 */

/**
	Stream file
	ZhaoYu,Yaosihai
 */

#include <stdio.h>
#include <stdlib.h>
#include <types.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "sys/file_req.h"
#include "file.h"

static struct buffer_block buffer_pool[BUF_BLOCK_NUM];

/**
	@brief Append a file by fd
 */
static ssize_t append_stream_file(int fd, void *user_buffer, uoffset old_file_size, ssize_t append_bytes)
{
	if (-1 != ftruncate(fd, old_file_size + append_bytes))
	{
		return sys_write(fd, user_buffer, old_file_size, append_bytes);
	}
	else
	{
		return -1;
	}
}

/**
	@brief ��buffer block�е������ݸ��µ��ļ�
 
	@return �ɹ�����true��ʧ���򷵻�false
 
	@note ��������������Ҫ�����ķ����ڣ���Ϊbuffer block������ʱ����ռ
 */
static bool update_dirty_buffer_block(struct stdio_file *file)
{
	bool ret = true;
	int size = -1;
	
	if (!file) return false;
	
	if (file->block->flags & BUF_BLOCK_DIRTY_FLAG)
	{
		/*
		 ���1��O_APPENDģʽ�£��κ�д�������ֻ��׷�ӵ��ļ�β��
		 ���2����fO_APPENDģʽ�£����file->pos > file->size��д�����ݳ����ļ���С����׷�ӵ��ļ�β��
		 */
		if (file->flags & O_APPEND || file->pos > file->size)
		{
			size = append_stream_file(file->fd, file->block->base, file->size, file->pos - file->size);
		}
		else
		{
			/* ��buffer block��BUF_BLOCK_OFFSET(file->pos)�ֽ�д��file��file->pos-BUF_BLOCK_OFFSET(file->pos)��ʼ�� */
			size = sys_write(file->fd, file->block->base, file->pos - BUF_BLOCK_OFFSET(file->pos), BUF_BLOCK_OFFSET(file->pos));
		}
		size = BUF_BLOCK_OFFSET(file->pos);
		
		/* ֻҪ���Ǵ���IO����û���� */
		/*
		 if (errno == EIO)
		 {
		 file->block->flags &= (~BUF_BLOCK_DIRTY_FLAG);
		 ret = false;
		 }*/
	}
	return ret;
}

/**
	@brief ����buffer block
 
	��֤��buffer block�ܱ���Ϊ����ʹ�ã�ͬʱ�����������Ҳ��Ҫ���µ��ļ�
	
	@return �ɹ�����true��ʧ���򷵻�false
 */
static bool swap_out_buffer_block(struct buffer_block *block)
{
	bool ret = false;
	
	ret = update_dirty_buffer_block(block->file);
	
	if (true == ret)
	{
		/* ���ԭfile�͸�buffer block�Ĺ�ϵ��������Ӱ�쵽ԭfile���������� */
		block->file->block		= NULL;
		block->file				= NULL;
		block->access_count		= 0;
		block->flags			= 0;
		block->valid_size		= 0;
		block->pre_id			= 0;
	}
	
	return ret;
}

/**
	@brief �ӻ��������ҳ��ɱ��滻Ϊ���е�buffer block
 
	@return �ɹ�����ָ���ػ����ɵ�buffer block����ָ�룬ʧ���򷵻�NULL
 */
static struct buffer_block *get_rebirth_buffer_block()
{
	int i, mix_access_count, mix_index;
	struct buffer_block *rebirth_buffer_block;
	
	/* ���ҷ��ʼ�����С��buffer block */
	mix_access_count	= buffer_pool[0].access_count;
	mix_index			= 0;
	for (i = 1; i < BUF_BLOCK_NUM; i++)
	{
		if (buffer_pool[i].access_count < mix_access_count)
		{
			mix_access_count	= buffer_pool[i].access_count;
			mix_index			= i;
		}
	}
	
	/* ��֤buffer block��ԭӵ����file���ܲ���buffer block */
	LOCK_FILE(buffer_pool[mix_index].file);
	/* buffer block����æʱ */
	if (!(buffer_pool[mix_index].flags & BUF_BLOCK_BUSY_FLAG))
	{
		/* ����buffer block */
		i = swap_out_buffer_block(&(buffer_pool[mix_index]));
		if (true == i)
		{
			rebirth_buffer_block = &(buffer_pool[mix_index]);
		}
	}
	UNLOCK_FILE(buffer_pool[mix_index].file);
	
	return rebirth_buffer_block;
}

/**
	@brief �ӻ������з������buffer block
 
	@return �ɹ�����ָ�����buffer block��ָ�룬ʧ���򷵻�NULL
 */
static struct buffer_block *get_free_buffer_block()
{
	int i, index;
	struct buffer_block *block;
	
	// TODO lock buffer_pool
	for (i = 0; i < BUF_BLOCK_NUM; i++)
	{
		/* buffer_block��file�ֶ�ΪNULL˵����buffer block���� */
		if (!buffer_pool[i].file)
		{
			block = &(buffer_pool[i]);
			break;
		}
	}
	// TODO unlock lock buffer_pool
	
	return block;
}

/**
	@brief Ϊstdio_file�������buffer block
 */
static void get_buffer_block(struct stdio_file *file)
{
	int i, index;
	struct buffer_block *block;
	
start:
	/* ��buffer pool�з������buffer block */
	file->block = get_free_buffer_block();
	if (NULL != file->block) goto end;
	
	/*
	 ˵��buffer pool��û�п���buffer block
	 ��buffer poolѡ��һ�����ʵ�buffer block��Ϊ����buffer block����
	 */
	file->block = get_rebirth_buffer_block();
	if (NULL != file->block) goto end;
	
	goto start;
	
end:
	file->block->file = file;
	file->block->flags |= BUF_BLOCK_BUSY_FLAG;
	file->block->access_count++;
}

static void put_buffer_block(struct stdio_file *file)
{
	file->block->flags &= (~BUF_BLOCK_BUSY_FLAG);	/* ȡ��BUF_BLOCK_BUSY_FLAGλ */
}


/**
	@brief Ϊ���buffer block

	@return �ɹ�����true��ʧ���򷵻�false
*/
static bool make_valid_data(struct stdio_file *file)
{
	int size;
	bool ret = true;
	int pos;

	/* buffer block����Ҳ�����л�buffer blockʱ��������ݸ��� */
	if (file->block->pre_id != BUF_BLOCK_ID(file->pos))
	{
		update_dirty_buffer_block(file);
		file->block->flags		&= (~BUF_BLOCK_VALID_FLAG);
	}

	/* Disk to buffer block if buffer is not valid */
	if (!(file->block->flags & BUF_BLOCK_VALID_FLAG))
	{
		/* 
			��file����BUF_BLOCK_SIZE�����ָ�ָ��buffer block�ı任���£�
			BUF_BLOCK_ID = 0��ӳ���ļ�0~255
			���У�file->pos = 0��ʾ�ļ����ֽڣ�file->pos = 255��ʾ�ļ���256�ֽڣ���buffer block��ĩ�ֽ�

			BUF_BLOCK_ID = 1��ӳ���ļ�256~511
			����
		*/
		pos = BUF_BLOCK_ID(file->pos) * BUF_BLOCK_SIZE;
		/* ��file��pos��ʼ������ȡBUF_BLOCK_SIZE�ֽڵ�buffer block�� */
		size = sys_read(file->fd, file->block->base, pos, BUF_BLOCK_SIZE);
		if (size > 0)
		{
			file->block->valid_size	= size;
			file->block->flags		|= BUF_BLOCK_VALID_FLAG;
		}
		else
		{
			ret = false;
		}
	}

	return ret;
}

/**
	@brief ��ȡ����

	@return ������ʾʵ�ʶ�ȡ���ֽ�����-1��ʾ����
*/

static ssize_t stdio_fread(void *ptr, void *buf, ssize_t n_bytes)
{		
	size_t start_pos, tmp_len, i;
	struct stdio_file *file	= (struct stdio_file *)ptr;

	/* �ļ�ֻ����д���� */
	if (file->flags & O_WRONLY)
	{
		set_errno(EPERM);
		return -1;
	}

	/* �ļ��������� */
	if (0 == file->size)
	{
		return 0;
	}

	LOCK_FILE(file);

	/* �ļ�����δ����buffer blockʱ��Ҫ���� */
	if (!file->block)
		get_buffer_block(file);	

	/* ����淶��type��'+'�����ƣ���֤freadʱ�����ݱ����� */
	update_dirty_buffer_block(file);

	tmp_len		= i = 0;
	start_pos	= file->pos;

	/* file->pos = (file->size - 1)��ʾfileĩ�ֽ� */
	while (file->pos < file->size && n_bytes > 0)
	{
		/* make_valid_dataʧ����ѭ��ִ�У�ֱ���ɹ� */
		if (make_valid_data(file))
		{
			/* tmp_len����С��0����Ϊfile->posС��file->size */
			tmp_len = (file->block->valid_size - BUF_BLOCK_OFFSET(file->pos));
			if (tmp_len > n_bytes)
				tmp_len = n_bytes;
			
			/* �����tmp_lenΪ0������� */


			memcpy(buf + i, file->block->base + BUF_BLOCK_OFFSET(file->pos), tmp_len);
	
			file->block->pre_id = BUF_BLOCK_ID(file->pos);
			n_bytes				-= tmp_len;
			file->pos			+= tmp_len;
			i					+= tmp_len;
		}
		else
		{
			/* seterrno */
			
			goto err;
		}
	}

err:
	put_buffer_block(file);

	UNLOCK_FILE(file);

	return file->pos - start_pos;
}

/**
	@brief ��ȡ����

	@return ������ʾʵ�ʶ�ȡ���ֽ�����0��ʾ�ѵ��ļ���β��-1��ʾ����
*/
static ssize_t stdio_fwrite(void *ptr, void *buf, ssize_t n_bytes)
{
	size_t start_pos, tmp_len, i;
	struct stdio_file *file	= (struct stdio_file *)ptr;

	/* �ļ�ֻ��������� */
	if((file->flags & O_WRONLY) == 0 && (file->flags & O_RDWR) == 0)
	{
		return 0;	
	}

	LOCK_FILE(file);

	/* �ļ�����δ����buffer blockʱ��Ҫ���� */
	if (!file->block)
		get_buffer_block(file);

	/* posix��׼O_APPENDģʽ�£�stdio_fwrite������file->posӰ�죬ʼ�մ�file->size��׷������ */
	if(file->flags & O_APPEND)
	{
		file->pos = file->size;
	}

	tmp_len		= i = 0;
	start_pos	= file->pos;

	while (n_bytes > 0)
	{
		/*
			����ֻ���ġ�����IO�����errno���������û�����ݵ�errno������Ϊ��file->pos >= file->sizeʱ�������
			��ʱ����չ�ļ���С
		*/
		if(make_valid_data(file) == EIO)
		{
			goto err;
		}

		/* 
			make_valid_data(file)�ɹ�
			file->pos >= file->sizeʱ��ִ������Ĵ���	
		*/	
		tmp_len = (BUF_BLOCK_SIZE - BUF_BLOCK_OFFSET(file->pos));
		if (tmp_len > n_bytes)
			tmp_len = n_bytes;

		/* user buffer���ݸ���buffer block�е����� */
		memcpy(file->block->base + BUF_BLOCK_OFFSET(file->pos), buf + i, tmp_len);

		file->block->pre_id = BUF_BLOCK_ID(file->pos);

		n_bytes		-= tmp_len;
		file->pos	+= tmp_len;
		i			+= tmp_len;

		file->block->flags |= BUF_BLOCK_DIRTY_FLAG;
	}
err:
	put_buffer_block(file);

	UNLOCK_FILE(file);
	
	return file->pos - start_pos;
}

/**
	@brief ��λ��

	@return �ɹ�����0��ʧ���򷵻�-1
*/
static int stdio_fseek(void *ptr, loff_t offset, int whence)
{	
	long tmp_pos;
	int ret = 0;
	struct stdio_file *file	= (struct stdio_file *)ptr;

	LOCK_FILE(file);
	
	if (file->block)
	{
		update_dirty_buffer_block(file);
	}

	switch (whence)
	{
	case SEEK_SET:
		tmp_pos = offset;
		break;

	case SEEK_CUR:
		tmp_pos = file->pos + offset;
		break;

	case SEEK_END:
		tmp_pos = file->size + offset;
		break;

	default:
		ret = -1;
	}

	if(tmp_pos < 0) 
	{
		ret = -1;
		goto err;	
	}

	file->pos = tmp_pos;

err:
	UNLOCK_FILE(file);
	return ret;
}

/**
	@brief �ر���

	@return �ɹ�����0��ʧ���򷵻�EOF
*/
static ssize_t stdio_fclose(void *ptr)
{
	int ret = 0;
	struct stdio_file *file	= (struct stdio_file *)ptr;
	if (!file) return -1;

	LOCK_FILE(file);
	if (file->block)
	{
		swap_out_buffer_block(file->block);
		file->block = NULL;
	}
	sys_close(file->fd);

	UNLOCK_FILE(file);

	free(file);

	return ret;
}


static const struct file_operations stdio_functions = {
	.read		= stdio_fread,
	.write		= stdio_fwrite,
	.seek		= stdio_fseek,
	.close		= stdio_fclose
};

void stream_file_init_ops(struct stdio_file *file)
{
	file->ops = &stdio_functions;
}

bool stream_file_buffer_init()
{
	int	i, j, index, buf_block_size;
	void *pool;
	struct buffer_block *block;
	
	buf_block_size = sizeof(struct buffer_block);
	pool = malloc(BUF_SIZE);
	if (!pool)
		goto err;
	//TODO: init_buffer will use the Virtual Alloc to create buffer pool
	
	for (i = 0, j = 0; i < (BUF_BLOCK_NUM * buf_block_size); i += buf_block_size, j += BUF_BLOCK_SIZE)
	{
		index = i / buf_block_size;
		
		buffer_pool[index].access_count	= 0;
		buffer_pool[index].flags		= 0;
		buffer_pool[index].base			= pool + j;
	}

	return true;
err:
	return false;
}