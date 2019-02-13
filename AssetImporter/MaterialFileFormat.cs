using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Numerics;

namespace AssetImporter
{
    public enum WrapMode
    {
        Repeat = 0x2901, MirroredRepeat = 0x8370, ClampToEdge = 0x812F
    };

    public enum MinificationFilter
    {
        Nearest = 0x2600, Linear = 0x2601, NearestMipmapNearest = 0x2700, LinearMipmapNearest = 0x2701,
	    NearestMipmapLinear = 0x2702, LinearMipmapLinear = 0x2703
    };

    public enum MagnificationFilter
    {
        Nearest = 0x2600, Linear = 0x2601
    };

    public class Sampler
    {
        public WrapMode UChannelWrap;
        public WrapMode VChannelWrap;
        public WrapMode RChannelWrap;
        public MinificationFilter MinificationFilter;
        public MagnificationFilter MagnificationFilter;
        public bool MaxAnisotropicFiltering;
    }

    public class MaterialFileFormat
    {
        public int PerObjectParameterCount;
        public string Shader;

        public List<Sampler> Samplers;
        public List<string> Textures;
        public List<(int, int)> SamplerTexturePairs;

        public List<(int, int)> IntParams;
        public List<(int, float)> FloatParams;
        public List<(int, Vector2)> Vec2Params;
        public List<(int, Vector3)> Vec3Params;
        public List<(int, Vector4)> Vec4Params;
        public List<(int, int)> TextureParams;

        public MaterialFileFormat()
        {
            Samplers = new List<Sampler>();
            Textures = new List<string>();
            SamplerTexturePairs = new List<(int, int)>();
            IntParams = new List<(int, int)>();
            FloatParams = new List<(int, float)>();
            Vec2Params = new List<(int, Vector2)>();
            Vec3Params = new List<(int, Vector3)>();
            Vec4Params = new List<(int, Vector4)>();
            TextureParams = new List<(int, int)>();
        }

        public void Export(string internalPath, string folder)
        {
            string name = internalPath.Split('.').Last();
            string file = folder + "\\" + name + ".mtasset";
            if (File.Exists(file))
                File.Delete(file);
            FileStream stream = File.Open(file, FileMode.CreateNew);
            BinaryWriter writer = new BinaryWriter(stream);

            writer.Write((UInt32)internalPath.Length);
            writer.Write(internalPath.ToCharArray());

            writer.Write((UInt32)Samplers.Count);
            writer.Write((UInt32)Textures.Count);
            writer.Write((UInt32)SamplerTexturePairs.Count);
            writer.Write((UInt32)IntParams.Count);
            writer.Write((UInt32)FloatParams.Count);
            writer.Write((UInt32)Vec2Params.Count);
            writer.Write((UInt32)Vec3Params.Count);
            writer.Write((UInt32)Vec4Params.Count);
            writer.Write((UInt32)TextureParams.Count);

            writer.Write(PerObjectParameterCount);
            writer.Write((UInt32)Shader.Length);
            writer.Write(Shader.ToCharArray());

            foreach (Sampler s in Samplers)
            {
                writer.Write((UInt32)s.UChannelWrap);
                writer.Write((UInt32)s.VChannelWrap);
                writer.Write((UInt32)s.RChannelWrap);
                writer.Write((UInt32)s.MinificationFilter);
                writer.Write((UInt32)s.MagnificationFilter);
                writer.Write(s.MaxAnisotropicFiltering ? (byte)1 : (byte)0);
            }

            foreach (string t in Textures)
            {
                writer.Write((UInt32)t.Length);
                writer.Write(t.ToCharArray());
            }

            foreach ((int, int) p in SamplerTexturePairs)
            {
                writer.Write(p.Item1);
                writer.Write(p.Item2);
            }

            foreach ((int, int) p in IntParams)
            {
                writer.Write(p.Item1);
                writer.Write(p.Item2);
            }

            foreach ((int, float) p in FloatParams)
            {
                writer.Write(p.Item1);
                writer.Write(p.Item2);
            }

            foreach ((int, Vector2) p in Vec2Params)
            {
                writer.Write(p.Item1);
                writer.Write(p.Item2.X);
                writer.Write(p.Item2.Y);
            }

            foreach ((int, Vector3) p in Vec3Params)
            {
                writer.Write(p.Item1);
                writer.Write(p.Item2.X);
                writer.Write(p.Item2.Y);
                writer.Write(p.Item2.Z);
            }

            foreach ((int, Vector4) p in Vec4Params)
            {
                writer.Write(p.Item1);
                writer.Write(p.Item2.X);
                writer.Write(p.Item2.Y);
                writer.Write(p.Item2.Z);
                writer.Write(p.Item2.W);
            }
            
            foreach ((int, int) p in TextureParams)
            {
                writer.Write(p.Item1);
                writer.Write(p.Item2);
            }

            stream.Flush();
            stream.Close();
        }
    }
}
