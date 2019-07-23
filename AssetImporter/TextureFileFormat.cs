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
        Texture2D=2, Texture2DArray=3, TextureCubemap=4, TextureCubemapArray=5, Texture3D=6
    }

    public enum TexturePixelFormat : int
    {
        R = 0x1903, RG = 0x8227, RGB = 0x1907, BGR = 0x80E0, RGBA = 0x1908,
        BGRA = 0x80E1, Depth = 0x1902, Stencil = 0x1901
    }

    public class TextureFileFormat
    {
        public List<MipmapLevel> MipmapLevels;

        public TextureType Type;
        public TexturePixelFormat Format;
        public bool IsFloatFormat;
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
            Format = TexturePixelFormat.BGRA;
        }

        public void Export(string internalPath, string folder)
        {
            string file = folder + "\\" + internalPath.Replace(".", "") + ".tasset";
            if (File.Exists(file))
                File.Delete(file);
            FileStream stream = File.Open(file, FileMode.CreateNew);
            MipmapLevels.Sort((a, b) => { return Math.Sign(b.Level - a.Level); });
            BinaryWriter writer = new BinaryWriter(stream);

            writer.Write((UInt32)internalPath.Length);
            writer.Write(internalPath.ToCharArray());

            writer.Write((int)Type);
            writer.Write((int)Format);
            writer.Write(IsFloatFormat ? 1 : 0);
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
