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
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    public struct InkVarInterop
    {
        [MarshalAs(UnmanagedType.U1)]
        public byte Type;

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
                Type = 0;
            }
            else if (arg is int)
            {
                IntVal = (int)arg;
                Type = 1;
            }
            else if (arg is string)
            {
                StringVal = (string)arg;
                Type = 2;
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
                    case 0:
                        return FloatVal;
                    case 1:
                        return IntVal;
                    case 2:
                        return StringVal;
                    default:
                    case 3:
                        return null;
                }
            }
        }
    }

    public class GlueStory
	{
		[DllImport("__Internal")]
		public static extern void ObserverCallback(int instanceId, [MarshalAs(UnmanagedType.LPStr)] string variableName, InkVarInterop newValue);

        [DllImport("__Internal")]
        public static extern InkVarInterop ExternalFunctionCallback([MarshalAs(UnmanagedType.I4)] int instanceId, [MarshalAs(UnmanagedType.LPStr)] string functionName, [MarshalAs(UnmanagedType.U4)] uint numArguments, [MarshalAs(UnmanagedType.LPArray)] InkVarInterop[] arguments);

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

        object InternalFunctionHandler(string functionName, object[] args)
        {
            // Convert arguments into Interop Variable structs
            InkVarInterop[] cArgs = new InkVarInterop[args.Length];
            for (int i = 0; i < args.Length; i++)
            {
                cArgs[i] = new InkVarInterop(args[i]);
            }

            // Call the callback
            InkVarInterop var = ExternalFunctionCallback(_instanceId, functionName, (uint)cArgs.Length, cArgs);

            return var.BoxedValue;
        }

        public void RegisterFunction(string functionName)
        {
            // Bind to internal C# method
            _story.BindExternalFunctionGeneral(functionName, args => InternalFunctionHandler(functionName, args));
        }

        public void UnregisterFunction(string functionName)
        {
            // Unbind
            _story.UnbindExternalFunction(functionName);
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
