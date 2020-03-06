#pragma once

#include <iostream>

using namespace std;

#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ipc.h>

/* System V IPC */

/* shared memory */
#include <sys/shm.h>
/* message queue */
#include <sys/msg.h>
/* semaphore array */
#include <sys/sem.h>

/*
 *  +-----------------+---------------+---------------+---------------+
 *	|				  | Message Queue |	  Semaphore   | Shared Memory |
 *	+=================+===============+===============+===============+
 *	| Headers		  | <sys/msg.h>   | <sys/sem.h>	  | <sys/shm.h>	  |
 *	+-----------------+---------------+---------------+---------------+
 *  | Create / Open   | msgget()	  | semget()	  | shmget()	  |
 *	+-----------------+---------------+---------------+---------------+
 *  | Control	      | msgctl()	  | semctl()	  | shmctl()	  |
 *	+-----------------+---------------+---------------+---------------+
 *  | Operation		  | msgsnd()	  | semop()		  | shmat()		  |
 *  |				  |				  |				  |				  |
 *  |                 | msgrsv()	  |				  | shmdt()	      |
 *	+-----------------+---------------+---------------+---------------+
 *
 *							     SystemV IPC
 */

namespace WAIpcSystemV {

	class CWASharedMemory {

	public:
		CWASharedMemory();
		~CWASharedMemory();

	public:
		int CreateSharedMemory(int Size);
		int GetSharedMemory(int Size);

	public:
		void* AttachSharedMemory();
		int DetachSharedMemory(void* ShmAddr);

	private:
		int ShmGet(int Size, int Flag);

	private:
		key_t m_Key;
		int m_Id;

	};

	/*
	 *	生命周期随内核,消息队列会一直存在,需要我们显示的调用接口删除或使用命令删除
	 *	消息队列可以双向通信
	 *	克服了管道只能承载无格式字节流的缺点
	 */

	class CWAMessageQueue {

	public:
		CWAMessageQueue();
		~CWAMessageQueue();

	public:
		int CreateMessageQueue();
		int GetMessageQueue();

	public:
		int SendMessage(int Type, int Size, char* Buf);
		int RecvMessage(int Type, int Size, char* Buf);

	private:
		int MsgGet(int Flag);

	private:
		key_t m_Key;
		int m_Id;

	};

	class CWASemaphoreArray {

	public:
		CWASemaphoreArray();
		~CWASemaphoreArray();

	public:
		int CreateSemaphoreArray(int SemNum = 1);
		int GetSemaphoreArray(int SemNum = 1);

	public:
		int SemaphoreWait(int Op = -1);
		int SemaphoreRelease(int Op = 1);

		int SemAryWait(int Index, int Op = -1);
		int SemAryRelease(int Index, int Op = 1);

	private:
		int SemGet(int SemNum, int Flag);
		int SemInit();

	private:
		key_t m_Key;
		int m_Id;
		int m_SemNum;

	};

};

/* POSIX IPC */

/*
 *  +-----------------------+---------------+---------------+---------------+
 *	|						| Message Queue |   Semaphore   | Shared Memory |
 *	+=======================+===============+===============+===============+
 *	| Headers				| <mqueue.h>    | <semaphore.h> | <sys/mman.h>  |
 *	+-----------------------+---------------+---------------+---------------+
 *  | Create / Open / Close | mq_open()	    | sem_open()	| shm_open()	|
 *	|						| 				|				|				|
 *	|						| mq_close()    | sem_close()	| shm_unlink()	|
 *	|						| 				|				|				|
 *	|						| mq_unlink()	| sem_unlink()	|				|
 *	|						|				|---------------|				|
 *	|						|				| sem_init()	|				|
 *	|						|				|				|				|
 *	|						|				| sem_destroy()	|				|
 *	+-----------------------+---------------+---------------+---------------+
 *  | Control				| mq_getattr()	| 				| ftruncate()	|
 *	|						|				|				|				|
 *	|						| mq_setattr()	|				| fstat()		|
 *	+-----------------------+---------------+---------------+---------------+
 *  | Operation				| mq_send()		| sem_wait()	| mmap()		|
 *  |						|				|				|				|
 *  |						| mq_receive()	| sem_trywait()	| munmap()		|
 *  |						|				|				|				|
 *  |						| mq_notify()	| sem_post()	|				|
 *  |						|				|				|				|
 *  |						|				| sem_getvalue()|				|
 *  +-----------------------+---------------+---------------+---------------+
 *
 *							       POSIX IPC
 */

/* mmap */
#include <sys/mman.h>
/* semaphore */
#include <semaphore.h>	
/* message queue */
#include <mqueue.h>

namespace WAIpcPOSIX {

	/* mmap() can be used for shared memory or mapping files. */

	class CWAMmap {

	public:
		CWAMmap();
		~CWAMmap();

	public:

		/* name must begin with '/' */
		int CreateSharedMemory(const char* Name, int Size);
		int GetSharedMemory(const char* Name);

		void* CreateMmapFd(int Fd, int Len, 
			int Prot = PROT_READ | PROT_WRITE, 
			int Flags = MAP_SHARED, 
			int Offset = 0, void* Addr = nullptr);

		/* address is valid when mmap() is called before fork(). */

		void* CreateMmapDevZero(int Len, 
			int Prot = PROT_READ | PROT_WRITE,
			int Flags = MAP_SHARED, 
			int Offset = 0, void* Addr = nullptr);

		void* CreateMmapNULL(int Len, 
			int Prot = PROT_READ | PROT_WRITE,
			int Flags = MAP_SHARED, 
			int Offset = 0, void* Addr = nullptr);

	private:
		int ShmOpen(const char* Name, int Flag);

	private:
		int m_Fd;
		int m_Len;
		void* m_Addr;
		string m_Name;

	};

	class CWAMQueue {

	public:
		CWAMQueue();
		~CWAMQueue();


	};

	class CWASemaphore {

	public:
		CWASemaphore(bool Destroy = false);
		~CWASemaphore();

	public:
		/* used for single process multi thread or child process. */
		/* also called memory semaphore. */
		int CreateUnnamedSem(int Shared = 0, int Value = 1);

		/* used for ipc. value less than SEM_VALUE_MAX(32767). */
		/* name must begin with '/' */
		int CreateNamedSem(const char* Name, int Value, 
			int Flag = O_CREAT | O_EXCL, int Mode = 0666);

		int GetNamedSem(const char* Name, int Value,
			int Flag = O_CREAT, int Mode = 0666);

	public:

		inline int DestroySem() { return sem_destroy(m_Sem); }
		inline int UnlinkSem() { return sem_unlink(m_Name.c_str()); }

		inline int SemWait() /* lock */ {return sem_wait(m_Sem); }
		inline int SemPost() /* unlock */ { return sem_post(m_Sem); }

	private:
		sem_t* SemOpen(const char* Name, int Flag, int Mode, int Value);

	private:
		sem_t* m_Sem;
		string m_Name;
		bool m_Destroy;

	};

}



