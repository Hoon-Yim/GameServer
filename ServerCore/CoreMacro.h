#pragma once

/*-------------
	 CRASH
-------------*/

#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

#define ASSERT_CRASH(expression)			\
{											\
	if (!(expression))						\
	{										\
		CRASH("ASSERT_CRASH");				\
		__analysis_assume(expression);		\
	}										\
}