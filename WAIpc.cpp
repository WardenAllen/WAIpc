#include "WAIpc.h"

static constexpr int WA_SHARED_MEMORY_KEY = 0x10000;
static constexpr int WA_MESSAGE_QUEUE_KEY = 0x20000;
static constexpr int WA_SEMAPHORE_ARRAY_KEY = 0x40000;

WAIpcSystemV::CWASharedMemory::CWASharedMemory() :
	m_Key(-1), m_Id(-1)
{
}

WAIpcSystemV::CWASharedMemory::~CWASharedMemory()
{
	if (m_Key > 0) shmctl(m_Id, IPC_RMID, NULL);
}

int WAIpcSystemV::CWASharedMemory::CreateSharedMemory(int Size)
{
	return ShmGet(Size, IPC_CREAT | IPC_EXCL | 0666);
}

int WAIpcSystemV::CWASharedMemory::GetSharedMemory(int Size)
{
	return ShmGet(Size, IPC_CREAT | 0666);
}

void* WAIpcSystemV::CWASharedMemory::AttachSharedMemory()
{
	/*

	shmat() attaches the System V shared memory segment identified by
		shmid to the address space of the calling process.The attaching
		address is specified by shmaddr with one of the following criteria :

	*If shmaddr is NULL, the system chooses a suitable(unused) page -
		aligned address to attach the segment.

		* If shmaddr isn't NULL and SHM_RND is specified in shmflg, the
		attach occurs at the address equal to shmaddr rounded down to the
		nearest multiple of SHMLBA.

		* Otherwise, shmaddr must be a page - aligned address at which the
		attach occurs.

		In addition to SHM_RND, the following flags may be specified in the
		shmflg bit - mask argument :

	SHM_EXEC(Linux - specific; since Linux 2.6.9)
		Allow the contents of the segment to be executed.The caller
		must have execute permission on the segment.

		SHM_RDONLY
		Attach the segment for read - only access.The process must
		have read permission for the segment.If this flag is not
		specified, the segment is attached for readand write access,
		and the process must have readand write permission for the
		segment.There is no notion of a write - only shared memory
		segment.

		SHM_REMAP(Linux - specific)
		This flag specifies that the mapping of the segment should
		replace any existing mapping in the range starting at shmaddr
		and continuing for the size of the segment.  (Normally, an
			EINVAL error would result if a mapping already exists in this
			address range.)  In this case, shmaddr must not be NULL.
	
	*/

	return shmat(m_Id, NULL, 0);
}

int WAIpcSystemV::CWASharedMemory::DetachSharedMemory(void* ShmAddr)
{
	return shmdt(ShmAddr);
}

int WAIpcSystemV::CWASharedMemory::ShmGet(int Size, int Flag)
{
	m_Key = ftok("/tmp", WA_SHARED_MEMORY_KEY);

	if (m_Key < 0) {
		cout << "ftok() error!" << endl;
		return -1;
	}

	/*
	 *	IPC_CREAT	Create key if key does not exist.
	 *	IPC_EXCL	Fail if key exists.
	 *	0666		4-Read 2-Write 1-Execute, like file.
	 */

	m_Id = shmget(m_Key, Size, Flag);

	if (m_Id < 0) {
		cout << "shmget() error!" << endl;
		return -2;
	}

	return m_Id;
}

WAIpcSystemV::CWAMessageQueue::CWAMessageQueue() :
	m_Key(-1), m_Id(-1)
{
}

WAIpcSystemV::CWAMessageQueue::~CWAMessageQueue()
{
	if (m_Key > 0) msgctl(m_Id, IPC_RMID, NULL);
}

int WAIpcSystemV::CWAMessageQueue::CreateMessageQueue()
{
	return MsgGet(IPC_CREAT | IPC_EXCL | 0666);
}

int WAIpcSystemV::CWAMessageQueue::GetMessageQueue()
{
	return MsgGet(IPC_CREAT);
}

