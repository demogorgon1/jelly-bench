#include "Pcheader.h"

#if defined(_WIN32)
	#include <windows.h>
#else
	#include <sys/times.h> 
	#include <unistd.h>
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
		#else
			// CPU time
			{
				struct tms cpuTimes;
				times(&cpuTimes);

				size_t clkTck = (size_t)sysconf(_SC_CLK_TCK);
				JELLY_ASSERT(clkTck != 0);

				aOut.m_kernel = (1000000 * (size_t)cpuTimes.tms_stime) / clkTck;
				aOut.m_user = (1000000 * (size_t)cpuTimes.tms_utime) / clkTck;
			}

			// I/O
			{
				char path[256];
				snprintf(path, sizeof(path), "/proc/%u/io", getpid());
				FILE* f = fopen(path, "r");
				JELLY_ASSERT(f != NULL);

				uint32_t values[7];

				int result = fscanf(f, 
					/* 0 */ "rchar: %u\n"
					/* 1 */ "wchar: %u\n"
					/* 2 */ "syscr: %u\n"
					/* 3 */ "syscw: %u\n"
					/* 4 */ "read_bytes: %u\n"
					/* 5 */ "write_bytes: %u\n"					
					/* 6 */ "cancelled_write_bytes: %u\n",
					&values[0], &values[1], &values[2], &values[3], &values[4], &values[5], &values[6]);
				JELLY_ASSERT(result == 7);
				fclose(f);

				aOut.m_writeOps = (size_t)values[3];
				aOut.m_writeBytes = (size_t)values[1];
			}
		#endif
	}

}