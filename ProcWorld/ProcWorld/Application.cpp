#include "Application.h"
#include "Timer.h"

#define GLEW_STATIC
#include <GL/glew.h>
#include "AssetManager.h"
#include "OpenGLRenderer.h"
#include "Camera.h"
#include "DensityPass.h"
#include "GeometryPass.h"
#include <gtc/matrix_transform.inl>
#include "GPUNoise.h"
#include <iostream>
#include "Cube.h"

Application::Application(std::string appConfigFileName) 
	: m_initializedWithError(false)
	, m_windowWidth(0)
	, m_windowHeight(0)
	, m_applicationWindow(sdl2::make_window("Invis", 0, 0, 0, 0, SDL_WINDOW_HIDDEN)) 
{
	// TODO : Get application settings from config file
	std::string defaultAppName = "Application";
	InitSDL();
	CreateAppWindow(defaultAppName);
	InitGLEW();
}

Application::Application(std::string appName, uint32_t windowWidth, uint32_t windowHeight)
	: m_initializedWithError(false)
	, m_windowWidth(windowWidth)
	, m_windowHeight(windowHeight) 
	, m_applicationWindow(sdl2::make_window("Invis", 0, 0, 0, 0, SDL_WINDOW_HIDDEN))
{
	InitSDL();
	CreateAppWindow(appName);
	InitGLEW();
}

