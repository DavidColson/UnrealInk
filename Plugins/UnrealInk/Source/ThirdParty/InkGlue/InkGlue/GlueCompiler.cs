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
			errors = new List<string>();
			warnings = new List<string>();
            authorMessages = new List<string>();

            // Create compiler object
            compiler = new Compiler(storyFileContents, new Compiler.Options
			{
				sourceFilename = storyFileName,
				pluginNames = null,
				countAllVisits = false,
				errorHandler = OnError,
				fileHandler = this
			});

			story = compiler.Compile();
		}

		public string CompileToJson()
		{
			if (story)
				return story.ToJson();
			return "";
		}
        public string[] GetErrors()
        {
            return errors.ToArray();
        }

        public string[] GetWarnings()
        {
            return warnings.ToArray();
        }

        public string[] GetAuthorMessages()
        {
            return authorMessages.ToArray();
        }

        private void OnError(string message, ErrorType type)
		{
			switch (type)
            {
                case ErrorType.Author:
                    authorMessages.Add(message);
                    break;

                case ErrorType.Warning:
                    warnings.Add(message);
                    break;

                case ErrorType.Error:
                    errors.Add(message);
                    break;
            }
		}

        Story story;
		Compiler compiler;
		List<string> errors;
        List<string> warnings;
        List<string> authorMessages;
    }
}
