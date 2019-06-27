#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

enum shader_program_type
{
	ShaderProgramType_Vertex,
	ShaderProgramType_Fragment,
	ShaderProgramType_Geometry,
	ShaderProgramType_Compute,

	ShaderProgramType_Count,

	ShaderProgramType_Program
};

class shader
{
public:
	unsigned int ID;

	union
	{
		const char *NameShader[ShaderProgramType_Count];

		struct
		{
			const char *Vert;
			const char *Frag;
			const char *Geom;
			const char *Comp;
		};
	};


	shader(const char* VertexPath, const char* FragmentPath, const char* GeometryPath = 0, const char *ComputePath = 0)
	{
		Vert = VertexPath;
		Frag = FragmentPath;
		Geom = GeometryPath;
		Comp = ComputePath;

		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;

		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			vShaderFile.open(VertexPath);
			fShaderFile.open(FragmentPath);
			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();

			if (GeometryPath != 0)
			{
				gShaderFile.open(GeometryPath);
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure e)
		{
			std::cout << VertexPath << ' ' << FragmentPath << '\n' << std::endl;
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const char* vShaderCode = vertexCode.c_str();
		const char * fShaderCode = fragmentCode.c_str();
		// compile shaders
		unsigned int vertex, fragment;
		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		CheckCompileErrors(vertex, ShaderProgramType_Vertex);
		// fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		CheckCompileErrors(fragment, ShaderProgramType_Fragment);

		unsigned int geometry;
		if (GeometryPath != nullptr)
		{
			const char * gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			CheckCompileErrors(geometry, ShaderProgramType_Geometry);
		}
		// shader Program
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (GeometryPath != 0)
		{
			glAttachShader(ID, geometry);
		}
		glLinkProgram(ID);
		CheckCompileErrors(ID, ShaderProgramType_Program);
		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (GeometryPath != 0)
		{
			glDeleteShader(geometry);
		}

	}
	
	void Use() const
	{
		glUseProgram(ID);
	}
	
	void SetBool(char *Name, bool Value) const
	{
		glUniform1i(glGetUniformLocation(ID, Name), (int)Value);
	}
	
	void SetInt(char *Name, int Value) const
	{
		glUniform1i(glGetUniformLocation(ID, Name), Value);
	}
	
	void SetFloat(char *Name, float Value) const
	{
		glUniform1f(glGetUniformLocation(ID, Name), Value);
	}
	
	void SetVec2(const char *Name, const v2 &Value) const
	{
		glUniform2fv(glGetUniformLocation(ID, Name), 1, &Value.E[0]);
	}
	void SetVec2(const char *Name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, Name), x, y);
	}
	
	void SetVec3(const char *Name, const v3 &Value) const
	{
		glUniform3fv(glGetUniformLocation(ID, Name), 1, &Value.E[0]);
	}
	
	void SetVec3(const char *Name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, Name), x, y, z);
	}
	
	void SetVec4(const char *Name, const v4 &Value) const
	{
		glUniform4fv(glGetUniformLocation(ID, Name), 1, &Value.E[0]);
	}

	void SetVec4(const char *Name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(ID, Name), x, y, z, w);
	}

	void SetMat4(const char *Name, const mat4 &Mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, Name), 1, GL_FALSE, &Mat.E[0][0]);
	}

private:
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	void CheckCompileErrors(GLuint Shader, shader_program_type ShaderType)
	{
		GLint Success;
		GLchar InfoLog[1024];
		if (ShaderType != ShaderProgramType_Program)
		{
			glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
			if (!Success)
			{
				glGetShaderInfoLog(Shader, 1024, NULL, InfoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << NameShader[ShaderType] << '\n' << InfoLog
					<< "-------------------------------------------------------\n" << std::endl;
			}
		}
		else
		{
			glGetProgramiv(Shader, GL_LINK_STATUS, &Success);
			if (!Success)
			{
				glGetProgramInfoLog(Shader, 1024, NULL, InfoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << "PROGRAM" << '\n' << InfoLog
					<< "-------------------------------------------------------\n" << std::endl;
			
				for (int ShaderIndex = 0;
					ShaderIndex < ShaderProgramType_Count;
					++ShaderIndex)
				{
					if (NameShader[ShaderIndex])
					{
						std::cout << NameShader[ShaderIndex] << std::endl;
					}
				}
			}
		}
	}
};