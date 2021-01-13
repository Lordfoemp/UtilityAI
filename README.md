# Utility Theory - AI
This is my research project for Gameplay Programming in DAE (= Digital Arts & Entertainment). 
In short, an other way of decision making in games.

What is Utility Theory in Game AI or also known as Utility AI?
Utility Theory is a way of making decisions that leans on weights for each action the AI can take.

1. The start
The process starts with thinking about all the actions the AI can make. 

2. Each action
When looking at each action, the agent gathers all the data it needs, just like a human being looks around in the world.
If the agent has all its data, then it starts looking at all it Considerations. 

3. Each consideration
is the AI thinking about what is important to that action.
Example: You encounter an enemy in front of you
- Consideration 1: Are you holding a gun?      (Yes or No)
- Consideration 2: How far away is the enemy?  (0 - ??? meters)
The data from those considerations are used as input for the scoring curves.

4. Scoring curves
The scoring curves are used to determine the importance of every consideration at any given moment.
Example: Remember that enemy of the previous example?
If you look at the distance between the agent and the enemy.
You have more urge te run away when the enemy is really close then when the enemy is far away. 
So if you put it into a linear function, the urge of the agent climbs steadyly.
But if you put the same distance into an exponential function, then the urge climbs very vast when the enemy gets really close.

The weights for every consideration are taken together and the best action is chosen with the highest value being the best action.
