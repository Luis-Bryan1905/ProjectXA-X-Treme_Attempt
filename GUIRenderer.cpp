#include "GUIRenderer.h"
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>
#include "ShaderUtils.h"
#include <iostream>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace GE {

	// Represents a vertex in the engine
	// This is a copy of the model vertex so is unnecessary
	// duplication.  Think about how you can combine the similar
	// renderers into one type of scene object
	struct Vertex {
		// Location
		float x, y, z;
		float u, v;

		// Constructors
		// Sets vertex to passed values
		Vertex(float _x, float _y, float _z, float _u, float _v) 
		{
			// Location
			x = _x;
			y = _y;
			z = _z;

			// Colour
			u = _u;
			v = _v;
		}

		// Default, set to 0.0
		Vertex() 
		{
			x = y = z = 0.0f;
			u = v = 0.0f;
		}
	};

	// Constructor
	GUIRenderer::GUIRenderer() {
		programId = 0;
		vertexLocation = 0;
		vertexUVLocation = 0;
		projectionUniformId = 0;
		samplerId = 0;
		projectionMat = glm::mat4(1.0f);
	}

	// Initialise
	void GUIRenderer::init(int window_width, int window_height)
	{
		// Load shader source from files.  Need the new ShaderUtils files off moodle
		// Text shaders are different to regular shaders because they only
		// transform based on projection
		std::string v_shader_source = loadShaderSourceCode("GUI.vert");
		std::string f_shader_source = loadShaderSourceCode("GUI.frag");

		// Rest of code is similar to model shader code
		const GLchar* v_source_array[] = { v_shader_source.c_str() };
		const GLchar* f_source_array[] = { f_shader_source.c_str() };

		// Compile shaders into a program
		if (!compileProgram(v_source_array, f_source_array, &programId)) {
			std::cerr << "Problem building program.  Check console log for more information." << std::endl;
		}

		// Now get a link to the vertexPos3D so we can link the attribute
		// to our vertices when rendering
		vertexLocation = glGetAttribLocation(programId, "vertexPos3D");

		// Check for errors
		if (vertexLocation == -1) {
			std::cerr << "Problem getting vertex3DPos" << std::endl;
		}

		// Now link to the vUV attribute
		vertexUVLocation = glGetAttribLocation(programId, "vUV");

		// Check for errors
		if (vertexUVLocation == -1) {
			std::cerr << "Problem getting vUV" << std::endl;
		}

		// Link the uniforms to the member fields
		projectionUniformId = glGetUniformLocation(programId, "projection");
		samplerId = glGetUniformLocation(programId, "sampler");

		// Difference: Create orthographic camera
		projectionMat = glm::ortho(0.0f, (float) window_width, (float) window_height, 0.0f);
	}

	// Main rendering function which draws a texture to GUI
	// All GUI objects must be a texture
	void GUIRenderer::drawTexture(int x, int y, Texture* text)
	{
		// Get the width and height of the text in pixels
		int texW = text->getWidth();
		int texH = text->getHeight(); 

		// A quad is created on the fly for the text in screen co-ordinates, not
		// world co-ordinates
		GLuint vboTextQuad;

		Vertex textQuad[] = {
			Vertex(x, y, 0.0f, 0.0f, 1.0f),
			Vertex(x, y + texH, 0.0f, 0.0f, 0.0f),
			Vertex(x + texW, y + texH, 0.0f, 1.0f, 0.0f),

			Vertex(x + texW, y + texH, 0.0f, 1.0f, 0.0f),
			Vertex(x + texW, y, 0.0f, 1.0f, 1.0f),
			Vertex(x, y, 0.0f, 0.0f, 1.0f),
		};

		// Create the vertex buffer for the quad
		glGenBuffers(1, &vboTextQuad);
		glBindBuffer(GL_ARRAY_BUFFER, vboTextQuad);

		// Transfer vertices to graphics memory
		glBufferData(GL_ARRAY_BUFFER, sizeof(textQuad), textQuad, GL_STATIC_DRAW);

		// Render text
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Setup pipeline for quad
		// Select the program into the rendering context
		glUseProgram(programId);

		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projectionMat));

		// Enable the attribute to be passed vertices from the vertex buffer object
		glEnableVertexAttribArray(vertexLocation);
		// Select the vertex buffer object into the context
		glBindBuffer(GL_ARRAY_BUFFER, vboTextQuad);

		// Define the structure of a vertex for OpenGL to select values from vertex buffer
		// and store in vertexPos2DLocation attribute
		glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));

		// Enable the attribute to be passed vertices from the vertex buffer object
		glEnableVertexAttribArray(vertexUVLocation);
		// Define where the vertex specification will find the colour data and how much
		// Colour data is four float values, located at where the r member is.  Stride is a vertex apart
		glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);
		glBindTexture(GL_TEXTURE_2D, text->getTextureName());

		// Draw the model
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Tidy up pipeline
		glBindTexture(GL_TEXTURE_2D, 0);

		// Unselect the attribute from the context
		glDisableVertexAttribArray(vertexLocation);
		glDisableVertexAttribArray(vertexUVLocation);

		// Unselect the program from the context
		glUseProgram(0);

		// Release the vertex buffer
		glDeleteBuffers(1, &vboTextQuad);

		glEnable(GL_DEPTH_TEST);
		//glDisable(GL_BLEND);
	}

	void GUIRenderer::destroy()
	{
		glDeleteProgram(programId);
	}
}
