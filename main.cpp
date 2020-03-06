
#include "WAIpc.h"
#include <string.h>
#include <unistd.h>




void Sleep1Sec()
{
	usleep(1000 * 1000);
}

//WAIpcPOSIX::CWASemaphore sem;

//void* Func(void* Args)
//{
//	int Tid = *(int*)Args;
//	for (int i = 0; i < 3; i++) 
//	{
//		cout << Tid << " Lock." << endl;
//		sem.SemWait();
//		Sleep1Sec();
//		sem.SemPost();
//		cout << Tid << " Unlock." << endl;
//	}
//
//	return nullptr;
//}

int main()
{

	{
		//struct STest
		//{
		//	int i32;
		//	char name[20];
		//} ttt{ 123, "Allen" };

		//{
		//	WAIpc::CWASharedMemory shm;
		//	shm.CreateSharedMemory(sizeof(STest));

		//	STest* p = (STest*)shm.AttachSharedMemory();
		//	memcpy(p, &ttt, sizeof(STest));
		//	shm.DetachSharedMemory(p);
		//	//cout << p->i32 << " " << p->name << endl;
		//}
	}

	{
		//{
		//	WAIpc::CWAMessageQueue mq;

		//	/*mq.CreateMessageQueue();
		//	char send[] = "warde";
		//	mq.SendMessage(1, 6, send);*/

		//	mq.GetMessageQueue();
		//	char recv[6] = { 0 };
		//	int size = mq.RecvMessage(1, 6, recv);
		//	cout << size << " " << recv << endl;
		//}
	}

	{
		//int chld, i, j;

		//WAIpcSystemV::CWASemaphoreArray sa;
		//if (sa.CreateSemaphoreArray() < 0) {
		//	cout << "create semaphore fail!" << endl;

		//	return 1;
		//}

		//while ((chld = fork()) == -1);
		//if (chld > 0)
		//{/*****父进程块*****/
		//	i = 1;
		//	while (i <= 3)
		//	{
		//		sleep(1);

		//		sa.SemaphoreWait();
		//		printf("parent in\n");

		//		sleep(1);
		//		printf("parent out\n");
		//		sa.SemaphoreRelease();

		//		i++;
		//	}
		//	
		//}
		//else
		//{/*****子进程块*****/
		//	j = 1;
		//	while (j <= 3)
		//	{
		//		sleep(1);

		//		sa.SemaphoreWait();
		//		printf("child in\n");

		//		sleep(1);
		//		printf("child out\n");
		//		sa.SemaphoreRelease();

		//		j++;
		//	}
		//	exit(0);
		//}
	}

	{
		//char* p = nullptr;

		//WAIpcPOSIX::CWAMmap mm;

		//{

		//	int chld = 0;
		//	if ((chld = fork()) == -1) return 1;

		//	if (chld > 0)
		//	{
		//		/*****父进程块*****/

		//		
		//		int fd = mm.CreateSharedMemory("/wa_posix_shm", 13);

		//		p = (char*)mm.CreateMmapFd(fd, 13);

		//		if (p == MAP_FAILED) cout << "mmap() fail" << endl;
		//		memcpy(p, "Hello world!", 13);

		//		cout << "parent " << p << endl;
		//	}
		//	else
		//	{
		//		/*****子进程块*****/

		//		usleep(1000 * 1000 * 2);

		//		int fd = mm.GetSharedMemory("/wa_posix_shm");

		//		p = (char*)mm.CreateMmapFd(fd, 13);

		//		cout << "child " << p << endl;
		//	}

		//}

	}

	/*{
		sem.CreateUnnamedSem();

		static const int TidArray[] = { 1, 2, 3 };
		pthread_t pid[3] = { 0 };

		for (int i = 0; i < 3; i++) {
			pthread_create(pid + i, NULL, Func, (void*)(TidArray + i));
		}

		for (int i = 0; i < 3; i++) {
			pthread_join(pid[i], NULL);
		}
	}*/

	{

		//int ret = 0;

		//ret = fork();

		//if (ret == -1) return 1;

		//if (ret > 0) {
		//	/* parent */

		//	WAIpcPOSIX::CWASemaphore sem;
		//	sem.CreateNamedSem("/wa_posix_sem", 1);

		//	Sleep1Sec();

		//	for (int i = 0; i < 3; i++) {
		//		cout << "Parent Lock." << endl;
		//		sem.SemWait();
		//		cout << "Parent Get it." << endl;
		//		Sleep1Sec();
		//		sem.SemPost();
		//		cout << "Parent Unlock." << endl;
		//	}

		//}
		//else
		//{
		//	/* child */

		//	Sleep1Sec();

		//	WAIpcPOSIX::CWASemaphore sem;
		//	sem.GetNamedSem("/wa_posix_sem", 1);

		//	for (int i = 0; i < 3; i++) {
		//		cout << "Child Lock." << endl;
		//		sem.SemWait();
		//		cout << "Child Get it." << endl;
		//		Sleep1Sec();
		//		sem.SemPost();
		//		cout << "Child Unlock." << endl;
		//	}
		//}

	}

	getchar();
	return 0;
}