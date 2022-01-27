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
	float cone_angle;
}		t_emitter;

typedef enum e_init_shape
{
	CUBE,
	SPHERE,
	RECT,
	SINE
} t_init_shape;

static ulong rand_ulong(__global ulong* seed)
{
	ulong value = *seed * 1103515245 + 12345;

	*seed = value;
	return value;
}

static float rand_float(__global ulong* seed)
{
	return (float)rand_ulong(seed) / ULONG_MAX;
}

static float rand_float_in_range(global ulong* seed, float a, float b)
{
	float	x = rand_float(seed);

	return (b - a) * x + a;
}

static void init_cube(__global t_particle * ps, __global ulong *sb, int num_particles)
{
	int				i = get_global_id(0);

	ps[i].pos = (float4)(
		rand_float_in_range(sb, -0.5, 0.5),
		rand_float_in_range(sb, -0.5, 0.5),
		rand_float_in_range(sb, -0.5, 0.5),
		1.0f
	);
};

static float4 random_point_inside_unit_sphere(__global ulong *sb)
{
	float u = rand_float_in_range(sb, 0.0, 1.0);
	float v = rand_float_in_range(sb, 0.0, 1.0);

	float theta = u * 2.0 * M_PI;
	float phi = acos(2.0 * v - 1.0);
	float r = cbrt(rand_float_in_range(sb, 0.0, 1.0));
	float sinTheta = sin(theta);
	float cosTheta = cos(theta);
	float sinPhi = sin(phi);
	float cosPhi = cos(phi);

	float4 p = (float4)(
		r * sinPhi * cosTheta * 0.5,
		r * sinPhi * sinTheta * 0.5,
		r * cosPhi * 0.5,
		1.0
	);
	return p;
}

static float2 random_point_unit_circle(__global ulong *sb)
{
	float radius = 1.0f;
	float r = radius * sqrt(rand_float_in_range(sb, 0.0, 1.0));
	float theta = rand_float_in_range(sb, 0.0, 1.0) * 2.0 * M_PI;
	float x = r * cos(theta);
	float y = r * sin(theta);
	return (float2)(x, y);
}

static void init_sphere(__global t_particle * ps, __global ulong *sb, int num_particles)
{
	int i = get_global_id(0);
	ps[i].pos = random_point_inside_unit_sphere(sb);
};

static void init_rect(__global t_particle * ps, __global ulong *sb, int num_particles)
{
	int i = get_global_id(0);

	ps[i].pos = (float4)(
		rand_float_in_range(sb, -0.5, 0.5),
		rand_float_in_range(sb, -0.5, 0.5),
		0.0f,
		1.0f
	);
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
};

__kernel void init_particles(__global t_particle * ps, __global ulong *sb, int num_particles, t_init_shape shape)
{
	int i = get_global_id(0);
	switch (shape)
	{
		case CUBE:
			init_cube(ps, &sb[i], num_particles);
			break;
		case SPHERE:
			init_sphere(ps, &sb[i], num_particles);
			break;
		case RECT:
			init_rect(ps, &sb[i], num_particles);
			break;
		case SINE:
			init_sine(ps, num_particles);
			break;
		default:
			break;
	}

	float init_mag = 1000.0;
	float4 init_vel = normalize(random_point_inside_unit_sphere(&sb[i])) * init_mag;

	if (shape == RECT)
	{
		init_vel.z = 0.0f;
	}

	ps[i].vel = init_vel;
	ps[i].life = 0.0;
}

static float3 rotate_vector(float3 v, float3 degrot)
{
	float3 res;
	float3 rot;
	rot.x = radians(degrot.x);
	rot.y = radians(degrot.y);
	rot.z = radians(degrot.z);

	float3 ax = (float3)(1.0, 0.0, 0.0);
	float3 ay = (float3)(0.0, 1.0, 0.0);
	float3 az = (float3)(0.0, 0.0, 1.0);
	v = v * cos(rot.x) + cross(ax, v) * sin(rot.x) + ax * dot(ax, v) * (1.0f - cos(rot.x));
	v = v * cos(rot.y) + cross(ay, v) * sin(rot.y) + ay * dot(ay, v) * (1.0f - cos(rot.y));
	v = v * cos(rot.z) + cross(az, v) * sin(rot.z) + az * dot(az, v) * (1.0f - cos(rot.z));
	return v;
}

static void reset_particle(__global t_particle *ps, __global ulong *sb, t_emitter e)
{
	int i = get_global_id(0);

	float2 c = random_point_unit_circle(&sb[i]);
	float4 p = (float4)(c.x, 1.0, c.y, 1.0);
	float3 rotated = rotate_vector(normalize(p.xyz), e.dir.xyz);
	float4 init_vel = (float4)(rotated.xyz, 1.0) * e.vel;

	ps[i].pos = e.pos;
	ps[i].vel = init_vel;
	ps[i].life = -(float)i * (1.0 / e.rate);
}

__kernel void init_particles_emitter(__global t_particle *ps, __global ulong *sb, int num_particles, t_emitter e)
{
	reset_particle(ps, sb, e);
}

#define MAX_VELOCITY 20000.0f
#define TIME_SCALE 0.00001f
#define MIN_DISTANCE 0.025f
#define G 1.0f

static float3 velocity_from_gravity_point(__global t_particle *p, __global float4 *gp)
{
	const float gravity_scale = 1.0f;
	
	float3 dir = gp->xyz - p->pos.xyz;
	float dist = length(dir);
	if (dist < MIN_DISTANCE)
		dist = MIN_DISTANCE;
	float f = G * (gp->w / (dist * 3.0f));
	float3 vel = normalize(dir) * f;

	return vel;
}

static float3 velocity_combined(__global t_particle *p, __global float4 *gps, int num_gp, bool use_mouse, float4 mouse_pos, float mouse_mass)
{
	float3 vel = (float3)(0.0, 0.0, 0.0);

	if (use_mouse)
	{
		vel += velocity_from_gravity_point(p, (__global float4*)&mouse_pos) * mouse_mass;
	}

	for (int j = 0; j < num_gp; j++)
	{
		__global float4 *g = gps + j;
		vel += velocity_from_gravity_point(p, g);
	}
	return vel;
}

__kernel void update_particles_gravity_points(__global t_particle *ps, __global float4 *gps, int num_gp, float4 m, float dt, int mg, float mgs)
{
	int i = get_global_id(0);
	
	ps[i].vel.xyz += velocity_combined(&ps[i], gps, num_gp, mg, m, mgs);
	ps[i].pos.xyz += ps[i].vel.xyz * dt * TIME_SCALE;
}

__kernel void update_particles_emitter(__global t_particle *ps, __global float4 *gps, __global ulong *sb, int num_gp, float4 m, float dt, int mg, float mgs, t_emitter e)
{
	int i = get_global_id(0);
	
	if (ps[i].life >= e.life)
	{
		reset_particle(ps, sb, e);
	}

	if (ps[i].life < 0.0)
	{
		ps[i].life += dt;
		return;
	}

	ps[i].life += dt;
	ps[i].vel.xyz += velocity_combined(&ps[i], gps, num_gp, mg, m, mgs);
	ps[i].pos.xyz += ps[i].vel.xyz * dt * TIME_SCALE;
}