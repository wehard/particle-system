#define CUBE_SIZE 1.0
#define SPHERE_RADIUS 0.5

typedef struct
{
	float4 pos;
	float4 vel;
	float life;
}		t_particle;

typedef struct
{
	float4 pos;
	float4 dir;
	float vel;
	float rate;
	float life;
}		t_emitter;

typedef enum e_init_shape
{
	CUBE,
	SPHERE,
	RECT,
	SINE
} t_init_shape;

static float noise3D(float x, float y, float z) {
	float ptr = 0.0f;
	return fract(sin(x*112.9898f + y*179.233f + z*237.212f) * 43758.5453f, &ptr);
}

/*
	Return a random unsigned long
*/
static ulong rand_ulong(__global ulong* seed)
{
	ulong value = *seed * 1103515245 + 12345;

	*seed = value;
	return value;
}

/*
	Return a random float in range [0, 1]
*/
static float rand_float(__global ulong* seed)
{
	return (float)rand_ulong(seed) / ULONG_MAX;
}

/*
	Return a random float in range [a, b]
*/
static float rand_float_in_range(global ulong* seed, float a, float b)
{
	float	x = rand_float(seed);

	return (b - a) * x + a;
}

float	random(int n)
{
	ulong	seed;

	seed = (n * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
	return ((float)(uint)(seed >> 16) / 4294967296.0);
}




static void init_cube(__global t_particle * ps, int num_particles)
{
	int				i = get_global_id(0);

	uint	cube_root = cbrt((float)num_particles);

	uint	x = fmod((float)i, (float)cube_root);
	uint	y = fmod((float)i / cube_root, (float)cube_root);
	uint	z = i / (cube_root * cube_root);

	float	sub_div = CUBE_SIZE / cube_root;
	float	sub_div_half = sub_div / 2.0f;

	ps[i].pos = (float4)(
		random(((i * 124322) >> 3) ^ i) * (float)M_PI / 3.0 - 0.5,
		random(((i * 124322) >> 2) ^ i) * (float)M_PI / 3.0 - 0.5,
		random(((i * 124322) >> 1) ^ i) * (float)M_PI / 3.0 - 0.5,
		1.0f
	);


	ps[i].vel = (float4)(
		0.0f,
		0.0f,
		0.0f,
		1.0f
	);
};

static void init_sphere(__global t_particle * ps, int num_particles)
{
	int i = get_global_id(0);

	int subd = 360;

	int lon = i / subd;
	int lat = i % subd;

	float sign = -2.0f * (lon % 2) + 1.0f;
	float phi = 2.0 * M_PI_F * lon / subd;
	float theta = M_PI_F * lat / subd;

	float radius = random(((i * 124322) >> 3) ^ i) * (float)M_PI / 3.0;

	radius /= 2.0;

	ps[i].pos = (float4)(
		radius * sin(theta) * cos(phi),
		radius * sign * cos(theta),
		radius * sin(theta) * sin(phi),
		1.0f
	);

	ps[i].vel = (float4)(
		0.0f,
		0.0f,
		0.0f,
		1.0f
	);
};

static void init_rect(__global t_particle * ps, int num_particles)
{
	int i = get_global_id(0);

	float fgi = (float)(i) / num_particles;

	ps[i].pos = (float4)(
		random(((i * 124322) >> 3) ^ i) * (float)M_PI / 3.0 - 0.5,
		random(((i * 124322) >> 2) ^ i) * (float)M_PI / 3.0 - 0.5,
		0.0f,
		1.0f
	);

	float3 rand_vel = (float3)(
		noise3D(fgi, 0.0f, i * 0.12230f) * 2.0f - 1.0f,
		noise3D(fgi, i, 0.134660f) * 2.0f - 1.0f,
		noise3D(fgi, i, 0.134660f) * 2.0f - 1.0f
	);

	rand_vel = normalize(rand_vel);

	ps[i].vel = (float4)(
		0.0,
		0.0,
		0.0,
		1.0f
	);

	ps[i].life = 0.1f * i;
};

static void init_sine(__global t_particle * ps, int num_particles)
{
	int i = get_global_id(0);

	ps[i].pos = (float4)(
		(i / (float)num_particles) * 2.0 - 1.0,
		sin(2.0 * M_PI * i / num_particles),
		0.0f,
		1.0f
	);

	ps[i].vel = (float4)(
		0.0f,
		0.0f,
		0.0f,
		1.0f
	);
};

__kernel void init_particles(__global t_particle * ps, int num_particles, t_init_shape shape)
{
	switch (shape)
	{
		case CUBE:
			init_cube(ps, num_particles);
			break;
		case SPHERE:
			init_sphere(ps, num_particles);
			break;
		case RECT:
			init_rect(ps, num_particles);
			break;
		case SINE:
			init_sine(ps, num_particles);
			break;
		default:
			break;
	}
}

__kernel void init_particles_emitter(__global t_particle *ps, int num_particles, t_emitter e)
{
	int i = get_global_id(0);

	ps[i].pos = e.pos;
	ps[i].vel = e.dir * e.vel;
	ps[i].life = 0.001f * i;
}

float3 lerp(float3 v1, float3 v2, float amount)
{
	float3 v;

	v.x = v1.x + (v2.x - v1.x) * amount;
	v.y = v1.y + (v2.y - v1.y) * amount;
	v.z = v1.z + (v2.z - v1.z) * amount;

	return v;
}

static float3 velocity_from_gravity_point(__global t_particle *p, __global float4 *gp)
{
	const float gravity_scale = 1.0f;
	
	float3 dir = gp->xyz - p->pos.xyz;
	float dist = length(dir);
	float f = 9.186f * gravity_scale * (1.0f / dist * dist);
	float3 vel = normalize(dir) * f;

	return vel;
}

__kernel void update_particles_gravity_points(__global t_particle *ps, __global float4 *gps, int num_gp, float4 m, float dt, int mg, float mgs)
{
	int i = get_global_id(0);
	
	__global t_particle *p = ps + i;

	float3 vel = (float3)(0.0);

	if (mg == 1)
	{
		vel += velocity_from_gravity_point(p, (__global float4*)&m) * mgs;
	}

	for (int j = 0; j < num_gp; j++)
	{
		__global float4 *g = gps + j;
		vel += velocity_from_gravity_point(p, g);
	}

	ps[i].vel.xyz += vel;
	ps[i].pos.xyz += ps[i].vel.xyz * dt * 0.00005f;
}

static void reset_particle_emitter()
{

}

__kernel void update_particles_emitter(__global t_particle *ps, __global float4 *gps, int num_gp, float4 m, float dt, int mg, t_emitter e)
{
	int i = get_global_id(0);
	
	__global t_particle *p = ps + i;

	float3 vel = (float3)(0.0);

	if (mg == 1)
	{
		vel += velocity_from_gravity_point(p, (__global float4*)&m);
	}

	for (int j = 0; j < num_gp; j++)
	{
		__global float4 *g = gps + j;
		vel += velocity_from_gravity_point(p, g);
	}

	ps[i].life -= dt;
	if (ps[i].life <= 0.0)
	{
		ps[i].pos = e.pos;
		ps[i].vel = normalize(e.dir) * e.vel;
		ps[i].life = e.life;
		return;
	}

	ps[i].vel.xyz += vel;
	ps[i].pos.xyz += ps[i].vel.xyz * dt * 0.00005f;
}