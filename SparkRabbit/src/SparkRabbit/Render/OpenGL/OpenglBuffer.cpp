#include "PreCompile.h"
#include "OpenglBuffer.h"
#include<glad/glad.h>

namespace SparkRabbit {
	OpenglVertexBuffer::OpenglVertexBuffer(float* vertices, uint32_t size)
	{
		glCreateBuffers(1, &m_renderID);
		glBindBuffer(GL_ARRAY_BUFFER, m_renderID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	OpenglVertexBuffer::~OpenglVertexBuffer()
	{
		glDeleteBuffers(1, &m_renderID);

	}


	void OpenglVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_renderID);
	}

	void OpenglVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	OpenglIndexBuffer::OpenglIndexBuffer(uint32_t* indices, uint32_t count) : m_count(count)
	{
		glCreateBuffers(1, &m_renderID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	OpenglIndexBuffer::~OpenglIndexBuffer()
	{
		glDeleteBuffers(1, &m_renderID);
	}

	void OpenglIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderID);
	}

	void OpenglIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
