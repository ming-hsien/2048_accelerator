#include <iostream>
// #include <cstdlib>
#include <vector>
#include <time.h>
#include <iomanip>
#include <math.h>
#include <pthread.h>
#include <bits/stdc++.h>

using namespace std;

typedef vector<vector<int>> Board;
typedef vector<int> Pattern;

int GAMES = 10000;

typedef struct student
{
    Board board;
    // Board UPboard;
    // Board DNboard;
    // Board LTboard;
    // Board RTboard;
    Pattern pattern[2];
    Pattern transpose[2];
    // int score;
} _2048_;

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

struct arg_in_thread
{
    _2048_ my2048;
    Board moved_board;
};

void *estimate_board(void *args)
{
    arg_in_thread *arg = (arg_in_thread *)args;
    _2048_ my2048 = arg->my2048;
    Board moved_board = arg->moved_board;

    int local_count_empty_lattice = 0;
    float local_tmp_estimate = 0.0;

    for (int lattice = 0; lattice < 16; lattice++)
    {
        int r = lattice / 4;
        int c = lattice % 4;
        if (moved_board[r][c] == 0)
        {
            // random generate 2 in board(r,c)
            moved_board[r][c] = 2;
            local_tmp_estimate += 0.9 * feat_estimate(moved_board, my2048.transpose);

            // random generate 4 in board(r,c)
            moved_board[r][c] = 4;
            local_tmp_estimate += 0.1 * feat_estimate(moved_board, my2048.transpose);

            ++local_count_empty_lattice;
        }
    }
    if (local_count_empty_lattice != 0)
        local_tmp_estimate /= local_count_empty_lattice;
    void *return_value = &local_tmp_estimate;
    return (void *)return_value;
}

int main()
{
    _2048_ my2048;
    my2048 = initialize();

    time_t begin, end;
    begin = time(NULL);

    float total_score = 0;
    int games = GAMES;

    for (int count = 0; count < games; count++)
    {
        my2048 = initialize();
        // int c = 0;
        // while(c < 10 ) {
        while (!game_over(my2048.board))
        {
            // stage: 0 1 2 3 => up down left right
            float Scores[4];

            // Setting threads
            pthread_t threads[4]; // create thread
            arg_in_thread arg[4];

            Board tmp_board;
            for (int stage = 0; stage < 4; stage++)
            {
                Board moved_board = next_move(my2048.board, stage);
                tmp_board = moved_board;

                arg[stage].my2048 = my2048;
                arg[stage].moved_board = moved_board;

                // Create a new thread and run pi() function with cor4ngrespoding arg[i] arguments
                pthread_create(&threads[stage], NULL, estimate_board, (void *)&arg[stage]);
            }

            /* Wait for the other threads*/
            void *return_status;
            float tmp_estimate[4];
            for (int i = 0; i < 4; ++i)
            {
                pthread_join(threads[i], &return_status);
                tmp_estimate[i] = *static_cast<float *>(return_status);
            }

            for (int i = 0; i < 4; i++)
            {
                Scores[i] = SumOfBoard(tmp_board) + tmp_estimate[i];
            }

            // 選出Scores最高的,當作移動的決策
            int max_sc = Scores[0];
            int best_move = 0;
            for (int stage = 1; stage < 4; stage++)
            {
                if (Scores[stage] > max_sc)
                {
                    best_move = stage;
                    max_sc = Scores[stage];
                }
            }
            // cout << best_move << endl;
            my2048.board = next_move(my2048.board, best_move);
            my2048.board = popup(my2048.board, count);

            // for (int x = 0; x < 4; x++) {
            //     for (int y = 0; y < 4; y++) {
            //         cout << setw(4) << my2048.board[x][y] << " ";
            //     }
            //     cout << endl;
            // }
            // cout << endl;
        }
        // cout << "Game Over !!" << endl;
        // for (int x = 0; x < 4; x++) {
        //     for (int y = 0; y < 4; y++) {
        //         cout << setw(4) << my2048.board[x][y] << " ";
        //     }
        //     cout << endl;
        // }
        // cout << SumOfBoard(my2048.board) << endl;
        total_score += SumOfBoard(my2048.board);
    }

    end = time(NULL);

    cout << "Average Score: " << total_score / games << endl;
    cout << "Execution Time: " << difftime(end, begin) << endl;
}
