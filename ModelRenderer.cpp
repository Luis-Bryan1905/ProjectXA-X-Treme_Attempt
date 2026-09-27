#include "ModelRenderer.h"
#include "Texture.h"
#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace GE
{
	ModelRenderer::ModelRenderer()
	{
		pos_x = pos_y = pos_z = 0.0f;
		rot_x = rot_y = rot_z = 0.0f;
		scale_x = scale_y = scale_z = 1.0f;

		programId = 0;

		vertexLocation = 0;

		vertexUVLocation = 0;

		transformUniformId = 0;
		viewUniformID = 0;
		projectionUniformID = 0;
	}

	ModelRenderer::~ModelRenderer()
	{
	}

	void ModelRenderer::init()
	{

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const GLchar* V_ShaderCode[] =
		{
			"#version 410\n" // Set the GLSL version to 4.10

			"in vec3 vertexPos3D;\n" // Declare an input variable for the vertex position in 3D space
			"in vec3 vertexNormal;\n" // Declare an input variable for the vertex normal vector
			"in vec2 vUV;\n" // Declare an input variable for the vertex texture coordinates
			"out vec2 uv;\n" // Declare an output variable for the texture coordinates to be passed to the fragment shader
			"out vec3 posW;\n" // Declare an output variable for the vertex position in world space to be passed to the fragment shader
			"out vec3 outNormal;\n" // Declare an output variable for the vertex normal vector to be passed to the fragment shader

			"out float fog_amount;\n" // Declare an output variable for the fog amount to be passed to the fragment shader

			"uniform mat4 viewMat;\n" // Declare a uniform variable for the view matrix to transform vertices from world space to camera space
			"uniform mat4 projMat;\n" // Declare a uniform variable for the projection matrix to transform vertices from camera space to clip space
			"uniform mat4 transformMat;\n" //
			
			"uniform float fog_start;\n"
			"uniform float fog_range;\n"

			"void main(){\n"
				"vec4 v = vec4(vertexPos3D.xyz, 1);\n"

				"posW = vec3(transformMat * v);\n"
				"outNormal = vec3(transformMat * vec4(vertexNormal, 0.0));\n"

				"vec4 posInWorld = transformMat  * v;\n"
				"vec4 posCamera = viewMat * posInWorld;\n"

				"float radius = -15.0;\n"

				"float thetaX = posCamera.x / radius;\n"
				"float thetaY = posCamera.y / radius;\n"

				"posCamera.x = sin(thetaX) * radius;\n"
				"posCamera.y = sin(thetaY) * radius;\n"

				"posCamera.z += (1.0 - cos(thetaX)) * radius;\n"
				"posCamera.z += (1.0 - cos(thetaY)) * radius;\n"


				"gl_Position = projMat * posCamera;\n"
				"uv = vUV;\n"

				"vec4 pos_rel_eye = viewMat * posInWorld;\n"
				"float distance = length(pos_rel_eye.xyz);\n"

				"fog_amount = (distance - fog_start) / fog_range;\n"

				"fog_amount = clamp(fog_amount, 0.0f, 1.0f);\n"

			"}\n"
		};

		const GLchar* V_ShaderCode2[] =
		{
			"#version 410\n"

			"in vec3 vertexPos3D;\n"
			"in vec3 vertexNormal;\n"
			"in vec2 vUV;\n"
			"out vec2 uv;\n"
			"out vec3 posW;\n"
			"out vec3 outNormal;\n"

			"out float fog_amount;\n"

			"uniform mat4 viewMat;\n"
			"uniform mat4 projMat;\n"
			"uniform mat4 transformMat;\n"

			"uniform float fog_start;\n"
			"uniform float fog_range;\n"

			"void main(){\n"
				"vec4 v = vec4(vertexPos3D.xyz, 1);\n"

				"posW = vec3(transformMat * v);\n"
				"outNormal = vec3(transformMat * vec4(vertexNormal, 0.0));\n"

				"vec4 posInWorld = transformMat  * v;\n"
				"v = projMat  * viewMat * posInWorld;\n"

				"gl_Position = v;\n"
				"uv = vUV;\n"

				"vec4 pos_rel_eye = viewMat * posInWorld;\n"
				"float distance = length(pos_rel_eye.xyz);\n"

				"fog_amount = (distance - fog_start) / fog_range;\n"

				"fog_amount = clamp(fog_amount, 0.0f, 1.0f);\n"

			"}\n"
		};

		if (Fisheye)
		{
			glShaderSource(vertexShader, 1, V_ShaderCode, NULL);
		}
		else
		{
			glShaderSource(vertexShader, 1, V_ShaderCode2, NULL);
		}


		glCompileShader(vertexShader);

		GLint isShaderCompiledOK = GL_FALSE;

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isShaderCompiledOK);

		if (isShaderCompiledOK != GL_TRUE)
		{
			std::cerr << "Unable to compile vertex shader" << std::endl; // Display error Message to console

			return;
		}

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		const GLchar* F_ShaderCode[] =
		{
			"#version 410\n"
			"in vec2 uv;\n"
			"in vec3 posW;\n"
			"in vec3 outNormal;\n"

			"in float fog_amount;\n"
			"uniform vec3 fog_colour;\n"

			"uniform vec3 lightColour;\n"
			"const vec3 ambientLight = vec3(0.1f, 0.1f, 0.1f);\n"
			"const vec3 lightPos = vec3(180.0f, 1000.0f, 360.0f);\n"
			"const float shininess = 32.0f;\n"
			"const float specularStrength = 0.2f;\n"

			"uniform vec3 viewPos;\n"
			"uniform sampler2D sampler;\n"
			"out vec4 fragmentColour;\n"

			"void main(){\n"
				"vec4 texColour = texture(sampler, uv).rgba;\n"

				"vec3 normalizedNormal = normalize(outNormal);\n"
				"vec3 lightDirection = normalize(lightPos - posW);\n"
				"float diffIllum = max(dot(normalizedNormal, lightDirection), 0.0f);\n"
				"vec3 diffuse = diffIllum * lightColour;\n"

				"vec3 viewDir = normalize(viewPos - posW);\n"
				"vec3 halfwayDir = normalize(lightDirection + viewPos);\n"
				//"vec3 reflectDirection = reflect(-lightDirection, normalizedNormal);\n"
				"float spec = pow(max(dot(viewDir, halfwayDir), 0.0), shininess);\n"
				"vec3 specular = specularStrength * spec * lightColour;\n"

				"vec3 finalColour = ambientLight + diffuse * texColour.rgb + specular;\n"

				//"fragmentColour = mix(texColour, vec4(fog_colour, 1.0f), fog_amount);\n" // This line mixes the base colour with the fog colour based on the fog amount, creating a fog effect
				//"fragmentColour = mix(vec4(finalColour, texColour.a), vec4(fog_colour, 1.0f), fog_amount);\n" // This line mixes the final lighting computed colour with the fog colour based on the fog amount, creating a fog effect
				"fragmentColour = vec4(texColour.rgb, texColour.a);\n" // This line outputs the final colour without any fog effect, for testing purposes
			"}\n"
		};

		glShaderSource(fragmentShader, 1, F_ShaderCode, NULL);

		glCompileShader(fragmentShader);

		isShaderCompiledOK = GL_FALSE;

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isShaderCompiledOK);

		if (isShaderCompiledOK != GL_TRUE)
		{
			GLint logLength;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
			std::vector<char> log(logLength);
			glGetShaderInfoLog(fragmentShader, logLength, NULL, log.data());
			std::cerr << log.data() << std::endl;

			return;
		}

		programId = glCreateProgram();

		glAttachShader(programId, vertexShader);
		glAttachShader(programId, fragmentShader);

		glLinkProgram(programId);

		GLint isProgramLinkedOK = GL_FALSE;

		glGetProgramiv(programId, GL_LINK_STATUS, &isProgramLinkedOK);

		if (isProgramLinkedOK != GL_TRUE)
		{
			std::cerr << "Failed to link program" << std::endl; // Display error Message to console
		}

		vertexLocation = glGetAttribLocation(programId, "vertexPos3D");

		if (vertexLocation == -1)
		{
			std::cerr << "Unable to get vertexPos3D" << std::endl; // Display error Message to console
		}

		vertexUVLocation = glGetAttribLocation(programId, "vUV");

		if (vertexUVLocation == -1)
		{
			std::cerr << "Unable to get vUV" << std::endl; // Display error Message to console
		}

		vertexNormal = glGetAttribLocation(programId, "vertexNormal");

		if (vertexNormal == -1)
		{
			std::cerr << "Unable to get vertexNormal" << std::endl; // Display error Message to console
		}

		viewUniformID = glGetUniformLocation(programId, "viewMat");
		projectionUniformID = glGetUniformLocation(programId, "projMat");

		samplerId = glGetUniformLocation(programId, "sampler");

		viewPosID = glGetUniformLocation(programId, "viewPos");
		lightColourID = glGetUniformLocation(programId, "lightColour");

		fogColourID = glGetUniformLocation(programId, "fog_colour");
		fogStartID = glGetUniformLocation(programId, "fog_start");
		fogRangeID = glGetUniformLocation(programId, "fog_range");

		glUseProgram(programId);
		glUniform1f(fogStartID, 5.0f);
		glUniform1f(fogRangeID, 200.0f);
		glm::vec3 fogColour = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(fogColourID, 1, glm::value_ptr(fogColour));

		glUseProgram(0);

	}

	void ModelRenderer::update()
	{
	}

	void ModelRenderer::draw(Camera* cam, Model* model)
	{
		glEnable(GL_CULL_FACE);


		glm::vec3 position = glm::vec3(cam->getPosX(), cam->getPosY(), cam->getPosZ());
		glm::vec3 lookAt = cam->getTarget();
		glm::mat4 viewMatrix = cam->getViewMatrix();
		glm::mat4 projectionMatrix = cam->getProjectionMatrix();

		glUseProgram(programId); // GOES FIRST BEFORE ANYTHING ELSE IN THE RENDERING CODE

		GLint transformUniformId = glGetUniformLocation(programId, "transformMat");

		glm::mat4 transformMatrix = glm::mat4(1.0f);	//create an identity matrix

		transformMatrix = glm::translate(transformMatrix, glm::vec3(pos_x, pos_y, pos_z));

		// Convert stored rotation degrees to radians for glm::rotate
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rot_x), glm::vec3(1.0f, 0.0f, 0.0f));
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rot_y), glm::vec3(0.0f, 1.0f, 0.0f));
		transformMatrix = glm::rotate(transformMatrix, glm::radians(rot_z), glm::vec3(0.0f, 0.0f, 1.0f));

		transformMatrix = glm::scale(transformMatrix, glm::vec3(scale_x, scale_y, scale_z));

		// Remember, order is important. if we rotate then move, the triangle is moved then rotated about the origin.

		glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(transformMatrix));

		glUniformMatrix4fv(viewUniformID, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionUniformID, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glUniform3f(viewPosID, cam->getPosX(), cam->getPosY(), cam->getPosZ());

		glUniform3f(lightColourID, 1.0f, 1.0f, 1.0f);

		glBindBuffer(GL_ARRAY_BUFFER, model->getVertices());

		glEnableVertexAttribArray(vertexLocation);

		glEnableVertexAttribArray(vertexUVLocation); // Enable the vertex colour attribute in the shader

		glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, u))); // index, size, type, normalized, stride, pointer

		glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)); // index, size, type, normalized, stride, pointer

		glEnableVertexAttribArray(vertexNormal); // Enable the vertex normal attribute in the shader

		glVertexAttribPointer(vertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx)); // Vertex normal index, size, type, normalized, stride, pointer

		glActiveTexture(GL_TEXTURE0); // Set the active texture unit to 0 (the default)
		glUniform1i(samplerId, 0); // Set the sampler uniform to use texture unit 0
		glBindTexture(GL_TEXTURE_2D, tex->getTextureName()); // Bind the model's texture to the active texture unit

		glUniform3f(lightColourID, 1.0f, 1.0f, 1.0f);

		glDrawArrays(GL_TRIANGLES, 0, model->getNumVertices()); // mode, first, count

		glDisableVertexAttribArray(vertexLocation);
		glDisableVertexAttribArray(vertexUVLocation);
		glDisableVertexAttribArray(vertexNormal);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glUseProgram(0); // GOES LAST AFTER ANYTHING ELSE IN THE RENDERING CODE

	}

	void ModelRenderer::destroy()
	{
	}
}