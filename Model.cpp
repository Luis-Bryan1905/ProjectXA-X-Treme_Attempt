#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <iostream>
#include "Model.h"

namespace GE
{
	bool Model::loadFromFile(const char* filename)
	{
		std::vector<Vertex> LoadedVertices; // Create a vector to hold the loaded vertices from the model file

		Assimp::Importer imp; // Create an instance of the Assimp Importer, which is used to load models

		const aiScene* pScene = imp.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipUVs); // Use the Importer to read the model file specified by filename, applying a preset of post-processing steps to optimize the model for real-time rendering

		if (!pScene) // If the scene failed to load
		{
			std::cout << "Error loading model: " << imp.GetErrorString() << std::endl;
			return false;
		}

		for (int MeshIdx = 0; MeshIdx < pScene->mNumMeshes; MeshIdx++) // Loop through each mesh in the scene
		{
			const aiMesh* mesh = pScene->mMeshes[MeshIdx]; // Get the mesh

			for (int faceIdx = 0; faceIdx < mesh->mNumFaces; faceIdx++) // Loop through each face in the mesh
			{
				const aiFace& face = mesh->mFaces[faceIdx]; // Get the face

				for (int vertIdx = 0; vertIdx < 3; vertIdx++)
				{
					const aiVector3D* pos = &mesh->mVertices[face.mIndices[vertIdx]]; // Get the vertex position for the current vertex index in the face

					const aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[vertIdx]]; // Get the vertex position for the current vertex index in the face

					const aiVector3D norm	 = mesh->mNormals[face.mIndices[vertIdx]]; // Get the vertex normal for the current vertex index in the face

					const aiVector3D t = mesh->mTangents[face.mIndices[vertIdx]]; // Get the vertex tangent for the current vertex index in the face

					const aiVector3D bt = mesh->mBitangents[face.mIndices[vertIdx]]; // Get the vertex bitangent for the current vertex index in the face

					LoadedVertices.push_back(Vertex(pos->x, pos->y, pos->z, uv.x, uv.y, norm.x, norm.y, norm.z, t.x, t.y, t.z, bt.x, bt.y, bt.z)); // Create a Vertex object with the position, uv, normal, tangent and bitangent data and add it to the LoadedVertices vector
				}
			}

		}

		NumVertices = LoadedVertices.size(); // Set the number of vertices to the size of the LoadedVertices vector

		glGenBuffers(1, &vb0); // Generate a vertex buffer object (VBO) and store its ID in vb0

		glBindBuffer(GL_ARRAY_BUFFER, vb0); // Bind the VBO to the GL_ARRAY_BUFFER target

		glBufferData(GL_ARRAY_BUFFER, NumVertices * sizeof(Vertex), LoadedVertices.data(), GL_STATIC_DRAW); // Upload the vertex data from the LoadedVertices vector to the GPU, specifying that the data will be used for static drawing
	
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the VBO from the GL_ARRAY_BUFFER target

		return true;
	
	}
}

