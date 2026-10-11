#include "Background.h"
#include "ShaderUtils.h"
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <glm/gtc/type_ptr.hpp>


namespace GE
{
	Background::Background(std::string fname, int windowWidth, int windowHeight)
	{;

		SDL_Surface* surfaceImage = IMG_Load(fname.c_str());
		if (!surfaceImage) 
		{
			std::cerr << "Unable to load image: " << fname << std::endl;
			return;
		}
		
		float windowAspectRatio = static_cast<float>(windowWidth) / windowHeight; // Calculate the aspect ratio of the window
		float imageAspectRatio = static_cast<float>(surfaceImage->w) / surfaceImage->h; // Calculate the aspect ratio of the image

		SDL_FreeSurface(surfaceImage);
		createBackground(fname);
		createBackgroundVBO(windowAspectRatio, imageAspectRatio);
		createBackgroundProgram();
	}

	struct QuadVertex
	{
		float x, y, u, v;
		QuadVertex(float _x, float _y, float _u, float _v)
		{
			x = _x;
			y = _y;
			u = _u;
			v = _v;
		}

		QuadVertex()
		{
			x = 0.0f;
			y = 0.0f;
			u = 0.0f;
			v = 0.0f;
		}
	};

	QuadVertex quad[] = 
	{
		//Triangle 1
		QuadVertex(-1.0f,  1.0f, 0.0f, 1.0f), // Top-left
		QuadVertex(-1.0f, -1.0f, 0.0f, 0.0f), // Bottom-left
		QuadVertex( 1.0f, -1.0f, 1.0f, 0.0f), // Bottom-right

		//Triangle 2
		QuadVertex(1.0f, -1.0f, 1.0f, 0.0f), // Bottom-right
		QuadVertex(1.0f,  1.0f, 1.0f, 1.0f), // Top-right
		QuadVertex(-1.0f,  1.0f, 0.0f, 1.0f),// Top-left

	};



	void Background::createBackground(std::string filenames)
	{
		glGenTextures(1, &BackgroundCubeMapName); // Generate a texture object for the background image

		glBindTexture(GL_TEXTURE_2D, BackgroundCubeMapName); // Bind the texture object to the GL_TEXTURE_2D target

		SDL_Surface* surfaceImage = IMG_Load(filenames.c_str());

		if (surfaceImage == nullptr)
		{
			std::cerr << "Unable to load " << filenames << ": " << SDL_GetError() << std::endl;
			return;
		}

		GLenum format = surfaceImage->format->format; // Get the pixel format of the loaded image surface

		switch (format) // Determine the OpenGL format based on the SDL pixel format
		{
		case SDL_PIXELFORMAT_RGBA32: // If the pixel format is RGBA32, set the OpenGL format to GL_RGBA
			format = GL_RGBA; // Set the OpenGL format to GL_RGBA
			break;

		case SDL_PIXELFORMAT_RGB24: // If the pixel format is RGB24, set the OpenGL format to GL_RGB
			format = GL_RGB; // Set the OpenGL format to GL_RGB
			break;

		default: // If the pixel format is not recognized, log an error message and set the OpenGL format to GL_RGB
			format = GL_RGB; // Set the OpenGL format to GL_RGB
			SDL_Log("Unsupported pixel format: %s. Defaulting to GL_RGB.", SDL_GetPixelFormatName(surfaceImage->format->format));
			break;
		}

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			format,
			surfaceImage->w,
			surfaceImage->h,
			0,
			format,
			GL_UNSIGNED_BYTE,
			surfaceImage->pixels
		);
		
		SDL_Log("Loaded background image: %s", filenames.c_str());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // Set the texture magnification filter to GL_NEAREST for pixelated scaling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // Set the texture minification filter to GL_NEAREST for pixelated scaling

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Set the texture wrapping mode for the S (horizontal) coordinate to GL_CLAMP_TO_EDGE to prevent repeating
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Set the texture wrapping mode for the T (vertical) coordinate to GL_CLAMP_TO_EDGE to prevent repeating

