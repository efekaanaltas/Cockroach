#pragma once

#include <string>
#include <glm/glm.hpp>

// TODO: REMOVE!
typedef unsigned int GLenum;

namespace Cockroach
{
	class Shader
	{
	public:
		Shader(const std::string& filepath);
		Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const float2& value);
		void UploadUniformFloat3(const std::string& name, const float3& value);
		void UploadUniformFloat4(const std::string& name, const float4& value);

		void UploadUniformMat3(const std::string& name, const mat3& matrix);
		void UploadUniformMat4(const std::string& name, const mat4& matrix);
	private:
		std::string ReadFile(const std::string& filepath);
		std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
		void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
	private:
		uint32_t rendererID;
	};
}