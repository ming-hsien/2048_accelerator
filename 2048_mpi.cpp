#include <iostream>
#include <mpi.h>
#include <vector>
#include <time.h>
#include <iomanip>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

typedef vector<vector<int>> Board;
typedef vector<int> Pattern;

typedef struct _2048_
{
    Board board;
    Pattern pattern[2];
    Pattern transpose[2];
    // int score;
} _2048_;

void print_board(Board b)
{
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            cout << setw(4) << b[x][y] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

float feat_estimate(Board b, Pattern transpose[2])
{
    float score = 0;
    // count isomorphism for each pattern
    for (int count = 0; count < 2; count++)
    {
        // each pattern have 8 exclusive transpose pattern => 0,90,180,270,vertical,horizontal,vertical&horizontal, horizontal&vertical
        for (int pat = 0; pat < 48; pat++)
        {
            // cout << transpose[count][pat] << " " << transpose[count][pat] % 4 << endl;
            int r = transpose[count][pat] / 4;
            int c = transpose[count][pat] % 4;
            score += int(b[r][c]);
        }
    }
    return score;
}

Board popup(Board b, unsigned int seed)
{
    vector<int> posBuffer;
    Board newb = b;
    // unsigned int seed = time(NULL);
    for (int i = 0; i < 16; i++)
    {
        int r = i / 4;
        int c = i % 4;
        if (newb[r][c] == 0)
        {
            posBuffer.push_back(i);
        }
    }
    if (posBuffer.size() > 0)
    {
        int pos = rand_r(&seed) % posBuffer.size();
        int r = posBuffer[pos] / 4;
        int c = posBuffer[pos] % 4;
        newb[r][c] = rand_r(&seed) % 10 ? 2 : 4;
    }
    return newb;
}

Board move_up(Board b)
{
    Board newb = b;
    for (int y = 0; y < 4; y++)
    {
        int front_num = 0;
        int front_pos = 0;
        int flag = 0;
        for (int x = 0; x < 4; x++)
        {
            if (newb[x][y] > 0 && newb[x][y] == front_num)
            {
                newb[front_pos][y] = newb[x][y] * 2;
                newb[x][y] = 0;
                front_num = 0;
                front_pos = 0;
            }
            else if (newb[x][y] > 0 && newb[x][y] != front_num)
            {
                front_num = newb[x][y];
                if (flag != x)
                {
                    front_pos = flag;
                    newb[flag][y] = newb[x][y];
                    newb[x][y] = 0;
                }
                front_pos = flag;
                flag += 1;
            }
            else
            {
                continue;
            }
        }
    }
    return newb;
}

Board move_down(Board b)
{
    Board newb = b;
    for (int y = 0; y < 4; y++)
    {
        int front_num = 0;
        int front_pos = 0;
        int flag = 3;
        for (int x = 3; x >= 0; x--)
        {
            if (newb[x][y] > 0 && newb[x][y] == front_num)
            {
                newb[front_pos][y] = newb[x][y] * 2;
                newb[x][y] = 0;
                front_num = 0;
                front_pos = 0;
            }
            else if (newb[x][y] > 0 && newb[x][y] != front_num)
            {
                front_num = newb[x][y];
                if (flag != x)
                {
                    front_pos = flag;
                    newb[flag][y] = newb[x][y];
                    newb[x][y] = 0;
                }
                front_pos = flag;
                flag -= 1;
            }
            else
            {
                continue;
            }
        }
    }
    return newb;
}

Board move_left(Board b)
{
    Board newb = b;
    for (int x = 0; x < 4; x++)
    {
        int front_num = 0;
        int front_pos = 0;
        int flag = 0;
        for (int y = 0; y < 4; y++)
        {
            if (newb[x][y] > 0 && newb[x][y] == front_num)
            {
                newb[x][front_pos] = newb[x][y] * 2;
                newb[x][y] = 0;
                front_num = 0;
                front_pos = 0;
            }
            else if (newb[x][y] > 0 && newb[x][y] != front_num)
            {
                front_num = newb[x][y];
                if (flag != y)
                {
                    front_pos = flag;
                    newb[x][flag] = newb[x][y];
                    newb[x][y] = 0;
                }
                front_pos = flag;
                flag += 1;
            }
            else
            {
                continue;
            }
        }
    }
    return newb;
}

Board move_right(Board b)
{
    Board newb = b;
    for (int x = 0; x < 4; x++)
    {
        int front_num = 0;
        int front_pos = 0;
        int flag = 3;
        for (int y = 3; y >= 0; y--)
        {
            if (newb[x][y] > 0 && newb[x][y] == front_num)
            {
                newb[x][front_pos] = newb[x][y] * 2;
                newb[x][y] = 0;
                front_num = 0;
                front_pos = 0;
            }
            else if (newb[x][y] > 0 && newb[x][y] != front_num)
            {
                front_num = newb[x][y];
                if (flag != y)
                {
                    front_pos = flag;
                    newb[x][flag] = newb[x][y];
                    newb[x][y] = 0;
                }
                front_pos = flag;
                flag -= 1;
            }
            else
            {
                continue;
            }
        }
    }
    return newb;
}

Board next_move(Board b, int stage)
{
    switch (stage)
    {
    case 0:
        return move_up(b);
    case 1:
        return move_down(b);
    case 2:
        return move_left(b);
    case 3:
        return move_right(b);
    default:
        return b;
    }
}

float SumOfBoard(Board b)
{
    float sum = 0;
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            sum += b[x][y];
        }
    }
    return sum;
}

