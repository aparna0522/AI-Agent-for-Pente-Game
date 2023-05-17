# Pente Game

## Project Description

Pente is an advanced version of tic tac toe, played on a 19x19 board, where players aim to create a sequence of five stones in a row while also having the option to capture five pairs of their opponent's stones to win the game. It requires more strategic thinking than tic tac toe due to the larger board and additional win condition.

The minimax algorithm is a decision-making algorithm used in game theory, which evaluates all possible moves in a game tree to determine the optimal move for a player assuming the opponent also plays optimally. Alpha-Beta Min Max algorithm reduces number of nodes evaluated in a search tree by discarding irrelevant subtrees.

Even with reduced evalutaions, large overhead of creating a new 19\*19 board per move to play on in futher depths in the game tree persists. The initial approach I followed consisted of creating a char 2D arary, having 19\*19\*1 bytes to store the state, ‘b’, ’w’, ’.’. Though not significant, causes bigger overhead as the depth increases and boards stored in memory explode. To minimize this issue, I implemented a bit board representation of the board, where the state of board is split storing all white stone positions and storing all balck stone positions. It uses just one bit to represent the presence or absence of a stone in a given row or column. This makes the board of size: 19\*19\*2 bits ~ 92 bytes. This also gave me an added advantage of reduced chache miss and increased performance in evaluating the board, where I evaluate number of consecutive 5s, 4s, 3s and 2s for each piece. This approach demonstrates the importance of considering memory optimization techniques in large-scale game tree searches.

<p> Learn more at <a href="https://en.wikipedia.org/wiki/Pente">Pente</a></p>


## Project Working Demo

Below video shows two AI agents competing with each other and displays the winner. The first agent is based on memory as well as time optimisation, whereas the second agent just uses time optimisation techniques: 

https://github.com/aparna0522/Pente_Game/assets/36110304/31e1b190-8f38-4fde-b164-41c457eec011


