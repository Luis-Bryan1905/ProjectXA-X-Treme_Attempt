#version 410 // Set the GLSL version to 4.10

in vec3 vertexPos3D; // Declare an input variable for the vertex position in 3D space
in vec3 vertexNormal; // Declare an input variable for the vertex normal vector
in vec2 vUV; // Declare an input variable for the vertex texture coordinates
out vec2 uv; // Declare an output variable for the texture coordinates to be passed to the fragment shader
out vec3 posW; // Declare an output variable for the vertex position in world space to be passed to the fragment shader
out vec3 outNormal; // Declare an output variable for the vertex normal vector to be passed to the fragment shader

out float fog_amount; // Declare an output variable for the fog amount to be passed to the fragment shader

uniform mat4 viewMat; // Declare a uniform variable for the view matrix to transform vertices from world space to camera space
uniform mat4 projMat; // Declare a uniform variable for the projection matrix to transform vertices from camera space to clip space
uniform mat4 transformMat; //

uniform float fog_start;
uniform float fog_range;

void main()
{
	vec4 v = vec4(vertexPos3D.xyz, 1);

	posW = vec3(transformMat * v);
	outNormal = vec3(transformMat * vec4(vertexNormal, 0.0));

	vec4 posInWorld = transformMat  * v;
	vec4 posCamera = viewMat * posInWorld;

	float radius = -15.0;

	float thetaX = posCamera.x / radius;
	float thetaY = posCamera.y / radius;

	posCamera.x = sin(thetaX) * radius;
	posCamera.y = sin(thetaY) * radius;

	posCamera.z += (1.0 - cos(thetaX)) * radius;
	posCamera.z += (1.0 - cos(thetaY)) * radius;


	gl_Position = projMat * posCamera;
	uv = vUV;

	vec4 pos_rel_eye = viewMat * posInWorld;
	float distance = length(pos_rel_eye.xyz);

	fog_amount = (distance - fog_start) / fog_range;

	fog_amount = clamp(fog_amount, 0.0f, 1.0f);

}