#ifndef UTILITIES_SHADER_INCLUDE
#define UTILITIES_SHADER_INCLUDE

vec2 CartesianToSphericalNormal(vec3 cartesian)
{
	float theta = acos(cartesian.z);
	float phi = atan(cartesian.y, cartesian.x);
	
	return vec2(theta, phi);
}

vec3 SphericalToCartesianNormal(vec2 spherical)
{
	float sinTh = sin(spherical.x);
	float cosTh = cos(spherical.x);
	float sinPh = sin(spherical.y);
	float cosPh = cos(spherical.y);

	return vec3(sinTh * cosPh, sinTh * sinPh, cosTh);
}

#endif