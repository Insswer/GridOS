#include "../../../programs/dl/main.c"

#include "cl_fname.h"

/******************************************************
 For CRT
 ******************************************************/
static void lock_image_list()
{
	//TODO
}

static void unlock_image_list()
{
	//TODO
}

static bool check_image_precicely(struct image *image, const char *fname)
{
	//TODO
	return true;
}

static void relink_dep(struct dl_structure *old_dl, struct image *old_image, struct image *new_image)
{
	int i;
	struct dependency_list *node, *new_node;

	INIT_LIST_HEAD(&new_image->dep_list);
	list_for_each_entry(node, &old_image->dep_list, list)
	{
		i = node - old_dl->static_dependency_pool;
		new_node = dl.static_dependency_pool + i;
		
		list_add_tail(&new_node->list, &new_image->dep_list);
	}
}

DLLEXPORT void *crt_ld_unload_bootstrap(void *old_cmdline)
{
	_unmap_exe_file(old_cmdline);
	
	/* Got new cmdline */
	return dl.cmdline_buffer;
} 

void dl_handle_over(void *old_dl)
{
	int i;
	struct dl_structure *old = old_dl;
	struct image *image, *new_image;
	
	dl = *old;
	INIT_LIST_HEAD(&dl.image_list_head);
	dl.normal_mode = 1;
	
	list_for_each_entry(image, &old->image_list_head, linear_list)
	{
		i = image - old->static_image_pool;
		new_image = dl.static_image_pool + i;
									
		list_add_tail(&new_image->linear_list, &dl.image_list_head);
		relink_dep(old, image, new_image);
		
		/* ��ִ���ļ�����������cmdline buffer */
		if (new_image->name >= old->cmdline_buffer && new_image->name < old->cmdline_buffer + sizeof(old->cmdline_buffer))
			new_image->name = dl.cmdline_buffer + (int)(new_image->name - old->cmdline_buffer);
		
	}
}

void *dl_open(const char *name, int mode)
{
	struct image *image, *got = NULL;
	const char *pure_src_name, *pure_dst_name;
	
	pure_src_name = cl_locate_pure_file_name(name);
	
	/*
		�ҵ����ƥ������֣�Ȼ��ȥ�ļ�ϵͳ��֤�����ǲ��ǶԵġ�
		�Դ˽�����ֱ�̬·�����⣬ֻ���ļ�ϵͳ��ȷ��·�����⡣
	*/
	lock_image_list();
	list_for_each_entry(image, &dl.image_list_head, linear_list)
	{
		/* һЩ��̬�Ķ�̬���л�����"../../abc.so"���������� */
		pure_dst_name = cl_locate_pure_file_name(image->name);
		if (!strcmp(pure_dst_name, pure_src_name))
		{
			/* ok�����������ֻ�ܹ��˵�����ز�����ȷƥ�佻���ļ�ϵͳ */
			if (check_image_precicely(image, name) == true)
			{
				got = image;
				got->ref++;
				break;
			}
		}
	}

	if (got)
		goto end;;

	got = load_image((xstring)name);
	if (!got)
		goto end;
	
	got->mode = mode;
	relocate(got);
	
end:
	unlock_image_list();
	return got;
}

bool dl_close(void *handle)
{
	struct image *image = handle;

	lock_image_list();
	unload_image(image);
	unlock_image_list();
	
	return true;
}

void *dl_sym(void *handle, const char *name)
{
	struct image *image = handle;
	unsigned long addr;
	
	if (ki_get_symbol(&image->exe_desc, name, &addr, 0) == true)
		return (void *)addr;
	
	return NULL;
}