#version 410

// Input to fragment shader
in vec2 uv;

// Texture
uniform sampler2D sampler;

// Final fragment colour
out vec4 fragmentColour;

// Shader body
void main()
{
	// Determine fragment colour from texture based on uv co-oords
	vec4 texColour = texture(sampler, uv);

	fragmentColour = texColour;
}