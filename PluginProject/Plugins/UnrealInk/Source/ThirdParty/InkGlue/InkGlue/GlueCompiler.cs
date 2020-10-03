using System;
using System.IO;
using System.Collections.Generic;
using System.Text;
using Ink.Runtime;
using Ink;

namespace InkGlue
{
	public class GlueInkCompiler: Ink.IFileHandler
	{
		public string ResolveInkFilename(string includeName)
		{
			var workingDir = Directory.GetCurrentDirectory();
			var fullRootInkPath = Path.Combine(workingDir, includeName);
			return fullRootInkPath;
		}

		public string LoadInkFileContents(string fullFilename)
		{
			return File.ReadAllText(fullFilename);
		}

		public GlueInkCompiler(string storyFileContents, string storyFileName)
		{
			// Create compiler object
			compiler = new Compiler(storyFileContents, new Compiler.Options
			{
				sourceFilename = storyFileName,
				pluginNames = null,
				countAllVisits = false,
				errorHandler = null, // TODO: Make an actual error handler that's useful
				fileHandler = this
			});

			story = compiler.Compile();
		}

		public string CompileToJson()
		{
			return story.ToJsonString();
		}

		Story story;
		Compiler compiler;
	}
}
