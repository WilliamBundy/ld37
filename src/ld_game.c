

typedef enum GameState_
{
	GameState_None,
	GameState_Menu,
	GameState_Play
} GameState;


typedef struct GameSettings_
{
	string window_title;
	Vec2i window_size;
	f32 display_scale;
	i32 display_index;

	string vert_shader;
	string frag_shader;

	SDL_Window* window;

} GameSettings;

typedef struct GameHandle_
{
	GameSettings* settings;

	SDL_Window* window;
	Vec2i window_size;

	MemoryArena* game_arena;
	MemoryArena* render_arena;

	SpriteRenderer* renderer;
	Vec2 display_size;
	f32 scale;
	
	string base_path;
	string pref_path;
	
} GameHandle;

void game_update_screen(GameHandle* game)
{
	SDL_GetWindowSize(game->window, &game->window_size.x, &game->window_size.y);
	glViewport(0, 0, game->window_size.x, game->window_size.y);
	game->display_size = v2(game->window_size.x * game->scale, game->window_size.y * game->scale);
}

GameHandle* game_init(GameSettings* settings)
{
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		log_error("Error: failed to init SDL: %s", SDL_GetError());
		return NULL;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Window* window = SDL_CreateWindow(settings->window_title, 
			SDL_WINDOWPOS_CENTERED_DISPLAY(settings->display_index), 
			SDL_WINDOWPOS_CENTERED_DISPLAY(settings->display_index),
			settings->window_size.x, settings->window_size.y,
			SDL_WINDOW_OPENGL | 
			SDL_WINDOW_RESIZABLE |
			SDL_WINDOW_MOUSE_FOCUS |
			SDL_WINDOW_INPUT_FOCUS);

	if(window == NULL) {
		log_error("Error: could not create window: %s", SDL_GetError());
		return NULL;
	}
	settings->window = window;

	SDL_GLContext opengl_context = SDL_GL_CreateContext(window);
	if(!gladLoadGL()) {
		log_error("Error: could not load OpenGL functions");
		return NULL;
	}

	i32 error = SDL_GL_SetSwapInterval(1);
	if(error == -1) {
		error = SDL_GL_SetSwapInterval(1);
		if(error == -1) {
			error = SDL_GL_SetSwapInterval(0);
		}

	}


	MemoryArena* game_arena = arena_bootstrap("GameArena", Megabytes(1));
	GameHandle* game = arena_push(game_arena, sizeof(GameHandle));
	
	game->settings = settings;
	game->window = window;
	game->window_size = settings->window_size;

	game->scale = settings->display_scale;
	game->display_size.x = game->window_size.x * game->scale;
	game->display_size.y = game->window_size.y * game->scale;

	game->base_path = SDL_GetBasePath();
	game->pref_path = NULL;
	//game->pref_path = SDL_GetPrefPath("WilliamBundy", "LD37");

	game->game_arena = game_arena;
	game->render_arena = arena_bootstrap("RenderArena", Megabytes(256));
	
	game->renderer = arena_push(game->game_arena, sizeof(SpriteRenderer));
	sprite_renderer_init_groups(game->renderer, 8, 
			200000, 
			game->render_arena);
	sprite_renderer_init_gl(game->renderer, settings->vert_shader, settings->frag_shader);
	
	return game;
}

i32 game_start(GameHandle* game, void (*update)(GameHandle*))
{
	i32 running = 1;
	SDL_Event event;
	glClearColor(0, 0, 0, 1);
	while(running) {
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					running = false;
					break;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		game_update_screen(game);
		(*update)(game);

		SDL_GL_SwapWindow(game->window);
	}
	SDL_Quit();
	return 0;
}
