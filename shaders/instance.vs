#version 400

layout (location = 0) in vec3 position;

uniform mat4 model[1000];
uniform mat4 view;
uniform mat4 proj;

void main()
{
	gl_Position = proj * (view * (model[gl_InstanceID] * vec4(position, 1)));
}