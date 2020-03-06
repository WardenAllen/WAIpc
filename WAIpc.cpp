#include "WAIpc.h"

/****************************************************************/
/************************* System V *****************************/
/****************************************************************/

static constexpr int WA_SYSTEM_V_SHARED_MEMORY_KEY = 0x10000;
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
	m_Key = ftok("/tmp", WA_SYSTEM_V_SHARED_MEMORY_KEY);

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

	内核为每个信号量集合设置了一个semid_ds结构
	struct semid_ds {
		struct ipc_permsem_perm;
		structsem* sem_base;	//信号数组指针
		ushort sem_nsem;		//此集中信号个数
		time_t sem_otime;		//最后一次semop时间
		time_t sem_ctime;		//最后一次创建时间
	};

	每个信号量由一个无名结构表示,它至少包含下列成员:
		struct {
		ushort_t semval;		//信号量的值
		short sempid;			//最后一个调用semop的进程ID
		ushort semncnt;			//等待该信号量值大于当前值的进程数(一有进程释放资源 就被唤醒)
		ushort semzcnt;			//等待该信号量值等于0的进程数
	};

	int semctl (int semid, int semnum, int cmd, ... );
	第四个参数是可选的,取决于第三个参数cmd.
		参数semnum指定信号集中的哪个信号(操作对象)
		参数cmd指定以下10种命令中的一种, 在semid指定的信号量集合上执行此命令.
		IPC_STAT	读取一个信号量集的数据结构semid_ds,并将其存储在semun中的buf参数中.
		IPC_SET     设置信号量集的数据结构semid_ds中的元素ipc_perm,其值取自semun中的buf参数.
		IPC_RMID	将信号量集从内存中删除.
		GETALL      用于读取信号量集中的所有信号量的值.
		GETNCNT		返回正在等待资源的进程数目.
		GETPID      返回最后一个执行semop操作的进程的PID.
		GETVAL      返回信号量集中的一个单个的信号量的值.
		GETZCNT		返回这在等待完全空闲的资源的进程数目.
		SETALL		设置信号量集中的所有的信号量的值.
		SETVAL      设置信号量集中的一个单独的信号量的值.

	*/

	m_SemNum = SemNum;

	if (SemGet(SemNum, IPC_CREAT | IPC_EXCL | 0666) < 0) return -1;

	return SemInit();
}

int WAIpcSystemV::CWASemaphoreArray::GetSemaphoreArray(int SemNum)
{
	if (SemGet(SemNum, 0) < 0) return -1;

	return SemInit();
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
	
	sem_num标明它是信号量集的第几个元素,从0开始

	sem_op指定信号量采取的操作
		<0相当于P操作,占有资源
		>0相当于V操作,释放资源
		=0进程睡眠直到信号量的值为0

	sem_flg指明操作的执行模式,两个标志位.
		一个是IPC_NOWAIT,指明以非阻塞方式操作信号量.
		一个是SEM_UNDO,指明内核为信号量操作保留恢复值.
		通常为SEM_UNDO,使操作系统跟踪信号
		SEM_UNDO用于将修改的信号量值在进程正常退出(调用exit退出或main执行完)
		或异常退出(如段异常, 除0异常, 收到KILL信号等)时归还给信号量.
　　		如信号量初始值是20,进程以SEM_UNDO方式操作信号量减2,减5,加1;
		在进程未退出时,信号量变成20-2-5+1=14;在进程退出时,将修改的值归还给信号量,信号量变成14+2+5-1=20.

	参数nops规定opsptr数组中元素个数.

	sem_op值:
		(1)若sem_op为正,这对应于进程释放占用的资源数.sem_op值加到信号量的值上.(V操作)
		(2)若sem_op为负, 这表示要获取该信号量控制的资源数.信号量值减去sem_op的绝对值.(P操作)
		(3)若sem_op为0, 这表示调用进程希望等待到该信号量值变成0

	如果信号量值小于sem_op的绝对值(资源不能满足要求),则:
		(1)若指定了IPC_NOWAIT,则semop()出错返回EAGAIN.
		(2)若未指定IPC_NOWAIT,则信号量的semncnt值加1(因为调用进程将进 入休眠状态),然后调用进程被挂起直至:
			①此信号量变成大于或等于sem_op的绝对值;
			②从系统中删除了此信号量,返回EIDRM;
			③进程捕捉到一个信号,并从信号处理程序返回,返回EINTR.(与消息队列的阻塞处理方式 很相似)
	
	*/

	sembuf semb = {0, (short)Op, (short int)SEM_UNDO};

	/*
	 *	int semop(int sem_id, struct sembuf* sops, size_t nsops);
	 *		
	 *		semid	信号量集的标识符
	 *		sops	操作结构体数组中, 每个 sembuf 结构体对应一个特定信号的操作
	 *		nsops	进行操作的信号的个数
	 */

	return semop(m_Id, &semb, 1);
}

int WAIpcSystemV::CWASemaphoreArray::SemaphoreRelease(int Op)
{
	sembuf semb = { 0, (short)Op, (short)SEM_UNDO };
	return semop(m_Id, &semb, 1);
}

