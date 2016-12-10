#define _CRT_SECURE_NO_WARNINGS

#include <SDL2/SDL.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef WB_DEBUG
#define wb_assert(condition, msg, ...) do { \
	if(!(condition)) { \
		log_error(msg, __VA_ARGS__); \
		__debugbreak(); \
	} \
} while(0)
#else 
#define wb_assert(condition, msg, ...)
#endif

#define log_error(fmt, ...) do { \
	char buf[4096]; \
	snprintf(buf, 4096, fmt, __VA_ARGS__); \
	fprintf(stderr, "%s \n", buf); \
} while(0)

#include "thirdparty.h"
#include "ld_platform.h"

#ifdef WB_WINDOWS
#include "ld_win32.c"
#endif

#ifdef WB_LINUX
#include "ld_linux.c"
#endif

#include "ld_math.c"
#include "ld_memory.c"
#include "ld_random.c"

#include "ld_renderer.c"
#include "ld_audio.c"

#include "ld_game.c"

Rect2 room_bg_texture = {{0, 128}, {1280, 720}};



int flag = 0;
void update(GameHandle* game)
{
	//printf("err: %d\n", glGetError());
	render_start(game->current_group);


	Sprite s;
	sprite_init(&s);

	int mx, my;
	int btn = SDL_GetMouseState(&mx, &my);
	mx /= game->scale;
	my /= game->scale;
	//printf("%d %d\n", mx, my);

	s.pos = v2(1280, 720);
	s.texture = rect2(128, 0, 1280, 720);
	s.size = v2(1280, 720);
	s.size.x *= 2;
	s.size.y *= 2;
	printf("%f %f\n", s.size.x, s.size.y);
	//s.center = v2(16, 0);
	s.color = create_color(1, 1,1 ,1);
	if(btn) {
		flag++;
		if(flag > Anchor_Left) flag = flag % 9;
	}

	//s.flags = flag | SpriteFlag_FlipHoriz | SpriteFlag_FlipVert;
	//printf("%d\n", s.flags & 0xf);
	//s.angle = 1;
	render_add(game->current_group, &s);

	game_set_scale(game, 1.0);
	render_draw(game->renderer, game->current_group, game->display_size, game->scale);
}




int main(int argc, char** argv)
{
	GameSettings settings = {0};
	settings.window_title = "William's Ludum Dare 37 Entry";
	settings.window_size = v2i(1280, 720);
	settings.display_scale = 1.0f;
	settings.archive_name = "assets.zip";
	settings.vert_shader = "src/shaders/vert.glsl";
	settings.frag_shader = "src/shaders/frag.glsl";
	settings.texture_file = "assets/graphics.png";
#ifdef WB_DEBUG
	settings.display_index = 1;
#else
	settings.display_index = 0;
#endif

	//game initializaiton
	GameHandle* game = game_init(&settings);
	if(game == NULL) return 1;

	game_start(game, &update);
	return 0;
}
