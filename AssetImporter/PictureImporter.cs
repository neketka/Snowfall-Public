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
            format.Type = TextureType.Texture2D;
            if (internalPath.Contains("_CUBE"))
            {
                format.Type = TextureType.TextureCubemap;
                internalPath = internalPath.Replace("_CUBE", "");
            }

            MipmapLevel level = new MipmapLevel();
            level.Level = 0;

            if (format.Type == TextureType.Texture2D)
            {
                format.Format = TexturePixelFormat.BGRA;
                Bitmap bmp = new Bitmap(inputFile);
                bmp.RotateFlip(RotateFlipType.RotateNoneFlipY);
                BitmapData data = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
                level.Data = new byte[data.Stride * data.Height];
                System.Runtime.InteropServices.Marshal.Copy(data.Scan0, level.Data, 0, level.Data.Length);
                format.BaseWidth = bmp.Width;
                format.BaseHeight = bmp.Height;
            }
            else if (format.Type == TextureType.TextureCubemap)
            {
                format.Format = TexturePixelFormat.BGRA;
                Bitmap bmp = new Bitmap(inputFile);
                bmp.RotateFlip(RotateFlipType.RotateNoneFlipY);
                int faceSize = Math.Min(bmp.Width / 4, bmp.Height / 3);
                level.Data = new byte[faceSize * faceSize * 6 * 4];
                Rectangle[] faces = new Rectangle[]
                {
                    new Rectangle(faceSize * 2, faceSize, faceSize, faceSize), //+X
                    new Rectangle(0, faceSize, faceSize, faceSize), //-X
                    new Rectangle(faceSize, 0, faceSize, faceSize), //+Y 
                    new Rectangle(faceSize, faceSize * 2, faceSize, faceSize), //-Y
                    new Rectangle(faceSize, faceSize, faceSize, faceSize), //+Z
                    new Rectangle(faceSize * 3, faceSize, faceSize, faceSize) //-Z
                };
                for (int i = 0; i < 6; ++i)
                {
                    Bitmap face = bmp.Clone(faces[i], PixelFormat.Format32bppArgb);
                    BitmapData data = face.LockBits(new Rectangle(0, 0, faceSize, faceSize), ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
                    System.Runtime.InteropServices.Marshal.Copy(data.Scan0, level.Data, faceSize * faceSize * 4 * i, faceSize * faceSize * 4);
                    face.UnlockBits(data);
                }
                format.BaseWidth = faceSize;
                format.BaseHeight = faceSize;
            }

            format.MipmapLevels.Add(level);
            format.Export(internalPath, targetFolder);
        }
    }
}
