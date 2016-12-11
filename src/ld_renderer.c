

typedef enum SpriteFlags_
{
	Anchor_Center = 0,
	Anchor_Top_Left = 1,
	Anchor_Top = 2,
	Anchor_Top_Right = 3,
	Anchor_Right = 4,
	Anchor_Bottom_Right = 5,
	Anchor_Bottom = 6 ,
	Anchor_Bottom_Left = 7,
	Anchor_Left = 8,
	SpriteFlag_FlipHoriz = Flag(4),
	SpriteFlag_FlipVert = Flag(5)
} SpriteFlags;

#define SpriteAnchorMask 0xF

const f32 SpriteAnchorX[] = {
	0.0f,
	-0.5f,
	0.0f,
	0.5f, 
	0.5f,
	0.5f, 
	0.0f, 
	-0.5f,
	-0.5f
};

const f32 SpriteAnchorY[] = {
	0.0f,
	-0.5f,
	-0.5f,
	-0.5f,
	0.0f,
	0.5f,
	0.5f,
	0.5f,
	0.0f
};

typedef struct Sprite_
{
	Vec2 pos;
	Vec2 size;
	Vec2 center;
	Rect2 texture;
	Color color;
	f32 angle;
	u32 flags;
} Sprite;

static inline 
void sprite_init(Sprite* s)
{
	s->pos = v2(0, 0);
	s->size = v2(16, 16);
	s->center = v2(0, 0);
	s->texture = rect2(0, 0, 1, 1);
	s->color = create_color(1, 1, 1, 1);
	s->angle = 0;
	s->flags = Anchor_Center;
}

typedef struct SpriteGroup_
{
	u32 texture;
	i32 texture_width;
	i32 texture_height;
	Vec2 offset;
	f32 ortho[16];

	Sprite* sprites;
	isize count;
	isize capacity;
} SpriteGroup;

void sprite_group_init(SpriteGroup* group, Sprite* sprites, isize capacity)
{
	group->sprites = sprites;
	group->capacity = capacity;
	group->count = 0;

	group->texture = 0;
	group->texture_width = 0;
	group->texture_height = 0;

	group->offset = v2(0, 0);
}

typedef struct SpriteRenderer_
{
	u32 shaders;
	u32 vbo;
	u32 vao;

	isize u_texture_size;
	isize u_ortho_matrix;
	isize u_scale;

	SpriteGroup* groups;
	isize group_count;
} SpriteRenderer;


void sprite_renderer_init_groups(SpriteRenderer* render, i32 count, i32 size, MemoryArena* arena)
{
	render->group_count = count;
	render->groups = arena_push(arena, sizeof(SpriteGroup) * count);
	for(isize i = 0; i < count; ++i) {
		sprite_group_init(render->groups + i, arena_push(arena, sizeof(Sprite) * size), size);
	}
}

void sprite_renderer_set_texture(SpriteRenderer* render, u32 texture, i32 width, i32 height)
{
	for(isize i = 0; i < render->group_count; ++i) {
		render->groups[i].texture = texture;
		render->groups[i].texture_width = width;
		render->groups[i].texture_height = height;
	}
}

