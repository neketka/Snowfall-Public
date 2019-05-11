using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace AssetImporter
{
    public struct GlyphDescription
    {
        public char Character;
        public int Width;
        public int Height;
        public int AtlasX;
        public int AtlasY;
        public int OffsetX;
        public int OffsetY;
        public int Advance;
    }

    public enum FontImageType
    {
        R8Bitmap=0, R8DistanceField=1, RGB8DistanceField=3, RGBA8DistanceField=4
    }

    public class FontFileFormat
    {
        public string FontName;
        public int Width;
        public int Height;
        public int Size;
        public List<GlyphDescription> Glyphs;
        public FontImageType ImageType;
        public byte[] AtlasData;

        public FontFileFormat()
        {
            Glyphs = new List<GlyphDescription>();
        }
        
        public void Export(string internalPath, string folder)
        {
            string file = folder + "\\" + internalPath.Split('.').Last() + ".fntasset";
            if (File.Exists(file))
                File.Delete(file);
            FileStream stream = File.Open(file, FileMode.CreateNew);

            BinaryWriter writer = new BinaryWriter(stream);

            writer.Write((UInt32)internalPath.Length);
            writer.Write(internalPath.ToCharArray());

            writer.Write((UInt32)FontName.Length);
            writer.Write(FontName.ToCharArray());

            writer.Write(Width);
            writer.Write(Height);
            writer.Write((UInt32)ImageType);
            writer.Write(AtlasData.Length);
            writer.Write(Glyphs.Count);

            writer.Write(AtlasData);
            foreach (GlyphDescription desc in Glyphs)
            {
                writer.Write((byte)desc.Character);

                writer.Seek(3, SeekOrigin.Current);

                writer.Write(desc.Width / (float)Width); // On scale of the atlas
                writer.Write(desc.Height / (float)Height);

                writer.Write(desc.AtlasX / (float)Width); // On scale of the atlas
                writer.Write(1f - desc.AtlasY / (float)Height);

                writer.Write(desc.Width / (float)Size); // On scale of the char
                writer.Write(desc.Height / (float)Size);

                writer.Write(desc.OffsetX / (float)Size); // On scale of the char
                writer.Write(desc.OffsetY / (float)Size);

                writer.Write(desc.Advance / (float)Size); // On scale of the char
            }

            stream.Flush();
            stream.Close();
        }
    }
}
