# this is a global tags
# another tag

VAR fogs_string_mood = "good"
VAR fogs_current_mood = 1

- I looked at Monsieur Fogg 
*   ... and I could contain myself no longer.
    'What is the purpose of our journey, Monsieur?'
    ~ fogs_string_mood = "excellent"
    ~ fogs_current_mood = 2
    'A wager,' he replied. 
    * *     'A wager!'[] I returned.
            He nodded. 
            * * *   'But surely that is foolishness!'
            * * *  'A most serious matter then!'
            - - -   He nodded again.
            TODO: Write this properly
            * * *   'But can we win?'
                    'That is what we will endeavour to find out,' he answered.
                    ~ fogs_string_mood = "good"
                    ~ fogs_current_mood = 1
            * * *   'A modest wager, I trust?'
                    'Twenty thousand pounds,' he replied, quite flatly.
            * * *   I asked nothing further of him then[.], and after a final, polite cough, he offered nothing more to me. <>
    * *     'Ah[.'],' I replied, uncertain what I thought.
    - -     After that, <>
*   ... but I said nothing[] and <> 
- we passed the day in silence
- -> END

=== function say_yes_to_everything ===
    ~ return true

=== function lerp(a, b, k) ===
    ~ return ((b - a) * k) + a