int WAIpcSystemV::CWASemaphoreArray::SemAryWait(int Index, int Op)
{
	sembuf semb = { (unsigned short)Index, (short)Op, (short int)SEM_UNDO };
	return semop(m_Id, &semb, 1);
}

int WAIpcSystemV::CWASemaphoreArray::SemAryRelease(int Index, int Op)
{
	sembuf semb = { (unsigned short)Index, (short)Op, (short int)SEM_UNDO };
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

int WAIpcSystemV::CWASemaphoreArray::SemInit()
{

	/* must init before use. */

	union semun
	{
		int val;
		struct semid_ds* buf;
		unsigned short* arry;
	};

	semun su;
	su.val = 1;

	for (int i = 0; i < m_SemNum; i++)
		if (semctl(m_Id, 0, SETVAL, su) == -1) return -1;

	return 0;
}

/****************************************************************/
/*************************** POSIX ******************************/
/****************************************************************/


WAIpcPOSIX::CWAMmap::CWAMmap() :
	m_Fd(-1), m_Len(0), m_Addr(nullptr), m_Name("")
{
}

WAIpcPOSIX::CWAMmap::~CWAMmap()
{
	if (m_Fd > 0) ::close(m_Fd);

	if (m_Addr != nullptr) munmap(m_Addr, m_Len);

	if (m_Name != "") shm_unlink(m_Name.c_str());
}

int WAIpcPOSIX::CWAMmap::CreateSharedMemory(const char* Name, int Size)
{
	m_Fd = ShmOpen(Name, O_CREAT | O_RDWR | O_EXCL);
	m_Name = Name;

	if (m_Fd > 0) ftruncate(m_Fd, Size);

	return m_Fd;
}

int WAIpcPOSIX::CWAMmap::GetSharedMemory(const char* Name)
{
	m_Fd = ShmOpen(Name, O_RDWR);
	m_Name = Name;
	return m_Fd;
}

void* WAIpcPOSIX::CWAMmap::CreateMmapFd(int Fd, int Len, int Prot, int Flags, int Offset, void* Addr)
{

	/*

	void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);

	该函数主要用途有三个:
		(1) 将普通文件映射到内存中,通常在需要对文件进行频繁读写时使用,用内存读写取代I / O读写,以获得较高的性能;
		(2) 将特殊文件进行匿名内存映射,为关联进程提供共享内存空间;
		(3) 为无关联的进程间的Posix共享内存（SystemV的共享内存操作是shmget / shmat）
	
	1. addr 指向欲映射的内存起始地址,通常设为 NULL,代表让系统自动选定地址,映射成功后返回该地址.

	2. length 代表将文件中多大的部分映射到内存.

	3. prot: 映射区域的保护方式.可以为以下几种方式的组合:
		PROT_EXEC	映射区域可被执行
		PROT_READ	映射区域可被读取
		PROT_WRITE	映射区域可被写入
		PROT_NONE	映射区域不能存取

	4. flags: 影响映射区域的各种特性.在调用mmap()时必须要指定MAP_SHARED 或MAP_PRIVATE.
		MAP_FIXED		如果参数start所指的地址无法成功建立映射时,则放弃映射,不对地址做修正.通常不鼓励用此.
		MAP_SHARED		对映射区域的写入数据会复制回文件内,而且允许其他映射该文件的进程共享.
		MAP_PRIVATE		对映射区域的写入操作会产生一个映射文件的复制,即私人的“写入时复制”（copy on write）对此区域作的任何修改都不会写回原来的文件内容.
		MAP_ANONYMOUS	建立匿名映射.此时会忽略参数fd,不涉及文件,而且映射区域无法和其他进程共享.
		MAP_DENYWRITE	只允许对映射区域的写入操作,其他对文件直接写入的操作将会被拒绝.
		MAP_LOCKED		将映射区域锁定住,这表示该区域不会被置换（swap）.

	5. fd: 要映射到内存中的文件描述符.如果使用匿名内存映射时,即flags中设置了MAP_ANONYMOUS,fd设为 - 1.
	
	6. offset: 文件映射的偏移量,通常设置为0,代表从文件最前方开始对应,offset必须是分页大小的整数倍.

	*/

	m_Fd = Fd;
	m_Len = Len;
	m_Addr = mmap(Addr, Len, Prot, Flags, Fd, Offset);

	return m_Addr;
}

void* WAIpcPOSIX::CWAMmap::CreateMmapDevZero(int Len, int Prot, int Flags, int Offset, void* Addr)
{
	m_Fd = open("/dev/zero", O_RDWR);
	if (m_Fd < 0) return nullptr;

	m_Len = Len;
	m_Addr = mmap(Addr, Len, Prot, Flags, m_Fd, Offset);

	return m_Addr;
}

void* WAIpcPOSIX::CWAMmap::CreateMmapNULL(int Len, int Prot, int Flags, int Offset, void* Addr)
{
	m_Len = Len;
	m_Addr = mmap(Addr, Len, Prot, Flags, -1, Offset);

	return m_Addr;
}

int WAIpcPOSIX::CWAMmap::ShmOpen(const char* Name, int Flag)
{
	return shm_open(Name, Flag, 0666);
}

WAIpcPOSIX::CWASemaphore::CWASemaphore(bool Destroy) :
	m_Sem(nullptr), m_Name(""), m_Destroy(Destroy)
{
}

WAIpcPOSIX::CWASemaphore::~CWASemaphore()
{
	if (m_Name.length() == 0) 
	{
		DestroySem();
		delete m_Sem;
		return;
	}

	if (!m_Destroy) return;
	
	UnlinkSem();
}

int WAIpcPOSIX::CWASemaphore::CreateUnnamedSem(int Shared, int Value)
{

	/*

	(1) 初始化指定的信号量
	int sem_init(sem_t * sem, int pshared, unsigned value);

	返回值: 成功-0; 错误-错误号.
	参数说明:
		sem:	信号量变量名
		value:	信号量的初始值
		pshared	信号量的共享范围
				如果pshared为0, 那么该信号量只能由初始化这个信号量的进程中的线程使用;
				如果pshared非零, 任何可以访问到这个信号量的进程都可以使用这个信号量.

	必须保证只调用sem_init()进行初始化一次, 对于一个已初始化过的信号量
	调用sem_init()的行为是未定义的

	(2) 销毁一个指定的信号量
	int sem_destroy(sem_t * sem);
		
	返回值: 成功-0; 错误-错误号.
	参数说明:sem:信号量变量名（sem_init内值）
	摧毁一个有线程阻塞在其上的信号量的行为是未定义的.

	(3) P操作:
	int sem_trywait(sem_t * sem);
	int sem_wait(sem_t * sem);
	int sem_timedwait(sem_t * sem, const struct timespec time);

	减1信号量的值. 信号量值为0时, sem_wait会阻塞, 直到成功使信号量减1或者被信号中断时才返回;
		sem_trywait为非阻塞版本,当信号量为0时,该函数返回 - 1并且将errno置为EAGAIN
		sem_timedwait带有超时功能,超时到期并且信号量计数没能减1,
		sem_timedwait将返回 - 1且将errno设置为ETIMEDOUT.（毕竟一直阻塞也不是办法..）

	(4) V操作:
	int sem_post(sem_t * sem);
	信号量加1,若此时有sem_wait正在阻塞则唤醒执行.

	(5) 取值操作:
	int sem_t getvalue(sem_t sem, int* val);
	获取信号量的值,一般只用来调试,打印val查看.

	*/

	m_Sem = new sem_t;

	if (m_Sem == nullptr) return -1;

	return sem_init(m_Sem, Shared, (unsigned)Value);
}

int WAIpcPOSIX::CWASemaphore::CreateNamedSem(const char* Name, int Value, int Flag, int Mode)
{
	/*

	(1) 创建或打开一个信号量
	sem_t* sem_open(const char* name, int oflag);
	sem_t* sem_open(const char* name, int oflag,
		mode_t mode, unsigned int value);

	返回值:信号量指针,该指针可供其他对该信号量进行操作的函数使用

	参数说明:
		name	信号量的名字,标识信号量;

		oflag	可以为:0, O_CREAT, O_EXCL.
				如果为0表示打开一个已存在的信号量,
				如果为 O_CREAT,表示如果信号量不存在就创建一个信号量,如果存在则打开被返回.
				此时mode和value需要指定.如果为O_CREAT | O_EXCL,表示如果信号量已存在会返回错误.

		mode	用于创建信号量时,表示信号量的权限位,和open函数一样
				包括:S_IRUSR,S_IWUSR,S_IRGRP,S_IWGRP,S_IROTH,S_IWOTH.

	(2) 关闭打开的信号量.当一个进程终止时,内核对其上仍然打开的所有有名信号量自动执行这个操作.
	int sem_close(sem_t * sem);
	返回值:成功返回0,失败返回 - 1
		调用sem_close关闭信号量并没有把它从系统中删除它,POSIX有名信号量是随内核持续的.
		即使当前没有进程打开某个信号量它的值依然保持.
		直到内核重新自举或调用sem_unlink()删除该信号量.

	(3) 将有名信号量立刻从系统中删除,但信号量的销毁是在所有进程都关闭信号量的时候(只close是不够的!)
	int sem_unlink(const char* name);
	返回值:成功返回0,失败返回 - 1

	有名信号量的PV操作与无名信号量类似.
	*/

	m_Name = Name;
	m_Sem = SemOpen(Name, Flag, Mode, Value);

	return m_Sem == nullptr;
}

int WAIpcPOSIX::CWASemaphore::GetNamedSem(const char* Name, int Value, int Flag, int Mode)
{
	m_Name = Name;
	m_Sem = SemOpen(Name, Flag, Mode, Value);

	return m_Sem == nullptr;
}

sem_t* WAIpcPOSIX::CWASemaphore::SemOpen(const char* Name, int Flag, int Mode, int Value)
{
	return sem_open(Name, Flag, Mode, (unsigned)Value);
}