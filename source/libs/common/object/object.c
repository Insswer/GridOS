/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   对象管理器
*/
#include <string.h>

#include "object.h"

static void object_free_handler(void * para, void *base, size_t size)
{
	struct cl_object_type * type = para;
	type->free_space(type, base, size, COMMON_OBJECT_MEMORY_TYPE_OBJ);
}

/**
	@brief 创立一个指定类型的对象	
*/
void *cl_object_create(struct cl_object_type *type)
{
	struct cl_object *object;
	bool expaned_memory = false;

again:
	object = (struct cl_object *)cl_bkb_alloc(&type->allocator);
	if (object == NULL)
	{
		void *base;
		size_t size;
		if (expaned_memory == false)
		{
			expaned_memory = true;
			if (type->add_space(type, &base, &size, COMMON_OBJECT_MEMORY_TYPE_OBJ) == true)
			{
				/* Add the space to allocator */
				cl_bkb_extend(&type->allocator, base, size, object_free_handler, type);
				goto again;
			}			
		}
		object = NULL;
		goto end;
	}
	memset(object, 0, type->size);
	object->type = type;
	type->ops->init(object);

end:
	return object;	
}

/**
	@brief Register type
*/
void cl_object_type_register(struct cl_object_type *type)
{
	/* Init allocator */
	memset(&type->allocator, 0, sizeof(type->allocator));
	type->allocator.name =(xstring)type->name;
	type->allocator.node_size = type->size;
}