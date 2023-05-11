// typedefine.h
// 统一所有的内置类型的宏定义，包括linux和windows

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
// WIN32相关
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

// LINUX文件权限
#define S_IRUSR				00400
#define S_IXUSR				00100
#define S_IWUSR				00200
#define S_IRGRP				00040
#define S_IXGRP				00010
#define S_IXOTH				00001
#define S_IROTH				00004

// 创建目录
#define _MKDIR_(path, win_param, linux_param) \
	CreateDirectory(path, win_param)

// 创建信号量
#define _CREATE_SEMAPHORE(a)	\
	(*a = CreateEvent(NULL, FALSE, FALSE, NULL)) != NULL

// 设置信号量
#define _SET_SEMAPHORE(a)	\
	SetEvent(a)

// 等待信号量
#define  _WAIT_SEMAPHORE_ALWAYS(a)	\
	WaitForSingleObject(a, INFINITE) == WAIT_OBJECT_0

// 销毁信号量
#define _DESTORY_SEMAPHORE(a)	\
	CloseHandle(a)

// 创建线程
#define _CREATE_THREAD(thread_handle, stack_size, thread_proc, thread_param)	\
	CreateThread(NULL, stack_size, (LPTHREAD_START_ROUTINE)thread_proc, thread_param, 0, NULL) != NULL

// 线程退出，释放资源，主要为linux定义
#define _THREAD_RELEASE(run_flag, thread_handle, p_thread_result)	\
	run_flag = false;

// 线程休眠
#define _USLEEP_(a)	\
	Sleep(a / 1000)

extern char _ioctl(int, int, uchar *);
#define _IOCTRL_(handler, type, cmd)	\
	_ioctl(handler, type, cmd);

#define _REBOOT_SYSTEM_		\
	ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0);
////////////////////////////////////////////////////////////////////////////
//
// linux相关
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

// linux线程相关头文件
#define _REENTRANT						// 线程可入宏，必须在所有头文件引用前
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

// 创建目录
#define _MKDIR_(path, win_param, linux_param) \
	mkdir(path, linux_param) == 0

// 创建信号量
#define _CREATE_SEMAPHORE(a)	\
	sem_init(a, 0, 0) == 0

// 设置信号量
#define _SET_SEMAPHORE(a)	\
	sem_post(&a)

// 等待信号量
#define  _WAIT_SEMAPHORE_ALWAYS(a)	\
	sem_wait(&a) == 0

// 销毁信号量
#define _DESTORY_SEMAPHORE(a)	\
	sem_destroy(&a)

// 创建线程
#define _CREATE_THREAD(thread_handle, stack_size, thread_proc, thread_param)	\
pthread_create(&thread_handle, NULL, thread_proc, thread_param) != 0

// 线程退出，释放资源，主要为linux定义
#define _THREAD_RELEASE(run_flag, thread_handle, p_thread_result)	\
	pthread_join(thread_handle, p_thread_result);

// 线程休眠
#define _USLEEP_(a)	\
	usleep(a)

// io
#define _IOCTRL_(handler, type, cmd)	\
	ioctl(handler, type, cmd)

// 同步磁盘数据,将缓存数据回写到硬盘,以防数据丢失[luther.gliethttp]
#define _REBOOT_SYSTEM_		\
	sync();		\
	reboot(RB_AUTOBOOT);

#endif
