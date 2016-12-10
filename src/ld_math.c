typedef struct Vec2_
{
	f32 x;
	f32 y;
} Vec2;

static inline
Vec2 v2(f32 x, f32 y)
{
	Vec2 v;
	v.x = x;
	v.y = y;
	return v;
}

static inline
Vec2 v2_add(const Vec2* a, const Vec2* b)
{
	Vec2 v;
	v.x = a->x + b->x;
	v.y = a->y + b->y;
	return v;
}

static inline
Vec2 v2_add_scaled(const Vec2* a, const Vec2* b, f32 f)
{
	Vec2 v;
	v.x = a->x + b->x * f;
	v.y = a->y + b->y * f;
	return v;
}

static inline
Vec2 v2_negate(const Vec2* a)
{
	Vec2 v;
	v.x = -a->x;
	v.y = -a->y;
	return v;
}

static inline
Vec2 v2_sub(const Vec2* a, const Vec2* b)
{
	Vec2 v;
	v.x = a->x - b->x;
	v.y = a->y - b->y;
	return v;
}

static inline
Vec2 v2_scale(const Vec2* a, f32 f)
{
	Vec2 v;
	v.x = a->x * f;
	v.y = a->y * f;
	return v;
}

static inline
Vec2 v2_mul(const Vec2* a, const Vec2* b)
{
	Vec2 v;
	v.x = a->x * b->x;
	v.y = a->y * b->y;
	return v;
}

static inline
f32 v2_mag2(const Vec2* a)
{
	return a->x * a->x + a->y * a->y;
}

static inline
f32 v2_mag(const Vec2* a)
{
	return sqrtf(v2_mag2(a));
}

static inline
f32 v2_dot(const Vec2* a, const Vec2* b)
{
	return a->x * b->x + a->y * b->y;
}

static inline
f32 v2_cross(const Vec2* a, const Vec2* b)
{
	return a->x * b->y - a->y * b->x;
}

static inline
Vec2 v2_normalize(const Vec2* a)
{
	Vec2 v;
	f32 mag = v2_mag(a);
	v.x = a->x / mag;
	v.y = a->y / mag;
	return v;
}

static inline
Vec2 v2_from_angle(f32 angle, f32 mag)
{
	Vec2 v;
	v.x = cosf(angle) * mag;
	v.y = sinf(angle) * mag;
	return v;
}

static inline
f32 v2_to_angle(const Vec2* a)
{
	return atan2f(a->y, a->x);
}

static inline 
Vec2 v2_perpendicular(const Vec2* a)
{
	Vec2 v;
	v.x = -a->y;
	v.y = a->x;
	return v;
}


/* "ip" or "In-Place" functions modify the first parameter */

static inline
void v2_add_ip(Vec2* a, const Vec2* b)
{
	a->x += b->x;
	a->y += b->y;
}

static inline
void v2_add_scaled_ip(Vec2* a, const Vec2* b, f32 f)
{
	a->x += b->x * f;
	a->y += b->y * f;
}

static inline
void v2_negate_ip(Vec2* a)
{
	a->x *= -1;
	a->y *= -1;
}

static inline
void v2_sub_ip(Vec2* a, const Vec2* b)
{
	a->x -= b->x;
	a->y -= b->y;
}

static inline
void v2_scale_ip(Vec2* a, f32 f)
{
	a->x *= f;
	a->y *= f;
}

static inline
void v2_mul_ip(Vec2* a, const Vec2* b)
{
	a->x *= b->x;
	a->y *= b->y;
}

static inline
void v2_normalize_ip(Vec2* a)
{
	f32 mag = v2_mag(a);
	a->x /= mag;
	a->y /= mag;
}

static inline
void v2_perpendicular_ip(Vec2* a)
{
	f32 x = a->x;
	a->x = -a->y;
	a->y = x;
}

typedef struct Vec3_
{
	f32 x;
	f32 y;
	f32 z;
} Vec3;


