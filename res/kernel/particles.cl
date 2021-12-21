typedef struct
{
	float			pos[3];
}				t_particle;

__kernel void init_particles(__global t_particle * ps)
{
	int i = get_global_id(0);
	ps[i].pos[0] = 0.5;
	ps[i].pos[1] = 0.5;
	ps[i].pos[2] = 0.0;
};

__kernel void update_particles(__global t_particle* ps, float dt, float mx, float my)
{
	int i = get_global_id(0);
	ps[i].pos[0] = ((float)i/10000) * mx;
	ps[i].pos[1] = ((float)i/10000) * my;
	ps[i].pos[2] = 0.0;
};
