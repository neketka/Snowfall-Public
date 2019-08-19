#include "TerrainShader"

#define layers 8

#ifdef MATERIAL

layout(location = 20) uniform sampler2DArray TerrainAtlas; // Snow/Ground: Diffuse, Normal, Roughness, AO

vec2 ParallaxOcclusionMapping(sampler2DArray depthMap, float layer, vec2 uv, vec2 displacement, float pivot) {
	const float layerDepth = 1.0 / float(layers);
	float currentLayerDepth = 0.0;

	vec2 deltaUv = displacement / float(layers);
	vec2 currentUv = uv + pivot * displacement;
	float currentDepth = texture(depthMap, vec3(currentUv, layer)).r;

	for (int i = 0; i < layers; i++) {
		if (currentLayerDepth > currentDepth)
			break;

		currentUv -= deltaUv;
		currentDepth = texture(depthMap, vec3(currentUv, layer)).r;
		currentLayerDepth += layerDepth;
	}

	vec2 prevUv = currentUv + deltaUv;
	float endDepth = currentDepth - currentLayerDepth;
	float startDepth =
		texture(depthMap, vec3(prevUv, layer)).r - currentLayerDepth + layerDepth;

	float w = endDepth / (endDepth - startDepth);

	return mix(currentUv, prevUv, w);
}

void MaterialProps(inout Material mat, PerVertexData data)
{
	vec3 view = transpose(data.TBNMatrix) * normalize(data.CameraPosition - data.PixelPosition);
	vec2 coord = data.Texcoord * 16;//ParallaxOcclusionMapping(TerrainAtlas, 4.0, , view.xy * 5, 0.0);

	vec4 alpha = data.Color;
	
	mat.Diffuse = (texture(TerrainAtlas, vec3(coord, 0)).rgb * alpha.r + texture(TerrainAtlas, vec3(coord, 5)).rgb * alpha.g);
	mat.Normal = data.TBNMatrix * (normalize(texture(TerrainAtlas, vec3(coord, 1)).rgb * alpha.r + texture(TerrainAtlas, vec3(coord, 6)).rgb * alpha.g) * 2.0 - 1.0);
	mat.Metalness = 0;
	mat.Roughness = texture(TerrainAtlas, vec3(coord, 2)).r * alpha.r + texture(TerrainAtlas, vec3(coord, 7)).r * alpha.g;
	mat.AO = texture(TerrainAtlas, vec3(coord, 3)).r * alpha.r + texture(TerrainAtlas, vec3(coord, 8)).r * alpha.g;
	mat.Alpha = texture(TerrainAtlas, vec3(coord, 0)).a;
	/*
	mat.Emissive = max(vec3(0), data.Tangent * vec3(1));
	mat.Alpha = 1.0;*/
}

#endif