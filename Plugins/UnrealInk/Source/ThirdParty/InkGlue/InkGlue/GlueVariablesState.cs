using Ink.Runtime;
using System.Linq;

namespace InkGlue
{
    public class GlueVariablesState
    {
        internal GlueVariablesState(VariablesState inVariablesState)
        {
            _variablesState = inVariablesState;
        }

        public string[] GetVariables()
        {
            return _variablesState.ToArray();
        }

        public object GetVariable(string name)
        {
            return _variablesState[name];
        }

        public void SetVariable(string name, object value)
        {
            _variablesState[name] = value;
        }

        VariablesState _variablesState;
    }
}