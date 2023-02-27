#pragma once

namespace SparkRabbit {
	class OpenglShader
	{
	public:
		OpenglShader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~OpenglShader();

		void Bind() const;
		void Unbind() const;

	private:
		uint32_t m_renderID;
	};
}