bool game_over(Board b)
{
    if (b == move_up(b) && b == move_down(b) && b == move_left(b) && b == move_right(b))
    {
        return true;
    }
    return false;
}

_2048_ initialize()
{
    _2048_ my2048;
    // my2048.score = 0;
    my2048.pattern[0] = {0, 1, 4, 5, 8, 9};
    my2048.pattern[1] = {0, 1, 2, 3, 4, 5};
    my2048.transpose[0] = {0, 1, 4, 5, 8, 9, 1, 2, 3, 5, 6, 7, 6, 7, 10, 11, 14, 15, 4, 5, 8, 9, 12, 13, 0, 1, 4, 5, 8, 9, 2, 3, 6, 7, 10, 11, 6, 7, 10, 11, 14, 15, 4, 5, 8, 9, 12, 13};
    my2048.transpose[1] = {0, 1, 2, 3, 4, 5, 2, 3, 6, 7, 11, 15, 10, 11, 12, 13, 14, 15, 0, 4, 8, 9, 12, 13, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 6, 7, 10, 11, 12, 13, 14, 15, 8, 9, 12, 13, 14, 15};
    my2048.board = {
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 2, 0, 0},
        {0, 0, 0, 0}};
    return my2048;
}

int main(int argc, char **argv)
{

    // MPI --------------------------------
    MPI_Init(&argc, &argv);
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int tag = 0;
    unsigned int dest = 0;
    MPI_Status status;
    // MPI --------------------------------
    // cout<<world_rank<<endl;

    _2048_ my2048;
    my2048 = initialize();

    time_t begin, end;

    if (world_rank == 0)
    {
        begin = time(NULL);
    }

    float total_score = 0;
    int games = 1000;

    for (int count = 0; count < games; count++)
    {
        my2048 = initialize(); // see here

        while (!game_over(my2048.board))
        {

            float Scores[4] = {0};

            // stage: 0 1 2 3 => up down left right
            int stage = world_rank % 4;
            // for (int stage = 0; stage < 4; stage++) {
            Board moved_board = next_move(my2048.board, stage);
            Board tmp_board = moved_board;
            int count_empty_lattice = 0;
            float tmp_estimate = 0.0;
            for (int lattice = 0; lattice < 16; lattice++)
            {
                int r = lattice / 4;
                int c = lattice % 4;
                if (moved_board[r][c] == 0)
                {
                    // random generate 2 in board(r,c)
                    moved_board[r][c] = 2;
                    tmp_estimate += 0.9 * feat_estimate(moved_board, my2048.transpose);

                    // random generate 4 in board(r,c)
                    moved_board[r][c] = 4;
                    tmp_estimate += 0.1 * feat_estimate(moved_board, my2048.transpose);

                    ++count_empty_lattice;
                }
            }
            if (count_empty_lattice != 0)
                tmp_estimate /= count_empty_lattice;
            // cout << SumOfBoard(moved_board) + tmp_estimate << endl;
            float score_local = SumOfBoard(tmp_board) + tmp_estimate;

            int best_move = 0;

            if (world_rank > 0)
            {
                // cout<<"world_rank : "<<world_rank<<endl;
                // print_board(my2048.board);
                // TODO: handle workers
                MPI_Send(&score_local, 1, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
            }

            else if (world_rank == 0)
            {
                Scores[world_rank] = score_local;
                float buf;
                for (int source = 1; source < world_size; source++)
                {
                    MPI_Recv(&buf, 1, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status);
                    Scores[source] = buf;
                }
                int max_sc = Scores[0];
                // for (int stage = 0; stage < 4; stage++)
                // {
                //     cout << Scores[stage] << " ";
                // }
                // cout << endl;
                for (int stage = 1; stage < 4; stage++)
                {
                    // cout<<Scores[stage]<<endl;
                    if (Scores[stage] > max_sc)
                    {
                        best_move = stage;
                        max_sc = Scores[stage];
                    }
                }
                MPI_Send(&best_move, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
                MPI_Send(&best_move, 1, MPI_INT, 2, tag, MPI_COMM_WORLD);
                MPI_Send(&best_move, 1, MPI_INT, 3, tag, MPI_COMM_WORLD);
            }
            if (world_rank > 0)
            {
                int best_move_buf;
                MPI_Recv(&best_move_buf, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
                my2048.board = next_move(my2048.board, best_move_buf);
                my2048.board = popup(my2048.board, count);
            }
            if (world_rank == 0)
            {
                my2048.board = next_move(my2048.board, best_move);
                my2048.board = popup(my2048.board, count);
            }
            // }

            // 選出Scores最高的,當作移動的決策

            // cout << best_move << endl;
        }

        // cout<<"world_rank : "<<world_rank<<endl;
        // print_board(my2048.board);

        if (world_rank == 0)
        {
            // cout << "Game Over !!" << endl;
            // for (int x = 0; x < 4; x++)
            // {
            //     for (int y = 0; y < 4; y++)
            //     {
            //         cout << setw(4) << my2048.board[x][y] << " ";
            //     }
            //     cout << endl;
            // }
            total_score += SumOfBoard(my2048.board);
        }
    }

    if (world_rank == 0)
    {
        end = time(NULL);
        cout << "Average Score: " << total_score / games << endl;
        cout << "Execution Time: " << difftime(end, begin) << endl;
    }
    MPI_Finalize();
    return 0;
}