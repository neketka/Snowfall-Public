#ifndef PARTICLE_DEF_INCLUDE
#define PARTICLE_DEF_INCLUDE

struct Particle
{
	vec4 Position; // + Life
	vec4 Velocity; // + Gravity Factor
	vec4 Scale; // + Time passed
	vec4 Color;
};

struct ParticleSystemEasing
{

};

#endif