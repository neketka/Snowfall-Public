using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace AssetImporter
{
    public class GLSLImporter
    {
        public static void Convert(string internalPath, string inputFile, string targetFolder)
        {
            ShaderFileFormat format = new ShaderFileFormat();
            format.Source = File.ReadAllText(inputFile);
            format.Export(internalPath, targetFolder);
        }
    }
}
