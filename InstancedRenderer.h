#pragma once
#include <GL/glew.h>
#include <vector>
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "CubeWorld.h"

namespace GE {
	// 1. Define a structure for the instance data

	struct InstancePosRotScale
	{
		float posX, posY, posZ; // Position of the instance
		float rotX, rotY, rotZ; // Rotation of the instance
		float scaleX, scaleY, scaleZ; // Scale of the instance
	};


	class InstancedRenderer
	{
	public:
		InstancedRenderer();
		~InstancedRenderer();

		void init();

		void destroy();

		void drawInstanced(Camera* cam, Model* m);

		void setInstanceData(const std::vector<InstancePosRotScale>& instances);
		
		void setTexture(Texture *_tex) {
			tex = _tex;
		}

	private:
		// Member fields
		// This member stores the program object that contains the shaders
		GLuint programId;

		// This member stores the attribute to select into the pipeline
		// to link the triangle vertices to the pipeline
		GLint vertexLocation;

		// Link to vColour attribute which receives a colour
		// and passes to fColour for fragment shader
		GLint vertexUVLocation;

		// Link instance matrix attribute
		GLint instanceMatLocation;

		// GLSL uniform variables for the transformation, view and projection matrices
		GLuint transformUniformId;
		GLuint viewUniformId;
		GLuint projectionUniformId;
		GLuint samplerId;

		int numInstances;

		GLuint instanceMatrixBuffer;

		Texture *tex;

		
	};
}

