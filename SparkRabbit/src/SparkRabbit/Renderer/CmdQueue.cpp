#include "PrecompileH.h"
#include "CmdQueue.h"

namespace SparkRabbit {

	CmdQueue::CmdQueue()
	{
		m_CmdBuffer = new uint8_t[10 * 1024 * 1024]; //10mb buffer
		m_CmdBufferPtr = m_CmdBuffer;
		memset(m_CmdBuffer, 0, 10 * 1024 * 1024);

	}

	CmdQueue::~CmdQueue()
	{
		delete[] m_CmdBuffer;
	}

	void* CmdQueue::Allocate(RenderCmdFn func, uint32_t size)
	{
		//future work : memory alignment
		*(RenderCmdFn*)m_CmdBufferPtr = func;
		m_CmdBufferPtr += sizeof(RenderCmdFn);

		*(uint32_t*)m_CmdBufferPtr = size;
		m_CmdBufferPtr += sizeof(uint32_t);

		void* memptr = m_CmdBufferPtr;
		m_CmdBufferPtr += size;

		m_CmdCount++;
		return memptr;
	}

	void CmdQueue::Execute()
	{
		//SPARK_CORE_TRACE("RenderCommandQueue::Execute -- {0} commands, {1} bytes",m_CmdCount, (m_CmdBufferPtr - m_CmdBuffer));
		uint8_t* buffer = m_CmdBuffer;

		for (uint32_t i = 0; i < m_CmdCount; i++)
		{
			RenderCmdFn func = *(RenderCmdFn*)buffer;
			buffer += sizeof(RenderCmdFn);
			uint32_t size = *(uint32_t*)buffer;
			buffer += sizeof(uint32_t);

			//SPARK_CORE_TRACE("Executing command #{0}: {1}", i, typeid(func).name());
			func(buffer);
			buffer += size;
		}
		m_CmdBufferPtr = m_CmdBuffer;
		m_CmdCount = 0;
	}
}