#pragma once

#include "SparkRabbit/Renderer/VertexArray.h"

namespace SparkRabbit{
	
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		void Bind() const override;
		void Unbind() const override;

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer) override;
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

		const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

		virtual uint32_t GetRendererID() const override { return m_RendererID; };
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_VertexBufferIndex = 0;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};

}
