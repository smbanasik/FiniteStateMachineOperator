# FiniteStateMachineOperator
A framework for constructing a finite state machine and operating it

## Motive
Finite State Machines (FSM) are great for mapping out rule based systems. In game development, they have applications in procedural generation,
ai behavior, and even player controls. Having a framework to create and operate these data structures will simplify the iterative process to create these.

## Usage

### Construction
Before utilizing the framework to construct your FSM, it's best to plan beforehand how you want your FSM to be. Once you've planned this out, build it from the bottom up.
This means that you should construct nodes with fewer children first, but this isn't a requirement, just a suggestion since it's smoother to create that way.

The FSM Operator will take two functions: an action function, which performs an action when a node has been reached, and an evaluation function, which determines which 
node to move towards next. Functions may be unique for each node, and neither are a requirement to operate the machine. A node with no action function will do nothing,
and a node with no evaluation function will pick the first child that was inserted.
These functions both take one parameter, the action function returns void, and the evaluation function must return an int. These parameters may be of 
whatever type you want, determined by the two template parameters. It is recommended that you set up a data structure specifically for sending and recieving information, and 
modifying these by reference.

Use the `create_node()` function to create a node, then use the `set_*()` functions and `add_child()` function to modify the new node.

For debugging purposes, you can use `print_FSM()` to output a string which represents the FSM in the DOT graph format. There's online interpreters which will take that
format and convert it into a readable graph. The function is simple, so the graph may look ugly, tweak the connections as needed.

### Operation

To begin operating the FSM, run the `start_operation()` function with the initial data for the nodes action function. The `get_is_operating()` function may be used to 
query whether an FSM Operator has begin the operation phase or not. When everything is prepared for the FSM Operator to change states, run the step function with the 
evaluation data and action data ready. Due to the design of the framework, it's a good idea for the action parameter to be a set of state, and for the action function to
modify that state, as opposed to the action function performing actions directly.
