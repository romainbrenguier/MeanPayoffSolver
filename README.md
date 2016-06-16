# MeanPayoffSolver
A solver for Mean Payoff Games, based on gain and bias equations and the Z3 SMT solver.


## Installation
This assumes the library Z3 is installed on your computer.
To compile the tool type:
> make

## Usage

Encoding of Mean payoff games: 
 States are represented by integers.
 Even states are minimizer states.
 Odd states are maximizer states.
 The initial state is 0.

The description of the game is read from the text file given in argument.
Each line of the file correspond to a transition of the game.
The line containns the index of the source, the index of the destination and the weight of the edge.

