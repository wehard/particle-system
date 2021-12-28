typedef struct
{
	float4 pos;
	float4 vel;
}		t_particle;

static float noise3D(float x, float y, float z) {
	float ptr = 0.0f;
	return fract(sin(x*112.9898f + y*179.233f + z*237.212f) * 43758.5453f, &ptr);
}

__kernel void init_particles(__global t_particle * ps, int num_particles)
{
	int i = get_global_id(0);

	float fgi = (float)(i) / num_particles;

	ps[i].pos = (float4)(
		noise3D(fgi, 0.0f, 0.12230f) * 2.0f - 1.0f,
		noise3D(fgi, 30.0f, 0.134660f) * 2.0f - 1.0f,
		0.0f,
		1.0f
	);

	ps[i].vel = (float4)(
		1.0f,
		0.0f,
		0.0f,
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
		1.0f,
		0.0f,
		0.0f,
		1.0f
	);
};

__kernel void update_particles(__global t_particle* ps, float dt, float mx, float my)
{
	int i = get_global_id(0);

	global t_particle *particle = ps + i;

	float3 coord = (float3)(ps[i].pos.x, ps[i].pos.y, ps[i].pos.z);
	float3 mouse = (float3)(mx, my, 0.0f);
	float3 dir = normalize(mouse - coord);

	float3	direction = mouse - coord;
	float	distance = length(direction);
	float3	velocity = normalize(direction) * (1.0f / (distance * 20.0f));

	float len_vel = length(velocity);

	if (len_vel > 2.0)
		velocity = normalize(velocity) * 2.0f;

	ps[i].vel = (float4)(velocity.x, velocity.y, velocity.z, 1.0f);


	// Then move towards that point with some speed

	float3 res = coord + velocity * dt * 0.5f;

	ps[i].vel = (float4)(velocity.x, velocity.y, velocity.z, 1.0);

	ps[i].pos = (float4)(res.x, res.y, res.z, 1.0f);
};
