#include "Pcheader.h"

#if defined(_WIN32)
	#include <windows.h>
	#include <psapi.h>	
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

				long long values[7];

				int result = fscanf(f, 
					/* 0 */ "rchar: %lld\n"
					/* 1 */ "wchar: %lld\n"
					/* 2 */ "syscr: %lld\n"
					/* 3 */ "syscw: %lld\n"
					/* 4 */ "read_bytes: %lld\n"
					/* 5 */ "write_bytes: %lld\n"					
					/* 6 */ "cancelled_write_bytes: %lld\n",
					&values[0], &values[1], &values[2], &values[3], &values[4], &values[5], &values[6]);
				JELLY_ASSERT(result == 7);
				fclose(f);

				aOut.m_writeOps = (size_t)values[3];
				aOut.m_writeBytes = (size_t)values[1];
			}
		#endif
	}

	size_t
	GetCurrentMemoryUsage()
	{
		#if defined(_WIN32)			
			HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
			JELLY_CHECK(h != INVALID_HANDLE_VALUE, "OpenProcess() failed: %u", GetLastError());

			PROCESS_MEMORY_COUNTERS_EX t;
			BOOL result = GetProcessMemoryInfo(h, (PPROCESS_MEMORY_COUNTERS)&t, (DWORD)sizeof(t));
			JELLY_CHECK(result != 0, "GetProcessMemoryInfo() failed: %u", GetLastError());

			CloseHandle(h);

			return (size_t)t.PrivateUsage;
		#else
			const char* path = "/proc/self/statm";
			FILE* fp = fopen(path, "r");
			JELLY_CHECK(fp != NULL, "fopen() failed: %u (path: %s)", errno, path);

			uint32_t rss;
			int result = fscanf(fp, "%*s%u", &rss);
			JELLY_CHECK(result == 1, "fscanf() failed: %u (path: %s)", errno, path);

			fclose(fp);

			return (size_t)rss * (size_t)sysconf(_SC_PAGESIZE);
		#endif
	}

}