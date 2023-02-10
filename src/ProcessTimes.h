#pragma once

namespace jellybench::ProcessTimes
{

	struct CPUTime
	{
		uint64_t	m_user = 0;
		uint64_t	m_kernel = 0;
	};

	void	Get(
				CPUTime&		aOut);

}