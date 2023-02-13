#pragma once

namespace jellybench::ProcessMonitor
{

	struct Stats
	{
		uint64_t	m_user = 0;
		uint64_t	m_kernel = 0;
		uint64_t	m_writeOps = 0;
		uint64_t	m_writeBytes = 0;
	};

	void	GetStats(
				Stats&		aOut);
	size_t	GetCurrentMemoryUsage();

}