Vec3 v3(f32 x, f32 y, f32 z)
{
	Vec3 v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

typedef struct Vec4_
{
	f32 x;
	f32 y;
	f32 z;
	f32 w;
} Vec4;

Vec4 v4(f32 x, f32 y, f32 z, f32 w)
{
	Vec4 v;
	v.x = x;
	v.y = y;
	v.z = z;
	v.w = w;
	return v;
}


typedef struct Vec2i_
{
	i32 x;
	i32 y;
} Vec2i;

Vec2i v2i(i32 x, i32 y)
{
	Vec2i v;
	v.x = x;
	v.y = y;
	return v;
}

typedef struct Vec3i_
{
	i32 x;
	i32 y;
	i32 z;
} Vec3i;

typedef struct Vec4i_
{
	i32 x;
	i32 y;
	i32 z;
	i32 w;
} Vec4i;

typedef struct Color_
{
	f32 r;
	f32 g;
	f32 b;
	f32 a;
} Color;

static inline
Color create_color(f32 r, f32 g, f32 b, f32 a)
{
	Color c;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	return c;
}

static inline
Color color_from_rgba(u32 rgba)
{
	u8 r = (rgba >> 24) & 0xFF;
	u8 g = (rgba >> 16) & 0xFF;
	u8 b = (rgba >> 8) & 0xFF;
	u8 a = (rgba >> 0) & 0xFF;
	Color c;
	c.r = ((f32)r) / 255.0;
	c.g = ((f32)g) / 255.0;
	c.b = ((f32)b) / 255.0;
	c.a = ((f32)a) / 255.0;
	return c;
}

static inline
u32 color_to_rgba(Color c)
{
	u8 r = (u8)(c.r * 255);
	u8 g = (u8)(c.g * 255);
	u8 b = (u8)(c.b * 255);
	u8 a = (u8)(c.a * 255);
	return (r << 24) | (g << 16) | (b << 8) | a;
}

/* For simplicity, "Rect2" and "AABB" share the same type, but are treated differently.
 * functions marked aabb_ will treat a Rect2 as a center and half-extents
 * functions marked rect_ will treat a Rect2 as the topleft and full width/height
 */
typedef struct Rect2_
{
	Vec2 pos;
	Vec2 size;
} Rect2;

static inline
Rect2 rect2(f32 x, f32 y, f32 w, f32 h)
{
	Rect2 r;
	r.pos = v2(x, y);
	r.size = v2(w, h);
	return r;
}

static inline
Rect2 rect2_v(Vec2 pos, Vec2 size)
{
	Rect2 r;
	r.pos = pos;
	r.size = size;
	return r;
}

#define AABB_x1(b) ((b).pos.x - (b).size.x)
#define AABB_x2(b) ((b).pos.x + (b).size.x)
#define AABB_y1(b) ((b).pos.y - (b).size.y)
#define AABB_y2(b) ((b).pos.y + (b).size.y)

#define AABBp_x1(b) ((b)->pos.x - (b)->size.x)
#define AABBp_x2(b) ((b)->pos.x + (b)->size.x)
#define AABBp_y1(b) ((b)->pos.y - (b)->size.y)
#define AABBp_y2(b) ((b)->pos.y + (b)->size.y)

static inline
i32 aabb_intersect(const Rect2* a, const Rect2* b)
{
	if(fabsf(b->pos.x - a->pos.x) > (b->size.x + a->size.x)) return false;
	if(fabsf(b->pos.y - a->pos.y) > (b->size.y + a->size.y)) return false;
	return true;
}

static inline
Vec2 aabb_overlap(const Rect2* a, const Rect2* b)
{
	f32 sx = (a->size.x + b->size.x) - fabsf(b->pos.x - a->pos.x);
	f32 sy = (a->size.y + b->size.y) - fabsf(b->pos.y - a->pos.y);
	if(sx > sy) {
		sx = 0;
		if(a->pos.y > b->pos.y) {
			sy *= -1;
		}
	} else {
		sy = 0;
		if(a->pos.x > b->pos.x) {
			sx *= -1;
		}
	}
	return v2(sx, sy);
}





