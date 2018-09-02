using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Ink.Runtime;

namespace InkGlue
{
	public class GlueStory
	{
		[DllImport("__Internal")]
		public static extern void ObserverCallbackInt(int instanceId, [MarshalAs(UnmanagedType.LPStr)] string variableName, int newValue);

		[DllImport("__Internal")]
		public static extern void ObserverCallbackFloat(int instanceId, [MarshalAs(UnmanagedType.LPStr)] string variableName, float newValue);

		[DllImport("__Internal")]
		public static extern void ObserverCallbackString(int instanceId, [MarshalAs(UnmanagedType.LPStr)] string variableName, string newValue);

		public GlueStory(string JsonString, int instanceId)
		{
			_story = new Story(JsonString);
			_instanceId = instanceId;
		}

		public bool CanContinue()
		{
			return _story.canContinue;
		}

		public string Continue()
		{
			return _story.Continue();
		}

		public GlueChoice[] CurrentChoices()
		{
			GlueChoice[] glueChoices = new GlueChoice[_story.currentChoices.Count];
			for (int i = 0; i < _story.currentChoices.Count; i++)
			{
				glueChoices[i] = new GlueChoice(_story.currentChoices[i]);
			}
			return glueChoices;
		}

		public void ChooseChoiceIndex(int index)
		{
			_story.ChooseChoiceIndex(index);
		}

		public string CurrentText()
		{
			return _story.currentText;
		}

		public GlueStoryState State()
		{
			return new GlueStoryState(_story.state);
		}

		public bool HasError()
		{
			return _story.hasError;
		}

		public string[] CurrentTags()
		{
			return _story.currentTags.ToArray();
		}

		public string[] CurrentErrors()
		{
			return _story.currentErrors.ToArray();
		}

		public void ResetState()
		{
			_story.ResetState();
		}

		public void ResetErrors()
		{
			_story.ResetErrors();
		}

		public void ResetCallstack()
		{
			_story.ResetCallstack();
		}

		public void ContinueAsync(float millisecondLimitAsync)
		{
			_story.ContinueAsync(millisecondLimitAsync);
		}

		public string ContinueMaximally()
		{
			return _story.ContinueMaximally();
		}

		public bool AsyncContinueComplete()
		{
			return _story.asyncContinueComplete;
		}

		public string[] TagsForContentAtPath(string path)
		{
			List<string> tags = _story.TagsForContentAtPath(path);
			if (tags == null)
				return new string[0];
			else
				return tags.ToArray();
		}

		public void ChoosePathString(string path, object[] arguments, bool resetCallStack = true)
		{
			_story.ChoosePathString(path, resetCallStack, arguments);
		}

		void InternalObserve(string variableName, object newValue)
		{
			if (newValue is int) {
				ObserverCallbackInt(_instanceId, variableName, (int)newValue);
			} else if (newValue is float){
				ObserverCallbackFloat(_instanceId, variableName, (float)newValue);
			} else if (newValue is string){
				ObserverCallbackString(_instanceId, variableName, (string)newValue);
			}
		}

		public void ObserveVariable(string variableName)
		{
			_story.ObserveVariable(variableName, InternalObserve);
		}

		public void RemoveVariableObserver(string variableName)
		{
			_story.RemoveVariableObserver(InternalObserve, variableName);
		}

		Story _story;
		int _instanceId = -1;
	}
}
