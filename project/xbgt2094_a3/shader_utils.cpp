#include "shader_utils.h"

namespace ShaderUtils
{
	static std::string errorString;

	bool CheckProgramLinkingStatus(unsigned int programId)
	{
		int success;
		char infoLog[1024];

		glGetProgramiv(programId, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(programId, 1024, NULL, infoLog);

			std::ostringstream s;
			s << "Error linking program:\n" << infoLog;
			errorString.append(s.str());
			return false;
		}

		return true;
	}

	bool CheckShaderCompilationStatus(unsigned int shaderId, const char* shaderTypeString)
	{
		int success;
		char infoLog[1024];

		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shaderId, 1024, NULL, infoLog);

			std::ostringstream s;
			s << "Error compiling " << shaderTypeString << " shader:\n" << infoLog;
			errorString.append(s.str());
			return false;
		}

		return true;
	}

	unsigned int CompileShader(const GLenum shaderType, const char* shaderCode, const char* shaderTypeString)
	{
		unsigned int shaderId;

		shaderId = glCreateShader(shaderType);
		glShaderSource(shaderId, 1, &shaderCode, NULL);
		glCompileShader(shaderId);

		if (!CheckShaderCompilationStatus(shaderId, shaderTypeString))
		{
			glDeleteShader(shaderId);
			return 0;
		}

		return shaderId;
	}

	unsigned int LoadShaderFromFile(const GLenum shaderType, const std::string& path, const char* shaderTypeString)
	{
		unsigned int shaderId = 0;

		std::ifstream shaderFile;
		std::string code;

		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			shaderFile.open(path);
			std::stringstream stream;

			stream << shaderFile.rdbuf();

			shaderFile.close();
			code = stream.str();
			shaderId = CompileShader(shaderType, code.c_str(), shaderTypeString);
		}
		catch (std::ifstream::failure& e)
		{
			std::ostringstream s;
			s << "ERROR: File " << path.c_str() << " not successfully read!\n" << e.what() << std::endl;
			errorString.append(s.str());
		}

		return shaderId;
	}

	unsigned int InitializeShaderProgram(const std::string& vertexPath, const std::string& fragmentPath, std::string& errorStr)
	{
		errorString.clear();

		unsigned int vertexShader, fragmentShader;
		unsigned int programId;

		vertexShader = LoadShaderFromFile(GL_VERTEX_SHADER, vertexPath, "VERTEX");
		fragmentShader = LoadShaderFromFile(GL_FRAGMENT_SHADER, fragmentPath, "FRAGMENT");

		programId = glCreateProgram();

		if (vertexShader == 0 || fragmentShader == 0)
		{
			errorStr = errorString;
			return 0;
		}

		glAttachShader(programId, vertexShader);
		glAttachShader(programId, fragmentShader);

		glLinkProgram(programId);

		if (!CheckProgramLinkingStatus(programId))
		{
			glDeleteProgram(programId);
			errorStr = errorString;

			return 0;
		}

		return programId;
	}
}