#include "../../Pcheader.h"

#include "../../Config.h"

#include "SQLiteWrite.h"

namespace jellybench::Write::SQLite
{

	SQLiteWrite::SQLiteWrite(
		const Config*		aConfig)
		: IWriteBackend(aConfig)
		, m_blobNode(aConfig)
	{
		m_flushTimer.SetTimeout(500);
	}

	SQLiteWrite::~SQLiteWrite()
	{

	}

	//------------------------------------------------------------------------

	void	
	SQLiteWrite::Update()
	{	
		if(m_flushTimer.HasExpired())
			m_blobNode.Flush();
	}

	void	
	SQLiteWrite::Set(
		uint32_t			aKey,
		uint32_t			aSeq,
		const void*			aBlob,
		size_t				aBlobSize) 
	{
		m_blobNode.Set(aKey, aSeq, aBlob, aBlobSize);
	}

	size_t	
	SQLiteWrite::GetDiskSpaceUsed() 
	{
		return m_blobNode.GetDiskSpaceUsed();
	}

	//------------------------------------------------------------------------

}