int WAIpcSystemV::CWAMessageQueue::SendMessage(int Type, int Size, char* Buf)
{
	msgbuf mb = { 0 };

	mb.mtype = Type;
	memcpy(mb.mtext, Buf, Size);

	return msgsnd(m_Id, (void*)&mb, Size, 0);
}

int WAIpcSystemV::CWAMessageQueue::RecvMessage(int Type, int Size, char* Buf)
{
	msgbuf mb = { 0 };

	if ((int)msgrcv(m_Id, &mb, Size, Type, IPC_NOWAIT) < 0)
		return -1;

	memcpy(Buf, mb.mtext, Size);

	return 0;
}

int WAIpcSystemV::CWAMessageQueue::MsgGet(int Flag)
{
	m_Key = ftok("/tmp", WA_MESSAGE_QUEUE_KEY);

	if (m_Key < 0) {
		cout << "ftok() error!" << endl;
		return -1;
	}

	m_Id = msgget(m_Key, Flag);

	if (m_Id < 0) {
		cout << "msgget() error!" << endl;
		return -2;
	}

	return m_Id;
}

WAIpcSystemV::CWASemaphoreArray::CWASemaphoreArray() :
	m_Key(-1), m_Id(-1)
{
}

WAIpcSystemV::CWASemaphoreArray::~CWASemaphoreArray()
{
	semctl(m_Id, 0, IPC_RMID);
}

int WAIpcSystemV::CWASemaphoreArray::CreateSemaphoreArray(int SemNum)
{
	/*

	�ں�Ϊÿ���ź�������������һ��semid_ds�ṹ
	struct semid_ds {
		struct ipc_permsem_perm;
		structsem* sem_base;	//�ź�����ָ��
		ushort sem_nsem;		//�˼����źŸ���
		time_t sem_otime;		//���һ��semopʱ��
		time_t sem_ctime;		//���һ�δ���ʱ��
	};

	ÿ���ź�����һ�������ṹ��ʾ,�����ٰ������г�Ա��
		struct {
		ushort_t semval;		//�ź�����ֵ
		short sempid;			//���һ������semop�Ľ���ID
		ushort semncnt;			//�ȴ����ź���ֵ���ڵ�ǰֵ�Ľ�����(һ�н����ͷ���Դ �ͱ�����)
		ushort semzcnt;			//�ȴ����ź���ֵ����0�Ľ�����
	};

	int semctl (int semid, int semnum, int cmd, ... );
	���ĸ������ǿ�ѡ��,ȡ���ڵ���������cmd��
		����semnumָ���źż��е��ĸ��ź�(��������)
		����cmdָ������10�������е�һ��, ��semidָ�����ź���������ִ�д����
		IPC_STAT	��ȡһ���ź����������ݽṹsemid_ds,������洢��semun�е�buf�����С�
		IPC_SET     �����ź����������ݽṹsemid_ds�е�Ԫ��ipc_perm,��ֵȡ��semun�е�buf������
		IPC_RMID	���ź��������ڴ���ɾ����
		GETALL      ���ڶ�ȡ�ź������е������ź�����ֵ��
		GETNCNT		�������ڵȴ���Դ�Ľ�����Ŀ��
		GETPID      �������һ��ִ��semop�����Ľ��̵�PID��
		GETVAL      �����ź������е�һ���������ź�����ֵ��
		GETZCNT		�������ڵȴ���ȫ���е���Դ�Ľ�����Ŀ��
		SETALL		�����ź������е����е��ź�����ֵ��
		SETVAL      �����ź������е�һ���������ź�����ֵ��

	*/

	return SemGet(SemNum, IPC_CREAT | IPC_EXCL | 0666);
}

int WAIpcSystemV::CWASemaphoreArray::GetSemaphoreArray(int SemNum)
{
	return SemGet(SemNum, 0);
}

