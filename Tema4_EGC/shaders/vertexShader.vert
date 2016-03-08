#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat3 normalMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 worldPos = model * vec4(position, 1);
	gl_Position = projection * view * worldPos;
	
	FragPos = vec3(worldPos);
	Normal = normalMatrix * normal;
	TexCoord = texcoord;
}