using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace AssetImporter
{
    public class ShaderFileFormat
    {
        public ShaderFileFormat()
        {
        }

        public void Export(string internalPath, string folder)
        {
            string file = folder + "\\" + internalPath.Replace(".", "") + ".sasset";
            if (File.Exists(file))
                File.Delete(file);
            FileStream stream = File.Open(file, FileMode.CreateNew);
            //Source = Source.Replace("\r\n", "\n");
            BinaryWriter writer = new BinaryWriter(stream);
            writer.Write((UInt32)internalPath.Length);
            writer.Write(internalPath.ToCharArray());
            writer.Write((UInt32)Source.Length);
            writer.Write(Source.ToCharArray());

            stream.Flush();
            stream.Close();
        }

        public string Source;
    }
}
