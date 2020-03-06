
#include "WAIpc.h"
#include <string.h>
#include <unistd.h>

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

	getchar();
	return 0;
}