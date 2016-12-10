
SDL_AudioDeviceID audio_device = 0;
sts_mixer_t mixer;

short* decoded;
u8* audio_pos;

#define USE_VORBIS

typedef f32 AudioType;
int AudioForSDL = AUDIO_F32SYS;
int AudioForSTS = STS_MIXER_SAMPLE_FORMAT_FLOAT;

typedef struct AudioStream_
{
	stb_vorbis* vorbis;
	sts_mixer_stream_t stream;
	AudioType* data;
} AudioStream;

static void audio_callback(void* user, u8* stream, i32 len) 
{
	sts_mixer_mix_audio(&mixer, stream, len / (sizeof(AudioType) * 2));
}

static void refill_stream(sts_mixer_sample_t* sample, void* user) 
{
	AudioStream* stream = user;
	//if (drflac_read_s32(stream->flac, sample->length, stream->data) < sample->length) drflac_seek_to_sample(stream->flac, 0);
	stb_vorbis_get_samples_float_interleaved(stream->vorbis, 2, stream->data, sample->length); 
}

void init_audio()
{
// Audio code
/*
	SDL_AudioSpec want, have;

	want.format = AudioForSDL;
	want.freq = 44100;
	want.channels = 2;
	want.userdata = NULL;
	want.samples = 4096;
	want.callback = audio_callback;
	audio_device = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);

	sts_mixer_init(&mixer, 44100, AudioForSTS);

	AudioStream stream;
	stb_vorbis_alloc stbvalloc;
	stbvalloc.alloc_buffer = malloc(Megabytes(8)+64);
	stbvalloc.alloc_buffer_length_in_bytes = Megabytes(8);

	int verror = 0;
	stream.vorbis = stb_vorbis_open_memory(passacaglia, len, &verror, &stbvalloc);
	stb_vorbis_info vinfo = stb_vorbis_get_info(stream.vorbis);
	stream.stream.sample.frequency = vinfo.sample_rate;

	stream.stream.userdata = &stream;
	stream.stream.callback = refill_stream;
	stream.stream.sample.audio_format = AudioForSTS;
	stream.stream.sample.length = 4096 * 2;
	stream.data = malloc(sizeof(AudioType) * 4096*2);
	stream.stream.sample.data = stream.data;

	refill_stream(&stream.stream.sample, &stream);

	sts_mixer_play_stream(&mixer, &stream.stream, 0.9f);
	SDL_PauseAudioDevice(audio_device, 0);
*/


}
