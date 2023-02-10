#include "Pcheader.h"

#if defined(_WIN32)
	#include <windows.h>
#endif

#include "ProcessTimes.h"

namespace jellybench::ProcessTimes
{

	void	
	Get(
		CPUTime& aOut)
	{
		#if defined(_WIN32)
			FILETIME creationTime;
			FILETIME exitTime;
			static_assert(sizeof(FILETIME) == sizeof(uint64_t));
			BOOL result = GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, (FILETIME*)&aOut.m_kernel, (FILETIME*)&aOut.m_user);
			JELLY_ASSERT(result != 0);
		#endif
	}

}