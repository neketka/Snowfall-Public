using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Drawing;
using System.Drawing.Imaging;

namespace AssetImporter
{
    public class PictureImporter
    {
        public static void Convert(string internalPath, string inputFile, string targetFolder)
        {
            TextureFileFormat format = new TextureFileFormat();
            format.Format = TexturePixelFormat.BGRA;
            format.Type = TextureType.Texture2D;

            MipmapLevel level = new MipmapLevel();
            level.Level = 0;

            Bitmap bmp = new Bitmap(inputFile);
            bmp.RotateFlip(RotateFlipType.RotateNoneFlipY);
            BitmapData data = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            level.Data = new byte[data.Stride * data.Height];
            System.Runtime.InteropServices.Marshal.Copy(data.Scan0, level.Data, 0, level.Data.Length);

            format.BaseWidth = bmp.Width;
            format.BaseHeight = bmp.Height;
            format.MipmapLevels.Add(level);
            format.Export(internalPath, targetFolder);
        }
    }
}