		SDL_FreeSurface(surfaceImage); // Free the SDL surface after uploading the texture data to the GPU
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture object to avoid accidental modification
	}

	void Background::createBackgroundVBO(float windowAspectRatio, float imageAspectRatio)  // Create a Vertex Buffer Object (VBO) for the background quad, adjusting its dimensions based on the aspect ratios of the window and the image
	{
		float quadWidth = 1.0f;
		float quadHeight = 1.0f;

		// Adjust quad dimensions based on aspect ratios
		if (windowAspectRatio > imageAspectRatio)  // If the window's aspect ratio is greater than the image's aspect ratio, adjust the quad's width to maintain the image's aspect ratio
		{
			quadWidth = windowAspectRatio / imageAspectRatio; // Adjust the quad's width to maintain the image's aspect ratio
		}
		else 
		{
			quadHeight = imageAspectRatio / windowAspectRatio; // Adjust the quad's height to maintain the image's aspect ratio
		}

		QuadVertex quad[] = // Redefine the vertices of the quad with adjusted dimensions and texture coordinates
		{
			QuadVertex(-quadWidth,  quadHeight, 0.0f, 0.0f), // Top-left
			QuadVertex(-quadWidth, -quadHeight, 0.0f, 1.0f), // Bottom-left
			QuadVertex(quadWidth, -quadHeight, 1.0f, 1.0f), // Bottom-right

			QuadVertex(quadWidth, -quadHeight, 1.0f, 1.0f), // Bottom-right
			QuadVertex(quadWidth,  quadHeight, 1.0f, 0.0f), // Top-right
			QuadVertex(-quadWidth,  quadHeight, 0.0f, 0.0f) // Top-left

		};

		glGenBuffers(1, &vboBackground); // Generate a buffer object for the background quad
		glBindBuffer(GL_ARRAY_BUFFER, vboBackground); // Bind the buffer to the GL_ARRAY_BUFFER target

		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW); // Upload the vertex data to the GPU

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the buffer to avoid accidental modification
	}

	void Background::createBackgroundProgram() // Create and compile the shader program for rendering the background quad
	{

		const GLchar* V_ShaderCode[] =
		{
			"#version 410\n"
			"in vec2 vertexPos2D; \n"
			"in vec2 vertexUV; \n"
			"out vec2 texCoord; \n"
			"void main() {\n"
				"gl_Position = vec4(vertexPos2D, 0.0, 1.0); \n" // Set the position of the vertex in clip space (x, y, z, w)
				"texCoord = vertexUV; \n"
			"}\n"
		};

		const GLchar* F_ShaderCode[] =
		{
			"#version 410\n"
			"in vec2 texCoord;\n"
			"uniform sampler2D sampler;\n"
			"out vec4 fragmentColour;\n"
			"void main(){\n"
			"fragmentColour = texture(sampler, texCoord);\n" // x, y, z, w
			"}\n"
		};

		bool result = compileProgram(V_ShaderCode, F_ShaderCode, &BackgroundProgramId); // Compile the vertex and fragment shaders and link them into a shader program

		if (!result)
		{
			std::cerr << "Unable to create Background Renderer" << std::endl; // Display error Message to console
		}

		vertexLocation = glGetAttribLocation(BackgroundProgramId, "vertexPos2D"); // Get the location of the vertex position attribute in the shader program
		if (vertexLocation == -1)
		{
			std::cerr << "Unable to get vertexPos2D" << std::endl; // Display error Message to console
		}

		vertexUVLocation = glGetAttribLocation(BackgroundProgramId, "vertexUV"); // Get the location of the vertex UV attribute in the shader program
		if (vertexLocation == -1)
		{
			std::cerr << "Unable to get vertexUV" << std::endl; // Display error Message to console
		}
	}

	void Background::render(Camera* cam)
	{
		glDisable(GL_CULL_FACE); // Disable face culling to render both front and back faces of the background quad

		bool isDepthTestEnable = glIsEnabled(GL_DEPTH_TEST); // Check if depth testing is currently enabled and store the result in a boolean variable

		glDisable(GL_DEPTH_TEST); // Disable depth testing to ensure the background quad is rendered behind all other objects in the scene
		
		glUseProgram(BackgroundProgramId); // Use the shader program for rendering the background quad

		glBindBuffer(GL_ARRAY_BUFFER, vboBackground); // Bind the vertex buffer object containing the background quad's vertex data

		glEnableVertexAttribArray(vertexLocation); // Enable the vertex position attribute array to allow the shader program to access the vertex position data
		glVertexAttribPointer(vertexLocation, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, x)); // Define the structure of the vertex position attribute, specifying the number of components, data type, normalization, stride, and offset

		glEnableVertexAttribArray(vertexUVLocation); // Enable the vertex UV attribute array to allow the shader program to access the vertex UV data
		glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, u)); // Define the structure of the vertex UV attribute, specifying the number of components, data type, normalization, stride, and offset

		glActiveTexture(GL_TEXTURE0); // Activate texture unit 0 to bind the background texture for rendering

		glUniform1i(samplerId, 0); // Set the sampler uniform in the shader program to use texture unit 0 for sampling the background texture

		glBindTexture(GL_TEXTURE_2D, BackgroundCubeMapName); // Bind the background texture to the GL_TEXTURE_2D target for rendering

		glDrawArrays(GL_TRIANGLES, 0, sizeof(quad) / sizeof(QuadVertex)); // Draw the background quad using the vertex data in the vertex buffer object, specifying the primitive type (triangles), starting index, and number of vertices to render

		glDisableVertexAttribArray(vertexLocation); // Disable the vertex position attribute array to prevent accidental modification of the vertex data after rendering
		glDisableVertexAttribArray(vertexUVLocation); // Disable the vertex UV attribute array to prevent accidental modification of the vertex data after rendering

		glUseProgram(0); // Unbind the shader program to prevent accidental modification of the shader state after rendering

		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the background texture to prevent accidental modification of the texture state after rendering
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the vertex buffer object to prevent accidental modification of the vertex data after rendering

		if (isDepthTestEnable) // If depth testing was enabled before rendering the background, re-enable it to restore the previous state
		{
			glEnable(GL_DEPTH_TEST); // Re-enable depth testing to restore the previous state
		}
	}

	void Background::clean()
	{
		glDeleteProgram(BackgroundProgramId);
		glDeleteBuffers(1, &vboBackground);
		glDeleteTextures(1, &BackgroundCubeMapName);
	}
}