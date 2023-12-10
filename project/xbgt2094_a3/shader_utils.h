#pragma once
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <stdio.h>

namespace ShaderUtils
{
	bool CheckProgramLinkingStatus(unsigned int programId);

	bool CheckShaderCompilationStatus(unsigned int shaderId, const char* shaderTypeString);

	unsigned int CompileShader(const GLenum shaderType, const char* shaderCode, const char* shaderTypeString);

	unsigned int LoadShaderFromFile(const GLenum shaderType, const std::string& path, const char* shaderTypeString);

	unsigned int InitializeShaderProgram(const std::string& vertexPath, const std::string& fragmentPath, std::string& errorStr);
}