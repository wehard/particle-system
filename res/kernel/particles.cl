typedef struct
{
	float			pos[3];
	float			vel[3];
}				t_particle;

typedef struct
{
	float			vel[3];
}				t_vel;

__kernel void init_particles(__global t_particle * ps)
{
	int i = get_global_id(0);
	// ps[i].pos[0] = 0.5;
	// ps[i].pos[1] = 0.5;
	// ps[i].pos[2] = 0.0;
};

__kernel void update_particles(__global t_particle* ps, float dt, float mx, float my)
{
	int i = get_global_id(0);

	ps[i].pos[0] = ps[i].pos[0] + ps[i].vel[0] * dt * 10.0;
	ps[i].pos[1] = ps[i].pos[1] + ps[i].vel[1] * dt;
	ps[i].pos[2] = 0.0;

	// ps[i].pos[0] = ((float)i/50) * mx;
	// ps[i].pos[1] = ((float)i/50) * my;
	// ps[i].pos[2] = 0.0;

	// ps[i].vel[0] = 0.0;
	// ps[i].vel[1] = 0.0;
	// ps[i].vel[2] = 0.0;
};
