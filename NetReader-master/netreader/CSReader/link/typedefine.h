// typedefine.h
// ͳһ���е��������͵ĺ궨�壬����linux��windows

#pragma once

typedef unsigned long ulong;
typedef unsigned int uint;

typedef unsigned short ushort;

typedef unsigned char uchar;

#include <string.h>
#include <fcntl.h>              
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <string>
#include <map>
#include <vector>
using namespace std;

//////////////////////////////////////////////////////////////////////////
//
// WIN32���
//
#ifdef _WIN32
#pragma warning (disable : 4996)
#include <Windows.h>
#include <time.h>

typedef BYTE uchar;
typedef WORD ushort;
//typedef DWORD uint;

#include <io.h>
#define _ACCESS_			_access

// LINUX�ļ�Ȩ��
#define S_IRUSR				00400
#define S_IXUSR				00100
#define S_IWUSR				00200
#define S_IRGRP				00040
#define S_IXGRP				00010
#define S_IXOTH				00001
#define S_IROTH				00004

// ����Ŀ¼
#define _MKDIR_(path, win_param, linux_param) \
	CreateDirectory(path, win_param)

// �����ź���
#define _CREATE_SEMAPHORE(a)	\
	(*a = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL

// �����ź���
#define _SET_SEMAPHORE(a)	\
	SetEvent(a)

// �ȴ��ź���
#define  _WAIT_SEMAPHORE_ALWAYS(a)	\
	WaitForSingleObject(a, INFINITE) == WAIT_OBJECT_0

// �����ź���
#define _DESTORY_SEMAPHORE(a)	\
	CloseHandle(a)

// �����߳�
#define _CREATE_THREAD(thread_handle, stack_size, thread_proc, thread_param)	\
	CreateThread(NULL, stack_size, (LPTHREAD_START_ROUTINE)thread_proc, thread_param, 0, NULL) != NULL

// �߳��˳����ͷ���Դ����ҪΪlinux����
#define _THREAD_RELEASE(run_flag, thread_handle, p_thread_result)	\
	run_flag = false;

// �߳�����
#define _USLEEP_(a)	\
	Sleep(a / 1000)

extern char _ioctl(int, int, uchar *);
#define _IOCTRL_(handler, type, cmd)	\
	_ioctl(handler, type, cmd);

#define _REBOOT_SYSTEM_		\
	ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
////////////////////////////////////////////////////////////////////////////
//
// linux���
//
#else
#include <stdint.h>
#include <dirent.h>
#include <unistd.h>
#include <linux/ioctl.h>
#include <sys/ioctl.h>
#include <sys/io.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/select.h>
#include <sys/reboot.h>

#include <assert.h>
#include <signal.h>
#include <termios.h>
#include <errno.h>

// linux�߳����ͷ�ļ�
#define _REENTRANT						// �߳̿���꣬����������ͷ�ļ�����ǰ
#include <pthread.h>
#include <semaphore.h>

#define _ACCESS_			access

typedef uint8_t uchar;
typedef uint16_t ushort;
typedef uint32_t uint;

typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned char UINT8;

// ����Ŀ¼
#define _MKDIR_(path, win_param, linux_param) \
	mkdir(path, linux_param) == 0

// �����ź���
#define _CREATE_SEMAPHORE(a)	\
	sem_init(a, 0, 0) == 0

// �����ź���
#define _SET_SEMAPHORE(a)	\
	sem_post(&a)

// �ȴ��ź���
#define  _WAIT_SEMAPHORE_ALWAYS(a)	\
	sem_wait(&a) == 0

// �����ź���
#define _DESTORY_SEMAPHORE(a)	\
	sem_destroy(&a)

// �����߳�
#define _CREATE_THREAD(thread_handle, stack_size, thread_proc, thread_param)	\
pthread_create(&thread_handle, NULL, thread_proc, thread_param) != 0

// �߳��˳����ͷ���Դ����ҪΪlinux����
#define _THREAD_RELEASE(run_flag, thread_handle, p_thread_result)	\
	pthread_join(thread_handle, p_thread_result);

// �߳�����
#define _USLEEP_(a)	\
	usleep(a)

// io
#define _IOCTRL_(handler, type, cmd)	\
	ioctl(handler, type, cmd)

// ͬ����������,���������ݻ�д��Ӳ��,�Է����ݶ�ʧ[luther.gliethttp]
#define _REBOOT_SYSTEM_		\
	sync();		\
	reboot(RB_AUTOBOOT);

#endif
