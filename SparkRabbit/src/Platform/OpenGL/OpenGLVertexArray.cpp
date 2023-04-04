#include "PrecompileH.h"
#include "OpenGLVertexArray.h"
#include "SparkRabbit/Renderer/Renderer.h"

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
		Renderer::Submit([this]() {
			glCreateVertexArrays(1, &m_RendererID);
		});
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{ 
		Renderer::Submit([this]() { 
			glDeleteVertexArrays(1, &m_RendererID); 
		}); 
	}

	void OpenGLVertexArray::Bind() const
	{
		Renderer::Submit([this]() {
			glBindVertexArray(m_RendererID);
		});
	}

	void OpenGLVertexArray::Unbind() const
	{
		Renderer::Submit([this]() {
			glBindVertexArray(0);
		});
	}

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertBuff)
	{
		SPARK_CORE_ASSERT(vertBuff->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		Bind();
		vertBuff->Bind();
		Renderer::Submit([this, vertBuff]() {
			const auto& layout = vertBuff->GetLayout();
			for (const auto& element : layout)
			{
				auto glBaseType = OpenGLShaderDataType(element.Type);
				glEnableVertexAttribArray(m_VertexBufferIndex);
				if (glBaseType == GL_INT)
				{
					glVertexAttribIPointer(m_VertexBufferIndex,
						element.GetComponentCount(),
						glBaseType,
						layout.GetStride(),
						(const void*)(intptr_t)element.Offset);
				}
				else
				{
					glVertexAttribPointer(m_VertexBufferIndex,
						element.GetComponentCount(),
						glBaseType,
						element.Normalized ? GL_TRUE : GL_FALSE,
						layout.GetStride(),
						(const void*)(intptr_t)element.Offset);
				}
				m_VertexBufferIndex++; 
			}
			});
		m_VertexBuffers.push_back(vertBuff);
	}

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuff)
	{
		Bind();
		indexBuff->Bind(); 
		m_IndexBuffer = indexBuff; 
	}

}


