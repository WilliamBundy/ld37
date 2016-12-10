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

#include "ld_game.c"

float t = 0;
void update(GameHandle* game)
{
	//printf("err: %d\n", glGetError());
	render_start(game->renderer->groups);
	Sprite s;
	sprite_init(&s);
	s.pos = v2(cosf(t) * 100 + game->display_size.x/2, sinf(t) * 100 + game->display_size.y / 2);
	t += 0.05f;
	
	s.size = v2(256, 256);
	//s.size = v2(0.5f, 0.5f);

	s.angle = t * 2;
	s.texture = rect2(0, 0, 32, 31.5);
	s.color = create_color(1, 1, 1, 1);

	render_add(game->renderer->groups, &s);
	s.pos = v2(0, 0);
	//render_add(game->renderer->groups, &s);

	render_draw(game->renderer, game->renderer->groups, game->display_size, 1.0f);
}

#if 1
SDL_AudioDeviceID audio_device = 0;
sts_mixer_t mixer;

typedef struct AudioStream_
{
	stb_vorbis* vorbis;
	sts_mixer_stream_t stream;
	f32* data;
} AudioStream;

static void audio_callback(void* user, u8* stream, i32 len) 
{
	sts_mixer_mix_audio(&mixer, stream, len / (sizeof(f32)*2));
}

static void refill_stream(sts_mixer_sample_t* sample, void* user) 
{
	AudioStream* stream = user;
	stb_vorbis_get_samples_float_interleaved(stream->vorbis, sample->length, stream->data, 4096*2);
}
#endif

char fn_buf[4096];

int main(int argc, char** argv)
{
	GameSettings settings = {0};
	settings.window_title = "William's Ludum Dare 37 Entry";
	settings.window_size = v2i(1280, 720);
	settings.display_scale = 1.0f;
#ifdef WB_DEBUG
	settings.display_index = 1;
#else
	settings.display_index = 0;
#endif

	//loading zip archive
	mz_zip_archive zip = {0};
	char* base_path = SDL_GetBasePath();
	snprintf(fn_buf, 4096, "%s%s", base_path, "assets.zip");
	i32 result = mz_zip_reader_init_file(&zip, fn_buf, MZ_ZIP_FLAG_COMPRESSED_DATA);
	SDL_free(base_path);

	usize len = 0;
	char* vs = mz_zip_reader_extract_file_to_heap(&zip, "src/shaders/vert.glsl", &len, 0);
	vs[len-1] = '\0';
	settings.vert_shader = vs;

	len = 0;
	char* fs = mz_zip_reader_extract_file_to_heap(&zip, "src/shaders/frag.glsl", &len, 0);
	fs[len - 1] = '\0';
	settings.frag_shader = fs;

	len = 0;
	u8* passacaglia_ogg = mz_zip_reader_extract_file_to_heap(&zip, "assets/passacaglia.ogg", &len, 0);

	//game initializaiton
	GameHandle* game = game_init(&settings);
	if(game == NULL) return 1;

	//vorbis memory streaming setup
	MemoryArena* vorbis_arena = arena_bootstrap("VorbisArena", Megabytes(8));
	stb_vorbis_alloc vorbis_alloc;
	vorbis_alloc.alloc_buffer = arena_push(vorbis_arena, Megabytes(8) + 1);
	vorbis_alloc.alloc_buffer_length_in_bytes = Megabytes(8);

	i32 vorbis_err = 0;
	stb_vorbis* vorbis = stb_vorbis_open_memory(passacaglia_ogg, len, &vorbis_err, &vorbis_alloc);
	printf("vorbis %d %lx\n", vorbis_err, (isize)vorbis);

	SDL_AudioSpec want, have;
	//sts_mixer_sample_t sample;
	AudioStream stream;

	want.format = AUDIO_F32SYS;
	want.freq = 44100;
	want.channels = 2;
	want.userdata = NULL;
	want.samples = 4096;
	want.callback = audio_callback;
	audio_device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
	log_error("Was there a SDL_Error? %s", SDL_GetError());

	sts_mixer_init(&mixer, 44100, STS_MIXER_SAMPLE_FORMAT_FLOAT);

	stream.vorbis = vorbis;
	stb_vorbis_seek_start(vorbis);
	stb_vorbis_info vinfo = stb_vorbis_get_info(vorbis);
	stream.stream.userdata = &stream;
	stream.stream.callback = refill_stream;
	stream.stream.sample.frequency = vinfo.sample_rate;
	stream.stream.sample.audio_format = STS_MIXER_SAMPLE_FORMAT_FLOAT;
	stream.stream.sample.length = 4096 * 2;
	stream.data = malloc(sizeof(float) * 8192);
	stream.stream.sample.data = stream.data;
	refill_stream(&stream.stream.sample, &stream);
	sts_mixer_play_stream(&mixer, &stream.stream, 0.6f);
	SDL_PauseAudioDevice(audio_device, 0);
	log_error("Was there a SDL_Error? %s", SDL_GetError());

	//renderer texture setup
	u32 texture;
	i32 w, h;
	len = 0;
	u8* raw_image = mz_zip_reader_extract_file_to_heap(&zip, "assets/graphics.png", &len, 0);
	texture = ogl_load_texture_from_memory(raw_image, len,  &w, &h);
	sprite_renderer_set_texture(game->renderer, texture, w, h);

	//game mainloop
	game_start(game, &update);
	return 0;
}
