
#include "WAIpc.h"
#include <string.h>

int main()
{

	struct STest
	{
		int i32;
		char name[20];
	} ttt{ 123, "Allen" };

	{
		WAIpc::CWASharedMemory shm;
		shm.CreateSharedMemory(sizeof(STest));

		STest* p = (STest*)shm.AttachSharedMemory();
		memcpy(p, &ttt, sizeof(STest));
		shm.DetachSharedMemory(p);
		//cout << p->i32 << " " << p->name << endl;
	}

	getchar();
	return 0;
}