#pragma once

#include <iostream>

using namespace std;

#include <string.h>

#include <sys/ipc.h>

/* unnamed semaphore */
#include <semaphore.h>	

/* named semaphore */
#include <sys/sem.h>

/* shared memory */
#include <sys/shm.h>

/* message queue */
#include <sys/msg.h>

//#include <unistd.h>
#include <fcntl.h>

namespace WAIpc {



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
	 *	生命周期随内核，消息队列会一直存在，需要我们显示的调用接口删除或使用命令删除
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

};

