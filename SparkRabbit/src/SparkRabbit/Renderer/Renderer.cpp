#include "PrecompileH.h"
#include "Renderer.h"

#include "Shader.h"
#include <Glad/glad.h>

#include"RenderAPI.h"
#include"SceneRenderer.h"
#include "Renderer2D.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

namespace SparkRabbit {
	RenderAPI::API RenderAPI::s_API = RenderAPI::API::OpenGL;  // for now

	struct RendererData
	{
		std::shared_ptr<RenderPass> m_ActiveRenderPass;
		CmdQueue m_CmdQueue;
		std::unique_ptr<ShaderLibrary> m_ShaderLibrary;
		std::shared_ptr<VertexArray> m_FullscreenQuadVertexArray;
	};

	static RendererData s_Data;

	void Renderer::Init()
	{
		s_Data.m_ShaderLibrary = std::make_unique<ShaderLibrary>();
		Renderer::Submit([]() { RenderAPI::Init(); });

		Renderer::GetShaderLibrary()->Load("assets/shaders/PBR_Static.glsl");//
		Renderer::GetShaderLibrary()->Load("assets/shaders/PBR_Anim.glsl");//

		SceneRenderer::Init();
		// Create fullscreen quad
		float x = -1;
		float y = -1;
		float width = 2, height = 2;
		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoord;
		};
		//Render our scene to this quad
		QuadVertex* data = new QuadVertex[4];

		data[0].Position = glm::vec3(x, y, 0.1f);
		data[0].TexCoord = glm::vec2(0, 0);

		data[1].Position = glm::vec3(x + width, y, 0.1f);
		data[1].TexCoord = glm::vec2(1, 0);

		data[2].Position = glm::vec3(x + width, y + height, 0.1f);
		data[2].TexCoord = glm::vec2(1, 1);

		data[3].Position = glm::vec3(x, y + height, 0.1f);
		data[3].TexCoord = glm::vec2(0, 1);

		s_Data.m_FullscreenQuadVertexArray = VertexArray::Create();
		auto quadVB = VertexBuffer::Create(data, 4 * sizeof(QuadVertex));
		quadVB->SetLayout({
			{ ShaderDataType::Vec3, "a_Position" },
			{ ShaderDataType::Vec2, "a_TexCoord" }
			});

		uint32_t indices[6] = { 0, 1, 2, 2, 3, 0, };
		auto quadIB = IndexBuffer::Create(indices, 6 * sizeof(uint32_t));

		s_Data.m_FullscreenQuadVertexArray->AddVertexBuffer(quadVB);
		s_Data.m_FullscreenQuadVertexArray->SetIndexBuffer(quadIB);

