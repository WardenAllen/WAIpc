#pragma once

#include <iostream>

using namespace std;

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

};

