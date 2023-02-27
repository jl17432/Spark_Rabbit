#pragma once
#include"Buffer.h"

namespace SparkRabbit {
	class OpenglVertexBuffer : public vertexBuffer
	{
	public:

		OpenglVertexBuffer(float* vertices, uint32_t size);
		virtual ~OpenglVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		/*virtual const BufferLayout& getLayout() const override { return m_layout; }
		virtual void setLayout(const BufferLayout& layout) override { m_layout = layout; }*/

	private:
		uint32_t m_renderID;
		//BufferLayout m_layout;
	};

	class OpenglIndexBuffer : public indexBuffer
	{
	public:

		OpenglIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~OpenglIndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		/*virtual const BufferLayout& getLayout() const override { return m_layout; }
		virtual void setLayout(const BufferLayout& layout) override { m_layout = layout; }*/

		virtual uint32_t GetCount() const { return m_count; }

	private:
		uint32_t m_renderID;
		uint32_t m_count;
		//BufferLayout m_layout;
	};
}

