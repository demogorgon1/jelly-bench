#include "Pcheader.h"

#if defined(_WIN32)
	#include <windows.h>
#endif

#include "ProcessMonitor.h"

namespace jellybench::ProcessMonitor
{

	void	
	GetStats(
		Stats& aOut)
	{
		#if defined(_WIN32)
			// CPU time
			{
				FILETIME creationTime;
				FILETIME exitTime;
				static_assert(sizeof(FILETIME) == sizeof(uint64_t));
				BOOL result = GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, (FILETIME*)&aOut.m_kernel, (FILETIME*)&aOut.m_user);
				JELLY_ASSERT(result != 0);

				aOut.m_kernel /= 10;
				aOut.m_user /= 10;
			}

			// I/O
			{
				IO_COUNTERS c;
				BOOL result = GetProcessIoCounters(GetCurrentProcess(), &c);
				JELLY_ASSERT(result != 0);

				aOut.m_writeOps = (size_t)c.WriteOperationCount;
				aOut.m_writeBytes = (size_t)c.WriteTransferCount;
			}
		#endif
	}

}