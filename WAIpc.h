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
	 *	�����������ںˣ���Ϣ���л�һֱ���ڣ���Ҫ������ʾ�ĵ��ýӿ�ɾ����ʹ������ɾ��
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

};