		Renderer2D::Init();
	}

	const std::unique_ptr<ShaderLibrary>& Renderer::GetShaderLibrary()
	{
		return s_Data.m_ShaderLibrary;
	}


	void Renderer::Clear(float r, float g, float b, float a)
	{
		Renderer::Submit([=]() {
			RenderAPI::Clear(r, g, b, a);
			});
	}

	void Renderer::DrawIndexed(uint32_t count, PrimitiveType type, bool depthTest)
	{
		Renderer::Submit([=]() {
			RenderAPI::DrawIndexed(count, type, depthTest);
			});
	}

	void Renderer::SetLineWidth(float thickness)
	{
		Renderer::Submit([=]() {
			RenderAPI::SetLineWidth(thickness);
			});
	}

	void Renderer::WaitAndRender()
	{
		s_Data.m_CmdQueue.Execute();
	}

	void Renderer::BeginRenderPass(const std::shared_ptr<RenderPass>& renderPass, bool clear)
	{
		SPARK_CORE_ASSERT(renderPass, "Render pass cannot be null!");

		// TODO: Convert all of this into a render command buffer
		s_Data.m_ActiveRenderPass = renderPass;

		renderPass->GetSpecification().TargetFramebuffer->Bind();
		if (clear)
		{
			const glm::vec4& clearColor = renderPass->GetSpecification().TargetFramebuffer->GetSpecification().ClearColor;
			Renderer::Submit([=]() {
				RenderAPI::Clear(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
				});
		}
	}

	void Renderer::EndRenderPass()
	{
		SPARK_CORE_ASSERT(s_Data.m_ActiveRenderPass, "No active render pass! Have you called Renderer::EndRenderPass twice?");
		s_Data.m_ActiveRenderPass->GetSpecification().TargetFramebuffer->Unbind();
		s_Data.m_ActiveRenderPass = nullptr;
	}

	void Renderer::SubmitQuad(const std::shared_ptr<MaterialInstance>& material, const glm::mat4& transform)
	{
		bool depthTest = true;
		if (material)
		{
			material->Bind();
			depthTest = material->GetFlag(MaterialFlag::DepthTest);

			auto shader = material->GetShader();
			shader->SetMat4("u_Transform", transform);
		}

		s_Data.m_FullscreenQuadVertexArray->Bind();
		Renderer::DrawIndexed(6, PrimitiveType::Triangles, depthTest);
	}

	void Renderer::SubmitFullscreenQuad(const std::shared_ptr<MaterialInstance>& material)
	{
		bool depthTest = true;
		if (material)
		{
			material->Bind();
			depthTest = material->GetFlag(MaterialFlag::DepthTest);
		}

		s_Data.m_FullscreenQuadVertexArray->Bind();
		Renderer::DrawIndexed(6, PrimitiveType::Triangles, depthTest);
	}

	void Renderer::SubmitMesh(const std::shared_ptr<Mesh>& mesh, const glm::mat4& transform, const std::shared_ptr<MaterialInstance>& overrideMaterial)
	{
		// auto material = overrideMaterial ? overrideMaterial : mesh->GetMaterialInstance();
		// auto shader = material->GetShader();
		// TODO: Sort this out
		mesh->m_VertexArray->Bind();

		auto& materials = mesh->GetMaterials();
		for (Submesh& submesh : mesh->m_Submeshes)
		{
			// Material
			auto material = materials[submesh.MaterialIndex];
			auto shader = material->GetShader();
			material->Bind();

			if (mesh->m_IsAnimated)
			{
				for (size_t i = 0; i < mesh->m_BoneTransforms.size(); i++)
				{
					std::string uniformName = std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
					mesh->m_MeshShader->SetMat4(uniformName, mesh->m_BoneTransforms[i]);
				}
			}
			shader->SetMat4("u_Transform", transform * submesh.Transform);

			Renderer::Submit([submesh, material]() {
				if (material->GetFlag(MaterialFlag::DepthTest))
					glEnable(GL_DEPTH_TEST);
				else
					glDisable(GL_DEPTH_TEST);

				glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex);
				});
		}
	}

	void Renderer::DrawBoundingBox(const std::shared_ptr<Mesh>& mesh, const glm::mat4& transform, const glm::vec4& color)
	{
		for (Submesh& submesh : mesh->m_Submeshes)
		{
			auto& box = submesh.Box;
			auto boxTransform = transform * submesh.Transform;
			DrawBoundingBox(box, boxTransform);
		}
	}

	void Renderer::DrawBoundingBox(const BoundingBox& box, const glm::mat4& transform, const glm::vec4& color /*= glm::vec4(1.0f)*/)
	{
		glm::vec4 min = { box.Min.x, box.Min.y, box.Min.z, 1.0f };
		glm::vec4 max = { box.Max.x, box.Max.y, box.Max.z, 1.0f };

		glm::vec4 corners[8] =
		{
			transform * glm::vec4 { box.Min.x, box.Min.y, box.Max.z, 1.0f },
			transform * glm::vec4 { box.Min.x, box.Max.y, box.Max.z, 1.0f },
			transform * glm::vec4 { box.Max.x, box.Max.y, box.Max.z, 1.0f },
			transform * glm::vec4 { box.Max.x, box.Min.y, box.Max.z, 1.0f },

			transform * glm::vec4 { box.Min.x, box.Min.y, box.Min.z, 1.0f },
			transform * glm::vec4 { box.Min.x, box.Max.y, box.Min.z, 1.0f },
			transform * glm::vec4 { box.Max.x, box.Max.y, box.Min.z, 1.0f },
			transform * glm::vec4 { box.Max.x, box.Min.y, box.Min.z, 1.0f }
		};

		for (uint32_t i = 0; i < 4; i++)
			Renderer2D::DrawLine(corners[i], corners[(i + 1) % 4], color);

		for (uint32_t i = 0; i < 4; i++)
			Renderer2D::DrawLine(corners[i + 4], corners[((i + 1) % 4) + 4], color);

		for (uint32_t i = 0; i < 4; i++)
			Renderer2D::DrawLine(corners[i], corners[i + 4], color);
	}

	CmdQueue& Renderer::GetRenderCommandQueue()
	{
		return s_Data.m_CmdQueue;
	}

	
	
}