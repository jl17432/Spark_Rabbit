#include "PrecompileH.h"
#include "Renderer2D.h"
#include "Renderer.h"
#include "VertexArray.h"
#include "Shader.h"

#include <glm/gtc/matrix_transform.hpp>

namespace SparkRabbit {
	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};

	struct Renderer2DData
	{
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

		static const uint32_t MaxLines = 10000;
		static const uint32_t MaxLineVertices = MaxLines * 2;
		static const uint32_t MaxLineIndices = MaxLines * 6;

		std::shared_ptr<VertexArray> QuadVertexArray;
		std::shared_ptr<VertexBuffer> QuadVertexBuffer;
		std::shared_ptr<Shader> TextureShader;
		std::shared_ptr<Texture2D> WhiteTexture;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;

		std::array<std::shared_ptr<Texture2D>, MaxTextureSlots> TextureSlots;
		uint32_t TextureSlotIndex = 1; // 0 = white texture

		glm::vec4 QuadVertexPositions[4];

		// Lines
		std::shared_ptr<VertexArray> LineVertexArray;
		std::shared_ptr<VertexBuffer> LineVertexBuffer;
		std::shared_ptr<Shader> LineShader;

		uint32_t LineIndexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		LineVertex* LineVertexBufferPtr = nullptr;
		glm::mat4 CameraViewProj;
		bool DepthTest = true;

		Renderer2D::Statistics Stats;
	};

	static Renderer2DData r2_Data;

	void Renderer2D::Init()
	{
		r2_Data.QuadVertexArray = VertexArray::Create();

		r2_Data.QuadVertexBuffer = VertexBuffer::Create(r2_Data.MaxVertices * sizeof(QuadVertex));
		r2_Data.QuadVertexBuffer->SetLayout({
			{ ShaderDataType::Vec3, "a_Position" },
			{ ShaderDataType::Vec4, "a_Color" },
			{ ShaderDataType::Vec2, "a_TexCoord" },
			{ ShaderDataType::Float, "a_TexIndex" },
			{ ShaderDataType::Float, "a_TilingFactor" }
			});
		r2_Data.QuadVertexArray->AddVertexBuffer(r2_Data.QuadVertexBuffer);

		r2_Data.QuadVertexBufferBase = new QuadVertex[r2_Data.MaxVertices];

		uint32_t* quadIndices = new uint32_t[r2_Data.MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < r2_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		std::shared_ptr<IndexBuffer> quadIB = IndexBuffer::Create(quadIndices, r2_Data.MaxIndices);
		r2_Data.QuadVertexArray->SetIndexBuffer(quadIB);
		delete[] quadIndices;

		r2_Data.WhiteTexture = Texture2D::Create(TextureFormat::RGBA, 1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		r2_Data.WhiteTexture->Lock();
		r2_Data.WhiteTexture->GetWritableBuffer().Write(&whiteTextureData, sizeof(uint32_t));
		r2_Data.WhiteTexture->Unlock();

		r2_Data.TextureShader = Shader::Create("assets/shaders/Renderer2D.glsl");

		// Set all texture slots to 0
		r2_Data.TextureSlots[0] = r2_Data.WhiteTexture;

		r2_Data.QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		r2_Data.QuadVertexPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		r2_Data.QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		r2_Data.QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		// Lines
		r2_Data.LineShader = Shader::Create("assets/shaders/Renderer2D_Line.glsl");
		r2_Data.LineVertexArray = VertexArray::Create();

		r2_Data.LineVertexBuffer = VertexBuffer::Create(r2_Data.MaxLineVertices * sizeof(LineVertex));
		r2_Data.LineVertexBuffer->SetLayout({
			{ ShaderDataType::Vec3, "a_Position" },
			{ ShaderDataType::Vec4, "a_Color" }
		});
		r2_Data.LineVertexArray->AddVertexBuffer(r2_Data.LineVertexBuffer);

		r2_Data.LineVertexBufferBase = new LineVertex[r2_Data.MaxLineVertices];

		uint32_t* lineIndices = new uint32_t[r2_Data.MaxLineIndices];
		for (uint32_t i = 0; i < r2_Data.MaxLineIndices; i++)
			lineIndices[i] = i;

		std::shared_ptr<IndexBuffer> lineIB = IndexBuffer::Create(lineIndices, r2_Data.MaxLineIndices);
		r2_Data.LineVertexArray->SetIndexBuffer(lineIB);
		delete[] lineIndices;
	}

	void Renderer2D::Shutdown()
	{
	}

	void Renderer2D::BeginScene(const glm::mat4& viewProj, bool depthTest)
	{
		r2_Data.CameraViewProj = viewProj;
		r2_Data.DepthTest = depthTest;

		r2_Data.TextureShader->Bind();
		r2_Data.TextureShader->SetMat4("u_ViewProjection", viewProj);

		r2_Data.QuadIndexCount = 0;
		r2_Data.QuadVertexBufferPtr = r2_Data.QuadVertexBufferBase;

		r2_Data.LineIndexCount = 0;
		r2_Data.LineVertexBufferPtr = r2_Data.LineVertexBufferBase;

		r2_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::EndScene()
	{
		uint32_t dataSize = (uint8_t*)r2_Data.QuadVertexBufferPtr - (uint8_t*)r2_Data.QuadVertexBufferBase;
		if (dataSize)
		{
			r2_Data.QuadVertexBuffer->SetData(r2_Data.QuadVertexBufferBase, dataSize);

			r2_Data.TextureShader->Bind();
			r2_Data.TextureShader->SetMat4("u_ViewProjection", r2_Data.CameraViewProj);

			for (uint32_t i = 0; i < r2_Data.TextureSlotIndex; i++)
				r2_Data.TextureSlots[i]->Bind(i);

			r2_Data.QuadVertexArray->Bind();
			Renderer::DrawIndexed(r2_Data.QuadIndexCount, PrimitiveType::Triangles, r2_Data.DepthTest);
			r2_Data.Stats.DrawCalls++;
		}

		dataSize = (uint8_t*)r2_Data.LineVertexBufferPtr - (uint8_t*)r2_Data.LineVertexBufferBase;
		if (dataSize)
		{
			r2_Data.LineVertexBuffer->SetData(r2_Data.LineVertexBufferBase, dataSize);

			r2_Data.LineShader->Bind();
			r2_Data.LineShader->SetMat4("u_ViewProjection", r2_Data.CameraViewProj);

			r2_Data.LineVertexArray->Bind();
			Renderer::SetLineWidth(2.0f);
			Renderer::DrawIndexed(r2_Data.LineIndexCount, PrimitiveType::Lines, r2_Data.DepthTest);
			r2_Data.Stats.DrawCalls++;
		}

	}


	void Renderer2D::FlushAndReset()
	{
		EndScene();

		r2_Data.QuadIndexCount = 0;
		r2_Data.QuadVertexBufferPtr = r2_Data.QuadVertexBufferBase;

		r2_Data.TextureSlotIndex = 1;
	}

	void Renderer2D::FlushAndResetLines()
	{

	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		if (r2_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[0];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[1];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[2];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[3];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadIndexCount += 6;

		r2_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const std::shared_ptr<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		if (r2_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < r2_Data.TextureSlotIndex; i++)
		{
			if (*r2_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)r2_Data.TextureSlotIndex;
			r2_Data.TextureSlots[r2_Data.TextureSlotIndex] = texture;
			r2_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[0];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[1];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[2];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[3];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadIndexCount += 6;

		r2_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, color);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color)
	{
		if (r2_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		const float textureIndex = 0.0f; // White Texture
		const float tilingFactor = 1.0f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[0];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[1];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[2];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[3];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadIndexCount += 6;

		r2_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const std::shared_ptr<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		DrawRotatedQuad({ position.x, position.y, 0.0f }, size, rotation, texture, tilingFactor, tintColor);
	}

	void Renderer2D::DrawRotatedQuad(const glm::vec3& position, const glm::vec2& size, float rotation, const std::shared_ptr<Texture2D>& texture, float tilingFactor, const glm::vec4& tintColor)
	{
		if (r2_Data.QuadIndexCount >= Renderer2DData::MaxIndices)
			FlushAndReset();

		constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < r2_Data.TextureSlotIndex; i++)
		{
			if (*r2_Data.TextureSlots[i].get() == *texture.get())
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)r2_Data.TextureSlotIndex;
			r2_Data.TextureSlots[r2_Data.TextureSlotIndex] = texture;
			r2_Data.TextureSlotIndex++;
		}

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[0];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 0.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[1];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 0.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[2];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 1.0f, 1.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadVertexBufferPtr->Position = transform * r2_Data.QuadVertexPositions[3];
		r2_Data.QuadVertexBufferPtr->Color = color;
		r2_Data.QuadVertexBufferPtr->TexCoord = { 0.0f, 1.0f };
		r2_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
		r2_Data.QuadVertexBufferPtr->TilingFactor = tilingFactor;
		r2_Data.QuadVertexBufferPtr++;

		r2_Data.QuadIndexCount += 6;

		r2_Data.Stats.QuadCount++;
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color)
	{
		if (r2_Data.LineIndexCount >= Renderer2DData::MaxLineIndices)
			FlushAndResetLines();

		r2_Data.LineVertexBufferPtr->Position = p0;
		r2_Data.LineVertexBufferPtr->Color = color;
		r2_Data.LineVertexBufferPtr++;

		r2_Data.LineVertexBufferPtr->Position = p1;
		r2_Data.LineVertexBufferPtr->Color = color;
		r2_Data.LineVertexBufferPtr++;

		r2_Data.LineIndexCount += 2;

		r2_Data.Stats.LineCount++;
	}

	void Renderer2D::ResetStats()
	{
		memset(&r2_Data.Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return r2_Data.Stats;
	}

}
