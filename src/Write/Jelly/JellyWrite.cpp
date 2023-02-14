#include "../../Pcheader.h"

#include "../../Config.h"

#include "JellyWrite.h"

namespace jellybench::Write::Jelly
{

	JellyWrite::JellyWrite(
		const Config*		aConfig)
		: IWriteBackend(aConfig)
	{
		m_host = std::make_unique<jelly::DefaultHost>(".", "write", &m_config->m_jellyConfig);
		m_host->DeleteAllFiles();

		m_blobNode = std::make_unique<BlobNodeType>(m_host.get(), 0);
		m_housekeepingAdvisor = std::make_unique<HouseKeepingAdvisorType>(m_host.get(), m_blobNode.get());
		m_processRequestsTimer.SetTimeout(500);
	}

	JellyWrite::~JellyWrite()
	{
		m_blobNode.reset();
	}

	//------------------------------------------------------------------------

	void	
	JellyWrite::Update() 
	{	
		if(m_processRequestsTimer.HasExpired())
		{
			m_blobNode->ProcessRequests();

			_UpdateRequests();
		}

		m_housekeepingAdvisor->Update([&](
			const HouseKeepingAdvisorType::Event& aEvent)
		{
			switch(aEvent.m_type)
			{
			case HouseKeepingAdvisorType::Event::TYPE_FLUSH_PENDING_WAL:
				m_blobNode->FlushPendingWAL(aEvent.m_concurrentWALIndex);
				break;

			case HouseKeepingAdvisorType::Event::TYPE_FLUSH_PENDING_STORE:
				m_blobNode->FlushPendingStore();
				break;

			case HouseKeepingAdvisorType::Event::TYPE_CLEANUP_WALS:
				m_blobNode->CleanupWALs();
				break;

			case HouseKeepingAdvisorType::Event::TYPE_PERFORM_COMPACTION:
				{
					std::unique_ptr<BlobNodeType::CompactionResultType> compactionResult(m_blobNode->PerformCompaction(aEvent.m_compactionJob));
					m_blobNode->ApplyCompactionResult(compactionResult.get());
				}
				break;

			default:
				JELLY_ASSERT(false);
			}
		});
	}

	void	
	JellyWrite::Set(
		uint32_t			aKey,
		uint32_t			aSeq,
		const void*			aBlob,
		size_t				aBlobSize) 
	{
		BlobType blob;
		blob.GetBuffer().SetSize(aBlobSize);
		memcpy(blob.GetBuffer().GetPointer(), aBlob, aBlobSize);

		std::unique_ptr<BlobNodeType::Request> req = std::make_unique<BlobNodeType::Request>();

		req->SetKey(aKey);
		req->SetSeq(aSeq);
		req->SetBlob(blob);

		m_blobNode->Set(req.get());

		m_requests.push_back(std::move(req));
	}

	size_t
	JellyWrite::GetDiskSpaceUsed()
	{
		m_host->PollSystemStats();

		jelly::IStats* stats = m_host->GetStats();
		
		stats->Update();

		return stats->GetGauge(jelly::Stat::ID_TOTAL_HOST_STORE_SIZE).m_value + stats->GetGauge(jelly::Stat::ID_TOTAL_HOST_WAL_SIZE).m_value;
	}

	//------------------------------------------------------------------------

	void	
	JellyWrite::_UpdateRequests()
	{
		for(size_t i = 0; i < m_requests.size(); i++)
		{
			std::unique_ptr<BlobNodeType::Request>& req = m_requests[i];

			if(req->IsCompleted())
			{
				// Cyclic remove
				if(i + 1 < m_requests.size())
					req = std::move(m_requests[m_requests.size() - 1]);

				m_requests.pop_back();	

				i--;
			}
		}
	}

}