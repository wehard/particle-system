typedef struct
{
	float4 pos;
	float4 vel;
}		t_particle;

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


#define CUBE_SIZE 1.0
#define SPHERE_RADIUS 0.5



__kernel void init_particles_cube(__global t_particle * ps, int num_particles)
{
	int				i = get_global_id(0);

	uint	cube_root = cbrt((float)num_particles);

	uint	x = fmod((float)i, (float)cube_root);
	uint	y = fmod((float)i / cube_root, (float)cube_root);
	uint	z = i / (cube_root * cube_root);

	float	sub_div = CUBE_SIZE / cube_root;
	float	sub_div_half = sub_div / 2.0f;

	ps[i].pos = (float4)(
		x * sub_div - 0.5 + sub_div_half,
		y * sub_div - 0.5 + sub_div_half,
		z * sub_div - 0.5 + sub_div_half,
		1.0f
	);


	ps[i].vel = (float4)(
		0.0f,
		0.0f,
		0.0f,
		1.0f
	);
};

__kernel void init_particles_sphere(__global t_particle * ps, int num_particles)
{
	int i = get_global_id(0);

	int subd = 360;

	int lon = i / subd;
	int lat = i % subd;

	float sign = -2.0f * (lon % 2) + 1.0f;
	float phi = 2.0 * M_PI_F * lon / subd;
	float theta = M_PI_F * lat / subd;

	ps[i].pos = (float4)(
		SPHERE_RADIUS * sin(theta) * cos(phi),
		SPHERE_RADIUS * sign * cos(theta),
		SPHERE_RADIUS * sin(theta) * sin(phi),
		1.0f
	);

	ps[i].vel = (float4)(
		0.0f,
		0.0f,
		0.0f,
		1.0f
	);
};

__kernel void init_particles_rect(__global t_particle * ps, int num_particles)
{
	int i = get_global_id(0);

	float fgi = (float)(i) / num_particles;

	ps[i].pos = (float4)(
		noise3D(fgi, 0.0f, 0.12230f) - 0.5f,
		noise3D(fgi, 30.0f, 0.134660f) - 0.5f,
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
};

__kernel void init_particles_sine(__global t_particle * ps, int num_particles)
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

float3 lerp(float3 v1, float3 v2, float amount)
{
	float3 v;

	v.x = v1.x + (v2.x - v1.x) * amount;
	v.y = v1.y + (v2.y - v1.y) * amount;
	v.z = v1.z + (v2.z - v1.z) * amount;

	return v;
}

__kernel void update_particles(__global t_particle* ps, float dt, float mx, float my)
{
	int i = get_global_id(0);

	global t_particle *particle = ps + i;

	float3 coord = (float3)(ps[i].pos.x, ps[i].pos.y, ps[i].pos.z);
	float3 mouse = (float3)(mx, my, 0.0f);
	float3 dir = normalize(mouse - coord);

	float3	direction = mouse - coord;

	float3 prev_vel = (float3)(ps[i].vel.x, ps[i].vel.y, ps[i].vel.z);
	float distance = length(direction);
	float3 new_vel = lerp(prev_vel, dir, (0.0001f / (distance * distance)));

	// float4 axis = cross(ps[i].vel, (float4)(dir.x, dir.y, dir.z, 1.0f));

	float len_vel = length(new_vel);
	if (len_vel > 0.1)
		new_vel = normalize(new_vel) * 0.1f;

	ps[i].vel = (float4)(new_vel.x, new_vel.y, new_vel.z, 1.0f);


	// Then move towards that point with some speed

	ps[i].pos.xyz = ps[i].pos.xyz + ps[i].vel.xyz * dt * 5.0f;

};

__kernel void update_particles_test(__global t_particle *ps, float dt, float mx, float my)
{
	int i = get_global_id(0);

	float dx = mx - ps[i].pos.x;
	float dy = my - ps[i].pos.y;
	float dz = 0.0;
	float ir = 1.0 / (dx * dx + dy * dy + dz * dz + 0.00001);
	ir = sqrt(ir);

	float att = 1.0f;
	float ax = att * ir * dx;
	float ay = att * ir * dy;
	float az = att * ir * dz;
	// for (int j = 0; j < mouse.n; j++)
	// {
	// 	dx = mouse.m[2 * j] - ps[i].x;
	// 	dy = mouse.m[2 * j + 1] - ps[i].y;
	// 	dz = mouse.z - ps[i].z;
	// 	ir = 1.0/(dx * dx + dy * dy + dz * dz + 0.00001);
	// 	ir = sqrt(ir);
	// 	ax += mouse.att * ir * dx;
	// 	ay += mouse.att * ir * dy;
	// 	az += mouse.att * ir * dz;
	// }

	ps[i].vel.x += 1.0 * ax;
	ps[i].vel.y += 1.0 * ay;
	ps[i].vel.z += 1.0 * az;

	ps[i].pos.xyz = ps[i].pos.xyz + ps[i].vel.xyz * dt * 0.001f;
}

// how much gravity point affects depending on distance from point
static float3 velocity_from_gravity_point(__global t_particle *p, __global float4 *gp)
{
	const float gravity_scale = 1.0f;
	// const float max_vel = 50.0f;
	
	float3 dir = gp->xyz - p->pos.xyz;
	float dist = length(dir);
	float f = 9.186f * gravity_scale * (1.0f / dist * dist);

	float3 vel = normalize(dir) * f;

	// if (length(vel) > max_vel)
	// {
	// 	vel = normalize(vel) * max_vel;
	// }

	return vel;
}

__kernel void update_particles_gravity_points(__global t_particle *ps, __global float4 *gps, int num_gp, float4 m, float dt)
{
	int i = get_global_id(0);
	
	__global t_particle *p = ps + i;

	float3 vel = (float3)(0.0);

	// if (num_gp == 0)
	// {
	// 	vel += velocity_from_gravity_point(p, (__global float4*)&m);
	// }


	for (int j = 0; j < num_gp; j++)
	{
		__global float4 *g = gps + j;
		vel += velocity_from_gravity_point(p, g);
	}

	ps[i].vel.xyz += vel;
	ps[i].pos.xyz += ps[i].vel.xyz * dt * 0.00005f;
}