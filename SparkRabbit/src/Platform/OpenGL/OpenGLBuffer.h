#pragma once

#include "SparkRabbit/Renderer/Buffer.h"
#include "SparkRabbit/Buffer.h"

namespace SparkRabbit {
	//////////////////////////////////////////////////////////////////////////////////
	// VertexBuffer
	//////////////////////////////////////////////////////////////////////////////////
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(void* data, uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Static);
		OpenGLVertexBuffer(uint32_t size, VertexBufferUsage usage = VertexBufferUsage::Dynamic);
		virtual ~OpenGLVertexBuffer();

		void SetData(void* data, uint32_t size, uint32_t offset = 0) override;
		void Bind() const override;

		const BufferLayout& GetLayout() const override { return m_Layout; }
		void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

		uint32_t GetSize() const override { return m_Size; }
		uint32_t GetRendererID() const override { return m_RendererID; }
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Size;
		VertexBufferUsage m_Usage;
		BufferLayout m_Layout;

		Buffer m_LocalData;
	};

	//////////////////////////////////////////////////////////////////////////////////
	// IndexBuffer
	//////////////////////////////////////////////////////////////////////////////////

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(void* data, uint32_t size);
		virtual ~OpenGLIndexBuffer();

		void SetData(void* data, uint32_t size, uint32_t offset = 0) override;
		void Bind() const override;

		uint32_t GetCount() const override { return m_Size / sizeof(uint32_t); }

		uint32_t GetSize() const override { return m_Size; }
		uint32_t GetRendererID() const override  { return m_RendererID; }
	private:
		uint32_t m_RendererID = 0;
		uint32_t m_Size;

		Buffer m_LocalData;
	};

}