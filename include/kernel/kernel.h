/**
	@defgroup mutex
	@ingroup kernel_api
 
	�ں˱�̽ӿ� Mutex
 
 @{
*/
#ifndef KE_H
#define KE_H

#define Y_SYNC_MAX_OBJS_COUNT 64
#define Y_SYNC_WAIT_INFINITE (-1UL)

/* Should met the size of kt_sync_base */
#define KE_SYNC_BASE_OBJECT_RESERVE 4
typedef enum
{
	KE_WAIT_OK = 0,
	KE_WAIT_TIMEDOUT,
	KE_WAIT_ABANDONED,
}kt_sync_wait_result;

/*******************************************************/
/* About thread */
/*******************************************************/
/**
	@brief ��ǰ�̵߳ȴ�

	��ǰ�̵߳ȴ�tiemout�Ĺ�ȥ��timeout ��ʱ��δ������ر�ע�⣬�ڲ�ͬ��ϵͳ�ϣ��δ��Ƶ�ʲ�һ��һ����
	����ֻ�еȵδ����˺��̲߳Ż�������У�����֮�����۷���ʲô����̶߳������˳���
	�ýӿ�һ������������ʹ�ã����Ƽ��������ط�ʹ�ã���Ϊ���ᵼ���߳��޷�����ɱ����

	@param[in] timeout Ҫ�ȴ��ĵδ���

	@return
		��0��
*/

signed long ke_sleep_timeout(signed long timeout);

#endif
/** @} */