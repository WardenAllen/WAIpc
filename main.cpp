
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
		//{/*****�����̿�*****/
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
		//{/*****�ӽ��̿�*****/
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



	getchar();
	return 0;
}