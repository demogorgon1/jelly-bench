#include "../Pcheader.h"

#include "../Config.h"
#include "../ProcessMonitor.h"
#include "../RandomBlobCache.h"

#include "IWriteBackend.h"

namespace jellybench::Write
{

	void	
	WriteBenchmark(
		const Config*	aConfig,
		IWriteBackend*	aBackend)
	{
		std::mt19937 random;
		RandomBlobCache randomBlobCache(aConfig, random);

		std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();

		double pending = 0.0;
		std::chrono::time_point<std::chrono::steady_clock> prevTime = startTime;
		uint32_t seq = 0;

		std::chrono::time_point<std::chrono::steady_clock> endTime = startTime + std::chrono::seconds(aConfig->m_writeRunSeconds);

		ProcessMonitor::Stats startStats;
		ProcessMonitor::GetStats(startStats);

		for(;;)
		{
			std::chrono::time_point<std::chrono::steady_clock> currentTime = std::chrono::steady_clock::now();
			if(currentTime >= endTime)
				break;

			uint32_t passed = (uint32_t)std::chrono::duration_cast<std::chrono::microseconds>(currentTime - prevTime).count();
			prevTime = currentTime;
			pending += ((double)passed / 1000000.0) * (double)aConfig->m_writeRate;

			while (pending > 0.0)
			{
				const RandomBlobCache::Blob& randomBlob = randomBlobCache.GetRandomBlob(random);
				aBackend->Set((size_t)random() % aConfig->m_blobKeyCount, seq++, &randomBlob[0], randomBlob.size());

				pending -= 1.0;
			}

			aBackend->Update();

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		ProcessMonitor::Stats endStats;
		ProcessMonitor::GetStats(endStats);

		uint32_t totalPassed = (uint32_t)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count();
		uint32_t kernel = (uint32_t)(endStats.m_kernel - startStats.m_kernel);
		uint32_t user = (uint32_t)(endStats.m_user - startStats.m_user);
		uint32_t writeOps = (uint32_t)(endStats.m_writeOps - startStats.m_writeOps);
		uint32_t writeBytes = (uint32_t)(endStats.m_writeBytes - startStats.m_writeBytes);

		printf("elapsed:     %u\n", totalPassed);
		printf("seq:	     %u\n", seq);
		printf("kernel:	     %u\n", kernel);
		printf("user:	     %u\n", user);
		printf("total:       %u\n", kernel + user);
		printf("avgcpu:      %f\n", (100.0f * (float)(kernel + user)) / (float)totalPassed);
		printf("wrops:       %u\n", writeOps);
		printf("wrbytes:     %u\n", writeBytes);
		printf("wrops/sec:   %f\n", (float)writeOps / ((float)totalPassed / 1000000.0f));
		printf("wrbytes/sec: %f\n", (float)writeBytes / ((float)totalPassed / 1000000.0f));
	}

}