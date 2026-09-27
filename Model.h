#include <GL/glew.h>
#pragma once

namespace GE
{
	struct Vertex //VERTEX CLASS
	{
		//location
		float x, y, z;
		float u, v;
		float nx, ny, nz; // normal
		float tx, ty, tz; //tangent
		float btx, bty, btz; //bitangent

		//contructors
		Vertex(float _x, float _y, float _z, float _u, float _v, float _nx, float _ny, float _nz, float _tx, float _ty, float _tz, float _btx, float _bty, float _btz) //XYZ, UV, Normal, Tangent, Bitangent
		{
			x = _x;
			y = _y;
			z = _z;

			u = _u;
			v = _v;

			nx = _nx;
			ny = _ny;
			nz = _nz;

			tx = _tx;
			ty = _ty;
			tz = _tz;

			btx = _btx;
			bty = _bty;
			btz = _btz;

		}

		Vertex()
		{
			x = y = z = 0.0f;

			u = v = 0.0f;

			nx = ny = nz = 0.0f;

			tx = ty = tz = 0.0f;

			btx = bty = btz = 0.0f;

		}

	};


	class Model
	{
	public:

		Model()
		{
			vb0 = 0;
			NumVertices = 0;
		}

		~Model()
		{
			glDeleteBuffers(1, &vb0);
		}

		bool loadFromFile(const char* filename);

		GLuint getVertices()
		{
			return vb0;
		}

		int getNumVertices()
		{
			return NumVertices;
		}

	private:

		GLuint vb0; // Vertex Buffer Object
		int NumVertices;
	};
};