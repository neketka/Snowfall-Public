using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace AssetImporter
{
    public class Program
    {
        static int successes = 0;
        static int failures = 0;
        static string output = ".\\";

        static void TryFile(string path, string internalPath)
        {
            try
            {
                switch(Path.GetExtension(path))
                {
                    case ".obj":
                        Console.Write("Converting " + Path.GetFileName(path) + " to mesh asset...");
                        OBJImporter.Convert(internalPath, path, output);
                        break;
                    case ".glsl":
                        Console.Write("Converting " + Path.GetFileName(path) + " to shader asset...");
                        GLSLImporter.Convert(internalPath, path, output);
                        break;
                    case ".jpg":
                    case ".jpeg":
                    case ".png":
                    case ".gif":
                        Console.Write("Converting " + Path.GetFileName(path) + " to texture asset...");
                        PictureImporter.Convert(internalPath, path, output);
                        break;
                    case ".fntjson":
                        Console.Write("Converting " + Path.GetFileName(path) + " to font asset...");
                        FontImporter.Convert(internalPath, path, output);
                        break;
                    default:
                        return;
                }
                Console.WriteLine("success");
                ++successes;
            }
            catch (Exception e)
            {
                Console.WriteLine("failure");
                Console.WriteLine(e.ToString());
                ++failures;
            }
        }

        static void WalkDirectory(string path, string internalPath, bool nonRecursive)
        {
            foreach (string file in Directory.GetFiles(path))
                TryFile(file, internalPath + (internalPath == "" ? "" : ".") + Path.GetFileNameWithoutExtension(file));
            if (nonRecursive)
                return;
            foreach (string dir in Directory.GetDirectories(path))
            {
                WalkDirectory(dir, (internalPath == "" ? "" : ".") + Path.GetFileNameWithoutExtension(dir), false);
            }
        }

        public static void Main(string[] args)
        {
            List<string> inDirectories = new List<string>();
            List<string> inDirectoriesRecursive = new List<string>();
            List<string> inFiles = new List<string>();
            for (int i = 0; i < args.Length; ++i)
            {
                switch(args[i])
                {
                    case "-d":
                        inDirectories.Add(args[++i]);
                        break;
                    case "-dr":
                        inDirectoriesRecursive.Add(args[++i]);
                        break;
                    case "-f":
                        inFiles.Add(args[++i]);
                        break;
                    case "-o":
                        output = args[++i];
                        break;
                }
            }
            if (!Directory.Exists(output))
            {
                Console.WriteLine("Please specify existing folder for output.");
                return;
            }
            if (inDirectories.Count == 0)
                inDirectories.Add(".\\");
            foreach (string dir in inDirectories)
                WalkDirectory(dir, "", true);
            foreach (string dir in inDirectoriesRecursive)
                WalkDirectory(dir, "", false);
            foreach (string file in inFiles)
                TryFile(file, Path.GetFileNameWithoutExtension(file));
            Console.WriteLine(successes.ToString() + " succeeded, " + failures + " failed");
        }
    }
}