int WAIpcSystemV::CWASemaphoreArray::SemaphoreWait(int Op)
{
	/*

	struct sembuf
	{
		unsigned short sem_num;		semaphore index in array
		short sem_op;               semaphore operation
		short sem_flg;              operation flags
	};
	
	sem_num���������ź������ĵڼ���Ԫ��,��0��ʼ

	sem_opָ���ź�����ȡ�Ĳ���
		<0�൱��P����,ռ����Դ
		>0�൱��V����,�ͷ���Դ
		=0����˯��ֱ���ź�����ֵΪ0

	sem_flgָ��������ִ��ģʽ,������־λ��
		һ����IPC_NOWAIT,ָ���Է�������ʽ�����ź�����
		һ����SEM_UNDO,ָ���ں�Ϊ�ź������������ָ�ֵ��
		ͨ��ΪSEM_UNDO,ʹ����ϵͳ�����ź�
		SEM_UNDO���ڽ��޸ĵ��ź���ֵ�ڽ��������˳�(����exit�˳���mainִ����)
		���쳣�˳�(����쳣, ��0�쳣, �յ�KILL�źŵ�)ʱ�黹���ź�����
����		���ź�����ʼֵ��20,������SEM_UNDO��ʽ�����ź�����2,��5,��1;
		�ڽ���δ�˳�ʱ,�ź������20-2-5+1=14;�ڽ����˳�ʱ,���޸ĵ�ֵ�黹���ź���,�ź������14+2+5-1=20��

	����nops�涨opsptr������Ԫ�ظ�����

	sem_opֵ��
		(1)��sem_opΪ��,���Ӧ�ڽ����ͷ�ռ�õ���Դ����sem_opֵ�ӵ��ź�����ֵ�ϡ�(V����)
		(2)��sem_opΪ��, ���ʾҪ��ȡ���ź������Ƶ���Դ�����ź���ֵ��ȥsem_op�ľ���ֵ��(P����)
		(3)��sem_opΪ0, ���ʾ���ý���ϣ���ȴ������ź���ֵ���0

	����ź���ֵС��sem_op�ľ���ֵ(��Դ��������Ҫ��),��
		(1)��ָ����IPC_NOWAIT,��semop()������EAGAIN��
		(2)��δָ��IPC_NOWAIT,���ź�����semncntֵ��1(��Ϊ���ý��̽��� ������״̬),Ȼ����ý��̱�����ֱ����
			�ٴ��ź�����ɴ��ڻ����sem_op�ľ���ֵ;
			�ڴ�ϵͳ��ɾ���˴��ź���,����EIDRM;
			�۽��̲�׽��һ���ź�,�����źŴ�����򷵻�,����EINTR��(����Ϣ���е���������ʽ ������)
	
	*/

	sembuf semb = {0, Op, SEM_UNDO};

	/*
	 *	int semop(int sem_id, struct sembuf* sops, size_t nsops);
	 *		
	 *		semid	�ź������ı�ʶ��
	 *		sops	�����ṹ��������, ÿ�� sembuf �ṹ���Ӧһ���ض��źŵĲ���
	 *		nsops	���в������źŵĸ���
	 */

	return semop(m_Id, &semb, 1);
}

int WAIpcSystemV::CWASemaphoreArray::SemaphoreRelease(int Op)
{
	sembuf semb = { 0, Op, SEM_UNDO };
	return semop(m_Id, &semb, 1);
}

int WAIpcSystemV::CWASemaphoreArray::SemAryWait(int Index, int Op)
{
	sembuf semb = { Index, Op, SEM_UNDO };
	return semop(m_Id, &semb, 1);
}

int WAIpcSystemV::CWASemaphoreArray::SemAryRelease(int Index, int Op)
{
	sembuf semb = { Index, Op, SEM_UNDO };
	return semop(m_Id, &semb, 1);
}

int WAIpcSystemV::CWASemaphoreArray::SemGet(int SemNum, int Flag)
{
	m_Key = ftok("/tmp", WA_SEMAPHORE_ARRAY_KEY);

	if (m_Key < 0) {
		cout << "ftok() error!" << endl;
		return -1;
	}

	m_Id = semget(m_Key, SemNum, Flag);

	if (m_Id < 0) {
		cout << "semget() error!" << endl;
		return -2;
	}

	return m_Id;
}
