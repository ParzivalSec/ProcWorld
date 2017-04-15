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

	OpenGLRenderer::ClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	Camera cam(glm::vec3(0, 0, 0), glm::vec3(0, 0, -50), glm::vec3(0, 1, 0), 
		45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

	AssetManager assMng;
	ShaderProgram& prg = assMng.AddShaderSet("Test");
	prg.AddShaders(GL_VERTEX_SHADER, "Color.vert", GL_FRAGMENT_SHADER, "Color.frag");
	prg.Link();

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
					break;

				case SDL_MOUSEMOTION:
					deltaX += static_cast<float>(event.motion.x - 600);
					deltaY += static_cast<float>(event.motion.y - 400);
					cam.ProcessMotion(deltaX * dt.count(), deltaY * dt.count());
					SDL_WarpMouseInWindow(m_applicationWindow.get(), 600, 400);
					break;

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
			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			for (size_t i = 0; i < 16; ++i) {

				OpenGLRenderer::UseShader(prg.m_id);
				OpenGLRenderer::SetUniformMatrix4fv(prg.m_id, "view", cam.GetViewMat());
				OpenGLRenderer::SetUniformMatrix4fv(prg.m_id, "projection", cam.GetProjectionMat());
				OpenGLRenderer::SetUniformMatrix4fv(prg.m_id, "model", glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 40.0f, 40.0f)));

				OpenGLRenderer::BindVertexArray(geometryPass.m_sliceVAOs[i]);
				// OpenGLRenderer::Draw(GL_TRIANGLES, 0, 3);
				glDrawArraysInstanced(GL_TRIANGLES, 0, geometryPass.m_vericesPerSlice[i] * 6, 1);
				OpenGLRenderer::UnbindVertexArray();
			}
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
