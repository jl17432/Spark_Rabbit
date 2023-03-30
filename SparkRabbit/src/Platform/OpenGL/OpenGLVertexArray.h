#pragma once

#include "SparkRabbit/Renderer/VertexArray.h"

namespace SparkRabbit{
	
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray() = default;

		//OpenGLVertexArray(const OpenGLVertexArray&) = delete;
		//OpenGLVertexArray& operator=(const OpenGLVertexArray&) = delete;

		//OpenGLVertexArray(OpenGLVertexArray&&) noexcept;
		//OpenGLVertexArray& operator=(OpenGLVertexArray&&) noexcept;

		void Bind() const override;
		void Unbind() const override;

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertBuff) override;
		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuff) override;
		const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }
	private:
		uint32_t m_RendererID;
		uint32_t m_VertexBufferIndex = 0;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};

}
