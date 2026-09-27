#version 410

// Inputs to vertex shader
in vec3 vertexPos3D;
in vec2 vUV;

// Output
out vec2 uv;

// Uniforms
uniform mat4 projection;

// Shader main code
void main() {
	// Convert input vertex to 4D 
	vec4 v = projection * vec4(vertexPos3D.xyz, 1.0);

	// Pass transformed vertex to next stage
	gl_Position = v;

	// As well as uv.  Don't transform uv
	uv = vUV;
}