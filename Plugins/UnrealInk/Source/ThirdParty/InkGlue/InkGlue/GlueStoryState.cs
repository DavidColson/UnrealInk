using System;
using System.Collections.Generic;
using System.Text;
using Ink.Runtime;

namespace InkGlue
{
    public class GlueStoryState
    {
		internal GlueStoryState(StoryState inStoryState)
		{
			_storyState = inStoryState;
		}

		public int InkSaveStateVersion()
		{
			return StoryState.kInkSaveStateVersion;
		}

		public string ToJson()
		{
			return _storyState.ToJson();
		}

		public void LoadJson(string json)
		{
			_storyState.LoadJson(json);
		}

		public int VisitCountAtPathString(string pathString)
		{
			return _storyState.VisitCountAtPathString(pathString);
		}

		public void ForceEnd()
		{
			_storyState.ForceEnd();
		}

		public string CurrentPathString()
        {
			return _storyState.currentPathString;
        }

		StoryState _storyState;
    }
}
