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

Rect2 room_bg_texture = {{128, 0}, {640, 360}};


typedef enum RoomObjectKind_
{
	RoomObject_Nothing = 0,
	RoomObject_Bin,
	RoomObject_Bookshelf,
	RoomObject_BrokenTV,
	RoomObject_Closet,
	RoomObject_Bed,
	RoomObject_Sofa,
	RoomObject_WoodStove,
	RoomObject_Table,
	RoomObject_Tree,
	RoomObjectKind_Count
} RoomObjectKind;

typedef struct RoomObject_
{
	RoomObjectKind kind;
	Sprite sprite;
	Vec2i gridpos;
} RoomObject;

RoomObject* objects_in_room;
#define RoomObjectGridWidth 4
#define RoomObjectGridHeight 3
#define RoomObjectGridSize RoomObjectGridWidth * RoomObjectGridHeight
#define RoomObjectCellX 280
#define RoomObjectCellY 180
#define StartingX 4
#define StartingY 1

void init_room_objects(GameHandle* game, u64 seed)
{
	//__debugbreak();
	objects_in_room = arena_push(game->play_arena, sizeof(RoomObject) * RoomObjectGridSize);
	RandomState random;
	RandomState* r = &random;
	randomstate_init(r, seed);
	rand_xoroshift(r);

	for(isize i = 0; i < RoomObjectGridSize; ++i) {
		RoomObject* thing = objects_in_room + i;
		thing->kind = RoomObject_Nothing;
		isize x = i % RoomObjectGridWidth;
		isize y = (i - x) / RoomObjectGridWidth;
		thing->gridpos = v2i(x, y);
	}
	
	for(isize i = 1; i < RoomObjectKind_Count; ++i) {
		RoomObject* thing = NULL;
		usize index = 0; 
		do {
			index = rand_range_int(r, 0, RoomObjectGridSize);
			if(objects_in_room[index].kind == RoomObject_Nothing) {
				thing = objects_in_room + index;
			}

		} while(!thing);
		isize x = index % RoomObjectGridWidth;
		isize y = (index - x) / RoomObjectGridWidth;
		sprite_init(&thing->sprite);
		thing->kind = i;
		thing->sprite.size = v2(256, 256);
		thing->sprite.texture = rect2(0, 16 + (-1 + thing->kind) * 128, 126, 126);
		thing->sprite.pos = v2(RoomObjectCellX * x + 64, RoomObjectCellY * y + 32);
		thing->sprite.flags = Anchor_Top_Left;
	}
}

typedef struct PathNode_
{
	i32 is_path_start;
	Vec2i start, end;
	RoomObject* thing_start;
	RoomObject* thing_end;
	i32 scavenge_at_end;
	i32 cost;
} PathNode;

PathNode nodes[1024];
i32 node_count;


void node_init(PathNode* node)
{
	node->is_path_start = 0;
	node->start = v2i(0, 0);;
	node->end = v2i(0, 0);
	node->thing_start = NULL;
	node->thing_end = NULL;
	node->scavenge_at_end = 0;
	node->cost = 0;
}

void clear_path()
{
	node_count = 1;
	nodes[0].is_path_start = true;
	nodes[0].start = v2i(StartingX, StartingY);
}




int last_mouse_state = 0;

void update(GameHandle* game)
{
	//printf("err: %d\n", glGetError());
	render_start(game->current_group);


	Sprite s;
	sprite_init(&s);

	int mx, my;
	int btn = SDL_GetMouseState(&mx, &my) & SDL_BUTTON(SDL_BUTTON_LEFT);
	int just_pressed = btn && btn != last_mouse_state;
	//printf("%d %d\n", mx, my);

	s.pos = v2(0, 0);
	s.texture = room_bg_texture;
	s.size = v2(1280, 720);
	s.flags = Anchor_Top_Left; 
	render_add(game->current_group, &s);

	int tx = mx - 64;
	tx /= RoomObjectCellX;
	int ty = my - 32;
	ty /= RoomObjectCellY;

	int mi = ty * RoomObjectGridWidth + tx;
	if(mx < 64 || mx > 1280 - 64) {
		mi = -1;
	}
	
	if(my < 32 || my > 720 - 32) {
		mi = -1;
	}


	for(isize i = 0; i < RoomObjectGridSize; ++i) {
		RoomObject* thing = objects_in_room + i;
		s = thing->sprite;
		if(i == mi) {
			s.pos.x += s.size.x / 2;
			s.pos.y += s.size.y / 2;
			s.size = v2_scale(&s.size, 1.2);
			s.flags = Anchor_Center;

			if(just_pressed) {
				PathNode* current_node = nodes + (node_count-1);
				i32 dx = (current_node->start.x - thing->gridpos.x);
				i32 dy = (current_node->start.y - thing->gridpos.y);
				printf("%d %d\n", dx, dy);

				dx = abs(dx);
				dy = abs(dy);
				
				if((dx * dy == 0) && dx <= 1 && dy <= 1) {
					current_node->end = thing->gridpos;
					current_node->thing_end = thing;
					node_count++;
					current_node = nodes + node_count-1;
					node_init(current_node);
					current_node->start = thing->gridpos;
					current_node->thing_start = thing;
				}
			}
			
		}
		render_add(game->current_group, &s);
	}
	
	i32 xoffset = 64 + 128;
	i32 yoffset = 32 + 128;
	sprite_init(&s);
	s.pos = v2(1280 - 48, StartingY * RoomObjectCellY + yoffset );
	s.size = v2(32, 32);
	s.texture = rect2(0, 0, 16, 16);
	s.color = create_color(0, 0, 0, 1);
	render_add(game->current_group, &s);

	sprite_init(&s);
	s.pos = v2(-100, -100);
	s.texture = rect2(2, 2, 14, 14);
	s.size = v2(2280, 1720);
	s.flags = Anchor_Top_Left; 
	s.color = create_color(0, 0, 0, 0.5);
	s.flags = Anchor_Top_Left; 
	render_add(game->current_group, &s);
	

	if(game->keys[SDL_SCANCODE_LCTRL] >= Button_Pressed && game->keys[SDL_SCANCODE_Z] == Button_JustPressed) {
		node_count--;
		nodes[node_count-1].thing_end = NULL;
		if(node_count < 1) {
			clear_path();
		}
	}



	for(isize i = 0; i < node_count; ++i) {
		PathNode* node = nodes + i;
		Vec2 start;
		start.x = node->start.x * RoomObjectCellX + xoffset;
		start.y = node->start.y * RoomObjectCellY + yoffset;
		if(node->is_path_start) {
			start.x = 1280 - 48;
		}
		if(node->thing_end != NULL) {
			Vec2 end;
			end.x = node->end.x * RoomObjectCellX + xoffset;
			end.y = node->end.y * RoomObjectCellY + yoffset;
			render_line(game->current_group, start, end, create_color(1, 1, 1, 0.2), 8);
		}

		sprite_init(&s);
		s.texture = rect2(128 + 640 + 16 + 64 + 64, 0, 32, 32);
		if(i == node_count - 1) {
			s.texture.pos.x += 32;
		}
		s.pos = start;
		s.size = v2(32, 32);
		render_add(game->current_group, &s);
	}




	game_set_scale(game, 1.0);
	render_draw(game->renderer, game->current_group, game->display_size, game->scale);
	last_mouse_state = btn;
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


	init_room_objects(game, 2);
	clear_path();


	game_start(game, &update);
	return 0;
}