void Application::Run() {
	bool running = true;
	int32_t status = 0;
	Timer clock;

	clock.StartTimer();
	duration<float> lastFrame(0), currentFrame(0);

	OpenGLRenderer::ClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	Camera cam(glm::vec3(0, 0, -50), glm::vec3(0, 20, 100), glm::vec3(0, 1, 0), 
		45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

	AssetManager assMng;
	ShaderProgram& prg = assMng.AddShaderSet("Color");
	prg.AddShaders(GL_VERTEX_SHADER, "Color.vert", GL_FRAGMENT_SHADER, "Color.frag");
	prg.Link();

	ShaderProgram& cubePrg = assMng.AddShaderSet("CubeShader");
	cubePrg.AddShaders(GL_VERTEX_SHADER, "CubeParallax.vert", GL_FRAGMENT_SHADER, "CubeParallax.frag");
	cubePrg.Link();
	GLuint cubeVAO = Cube::CreateCubeVAO();

	ShaderProgram& density = assMng.AddShaderSet("Density");
	density.AddShaders(GL_VERTEX_SHADER, "build_density_tex.vert",
		GL_GEOMETRY_SHADER, "build_density_tex.geom",
		GL_FRAGMENT_SHADER, "build_density_tex.frag");

	density.Link();

	// INFO: setup 3DTetxure to render the density values to
	DensityPass densityPass(96, 96, 256);
	densityPass.CreateDensityTexture();

	GPUNoise noise;
	noise.GenerateNoiseTextures(32, assMng);
	noise.GeneratePerlinNoise();

	densityPass.FillDensityTexture(density, noise.m_noiseTextures[0]);
	
	GeometryPass geometryPass;
	geometryPass.SetupResources(assMng);
	geometryPass.GenerateGeometry(densityPass);

	// TODO: Refactor aset manager to use a PackedArray
	Texture rockTexture = assMng.LoadTexture("rock.jpg", "rock");
	Texture mossTexture = assMng.LoadTexture("crate.jpg.png", "moss");
	Texture sandTexture = assMng.LoadTexture("sand.jpg", "sand");

	// INFO: Load height maps
	Texture rockHeightMap = assMng.LoadTexture("rock_height.png", "rock_height_map");
	Texture mossHeightMap = assMng.LoadTexture("crate_height.png", "moss_height_map");
	Texture sandHeightMap = assMng.LoadTexture("sand_height.png", "sand_height_map");

	// INFO: Load normal maps
	Texture rockNormalMap = assMng.LoadTexture("rock_normals.png", "rock_normal_map");
	Texture mossNormalMap = assMng.LoadTexture("crate_normal.png", "moss_normal_map");
	Texture sandNormalMap = assMng.LoadTexture("sand_normals.png", "sand_normal_map");
	

	Texture diffuseCrate = assMng.LoadTexture("crate.jpg.png", "cube_diffuse");
	Texture heightCrate = assMng.LoadTexture("crate_height.png", "cube_height");
	Texture normalMapCrate = assMng.LoadTexture("crate_normals.png", "cube_normal");

	Texture diffuseWall = assMng.LoadTexture("wall.png", "wall_diffuse");
	Texture heightWall = assMng.LoadTexture("wall_height.png", "wall_height");
	Texture normalMapWall = assMng.LoadTexture("wall_normals.png", "wall_normal");

	Texture diffuseBricks = assMng.LoadTexture("bricks2.jpg", "brick_diffuse");
	Texture heightBricks = assMng.LoadTexture("bricks2_disp.jpg", "brick_height");
	Texture normalMapBricks = assMng.LoadTexture("bricks2_normal.jpg", "brick_normal");

	bool draw_wireframe = false;
	int initialSteps = 16;
	int refinementSteps = 8;
	bool tweakInitialSteps = true;

	duration<float> average_deltaTime;
	int fpsCounter = 0;
	while (running) {
		currentFrame = clock.GetElapsedTime();
		duration<float> dt = currentFrame - lastFrame;
		
		SDL_Event event;
		float deltaX = 0.0f;
		float deltaY = 0.0f;

		// Input Section
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					status = 1;
					break;

				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) {
						status = 1;
						break;
					}

					if (event.key.keysym.sym == SDLK_w) {
						cam.ActivateKey(MOVEMENT_DIRECTION::FORWARD);
					} 
					else if (event.key.keysym.sym == SDLK_s) {
						cam.ActivateKey(MOVEMENT_DIRECTION::BACKWARD);
					}
					else if (event.key.keysym.sym == SDLK_d) {
						cam.ActivateKey(MOVEMENT_DIRECTION::RIGHT);
					}
					else if (event.key.keysym.sym == SDLK_a) {
						cam.ActivateKey(MOVEMENT_DIRECTION::LEFT);
					}
					break;

				case SDL_KEYUP:
					if (event.key.keysym.sym == SDLK_w) {
						cam.DeactivateKey(MOVEMENT_DIRECTION::FORWARD);
					}
					else if (event.key.keysym.sym == SDLK_s) {
						cam.DeactivateKey(MOVEMENT_DIRECTION::BACKWARD);
					}
					else if (event.key.keysym.sym == SDLK_d) {
						cam.DeactivateKey(MOVEMENT_DIRECTION::RIGHT);
					}
					else if (event.key.keysym.sym == SDLK_a) {
						cam.DeactivateKey(MOVEMENT_DIRECTION::LEFT);
					}
					else if (event.key.keysym.sym == SDLK_l) {
						draw_wireframe = !draw_wireframe;
					}
					else if (event.key.keysym.sym == SDLK_p)
					{
						tweakInitialSteps = !tweakInitialSteps;
					}
					break;

				case SDL_MOUSEMOTION:
					deltaX += static_cast<float>(event.motion.x - 600);
					deltaY += static_cast<float>(event.motion.y - 400);
					cam.ProcessMotion(deltaX * dt.count(), deltaY * dt.count());
					SDL_WarpMouseInWindow(m_applicationWindow.get(), 600, 400);
					break;

				case SDL_MOUSEWHEEL:
					if (event.wheel.y >= 0)
					{
						tweakInitialSteps ? ++initialSteps : ++refinementSteps;
						initialSteps > 24 ? initialSteps = 24 : void(1);
						refinementSteps > 24 ? refinementSteps = 24 : void(1);

						std::cout << "Steps: " << initialSteps << " / " << refinementSteps << std::endl;
					}
					else
					{
						tweakInitialSteps ? --initialSteps : --refinementSteps;
						initialSteps < 1 ? initialSteps = 1 : void(1);
						refinementSteps < 1 ? refinementSteps = 1 : void(1);

						std::cout << "Steps: " << initialSteps << " / " << refinementSteps << std::endl;
					}

				default:
					break;
			}
		}
		
		// Check if assets have to be reloaded
		assMng.ReloadData();
		cam.ProcessMovement(dt.count());
		// Update & render section
		Update(dt.count());

		OpenGLRenderer::ViewPort(0, 0, m_windowWidth, m_windowHeight);
		OpenGLRenderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		// Render
		{
			draw_wireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			for (size_t i = 0; i < 16; ++i) {

				OpenGLRenderer::UseShader(prg.m_id);
				OpenGLRenderer::SetUniformMatrix4fv(prg.m_id, "view", cam.GetViewMat());
				OpenGLRenderer::SetUniformMatrix4fv(prg.m_id, "projection", cam.GetProjectionMat());
				OpenGLRenderer::SetUniformMatrix4fv(prg.m_id, "model", glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 40.0f, 40.0f)));
				OpenGLRenderer::SetUniformVector3(prg.m_id, "eyeWorldPosition", cam.GetPosition());

				GLuint attrLocation = glGetUniformLocation(prg.m_id, "initialSteps");
				glUniform1i(attrLocation, initialSteps);

				attrLocation = glGetUniformLocation(prg.m_id, "refinementSteps");
				glUniform1i(attrLocation, refinementSteps);

				// Bind textures
				// TODO: If pixel shader displ. mapping works, refactor this here
				// Maybe use some BindTextureToUnit() function
				GLuint texLoc = glGetUniformLocation(prg.m_id, "mossTexture");
				glUniform1i(texLoc, 0);

				texLoc = glGetUniformLocation(prg.m_id, "rockTexture");
				glUniform1i(texLoc, 1);

				texLoc = glGetUniformLocation(prg.m_id, "sandTexture");
				glUniform1i(texLoc, 2);

				texLoc = glGetUniformLocation(prg.m_id, "mossHeightMap");
				glUniform1i(texLoc, 3);

				texLoc = glGetUniformLocation(prg.m_id, "rockHeightMap");
				glUniform1i(texLoc, 4);

				texLoc = glGetUniformLocation(prg.m_id, "sandHeightMap");
				glUniform1i(texLoc, 5);

				texLoc = glGetUniformLocation(prg.m_id, "mossNormalMap");
				glUniform1i(texLoc, 6);

				texLoc = glGetUniformLocation(prg.m_id, "rockNormalMap");
				glUniform1i(texLoc, 7);

				texLoc = glGetUniformLocation(prg.m_id, "sandNormalMap");
				glUniform1i(texLoc, 8);
				
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mossTexture.textureID);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, rockTexture.textureID);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, sandTexture.textureID);

				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, mossHeightMap.textureID);

				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, rockHeightMap.textureID);

				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, sandHeightMap.textureID);

				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, mossNormalMap.textureID);

				glActiveTexture(GL_TEXTURE7);
				glBindTexture(GL_TEXTURE_2D, rockNormalMap.textureID);

				glActiveTexture(GL_TEXTURE8);
				glBindTexture(GL_TEXTURE_2D, sandNormalMap.textureID);

				OpenGLRenderer::BindVertexArray(geometryPass.m_sliceVAOs[i]);
				glDrawArraysInstanced(GL_TRIANGLES, 0, geometryPass.m_vericesPerSlice[i] * 6, 1);
				OpenGLRenderer::UnbindVertexArray();
			}
		}

		// INFO: Draw cube to test parallax
		{
			OpenGLRenderer::UseShader(cubePrg.m_id);
			OpenGLRenderer::SetUniformMatrix4fv(cubePrg.m_id, "view", cam.GetViewMat());
			OpenGLRenderer::SetUniformMatrix4fv(cubePrg.m_id, "projection", cam.GetProjectionMat());
			OpenGLRenderer::SetUniformMatrix4fv(cubePrg.m_id, "model", glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 10.0f, 10.0f)));
			OpenGLRenderer::SetUniformVector3(cubePrg.m_id, "eyePosition", cam.GetPosition());

			GLuint attrLocation = glGetUniformLocation(cubePrg.m_id, "initialSteps");
			glUniform1i(attrLocation, initialSteps);

			attrLocation = glGetUniformLocation(cubePrg.m_id, "refinementSteps");
			glUniform1i(attrLocation, refinementSteps);

			GLuint texLoc = glGetUniformLocation(cubePrg.m_id, "diffuse");
			glUniform1i(texLoc, 0);
			texLoc = glGetUniformLocation(cubePrg.m_id, "height");
			glUniform1i(texLoc, 1);
			texLoc = glGetUniformLocation(cubePrg.m_id, "normalMap");
			glUniform1i(texLoc, 2);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseCrate.textureID);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, heightCrate.textureID);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, normalMapCrate.textureID);

			Cube::DrawCube(cubeVAO);

			glm::mat4 model_second = glm::translate(glm::mat4(1.0f), glm::vec3(-20.0f, 0.0f, -10.0f)); 
			model_second = glm::rotate(model_second, glm::radians(-45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model_second = glm::scale(model_second, glm::vec3(10.0f, 10.0f, 10.0f));
			OpenGLRenderer::SetUniformMatrix4fv(cubePrg.m_id, "model", model_second);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseWall.textureID);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, heightWall.textureID);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, normalMapWall.textureID);

			Cube::DrawCube(cubeVAO);

			glm::mat4 model_third = glm::translate(glm::mat4(1.0f), glm::vec3(20.0f, 0.0f, -10.0f));
			model_third = glm::rotate(model_third, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model_third = glm::scale(model_third, glm::vec3(10.0f, 10.0f, 10.0f));
			OpenGLRenderer::SetUniformMatrix4fv(cubePrg.m_id, "model", model_third);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseBricks.textureID);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, heightBricks.textureID);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, normalMapBricks.textureID);

			Cube::DrawCube(cubeVAO);
		}

		SDL_GL_SwapWindow(m_applicationWindow.get());

		if (status == 1)
			running = false;

		// TODO: Move FPS counter away from here, UI display
		if (fpsCounter >= 2000) {
			std::string windowTitle("ProcWorld by Lukas Vogl (FPS: " + std::to_string(1.0f / (average_deltaTime.count() / 2000.0f)) + ")");
			SDL_SetWindowTitle(m_applicationWindow.get(), windowTitle.c_str());
			fpsCounter = 0;
			average_deltaTime = duration<float>::zero();
		} else {
			average_deltaTime += dt;
			++fpsCounter;
		}

		lastFrame = currentFrame;
	}
}

