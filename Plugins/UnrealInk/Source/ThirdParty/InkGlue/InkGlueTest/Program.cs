using System;
using System.IO;
using System.Linq;
using InkGlue;

namespace InkGlueTest
{
    class Program
    {   
        // Ink Glue Test
        // Testing program especially for testing specific things in the managed world and debugging C# code
        // -------------------------------------------------------------------------------------------------


        static void Main(string[] args)
        {
            string text = File.ReadAllText(@"C:\Dev\UnrealInkExample\Content\MonsieurFog.ink");

            InkGlue.GlueInkCompiler compiler = new InkGlue.GlueInkCompiler(text, "MonsieurFog.ink");

            string json = compiler.CompileToJson();

            GlueStory story = new GlueStory(json, 1);

            story.ObserveVariable("fogs_string_mood");
            story.ObserveVariable("fogs_current_mood");
            story.RemoveVariableObserver("fogs_string_mood");

            while(story.CanContinue())
            {
                Console.WriteLine(story.ContinueMaximally());

                if (story.CurrentChoices().Length > 0)
                {
                    for (int i = 0; i < story.CurrentChoices().Length; i++)
                    {
                        GlueChoice choice = story.CurrentChoices()[i];
                        Console.WriteLine(i + " " + choice.Text());
                    }
                }
                Console.WriteLine("Enter Choice: ");
                string chosen = Console.ReadLine();
                int chosenIndex = Int32.Parse(chosen);
                story.ChooseChoiceIndex(chosenIndex);
            }

            //int choice = Console.ReadLine();
        }
    }
}
