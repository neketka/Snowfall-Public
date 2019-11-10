#ifdef COMPUTE

#include "ParticleDef"

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(binding = 0, std430) buffer AliveBuffer
{
	int AliveParticleCount;
	int TotalParticleCount;
	int Padding0[2];
	Particle AliveParticles[];
};

layout(binding = 1, std430) buffer DeadBuffer
{
	int DeadParticleCount;
	int DeadParticleIndices[];
};

layout(location = 0) uniform float deltaTime;
layout(location = 1) uniform vec3 gravity;
layout(location = 2) uniform vec3 minKillBoxExtent;
layout(location = 3) uniform vec3 maxKillBoxExtent;
layout(location = 4) uniform bool hasKillBox;

void main()
{
	int particleIndex = gl_GlobalInvocationID.x;

	Particle particle = Particles[particleIndex];

	particle.Velocity.xyz += gravity * particleVelocity.w * deltaTime;
	particle.Position.xyz += particle.Velocity.xyz * deltaTime;
	particle.Scale.w += deltaTime;

	bool outOfKillBox = hasKillBox && (any(greaterThan(particle.Position.xyz, maxKillBoxExtent)) || any(lessThan(particle.Position.xyz, minKillBoxExtent)));
	int dead = int(particle.Scale.w > particle.Position.w || outOfKillBox); //Helps reduce branching

	DeadParticleIndices[atomicAdd(DeadParticleCount, dead)] = particleIndex;
	atomicAdd(AliveParticleCount, -dead);

	Particles[particleIndex] = particle;
}

#endif