void Application::InitSDL(void) const {
	int32_t sdlErrorCode = 0;

	sdlErrorCode = SDL_Init(SDL_INIT_VIDEO);
	sdlErrorCode = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	sdlErrorCode = SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	sdlErrorCode = SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	sdlErrorCode = SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	m_initializedWithError = sdlErrorCode < 0;
};

void Application::InitGLEW(void) const {
	GLenum status = glewInit();
	m_initializedWithError = status != GLEW_OK;

	if (m_initializedWithError) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(status));
	}
}

void Application::CreateAppWindow(std::string& windowName) {
	m_applicationWindow = sdl2::make_window(windowName.c_str(),
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		m_windowWidth, m_windowHeight, SDL_WINDOW_OPENGL);

	m_initializedWithError = !m_applicationWindow;

	m_openglContext = SDL_GL_CreateContext(m_applicationWindow.get());

	SDL_GL_SetSwapInterval(0);

	uint32_t windowHalfWidth = static_cast<uint32_t>(m_windowWidth * 0.5f);
	uint32_t windowHalfHeight = static_cast<uint32_t>(m_windowHeight * 0.5f);
	SDL_WarpMouseInWindow(m_applicationWindow.get(), windowHalfWidth, windowHalfHeight);
	SDL_ShowCursor(SDL_DISABLE);
}

void Application::Update(float dt) {
	// TODO: Update stuff
}

void Application::Render(void) {

}

Application::~Application() {
	SDL_GL_DeleteContext(m_openglContext);
	SDL_Quit();
}
