#include "WAIpc.h"

static constexpr int WA_SHARED_MEMORY_KEY = 0x10000;
static constexpr int WA_MESSAGE_QUEUE_KEY = 0x20000;

WAIpc::CWASharedMemory::CWASharedMemory() :
	m_Key(-1), m_Id(-1)
{
}

WAIpc::CWASharedMemory::~CWASharedMemory()
{
	if (m_Key > 0) shmctl(m_Id, IPC_RMID, NULL);
}

int WAIpc::CWASharedMemory::CreateSharedMemory(int Size)
{
	return ShmGet(Size, IPC_CREAT | IPC_EXCL | 0666);
}

int WAIpc::CWASharedMemory::GetSharedMemory(int Size)
{
	return ShmGet(Size, IPC_CREAT | 0666);
}

void* WAIpc::CWASharedMemory::AttachSharedMemory()
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

int WAIpc::CWASharedMemory::DetachSharedMemory(void* ShmAddr)
{
	return shmdt(ShmAddr);
}

int WAIpc::CWASharedMemory::ShmGet(int Size, int Flag)
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

WAIpc::CWAMessageQueue::CWAMessageQueue() :
	m_Key(-1), m_Id(-1)
{
}

WAIpc::CWAMessageQueue::~CWAMessageQueue()
{
	if (m_Key > 0) msgctl(m_Id, IPC_RMID, NULL);
}

int WAIpc::CWAMessageQueue::CreateMessageQueue()
{
	return MsgGet(IPC_CREAT | IPC_EXCL | 0666);
}

int WAIpc::CWAMessageQueue::GetMessageQueue()
{
	return MsgGet(IPC_CREAT);
}

int WAIpc::CWAMessageQueue::SendMessage(int Type, int Size, char* Buf)
{
	msgbuf mb = { 0 };

	mb.mtype = Type;
	memcpy(mb.mtext, Buf, Size);

	return msgsnd(m_Id, (void*)&mb, Size, 0);
}

int WAIpc::CWAMessageQueue::RecvMessage(int Type, int Size, char* Buf)
{
	msgbuf mb = { 0 };

	if ((int)msgrcv(m_Id, &mb, Size, Type, IPC_NOWAIT) < 0)
		return -1;

	memcpy(Buf, mb.mtext, Size);

	return 0;
}

int WAIpc::CWAMessageQueue::MsgGet(int Flag)
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