void sprite_renderer_init_gl(SpriteRenderer* render, string vert_source, string frag_source)
{
	glGenVertexArrays(1, &render->vao);
	glBindVertexArray(render->vao);
	glGenBuffers(1, &render->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, render->vbo);

	usize stride = sizeof(Sprite);
	usize vertex_count = 1;

	isize array_index = 0;

	glVertexAttribPointer(array_index, 2, GL_FLOAT, GL_FALSE, stride, &((Sprite*)NULL)->pos);
	glEnableVertexAttribArray(array_index);
	glVertexAttribDivisor(array_index, vertex_count);
	array_index++;

	glVertexAttribPointer(array_index, 2, GL_FLOAT, GL_FALSE, stride, &((Sprite*)NULL)->size);
	glEnableVertexAttribArray(array_index);
	glVertexAttribDivisor(array_index, vertex_count);
	array_index++;

	glVertexAttribPointer(array_index, 2, GL_FLOAT, GL_FALSE, stride, &((Sprite*)NULL)->center);
	glEnableVertexAttribArray(array_index);
	glVertexAttribDivisor(array_index, vertex_count);
	array_index++;

	glVertexAttribPointer(array_index, 4, GL_FLOAT, GL_FALSE, stride, &((Sprite*)NULL)->texture);
	glEnableVertexAttribArray(array_index);
	glVertexAttribDivisor(array_index, vertex_count);
	array_index++;

	glVertexAttribPointer(array_index, 4, GL_FLOAT, GL_FALSE, stride, &((Sprite*)NULL)->color);
	glEnableVertexAttribArray(array_index);
	glVertexAttribDivisor(array_index, vertex_count);
	array_index++;

	glVertexAttribPointer(array_index, 1, GL_FLOAT, GL_FALSE, stride, &((Sprite*)NULL)->angle);
	glEnableVertexAttribArray(array_index);
	glVertexAttribDivisor(array_index, vertex_count);
	array_index++;

	glVertexAttribIPointer(array_index, 1, GL_UNSIGNED_INT, stride, &((Sprite*)NULL)->flags);
	glEnableVertexAttribArray(array_index);
	glVertexAttribDivisor(array_index, vertex_count);
	array_index++;

	glBindVertexArray(0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	u32 vert_shader = glCreateShader(GL_VERTEX_SHADER);
	{
		u32 shader = vert_shader;
		string src = vert_source;

		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);
		u32 success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		i32 log_size;
		char shader_log[4096];
		glGetShaderInfoLog(shader, 4096, &log_size, shader_log);
		if(!success) {
			log_error("Error: could not compile vertex shader\n\n%s\n\n", shader_log);
		} else {
			printf("Vertex shader compiled successfully \n");
		}
	}

	u32 frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	{
		u32 shader = frag_shader;
		string src = frag_source;

		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);
		u32 success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		i32 log_size;
		char shader_log[4096];
		glGetShaderInfoLog(shader, 4096, &log_size, shader_log);
		if(!success) {
			log_error("Error: could not compile fragment shader\n\n%s\n\n", shader_log);
		} else {
			printf("Frag shader compiled successfully \n");
		}
		
	}
	render->shaders = glCreateProgram();
	glAttachShader(render->shaders, vert_shader);
	glAttachShader(render->shaders, frag_shader);
	glLinkProgram(render->shaders);
	glUseProgram(render->shaders);
	glDeleteShader(vert_shader);
	glDeleteShader(frag_shader);

	render->u_texture_size = glGetUniformLocation(render->shaders, "u_texture_size");
	render->u_ortho_matrix = glGetUniformLocation(render->shaders, "u_ortho_matrix");
	render->u_scale = glGetUniformLocation(render->shaders, "u_scale");
}

void render_start(SpriteGroup* group)
{
	group->count = 0;
}

static inline
void render_add(SpriteGroup* group, const Sprite* sprite)
{
	Sprite s = *sprite;
	s.texture.pos.x /= (float)group->texture_width;
	s.texture.pos.y /= (float)group->texture_height;
	s.texture.size.x /= (float)group->texture_width;
	s.texture.size.y /= (float)group->texture_height;
	group->sprites[group->count++] = s;
}

void render_calculate_ortho_matrix(f32* ortho, Vec4 screen, float nearplane, float farplane)
{
	//v4 == x, y, z, w;
	//   == l, t, r, b
	ortho[0] = 2.0f / (screen.z - screen.x);
	ortho[1] = 0;
	ortho[2] = 0;
	ortho[3] = -1.0f * (screen.x + screen.z) / (screen.z - screen.x);

	ortho[4] = 0;
	ortho[5] = 2.0f / (screen.y - screen.w);
	ortho[6] = 0;
	ortho[7] = -1 * (screen.y + screen.w) / (screen.y - screen.w);

	ortho[8] = 0;
	ortho[9] = 0;
	ortho[10] = (-2.0f / (farplane - nearplane));
	ortho[11] = (-1.0f * (farplane + nearplane) / (farplane - nearplane));

	ortho[12] = 0;
	ortho[13] = 0;
	ortho[14] = 0;
	ortho[15] = 1.0f;
}

