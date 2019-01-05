using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using System.IO;

namespace AssetImporter
{
    public class Vertex
    {
        public Vector3 Position;
        public Vector4 Color;
        public Vector3 Normal;
        public Vector3 Tangent;
        public Vector2 Texcoord;
    }

    public class MeshFileFormat
    {
        public MeshFileFormat()
        {
            Vertices = new List<Vertex>();
            Indices = new List<int>();
        }

        public void Export(string internalPath, string folder)
        {
            string file = folder + "\\" + ObjectName + ".masset";
            if (File.Exists(file))
                File.Delete(file);
            FileStream stream = File.Open(file, FileMode.CreateNew);
            BinaryWriter writer = new BinaryWriter(stream);

            writer.Write((UInt32)(internalPath + "." + ObjectName).Length);
            writer.Write((internalPath + "." + ObjectName).ToCharArray());
            writer.Write(Vertices.Count);
            writer.Write(Indices.Count);

            foreach (Vertex v in Vertices)
            {
                writer.Write(v.Position.X);
                writer.Write(v.Position.Y);
                writer.Write(v.Position.Z);

                writer.Write(v.Color.X);
                writer.Write(v.Color.Y);
                writer.Write(v.Color.Z);
                writer.Write(v.Color.W);

                writer.Write(v.Normal.X);
                writer.Write(v.Normal.Y);
                writer.Write(v.Normal.Z);

                writer.Write(v.Tangent.X);
                writer.Write(v.Tangent.Y);
                writer.Write(v.Tangent.Z);

                writer.Write(v.Texcoord.X);
                writer.Write(v.Texcoord.Y);
            }

            foreach (int i in Indices)
            {
                writer.Write(i);
            }
            
            stream.Flush();
            stream.Close();
        }

        public void CalculateNormals()
        {
            for (int i = 0; i < Indices.Count; i += 3)
            {
                Vertex v1 = Vertices[Indices[i]];
                Vertex v2 = Vertices[Indices[i + 1]];
                Vertex v3 = Vertices[Indices[i + 2]];

                Vector3 normal = Vector3.Cross(v2.Position - v1.Position, v3.Position - v2.Position);
                v1.Normal = normal;
                v2.Normal = normal;
                v3.Normal = normal;
            }
            foreach (Vertex v in Vertices)
                v.Normal = Vector3.Normalize(v.Normal);
        }

        public void CalculateTangents()
        {
            for (int i = 0; i < Indices.Count; i += 3)
            {
                Vertex v1 = Vertices[Indices[i]];
                Vertex v2 = Vertices[Indices[i + 1]];
                Vertex v3 = Vertices[Indices[i + 2]];

                Vector3 deltaPos1 = v2.Position - v1.Position;
                Vector3 deltaPos2 = v3.Position - v1.Position;

                Vector2 deltaUV1 = v2.Texcoord - v1.Texcoord;
                Vector2 deltaUV2 = v3.Texcoord - v1.Texcoord;

                float r = 1.0f / (deltaUV1.X * deltaUV2.Y - deltaUV1.Y * deltaUV2.X);
                Vector3 tangent = (deltaPos1 * deltaUV2.Y - deltaPos2 * deltaUV1.Y) * r;

                if (float.IsInfinity(r))
                    tangent = Vector3.Zero;

                v1.Tangent = tangent;
                v2.Tangent = tangent;
                v2.Tangent = tangent;
            }
        }

        public string ObjectName;
        public List<Vertex> Vertices;
        public List<int> Indices;
    }
}
