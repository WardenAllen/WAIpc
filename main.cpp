
#include "WAIpc.h"
#include <string.h>

int main()
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

	getchar();
	return 0;
}