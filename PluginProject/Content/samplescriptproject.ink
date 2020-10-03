VAR gameAction = ""

Lonely Mountain, Outpost 21, 1872
Katala's Alchemy Shop
-> day1

=== day1 ===
Misterious Stranger: "So, are you back in business?"

+ "Running low on coin. I have no other options"
	-> coin
+ [...] -> silent

=== coin ===
Misterious Stranger: "Heh, so I guess you've forgotten how many lifes were destroyed from your craft?"
Misterious Stranger: "Anyways, word has spread that you have re-opened. Some far-away travelers are in town looking for you"
Misterious Stranger: "I'll be back tomorrow with the latest. Good luck, Katala"
    -> client1day1

+ "May the Mountain Spirit be with you"
    -> ending


=== silent ===
Misterious Stranger: "I understand. The past has been harsh with you. There's new people around. They are waiting for you. Good luck. 
-> client1day1


=== client1day1 ===
Melia, The Whimsical Celestial: "Howdy! What a lovely shop you got! Are you the Alchemist everyone is talking about?
+ "I guess so..."
   -->iguessso

   
=== iguessso ===
Melia, The Whimsical Celestial: "Beautiful! It has been a while since someone has dared to work on the craft again. Now that you are here, I think I can finally complete my purpose
+ "What would that purpose be"
   --> purposeclient1

=== purposeclient1 ===
Melia, The Whimsical Celestial: "I need to kill a Mountain Folk spellbender"
 + "You serious? Why?"
   -->purposeclient1why
   =purposeclient1why
   Melia, The Whimsical Celestial: "Does it matter?"
   Katala: "Not really..."
   Melia: "So, they say you can craft with the long forgotten recipes. Can you create one for me? 
   Katala: "That depends. Tell me more about this spellbender."
   Melia, The Whimsical Celestial: "Hmm, I don't know much about him. Just that it has a scar across his face. Stories tell that he's fast. Very fast. Other Celestials have either died or came back with frostbitten body parts. Not a very fun sight. 
   Katala: "I'll see what I can do"
   ~ gameAction = "craft"
   Melia, The Whimsical Celestial": Oh how lovely, I'll wait here, buttercup!"
   -->ending
=== ending
Melia, The Whimsical Celestial: "Oooh!, pretty colors! Thank you, Alchemist. 
-> END
