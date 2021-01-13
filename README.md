# Abstract Goal Action Planning - Jonas Briers
## Original idea
My original idea was to implement Goal Oriented Action Planning (GOAP).
GOAP is able to determine the most efficient way to achieve a certain goal.
It does this by creating a navmesh where all nodes are the different actions the agent can do. 
Every action contains certain preconditions and consequences (boolean values), it uses this to construct the navmesh.
Every action also contains a cost, this cost is used as the cost of the connection in the navmesh.
GOAP will then use some pathfinding (typically A*) to find the shortest path from the current world state to the goal world state.
## Limitations of GOAP
Goap has some limitations that make it not ideal for my project:
 - conditions and consequences have to be boolean values (or at least I haven't seen an implementation where this wasn't the case).
 - A strict goal has to be defined.
In my project I don't have a strict goal, the goal is loosely defined as "don't die, hopefully get some kills and pick up items".
This made me think if I could adapt GOAP to meet the neads of this project better.
## The solution
1 possible solution was to have 2 AIs: 1 behavior thee that defines what the goal is and 1 GOAP that tries to achieve that goal.
However, I didn't like this approach as it negates the advantages of using AGAP, that's why I tried to modify AGAP to fit my needs better.
I made some important changes to the original algorythm:
 - A worldState could contain anything (bools, ints, floats or even a Vector2 or string).
 - Instead of a clear goal I will define a function that evalueates a worldState and gives it a score.
 - It will use a depth first algorythm to determine the path with the highest possible score.
Because these changes seemed to different from GOAP I decided to rename my algorythm to "Abstract Goal Action Planning (AGAP)".
I have tried to find if a similar algorythm already exists, but I couldn't find it (it's difficult to look for something if you don't know the name).
## Implementation
### World State
I have a very simple implementation of a worldState: it's just a struct containing all the variables that I need. The reason I do this is because I want to easilly ba able to copy and compare my worldState.
```cpp
// Just an example, my actual worldState contains more variables
struct WorldState
{
  uint32_t health;
  uint32_t energy;
  uint8_t pistolsInInventory
  uint8_t medkitsInInventory
  bool IsHurt;
}
```
### Actions
Every action inherits from my IAction interface:
```cpp
class IAction
{
public:
	IAction( InterfaceWrapper& examInterface, std::string&& descriptor );

	virtual float GetCost( ) const = 0; // Every action has a certain cost
	virtual bool IsAvailable( const WorldState& worldState ) const = 0; // Checks if this action is available based on the current worldState
	virtual WorldState GetResult( WorldState worldState ) = 0; // Modifies the worldState depending on what this action does
	virtual void Update( float dt ) = 0; // Executes the action
	virtual void Start( ) = 0; // Gets called the first time this action starts

	const std::string& GetDescriptor( ) const;
protected:
	std::reference_wrapper<InterfaceWrapper> m_ExamInterface; // Every action has access to the exam interface
private:
	std::string m_Descriptor; // Used to log the current action plan to the console
};
```
### Planning
For my planning I use a recursive depth first search algorythm that finds the best possible worlState (pseudocode because actual code is very long and contains many checks):
```
GetBestAction( worldState, depth ):
  best = [worst possible score (FLT_MIN)]
  for all possible actions:
    get cost of current action
    worldState = get result of action
    if depth is equal to the plan depth:
      result = EvaluateWorldState( worldState )
    else:
      result = EvaluateWorldState( GetBestAction( worldState, depth + 1 ) )
    compare result to best result and replace best if result is better
  return best
```
## Conclusion
A the start I had no idea if this system was gonna work at all, so I was very happy to see my agent get a decent score (current high score is about 1100).
Both GOAP and AGAP have certain advantages and disadvantages:
| GOAP | AGAP |
| ----------- | ----------- |
| Fast | Slow |
| Needs clearly defined goal | Goal can be more abstract |
| WorldState consists of booleans | WorldState consists of any kinds of variables |

While it was very fun and interresting to try to come up with my own AI system based on AGAP, I don't think it's gonna be the next big thing in the industry.
AGAP fits the needs of my project and works as I hoped it would work, so I consider my experiment a success, although there is still much room for improvement.
## Bloopers
I had some interresting bugs / mistakes in my project that I personally think are quite funny:
 - I have a variable called "laziness", this determines how much the AI prefers a cheap path over a high rewarding path. At some point this value was so high my agent decided to do the cheapest possible action at all times. This caused it barely do anything at all and just chill untill I patched it.
 - I have a variable called "shortTermBias" that determines how much my AI prefers short term rewards over long term rewards. This is supposed to be a value higher than 1. Because I accidentally gave it a value less than 1 my agent would prefer long term rewards over the exact same score now. This would cause my agent to procrastinate. The agent would literally do any random task so it could do the important tasks later. So I ended up accidentally implementing a student :>
