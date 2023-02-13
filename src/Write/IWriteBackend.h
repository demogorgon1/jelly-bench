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

	protected:
		
		const Config*		m_config;
	};

}