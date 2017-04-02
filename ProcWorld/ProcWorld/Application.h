#pragma once

#include "SDLWrapper.h"

class Application {
public:
	explicit Application(std::string appConfigFileName);
	Application(std::string appName, uint32_t windowWidth, uint32_t windowHeight);

	bool HasErrors(void) const { return m_initializedWithError; }

	void Run(void);

	~Application();

private:

	void InitSDL(void) const;
	void InitGLEW(void) const;

	void CreateAppWindow(std::string& windowName);

	void Update(float dt);
	void Render(void);

	mutable bool m_initializedWithError;

	uint32_t m_windowWidth;
	uint32_t m_windowHeight;

	sdl2::window_ptr_t m_applicationWindow;
	SDL_GLContext m_openglContext;
};
