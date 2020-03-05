#pragma once

#include <iostream>

using namespace std;

#include <string.h>
//#include <unistd.h>
#include <fcntl.h>

#include <sys/ipc.h>

/* System V IPC */

/* semaphore array */
#include <sys/sem.h>
/* shared memory */
#include <sys/shm.h>
/* message queue */
#include <sys/msg.h>

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
	 *	�����������ں�,��Ϣ���л�һֱ����,��Ҫ������ʾ�ĵ��ýӿ�ɾ����ʹ������ɾ��
	 *	��Ϣ���п���˫��ͨ��
	 *	�˷��˹ܵ�ֻ�ܳ����޸�ʽ�ֽ�����ȱ��
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
		int CreateSemaphoreArray(int SemNum);
		int GetSemaphoreArray(int SemNum);

	public:
		int SemaphoreWait(int Op = -1);
		int SemaphoreRelease(int Op = 1);

		int SemAryWait(int Index, int Op = -1);
		int SemAryRelease(int Index, int Op = 1);

	private:
		int SemGet(int SemNum, int Flag);

	private:
		key_t m_Key;
		int m_Id;

	};

};

/* POSIX IPC */

/* semaphore */
#include <semaphore.h>	

