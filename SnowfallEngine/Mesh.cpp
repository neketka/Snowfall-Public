#include "stdafx.h"

#include "Mesh.h"

void Mesh::CalculateNormals()
{
	for (int i = 0; i < Indices.size(); i += 3)
	{
		RenderVertex& v1 = Vertices[Indices[i]];
		RenderVertex& v2 = Vertices[Indices[i + 1]];
		RenderVertex& v3 = Vertices[Indices[i + 2]];

		glm::vec3 normal = glm::cross(v2.Position - v1.Position, v3.Position - v2.Position);
		v1.Normal = normal;
		v2.Normal = normal;
		v3.Normal = normal;
	}
	for (RenderVertex& v : Vertices)
		v.Normal = glm::normalize(v.Normal);
}

void Mesh::CalculateTangents()
{
	for (int i = 0; i < Indices.size(); i += 3)
	{
		RenderVertex& v1 = Vertices[Indices[i]];
		RenderVertex& v2 = Vertices[Indices[i + 1]];
		RenderVertex& v3 = Vertices[Indices[i + 2]];

		glm::vec3 deltaPos1 = v2.Position - v1.Position;
		glm::vec3 deltaPos2 = v3.Position - v1.Position;

		glm::vec2 deltaUV1 = v2.Texcoord - v1.Texcoord;
		glm::vec2 deltaUV2 = v3.Texcoord - v1.Texcoord;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;

		v1.Tangent = tangent;
		v2.Tangent = tangent;
		v2.Tangent = tangent;
	}
}

BoundingBox Mesh::CalculateAABB()
{
	BoundingBox box;

	box.MinExtent = box.MaxExtent = Vertices[0].Position;

	for (RenderVertex& vertex : Vertices)
	{
		glm::vec3 pos = vertex.Position;

		if (pos.x < box.MinExtent.x)
			box.MinExtent.x = pos.x;
		if (pos.y < box.MinExtent.y)
			box.MinExtent.y = pos.y;
		if (pos.z < box.MinExtent.z)
			box.MinExtent.y = pos.z;

		if (pos.z > box.MaxExtent.x)
			box.MaxExtent.y = pos.x;
		if (pos.y > box.MaxExtent.y)
			box.MaxExtent.y = pos.y;
		if (pos.z > box.MaxExtent.z)
			box.MaxExtent.y = pos.z;
	}

	return box;
}
