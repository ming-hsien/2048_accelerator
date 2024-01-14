# Description
During a certain class, I learned how to implement a program using reinforcement learning to train a machine to play the 2048 game. However, we observed that the training process took a considerable amount of time, prompting the idea of parallelizing the training process to reduce the waiting time.

2048 is a well-known game where initially, a 4x4 grid is presented, and a randomly chosen cell is populated with a number 2 or 4. In each turn, the player chooses to move upward, downward, left, or right. During the movement, the numbers on the grid slide in the chosen direction, and if two identical numbers collide, they merge to form a new single number. After the slide, a new 2 or 4 is randomly generated in an empty cell. The player repeats these actions until there are no adjacent identical numbers on the grid, and no empty cells are left, signaling the end of the game.

The training process goes through several stages, broadly categorized into two types: Dependent and Independent. After completing each game, the program gradually updates the training parameters through Back Propagation. This process involves updating parameters from later stages to the current previous grid, creating dependencies between different grid states, hence categorized as Dependent. The Independent part, which is the focus of parallelization in our experiment, occurs during the calculation of the best move (Select best move) at each movement. This involves computing and determining the optimal path (up, down, left, right). Subsequently, we check if the best move is found (Selected?). If not, the game is considered finished, and the experimental parameters are updated (e.g., reducing learning rate), and the total score of the game is recorded before moving on to the next game. Otherwise, we update the reward parameters and proceed to the next movement.

# Platform
Ubuntu 22.04.3 LTS \
CPU: i5-12400 \
Core: 6 \
Core Threads: 12

# Run
Serial
```
g++ 2048_serial.cpp -o 2048_serial
./2048_serial
```
MPI
```
mpicxx -o mpi 2048_mpi_gather_bcast.cpp
mpirun -np 4 mpi
```
Pthread
```
g++ 2048_pthread.cpp -I./common -std=c++17 -Wall -lpthread -o 2048_pthread
./2048_pthread
```
OpenMP
```
g++ -fopenmp 2048_openmp.cpp -o 2048_openmp
./2048_openmp
```



