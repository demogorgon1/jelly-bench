#pragma once

namespace jellybench
{
	struct Config;
}

namespace jellybench::Write
{
	
	class IWriteBackend
	{
	public:
		IWriteBackend(
			const Config*						aConfig)
			: m_config(aConfig)
			, m_logicalWriteBytes(0)
		{

		}

		virtual			
		~IWriteBackend() 
		{
		}

		// Virtual interface
		virtual void	Update() = 0;
		virtual void	Set(
							uint32_t			aKey,
							uint32_t			aSeq,
							const void*			aBlob,
							size_t				aBlobSize) = 0;
		virtual size_t	GetDiskSpaceUsed() = 0;

		// Data access
		size_t			GetLogicalWriteBytes() const { return m_logicalWriteBytes; }

	protected:
		
		const Config*		m_config;
		size_t				m_logicalWriteBytes;
	};

}