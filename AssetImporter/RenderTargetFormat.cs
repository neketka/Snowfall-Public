using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace AssetImporter
{
    public enum TextureAssetMode
    {
        Internal = 0, ImportAsset = 1, ExportAsset = 2
    }

    public enum TextureInternalFormat
    {
        Depth16I = 0x81A5, Depth24I = 0x81A6, Depth32I = 0x81A7, Depth32F = 0x8CAC,
	    Depth24IStencil8UI = 0x88F0, Depth32FStencil8UI = 0x8CAD,
	    Stencil8UI = 0x8D48,
	    R8 = 0x8229, RG8 = 0x822B, RGB8 = 0x8051, RGBA8 = 0x8058,
	    R8I = 0x8231, RG8I = 0x8237, RGB8I = 0x8D8F, RGBA8I = 0x8D8F,
	    R16I = 0x8233, RG16I = 0x8239, RGB16I = 0x8D89, RGBA16I = 0x8D88,
	    R16F = 0x822D, RG16F = 0x822F, RGB16F = 0x881B, RGBA16F = 0x881A,
	    R32F = 0x822E, RG32F = 0x8230, RGB32F = 0x8815, RGBA32F = 0x8814
    };

    public class RenderTextureData
    {
        public TextureAssetMode AssetMode;
        
        public TextureType Type;
        public TextureInternalFormat Format;

        public int Width;
        public int Height;
        public int Depth;

        public string AssetName; //Internal skips this field
    }

    public class TextureLayerAttachment
    {
        public int TextureIndex;
        public int Level;
        public int Layer;
    }

    public class RenderTargetFormat
    {
        public List<RenderTextureData> TextureAssets;
        public List<TextureLayerAttachment> Attachments;

        public RenderTargetFormat()
        {
            TextureAssets = new List<RenderTextureData>();
            Attachments = new List<TextureLayerAttachment>();
        }

        public void Export(string internalPath, string folder)
        {
            string name = internalPath.Split('.').Last();
            string file = folder + "\\" + name + ".rtasset";
            if (File.Exists(file))
                File.Delete(file);
            FileStream stream = File.Open(file, FileMode.CreateNew);
            BinaryWriter writer = new BinaryWriter(stream);

            writer.Write((UInt32)internalPath.Length);
            writer.Write(internalPath.ToCharArray());

            writer.Write((int)TextureAssets.Count);
            writer.Write((int)Attachments.Count);

            foreach (RenderTextureData data in TextureAssets)
            {
                writer.Write((int)data.AssetMode);
                writer.Write((int)data.Type);
                writer.Write((int)data.Format);
                writer.Write(data.Width);
                writer.Write(data.Height);
                writer.Write(data.Depth);
                if (data.AssetMode != TextureAssetMode.Internal)
                {
                    writer.Write((UInt32)data.AssetName.Length);
                    writer.Write(data.AssetName.ToCharArray());
                }
            }

            foreach (TextureLayerAttachment attachment in Attachments)
            {
                writer.Write(attachment.TextureIndex);
                writer.Write(attachment.Level);
                writer.Write(attachment.Layer);
            }

            writer.Flush();
            writer.Close();
        }
    }
}
