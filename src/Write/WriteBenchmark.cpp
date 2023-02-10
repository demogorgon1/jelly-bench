#include "../Pcheader.h"

#include "../Config.h"
#include "../ProcessTimes.h"
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

		ProcessTimes::CPUTime startCPUTime;
		ProcessTimes::Get(startCPUTime);

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

		ProcessTimes::CPUTime endCPUTime;
		ProcessTimes::Get(endCPUTime);

		uint32_t totalPassed = (uint32_t)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - startTime).count();

		printf("elapsed: %u\n", totalPassed);
		printf("seq:	 %u\n", seq);
		printf("kernel:	 %llu\n", endCPUTime.m_kernel - startCPUTime.m_kernel);
		printf("user:	 %llu\n", endCPUTime.m_user - startCPUTime.m_user);
	}

}