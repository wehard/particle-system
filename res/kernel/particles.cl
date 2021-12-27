typedef struct
{
	float			pos[3];
	float			vel[3];
}				t_particle;

__kernel void init_particles(__global t_particle * ps, int num_particles)
{
	int i = get_global_id(0);

	// ps[i].pos[0] = cos(2.0 * M_PI * i / num_particles);
	// ps[i].pos[1] = sin(2.0 * M_PI * i / num_particles);
	// ps[i].pos[2] = 0.0;

	ps[i].pos[0] = (i / (float)num_particles) * 2.0 - 1.0;
	ps[i].pos[1] = sin(2.0 * M_PI * i / num_particles);
	ps[i].pos[2] = 0.0;

	ps[i].vel[0] = 1.0;
	ps[i].vel[1] = 0.0;
	ps[i].vel[2] = 0.0;
};

__kernel void update_particles(__global t_particle* ps, float dt, float mx, float my)
{
	int i = get_global_id(0);

	float3 coord = (float3)(ps[i].pos[0], ps[i].pos[1], ps[i].pos[2]);
	float3 mouse = (float3)(mx, my, 0.0);
	float3 dir = normalize(mouse - coord);

	float3	direction = mouse - coord;
	float	distance = length(direction);
	float3	velocity = normalize(direction) * (1.0f / (distance * distance));

	float len_vel = length(velocity);

	if (len_vel > 2.0)
		velocity = normalize(velocity) * 2.0f;


	ps[i].vel[0] = velocity.x;
	ps[i].vel[1] = velocity.y;
	ps[i].vel[2] = velocity.z;


	// Then move towards that point with some speed

	float3 res = coord + velocity * dt;

	ps[i].pos[0] = res.x;
	ps[i].pos[1] = res.y;
	ps[i].pos[2] = res.z;

	// ps[i].pos[0] = ps[i].pos[0] + ps[i].vel[0] * dt * 1.0;
	// ps[i].pos[1] = ps[i].pos[1] + ps[i].vel[1] * dt * 1.0;
	// ps[i].pos[2] = 0.0;

	// ps[i].pos[0] = ((float)i/50) * mx;
	// ps[i].pos[1] = ((float)i/50) * my;
	// ps[i].pos[2] = 0.0;

	// ps[i].vel[0] = 0.0;
	// ps[i].vel[1] = 0.0;
	// ps[i].vel[2] = 0.0;
};