void render_draw(SpriteRenderer* r, SpriteGroup* group, Vec2 size, f32 scale)
{
	glUseProgram(r->shaders);
	//group->offset.x = roundf(group->offset.x);
	//group->offset.y = roundf(group->offset.y);

	glUniform1f(r->u_scale, scale);
	glUniform2f(r->u_texture_size,
		group->texture_width,
		group->texture_height);

#if 1
	Vec4 screen = v4(
		group->offset.x, group->offset.y, 
		size.x + group->offset.x,
		size.y + group->offset.y);
#else
	Vec4 screen = v4(0, 0, size.x, size.y);
#endif

	render_calculate_ortho_matrix(group->ortho, screen, 1, -1);
	glUniformMatrix4fv(r->u_ortho_matrix, 
		1, 
		GL_FALSE,
		group->ortho);

	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, group->texture);
	glBindVertexArray(r->vao);
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
	glBufferData(GL_ARRAY_BUFFER, 
			group->count * sizeof(Sprite), 
			group->sprites, 
			GL_STREAM_DRAW);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, group->count);

	glBindVertexArray(0);
}

GLuint ogl_add_texture(u8* data, isize w, isize h) 
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	u32 error = glGetError();
	if(error != 0) {
		printf("There was an error adding a texture: %d \n", error);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

GLuint ogl_load_texture(char* filename, isize* w_o, isize* h_o)
{
	int w, h, n;
	char file[4096];
	char* base_path = SDL_GetBasePath();
	isize len = snprintf(file, 4096, "%s%s", base_path, filename);
	u8* data = (u8*)stbi_load(file, &w, &h, &n, STBI_rgb_alpha);
	//TODO(will) do error checking
	GLuint texture = ogl_add_texture(data, w, h);
	if(texture == 0) {
		printf("There was an error loading %s \n", filename);
	}
	if(w_o != NULL) *w_o = w;
	if(h_o != NULL) *h_o = h;

	SDL_free(base_path);
	STBI_FREE(data);
	return texture;
}

GLuint ogl_load_texture_from_memory(u8* buf, i32 size, i32* x, i32* y)
{
	i32 w, h, n;
	u8* data = stbi_load_from_memory(buf, size, &w, &h, &n, STBI_rgb_alpha);
	GLuint texture = ogl_add_texture(data, w, h);
	if(texture == 0) {
		log_error("Error loading texture\n");
	}
	if(x) *x = w;
	if(y) *y = h;

	STBI_FREE(data);
	return texture;
} 

Sprite create_box_primitive(Vec2 pos, Vec2 size, Color color)
{
	Sprite s;
	sprite_init(&s);
	s.pos = pos;
	s.texture = rect2(0, 0, 16, 16);
	s.size = size;
	s.color = color;
	return s;
}

void render_box(SpriteGroup* group, Vec2 pos, Vec2 size, Color color)
{
	Sprite s = create_box_primitive(pos, size, color);
	render_add(group, &s);
}

Sprite create_line_primitive(Vec2 start, Vec2 end, Color color, i32 thickness)
{
	Vec2 dline;
	dline.x = end.x - start.x;
	dline.y = end.y - start.y;
	Sprite s;
	if(dline.y == 0) {
		if(dline.x < 0) {
			dline.x *= -1;
			Vec2 temp = end;
			end = start;
			start = temp;
		}
		Vec2 lstart = start;
		lstart.x += dline.x / 2.0f;
		s = create_box_primitive(lstart, v2(dline.x, thickness), color);
	} else if(dline.x == 0) {
		if(dline.y < 0) {
			dline.y *= -1;
			Vec2 temp = end;
			end = start;
			start = temp;
		}
		Vec2 lstart = start;
		lstart.y += dline.y / 2;
		s = create_box_primitive(lstart, v2(thickness, dline.y), color);
	} else {
		Vec2 lstart = start;
		f32 mag = v2_mag(&dline);
		start.x += dline.x / 2.0f + mag;
		start.y += dline.y / 2.0f + mag;

		s = create_box_primitive(lstart, v2(mag, thickness), color);
		f32 angle = atan2f(dline.y, dline.x);
		s.angle = -angle;
	}
	return s;
}
void render_line(SpriteGroup* group, Vec2 start, Vec2 end, Color color, i32 thickness)
{
	Sprite s = create_line_primitive(start, end, color, thickness);
	render_add(group, &s);
}
