#pragma once

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "Camera.h"
#include "ModelRenderer.h"
#include "Model.h"
#include "Texture.h"
#include "SkyboxRenderer.h"
#include "InstancedRenderer.h"
#include "GUIRenderer.h"
#include "CubeWorld.h"


namespace GE
{

	struct MeshInstance
	{
		Model* model;
		ModelRenderer* renderer;
	};


	class GameEngine // Basic Game Engine Class
	{
	public:

		GameEngine(); // Constructor

		virtual ~GameEngine(); // Destructor

		bool init(); // Initialize the engine

		bool keep_running(); // Check if engine should keep running

		void processInput(); // Process input

		void update(); // Update game logic

		void draw(); // Render the game

		void drawScene(); // Render the Scene

		void LoadMesh(const char* mesh, Texture* tex, glm::vec3 pos);

		void shutdown(); // Shutdown the engine

		void setwindowtitle(const char* new_title); // Set window title

		void DebugFPS(const char* fps); // Debug function to calculate and display frames per second (FPS)

		void display_info_message(const char* msg); // Display an info message

		void PushMeshInstance(int x, int y, int z, int Scalex, int Scaley, int Scalez); // Add a instance to the scene

		int ResolutionX;
		int ResolutionY;
		int ResolutionMultiplier;

		SDL_GameController* joystick = nullptr;

		float xDir = 0;
		float yDir = 0;



		//Analog joystick dead zone
		const int JOYSTICK_DEAD_ZONE = 8000;

	private:

		SDL_Window* window; // SDL Window
		SDL_GLContext glContext; // OpenGL Context

		glm::vec3 dist;  // Distance vector for camera movement

		Camera* cam; // Camera

		Model* m; // Model

		Texture* tex;  // Texture
		Texture* NormTex;  // Texture

		ModelRenderer* mr; // Model Renderer

		SkyboxRenderer* skybox; // Skybox Renderer

		std::vector<MeshInstance> MeshList;  // List of mesh instances in the scene, each with a model and a renderer
		std::vector<Texture> TextureList;  // List of textures in the scene
		std::vector<InstancePosRotScale> instances; // Create a vector to store the position, rotation and scale of each instance

		InstancedRenderer* ir; // Renderer for instanced rendering of multiple objects with the same model but different transformations

		GUIRenderer* gr; // Renderer for GUI elements (e.g. text, images)	
		GUIText* FPSMsg; // GUI text object for displaying the frames per second (FPS) message on the screen
		GUIImage* HUDImage; // GUI image object for the heads-up display (HUD) on the screen

		CubeWorld* cubeWorld; // CubeWorld object for rendering a voxel-based world
	};



}