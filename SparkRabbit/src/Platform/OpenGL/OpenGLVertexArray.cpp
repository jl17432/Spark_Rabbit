#include "PrecompileH.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace SparkRabbit {

	static GLenum OpenGLShaderDataType(ShaderDataType type)
	{
		switch (type)
		{
		case SparkRabbit::ShaderDataType::Float:    return GL_FLOAT;
		case SparkRabbit::ShaderDataType::Vec2:		return GL_FLOAT;
		case SparkRabbit::ShaderDataType::Vec3:		return GL_FLOAT;
		case SparkRabbit::ShaderDataType::Vec4:		return GL_FLOAT;
		case SparkRabbit::ShaderDataType::Mat3:     return GL_FLOAT;
		case SparkRabbit::ShaderDataType::Mat4:     return GL_FLOAT;
		case SparkRabbit::ShaderDataType::Int:      return GL_INT;
		case SparkRabbit::ShaderDataType::Ivec2:    return GL_INT;
		case SparkRabbit::ShaderDataType::Ivec3:    return GL_INT;
		case SparkRabbit::ShaderDataType::Ivec4:    return GL_INT;
		case SparkRabbit::ShaderDataType::Bool:     return GL_BOOL;
		}

		SPARK_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertBuff)
	{
		SPARK_CORE_ASSERT(vertBuff->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(m_RendererID);
		vertBuff->Bind();
		auto const& layout = vertBuff->GetLayout();
		for (auto const& element : layout)
		{
			glEnableVertexAttribArray(m_VertexBufferIndex); 
			glVertexAttribPointer(m_VertexBufferIndex, 
								element.GetComponentCount(), 
								OpenGLShaderDataType(element.Type), 
								element.Normalized ? GL_TRUE : GL_FALSE, 
								layout.GetStride(), 
								(const void*)element.Offset); 
			m_VertexBufferIndex++; 
		}
		m_VertexBuffers.push_back(vertBuff); 
	}

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuff)
	{
		glBindVertexArray(m_RendererID); 
		indexBuff->Bind(); 
		m_IndexBuffer = indexBuff; 
	}

}


