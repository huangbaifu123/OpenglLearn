#version 330 core
layout (location = 0) in vec3 aPos; // 位置变量的属性位置值为0
layout (location = 1) in vec3 nVec;
layout (location = 2) in vec2 texcoords;

out vec3 normalVec;
out vec3 FragPos;
out vec2 Texcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	normalVec = nVec;
	//normalVec = mat3(transpose(inverse(model))) * nVec;
	FragPos = vec3(model * vec4(aPos, 1.0));
	Texcoords = texcoords;
}