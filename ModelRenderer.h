#pragma once
#include <GL/glew.h>
#include "Camera.h"
#include "Model.h"
#include "Texture.h"

namespace GE
{
	class ModelRenderer
	{
		public:
			ModelRenderer();

			~ModelRenderer();

			void init();

			void update();

			void draw(Camera* cam, Model *model);


			void destroy();

			float getPosX()
			{
				return pos_x;
			}

			float getPosY()
			{
				return pos_y;
			}

			float getPosZ()
			{
				return pos_z;
			}

			float getRotX()
			{
				return rot_x;
			}

			float getRotY()
			{
				return rot_y;
			}

			float getRotZ()
			{
				return rot_z;
			}

			float getScaleX()
			{
				return scale_x;
			}

			float getScaleY()
			{
				return scale_y;
			}

			float getScaleZ()
			{
				return scale_z;
			}

			void setPos(float x, float y, float z)
			{
				pos_x = x;
				pos_y = y;
				pos_z = z;
			}

			void setRotation(float rx, float ry, float rz)
			{
				rot_x = rx;
				rot_y = ry;
				rot_z = rz;
			}

			void setScale(float sx, float sy, float sz)
			{
				scale_x = sx;
				scale_y = sy;
				scale_z = sz;
			}

			void setTexture(Texture* _tex)
			{
				tex = _tex;
			}

			void ActivateFisheye()
			{
				Fisheye = true;

			}

			void DeactivateFisheye()
			{
				Fisheye = false;
			}

			bool isFisheyeActive()
			{
				return Fisheye;
			}

	private:
		GLuint programId;

		GLint vertexLocation;

		Texture* tex;

		GLint vertexUVLocation;

		GLint vertexNormal;

		float pos_x, pos_y, pos_z;
		float rot_x, rot_y, rot_z;
		float scale_x, scale_y, scale_z;

		GLuint transformUniformId;
		GLuint viewUniformID;
		GLuint projectionUniformID;
		GLuint samplerId;

		GLuint fogColourID;
		GLuint fogStartID;
		GLuint fogRangeID;

		GLuint viewPosID;

		GLuint lightColourID;

		bool Fisheye = true;

	};

}


