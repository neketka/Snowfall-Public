using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Drawing.Imaging;
using Newtonsoft.Json.Linq;
using System.IO;

namespace AssetImporter
{
    public class FontImporter
    {
        public static void Convert(string internalPath, string inputFile, string targetFolder)
        {
            JObject obj = JObject.Parse(File.ReadAllText(inputFile));
            FontFileFormat format = new FontFileFormat();
            string path = Path.GetDirectoryName(inputFile) + "\\" +  Path.GetFileNameWithoutExtension(inputFile) + ".fntpng";
            Bitmap bmp = new Bitmap(path);
            bmp.RotateFlip(RotateFlipType.RotateNoneFlipY);
            BitmapData data = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height), ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);

            format.FontName = obj["name"].Value<string>();
            format.Width = obj["width"].Value<int>();
            format.Height = obj["height"].Value<int>();
            format.Size = obj["size"].Value<int>();
            format.ImageType = FontImageType.R8DistanceField;
            format.AtlasData = new byte[bmp.Width * bmp.Height];          

            unsafe
            {
                byte *ptr = (byte *)data.Scan0.ToPointer();
                for (int i = 0; i < format.AtlasData.Length; ++i)
                    format.AtlasData[i] = ptr[i * 3];
            }

            bmp.UnlockBits(data);

            JObject chars = (JObject)obj["characters"];
            
            foreach (JProperty prop in chars.Properties())
            {
                JToken cdesc = prop.Value;
                GlyphDescription desc;

                desc.Character = prop.Name[0];
                desc.AtlasX = cdesc.Value<int>("x");
                desc.AtlasY = cdesc.Value<int>("y");
                desc.Width = cdesc.Value<int>("width");
                desc.Height = cdesc.Value<int>("height");
                desc.OffsetX = cdesc.Value<int>("originX");
                desc.OffsetY = cdesc.Value<int>("originY");
                desc.Advance = cdesc.Value<int>("advance");

                format.Glyphs.Add(desc);
            }

            format.Export(internalPath, targetFolder);
        }
    }
}
