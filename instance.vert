#version 410

// Inputs to vertex shader
in vec3 vertexPos3D;
in vec2 vUV;

// 14. The transformation matrix comes from the vector of
// of position, rotation, scales converted to an array
in mat4 instance_transform_mat;

// Output
out vec2 uv;

// View project matrices.  Note no model matrix as each instance
// has its own matrix
uniform mat4 view;
uniform mat4 projection;

// Shader main code
void main() {
	// Convert input vertex to 4D 
	vec4 v = vec4(vertexPos3D.xyz, 1);

	// Transform instance
	v = projection * view * instance_transform_mat * v;

	// Pass transformed vertex to next stage
	gl_Position = v;

	// As well as uv.  Don't transform uv
	uv = vUV;
}