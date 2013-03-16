/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   ͨ�ö��������
*/
#ifndef COMMON_OBJECT_H
#define COMMON_OBJECT_H

#include <list.h>
#include <types.h>
#include <kernel/ke_atomic.h>

#include "blkbuf.h"

struct cl_object
{
	char			*name;					/* ������ */
	void			*type;					/* ָ��type */
	struct list_head	list;					/* ͬһ��������� */
	struct ke_atomic	ref;					/* �������ü����� */
};

struct cl_object_ops
{
	/* ���̹ر�һ�����󣬾�����ϵͳӦ�ö���������Ӧ */
	bool (*close)(struct cl_object *obj);

	/* �´����Ķ��󱻳�ʼ�� */
	void (*init)(struct cl_object *object);

	/* ϣ���õ������ͬ���������ڹ���һЩ�¼�������ʲô�� */
	void *(*get_sync_object)(struct cl_object *obj);	
};

enum cl_object_memory_type
{
	COMMON_OBJECT_MEMORY_TYPE_NAME,
	COMMON_OBJECT_MEMORY_TYPE_OBJ,
	COMMON_OBJECT_MEMORY_TYPE_NODE,
};

struct cl_object_type
{
	const char	*name;
	size_t		size;
	bool (*add_space)(struct cl_object_type *type, void **base, size_t *size, enum cl_object_memory_type);
	void (*free_space)(struct cl_object_type *type, void *base, size_t size, enum cl_object_memory_type);

	/* ����������� */
	struct cl_object_ops *ops;

	/* HIDE to user */
	struct cl_bkb allocator;
};

/* ���� */
/**
	@brief ����һ��ָ�����͵Ķ���	
*/
void *cl_object_create(struct cl_object_type *type);

/**
	@brief �رնԶ����ʹ��
*/
void cl_object_close(void *object);

/**
	@brief Register type
*/
void cl_object_type_register(struct cl_object_type *type);

#endif
