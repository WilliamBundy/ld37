
static inline 
u64 rand_splitmix64(u64* x)
{
	*x += UINT64_C(0x9E3779B97F4A7C15);
	u64 z = *x;
	z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
	z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
	return z ^ (z >> 31);	
}

static inline 
u64 rand_rotate_left(const u64 t, i64 k)
{
	return (t << k) | (t >> (64 - k));
}

typedef struct RandomState_
{
	u64 x;
	u64 y;
} RandomState;

u64 rand_xoroshift(RandomState* r)
{
	u64 a = r->x;
	u64 b = r->y;
	u64 result = a + b;

	b ^= a;
	r->x = rand_rotate_left(a, 55) ^ b ^ (b << 14);
	r->y = rand_rotate_left(b, 36);
	return result;
}

void randomstate_init(RandomState* r, u64 seed) 
{
	rand_splitmix64(&seed);
	rand_splitmix64(&seed);
	r->x = rand_splitmix64(&seed);
	r->y = rand_splitmix64(&seed);
	rand_xoroshift(r);
}

static inline
f64 rand_f64(RandomState* r)
{
	u64 x = rand_xoroshift(r);
	return (f64)x / UINT64_MAX;
}

static inline
f32 rand_f32(RandomState* r)
{
	return (f32)rand_f64(r);
}

static inline
f32 rand_range(RandomState* r, f32 min, f32 max)
{
	return rand_f64(r) * (max - min) + min;
}

static inline
i32 rand_range_int(RandomState* r, i32 min, i32 max)
{
	return (i32)(rand_f64(r) * ((max + 1) - min) + min);
}

