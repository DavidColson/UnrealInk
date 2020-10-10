using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Ink.Runtime;

namespace InkGlue
{
	public enum InkVarType
    {
        Float,
        Int,
        String,
        None
    }

    // Don't change structure of this without changing matching C++ interop struct
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct InkVarInterop
    {
        [MarshalAs(UnmanagedType.U1)]
        public InkVarType Type;

        [MarshalAs(UnmanagedType.R4)]
        public float FloatVal;

        [MarshalAs(UnmanagedType.I4)]
        public int IntVal;

        [MarshalAs(UnmanagedType.LPStr)]
        public string StringVal;

        public InkVarInterop(object arg)
        {
            FloatVal = 0; IntVal = 0; StringVal = null;

            if (arg is float)
            {
                FloatVal = (float)arg;
                Type = InkVarType.Float;
            }
            else if (arg is int)
            {
                IntVal = (int)arg;
                Type = InkVarType.Int;
            }
            else if (arg is string)
            {
                StringVal = (string)arg;
                Type = InkVarType.String;
            }
            else
            {
                throw new Exception("Invalid Ink Variable Type: " + arg.GetType());
            }
        }

        public object BoxedValue
        {
            get
            {
                switch (Type)
                {
                    case InkVarType.Float:
                        return FloatVal;
                    case InkVarType.Int:
                        return IntVal;
                    case InkVarType.String:
                        return StringVal;
                    default:
                    case InkVarType.None:
                        return null;
                }
            }
        }
    }

    public class GlueStory
	{
		[DllImport("__Internal")]
		public static extern void ObserverCallback(int instanceId, [MarshalAs(UnmanagedType.LPStr)] string variableName, InkVarInterop newValue);

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

        public bool HasWarning()
        {
            return _story.hasWarning;
        }

        public string[] CurrentTags()
		{
			return _story.currentTags.ToArray();
		}

        public string[] GlobalTags()
        {
            return _story.globalTags.ToArray();
        }

        public string[] CurrentErrors()
		{
			return _story.currentErrors.ToArray();
		}

        public string[] CurrentWarnings()
        {
            return _story.currentWarnings.ToArray();
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

        public GlueVariablesState VariablesState()
        {
            return new GlueVariablesState(_story.variablesState);
        }

        void InternalObserve(string variableName, object newValue)
		{
			ObserverCallback(_instanceId, variableName, new InkVarInterop(newValue));
		}

		public void ObserveVariable(string variableName)
		{
			_story.ObserveVariable(variableName, InternalObserve);
		}

		public void RemoveVariableObserver(string variableName)
		{
			_story.RemoveVariableObserver(InternalObserve, variableName);
		}

		public bool HasFunction(string functionName)
        {
			return _story.HasFunction(functionName);
        }

		public object EvaluateFunction(string functionName, out string textOutput, object[] arguments)
        {
			return _story.EvaluateFunction(functionName, out textOutput, arguments);
        }

		public string BuildStringOfHeirarchy()
        {
			return _story.BuildStringOfHierarchy();
        }

        public GlueStoryState CopyStateForBackgroundThreadSave()
        {
            return new GlueStoryState(_story.CopyStateForBackgroundThreadSave());
        }

        public void BackgroundSaveComplete()
        {
            _story.BackgroundSaveComplete();
        }

        Story _story;
		int _instanceId = -1;
	}
}
