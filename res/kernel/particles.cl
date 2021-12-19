typedef struct
{
	float			pos[3];
}				t_particle;

__kernel void init_particles(__global t_particle * ps)
{
	int i = get_global_id(0);
	ps[i].pos[0] = 1.0;
	ps[i].pos[1] = 1.0;
	ps[i].pos[2] = 1.0;
};

__kernel void update_particles(__global t_particle* ps, float dt)
{
	int i = get_global_id(0);
	ps[i].pos[0] = 1.0 / 50.0 * dt;
	ps[i].pos[1] = 1.0 / 50.0 * dt;
	ps[i].pos[2] = 1.0 / 50.0 * dt;
};
