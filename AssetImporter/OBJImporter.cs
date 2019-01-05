using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Numerics;
using System.IO;

namespace AssetImporter
{
    public class OBJImporter
    {
        public static void Convert(string internalPath, string inputFile, string targetFolder)
        {
            MeshFileFormat format = new MeshFileFormat();
            List<Vector3> pos = new List<Vector3>();
            List<Vector3> normals = new List<Vector3>();
            List<Vector2> uvs = new List<Vector2>();

            StreamReader reader = new StreamReader(inputFile);
            int index = 0;
            bool normalsPresent = false;
            while (!reader.EndOfStream)
            {
                string line = reader.ReadLine();
                if (line[0] == 'o')
                {
                    if (format.Indices.Count > 0)
                    {
                        format.Export(internalPath, targetFolder);
                        format.Vertices.Clear();
                        format.Indices.Clear();
                    }
                    format.ObjectName = line.Substring(2);
                    continue;
                }

                string[] command = line.Split(' ');
                if (command.Length < 3)
                    continue;

                if (command[0] == "v")
                {
                    if (command.Length < 4)
                        continue;
                    pos.Add(new Vector3(float.Parse(command[1]), float.Parse(command[2]), float.Parse(command[3])));
                }
                else if (command[0] == "vn")
                {
                    if (command.Length < 4)
                        continue;
                    normals.Add(new Vector3(float.Parse(command[1]), float.Parse(command[2]), float.Parse(command[3])));
                }
                else if (command[0] == "vt")
                {
                    uvs.Add(new Vector2(float.Parse(command[1]), float.Parse(command[2])));
                }
                else if (command[0] == "f")
                {
                    if (command.Length < 4)
                        continue;
                    for (int i = 1; i < 4; ++i)
                    {
                        string[] indices = command[i].Split('/');
                        Vertex v = new Vertex();
                        if (indices.Length == 0)
                        {
                            v.Position = pos[int.Parse(command[i]) - 1];
                        }
                        else if (indices.Length == 2)
                        {
                            v.Position = pos[int.Parse(indices[0]) - 1];
                            v.Texcoord = uvs[int.Parse(indices[1]) - 1];
                        }
                        else if (indices.Length == 3)
                        {
                            v.Position = pos[int.Parse(indices[0]) - 1];
                            v.Normal = normals[int.Parse(indices[2]) - 1];
                            if (indices[1] != "")
                                v.Texcoord = uvs[int.Parse(indices[1]) - 1];
                            normalsPresent = true;
                        }
                        v.Color = new Vector4(1, 1, 1, 1);
                        format.Vertices.Add(v);
                        format.Indices.Add(index++);
                    }
                }
            }
            if (!normalsPresent)
                format.CalculateNormals();
            format.CalculateTangents();
            if (format.Indices.Count > 0)
                format.Export(internalPath, targetFolder);
            reader.Close();
        }
    }
}
