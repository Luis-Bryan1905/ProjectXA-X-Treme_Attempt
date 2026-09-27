#include "GameEngine.h"
#include <iostream>
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_ttf.h>	
#include <fstream>

namespace GE
{

	GameEngine::GameEngine()
	{

	}

	GameEngine::~GameEngine()
	{

	}

	bool GameEngine::init()
	{


		bool VSync = true;

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) // If SDL2 fails to Initlise
		{
			std::cout << "could not initialise SDL!\n"; // Display error Message to console

			std::cout << SDL_GetError() << "\n"; // Display error Message to console

			return false;
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); // Set OpenGL version to 3
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3); // Set OpenGL version to 3.3
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY); // Set OpenGL profile to compatibility mode (allows use of deprecated features)
		ResolutionMultiplier = 3;
		ResolutionX = 320 * ResolutionMultiplier;
		ResolutionY = 240 * ResolutionMultiplier;

		window = SDL_CreateWindow("X-Treme Attempt", 150, 150, ResolutionX, ResolutionY, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

		if (window == nullptr)
		{
			std::cout << "could not create window!: " << SDL_GetError() << std::endl; // Display error Message to console

			std::cout << SDL_GetError() << "\n"; // Display error Message to console

			return false;
		}

		glContext = SDL_GL_CreateContext(window);

		if (glContext == nullptr)
		{
			std::cout << "could not create GL context!: " << SDL_GetError() << std::endl; // Display error Message to console

			std::cout << SDL_GetError() << "\n"; // Display error Message to console

			return false;
		}

		GLenum status = glewInit();

		if (status != GLEW_OK)
		{
			std::cout << "could not create GLEW!: " << SDL_GetError() << std::endl; // Display error Message to console

			return false;
		}

		if (VSync == true)
		{
			if (SDL_GL_SetSwapInterval(1) != 0)
			{
				std::cout << "could not set VSync: " << SDL_GetError() << std::endl; // Display error Message to console

				return false;
			}
		}


		dist = glm::vec3(0.0f, 0.0f, -100.0f); // Set initial distance for camera to look at the scene from a distance

		cam = new Camera
		(
			glm::vec3(5.0f, 2.0f, 7.75f), // Camera position
			glm::vec3(0.0f, 0.0f, 0.0f) + dist,  // Camera Look at
			glm::vec3(0.0f, 1.0f, 0.0f), // Camera Up direction
			120.0f, (float)ResolutionX / (float)ResolutionY, 0.1f, 10000.0f // Camera FOV, aspect ratio, near clip, far clip
		); 
		
		mr = new ModelRenderer();
		mr->init();
		mr->setPos(0.0f, 0.0f, 0.0f);
		mr->setRotation(0.0f, 0.0f, 0.0f);
		
		ir = new InstancedRenderer(); // Create an instanced renderer for rendering multiple instances of the same model efficiently
		Texture* BALL01 = new Texture(".//Assets//PACKAGEX//PCX//SYSTEM//Sonic//BALL01.png");
		ir->setTexture(BALL01); // Set the texture of the instanced renderer to the tree texture
		ir->init(); // Initialize the instanced renderer
		m = new Model();
		bool result = m->loadFromFile(".//scene_cube.obj");
		if (!result) // If the model failed to load
		{
			std::cout << "Failed to load model!" << std::endl; // Display error Message to console
			return false;
		}

		cubeWorld = new CubeWorld(this); // Create a cube world with the same resolution as the window

		//WIP: find way to allow user to open PNG file and use it for cube world layout?
		//cubeWorld->init(".//Assets/Z17/sonic/JADE2.PCX/JADE2-4a.png"); // Initialize the cube world
		cubeWorld->init(".//Assets/PACKAGEX/DEF/JADE1.DEF"); // Initialize the cube world

		ir->setInstanceData(instances); // Set the instance data for the instanced renderer

		xDir = 0; // Initialize joystick Y direction variables to 0
		yDir = 0; // Initialize joystick X direction variables to 0

		if (SDL_NumJoysticks() > 0) // If there is at least one joystick connected
		{
			for (int i = 0; i < SDL_NumJoysticks(); i++) // Loop through all joysticks
			{
				if (SDL_IsGameController(i)) // If the joystick is a game controller
				{
					joystick = SDL_GameControllerOpen(i); // Open the game controller
					if (joystick) // If the game controller was successfully opened
					{
						SDL_Log("Game controller connected");
						break;
					}
				}
			}
		}

		gr = new GUIRenderer(); // Create a renderer for GUI elements (e.g. text, images)
		gr->init(ResolutionX / ResolutionMultiplier, ResolutionY / ResolutionMultiplier); // Initialize the GUI renderer with the window resolution
		FPSMsg = new GUIText(0, 0, "FPS:", ".//Assets//Font//DebugFont.ttf"); // Create a GUI text object for displaying the frames per second (FPS) message on the screen

		std::cout << "Engine Initalised: " << std::endl;

		return true;

	}

	void GameEngine::PushMeshInstance(int x, int y, int z, int Scalex, int Scaley, int Scalez) // Add a instance to the scene
	{
		instances.push_back // Add a new instance to the vector with the specified position, rotation and scale
		({

			0.0f + (x), // X position of the instance (all instances will be at the same X position)
			0.0f + (y), // Y position of the tree instance (all instances will be at the same Y position)
			0.0f + (z), // Z position of the tree instance (all instances will be at the same Z position)
			0.0f, // Rotation around X axis (all instances will have the same rotation)
			0.0f, // Rotation around Y axis (all instances will have the same rotation)
			0.0f, // Rotation around Z axis (all instances will have the same rotation)
			1.0f + (Scalex), // Scale on X axis (all instances will be scaled to half their original size)
			1.0f + (Scaley), // Scale on Y axis (all instances will be scaled to half their original size)
			1.0f + (Scalez) // Scale on Z axis (all instances will be scaled to half their original size)

		});
	}

	void GameEngine::LoadMesh(const char* mesh, Texture* tex, glm::vec3 pos) // Load a mesh from file and create a model and renderer for it, then add it to the list of meshes in the scene
	{
		Model* newModel = new Model(); // Create a new model object

		if (!newModel->loadFromFile(mesh)) // If the model failed to load
		{
			std::cout << "Failed to load: " << mesh << std::endl;
			delete newModel;
			return;
		}

		ModelRenderer* newRenderer = new ModelRenderer(); // Create a new model renderer for the model
		newRenderer->init(); // Initialize the model renderer
		newRenderer->setTexture(tex); // Set the texture of the model renderer to the provided texture
		newRenderer->setPos(pos.x, pos.y, pos.z); // Set the position of the model renderer to the provided position

		MeshList.push_back({ newModel, newRenderer }); // Add the new model and renderer as a mesh instance to the list of meshes in the scene
		SDL_Log("Loaded Mesh: %s", mesh); // Log the loaded mesh to the console
	}

	bool GameEngine::keep_running() // Check if the engine should keep running based on user input (e.g. pressing the escape key or closing the window)
	{

		SDL_PumpEvents(); // Update SDL's internal event state

		SDL_Event evt; 

		while (SDL_PollEvent(&evt)) // Poll for events and process them
		{
			if (evt.type == SDL_KEYDOWN) // If a key is pressed down
			{

				if (evt.key.keysym.scancode == SDL_SCANCODE_ESCAPE) // If the escape key is pressed
				{
					return false; // Signal to stop running the engine
				}
			}

			if (evt.type == SDL_QUIT) // If the window is closed
			{
				return false; // Signal to stop running the engine
			}


		}

		return true; // Signal to keep running the engine
	}

	void GameEngine::processInput() // Process user input for camera movement and other interactions
	{

		const float camSpeed = 0.3f; // Camera movement speed
		const float mouseSens = 0.1f; // Mouse sensitivity for camera rotation

		int mouseX, mouseY = 0; // Variables to store the current mouse position

		SDL_GetMouseState(&mouseX, &mouseY); // Get the current mouse position relative to the window

		int diffx = mouseX - (ResolutionX / 2); // Calculate the difference in X position from the center of the screen (for camera rotation)
		int diffy = (ResolutionY / 2) - mouseY; // Invert Y axis for more intuitive camera control (moving mouse up should look up)

		glm::vec3 direction; // Calculate the new direction vector for the camera based on the updated yaw and pitch angles using spherical coordinates conversion
		direction.x = cos(glm::radians(cam->getYaw())) * cos(glm::radians(cam->getPitch())); // Calculate the X component of the direction vector based on yaw and pitch
		direction.y = sin(glm::radians(cam->getPitch())); // Calculate the Y component of the direction vector based on pitch
		direction.z = sin(glm::radians(cam->getYaw())) * cos(glm::radians(cam->getPitch())); // Calculate the Z component of the direction vector based on yaw and pitch

		cam->setTarget(glm::normalize(direction)); // Set the camera's target direction to the normalized direction vector calculated from the yaw and pitch angles

		
		SDL_PumpEvents(); // Ensure SDL's internal state is updated before querying keyboard state.
		const Uint8* keyboard = SDL_GetKeyboardState(NULL);

		if (keyboard[SDL_SCANCODE_W] || SDL_GameControllerGetButton(joystick, SDL_CONTROLLER_BUTTON_DPAD_UP))
		{
			cam->setPos(cam->getPos() + cam->getTarget() * camSpeed); // Move the camera forward in the direction it is facing
		}

		if (keyboard[SDL_SCANCODE_S] || SDL_GameControllerGetButton(joystick, SDL_CONTROLLER_BUTTON_DPAD_DOWN))
		{
			cam->setPos(cam->getPos() - cam->getTarget() * camSpeed); // Move the camera backward in the opposite direction it is facing
		}

		if (keyboard[SDL_SCANCODE_A] || SDL_GameControllerGetButton(joystick, SDL_CONTROLLER_BUTTON_DPAD_LEFT))
		{
			cam->setPos(cam->getPos() - glm::normalize(glm::cross(cam->getTarget(), cam->getUpDir())) * camSpeed); // Move the camera left by calculating the right vector using the cross product of the camera's target and up direction, and moving in the opposite direction
		}

		if (keyboard[SDL_SCANCODE_D] || SDL_GameControllerGetButton(joystick, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) )
		{
			cam->setPos(cam->getPos() + glm::normalize(glm::cross(cam->getTarget(), cam->getUpDir())) * camSpeed); // Move the camera right by calculating the right vector using the cross product of the camera's target and up direction, and moving in that direction
		}

		if (keyboard[SDL_SCANCODE_Q] || SDL_GameControllerGetButton(joystick, SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
		{
			cam->setPos(cam->getPos() + cam->getUpDir() * camSpeed); // Move the camera up in the direction of its up vector#
		}

		if (keyboard[SDL_SCANCODE_E] || SDL_GameControllerGetButton(joystick, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
		{
			cam->setPos(cam->getPos() - cam->getUpDir() * camSpeed); // Move the camera down in the opposite direction of its up vector
		}

		if (keyboard[SDL_SCANCODE_F])
		{
			if (mr->isFisheyeActive())
			{
				mr->DeactivateFisheye(); // Activate the fisheye effect on the model renderer
				mr->init(); // Reinitialize the model renderer to apply the fisheye effect
			}
			else
			{
				mr->ActivateFisheye(); // Deactivate the fisheye effect on the model renderer
				mr->init(); // Reinitialize the model renderer to apply the fisheye effect
			}

		}

		if (SDL_GameControllerGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX))
		{
			xDir = SDL_GameControllerGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f; // Normalize to -1.0 to 1.0
			
			if (abs(SDL_GameControllerGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTX)) < JOYSTICK_DEAD_ZONE) // if inside of dead zone
			{
				xDir = 0; // no movement
			}
			cam->setPos(cam->getPos() + glm::normalize(glm::cross(cam->getTarget(), cam->getUpDir())) * xDir * camSpeed);

		}

		if (SDL_GameControllerGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY))
		{
			yDir = SDL_GameControllerGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f; // Normalize to -1.0 to 1.0

			if (abs(SDL_GameControllerGetAxis(joystick, SDL_CONTROLLER_AXIS_LEFTY)) < JOYSTICK_DEAD_ZONE) // if inside of dead zone
			{
				yDir = 0; // no movement
			}
			cam->setPos(cam->getPos() + cam->getTarget() * -yDir * camSpeed);


		}


		cam->updateCamMatrices();
	}

	void GameEngine::update()
	{

	}

	void GameEngine::drawScene()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set the clear background to a color (RGBA)
		glEnable(GL_DEPTH_TEST); // Enable depth testing to ensure proper rendering of 3D objects based on their distance from the camera
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the color buffer and depth buffer to prepare for rendering the new frame

		ir->drawInstanced(cam, m); // Draw the instanced objects (e.g. multiple instances) in the scene

	}

	void GameEngine::draw()
	{
		drawScene(); // Draw the scene

		gr->drawText(FPSMsg); // Draw the FPS message on the screen using the GUI renderer

		SDL_GL_SwapWindow(window); // Swap the window buffers to display the rendered frame on the screen
	}

	void GameEngine::shutdown() // Clean up and free resources before shutting down the engine
	{

		if (mr != nullptr)
		{
			mr->destroy();
			delete mr;
		}

		if (skybox != nullptr)
		{
			skybox->destroy();
			delete skybox;
		}

		if (gr != nullptr)
		{
			delete gr;
			delete FPSMsg;
			delete HUDImage;
		}

		for (auto& m : MeshList)
		{
			delete m.model;
			delete m.renderer;
		}
		
		if (ir != nullptr)
		{
			delete ir;
		}

		if (cubeWorld != nullptr)
		{
			delete cubeWorld;
		}

		MeshList.clear();

		delete cam;

		SDL_DestroyWindow(window);

		window = nullptr;

		SDL_Quit();
	}

	void GameEngine::setwindowtitle(const char* new_title) // Set the title of the window to the provided string
	{
		SDL_SetWindowTitle(window, new_title);
	}

	void GameEngine::display_info_message(const char* msg) // Display an informational message box with the provided message
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "X-Treme Attempt", msg, nullptr); // Display a simple message box with the title "Basic Game Engine" and the provided message, with an information icon
	}

	void GameEngine::DebugFPS(const char* fps) // Update the FPS message text with the provided FPS string for debugging purposes
	{

		FPSMsg->setText(fps); // Set the text of the FPS message to the provided string
	}
}

