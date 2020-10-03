using System;
using System.Collections.Generic;
using System.Text;
using Ink.Runtime;


namespace InkGlue
{
    public class GlueChoice
    {
		internal GlueChoice(Choice inChoice)
		{
			_choice = inChoice;
		}

		public string Text()
		{
			return _choice.text;
		}

		public string PathStringOnChoice()
		{
			return _choice.pathStringOnChoice;
		}

		public string SourcePath()
		{
			return _choice.sourcePath;
		}

		public int Index()
		{
			return _choice.index;
		}

		Choice _choice;
    }
}
