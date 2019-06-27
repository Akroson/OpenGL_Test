#version 330 core
//out vec3 FragPos;

layout (location = 0) in vec3 aPos;

uniform mat4 MVP;

void main()
{
	//FragPos = vec3(model * vec4(aPos, 1.0f));
	gl_Position = MVP * vec4(aPos, 1.0f);
}