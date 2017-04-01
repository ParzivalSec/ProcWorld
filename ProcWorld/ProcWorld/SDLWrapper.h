#pragma once
#include <memory>
#include <SDL.h>
#include <system_error>

template<typename Creator, typename Destructor, typename... Arguments>
auto make_resource(Creator c, Destructor d, Arguments&&... args) {
	auto r = c(std::forward<Arguments>(args)...);

	if (!r) { throw std::system_error(errno, std::generic_category()); }

	// return a unique_ptr for the created resource
	// Using decltype because types are needed and because r and d were
	// already declared decltype returns their type
	return std::unique_ptr<std::decay_t<decltype(*r)>, decltype(d)>(r, d);
}

namespace sdl2 {
	using window_ptr_t = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;

	/// make_resource specialization for SDL_Window as unique_ptr
	inline window_ptr_t make_window(const char* title, int x, int y, int w, int h, Uint32 flags) {
		return make_resource(SDL_CreateWindow, SDL_DestroyWindow, title, x, y, w, h, flags);
	}
}
