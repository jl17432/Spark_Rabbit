#pragma once

namespace SparkRabbit{

	class CmdQueue 
	{
	public:
		typedef void(*RenderCmdFn)(void*);

		CmdQueue();
		~CmdQueue();

		void* Allocate(RenderCmdFn func, uint32_t size);

		void Execute();
	private:
		uint8_t* m_CmdBuffer;
		uint8_t* m_CmdBufferPtr;
		uint32_t m_CmdCount = 0;
	};


}

