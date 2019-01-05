using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Drawing;
using ImageProcessor;
using ImageProcessor.Imaging.Formats;

namespace AssetImporter
{
    public class MipmapLevel
    {
        public int Level;
        public byte[] Data;
    }

    public enum TextureType : int
    {
        Texture1D=0, Texture2D=1, Texture2DArray=2, TextureCubemap=3, TextureCubemapArray=4, Texture3D=5
    }

    public enum TexturePixelFormat : int
    {
        R8I=0, R32F=1, RG8I=2, RG32F=3, RGB8I=4, BGR8I=5, RGB32F=6, BGRA8I=7, ARGB8I=8, RGBA8I=9, RGBA32F=10
    }

    public class TextureFileFormat
    {
        public List<MipmapLevel> MipmapLevels;

        public TextureType Type;
        public TexturePixelFormat Format;
        public int BaseWidth;
        public int BaseHeight;
        public int BaseDepth;
        public int FrameCount;
        public float FramesPerSecond;

        public TextureFileFormat()
        {
            MipmapLevels = new List<MipmapLevel>();
            BaseWidth = 1;
            BaseHeight = 1;
            BaseDepth = 1;
            FrameCount = 1;
            FramesPerSecond = 0;
            Type = TextureType.Texture2D;
            Format = TexturePixelFormat.ARGB8I;
        }

        public void Export(string internalPath, string folder)
        {
            string file = folder + "\\" + internalPath.Split('.').Last() + ".tasset";
            if (File.Exists(file))
                File.Delete(file);
            FileStream stream = File.Open(file, FileMode.CreateNew);
            MipmapLevels.Sort((a, b) => { return Math.Sign(b.Level - a.Level); });
            BinaryWriter writer = new BinaryWriter(stream);

            writer.Write((UInt32)internalPath.Length);
            writer.Write(internalPath.ToCharArray());

            writer.Write((int)Type);
            writer.Write((int)Format);
            writer.Write(BaseWidth);
            writer.Write(BaseHeight);
            writer.Write(BaseDepth);
            writer.Write(FrameCount);
            writer.Write(FramesPerSecond);
            writer.Write(MipmapLevels.Count);
            
            foreach (MipmapLevel level in MipmapLevels)
            {
                writer.Write(level.Level);
                writer.Write(level.Data.Length);
                writer.Write(level.Data);
            }

            stream.Flush();
            stream.Close();
        }
    }
}
