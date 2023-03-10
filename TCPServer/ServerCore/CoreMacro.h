#pragma once

#include <assert.h>

#define OUT

#define NAMESPACE_BEGIN(name)	namespace name {
#define NAMESPACE_END			}

/*---------------
	  Lock
---------------*/

#define USE_MANY_LOCKS(count)	Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
#define	READ_LOCK_IDX(idx)		ReadLockGuard readLockGuard_##idx(_locks[idx], typeid(this).name());
#define READ_LOCK				READ_LOCK_IDX(0)
#define	WRITE_LOCK_IDX(idx)		WriteLockGuard writeLockGuard_##idx(_locks[idx], typeid(this).name());
#define WRITE_LOCK				WRITE_LOCK_IDX(0)

/*---------------
	  Crash
---------------*/

#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	assert(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expr)			\
{									\
	if (!(expr))					\
	{								\
		CRASH("ASSERT_CRASH");		\
		assert(expr);				\
	}								\
}

#define IS_VALID_SOCKET(socket_num) ((socket_num) != -1)

#define	ERROR_PRINT							\
{											\
		cout << strerror(errno) << endl;	\
}											

#define IF_FALSE_PRINT_AND_RETURN_FALSE(expression)		\
{														\
	if ((expression) == false)							\
	{													\
		ERROR_PRINT;									\
		return false;									\
	}													\
}														