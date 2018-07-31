#include<iostream>
#include<time.h>
#define UNPLAYED_CELL -1

using namespace std;


class Board{
    public:
        int board[9];
        bool gameOver;
        int winner;

    Board(): gameOver(false), winner(-1){
        for(int i=0; i<9; i++) board[i] = UNPLAYED_CELL;
    }

    void printBoard(){
        for(int i=0; i<9; i++){
            if (board[i] == UNPLAYED_CELL) cout << " | " <<  "_";
            else if (board[i] == 1) cout << " | " << "x";
            else cout << " | " << "o";
            if ( ( (i+1) % 3) == 0) cout << " | " << endl;
        }    
        cout << endl;
    }

    bool playRandomMove(bool x_or_o){
        int randPos = rand() % 9;
        if (isAlreadyPlayed(randPos)) return playRandomMove(x_or_o);
        
        if(x_or_o) board[randPos] = 0;
        else board[randPos] = 1;

        return true;
    }

    bool isAlreadyPlayed(int pos){
        if(board[pos]==UNPLAYED_CELL) return false;
        return true;
    }

    void play9moves(){
        for(int i=0; i<9; i++){
            if (i%2) playRandomMove(true);
            else playRandomMove(false);

            printBoard();
            if (isGameOver()) break;
            if (i==8) cout << "Velha!" << endl;
        }    
    }

    bool isGameOver(){
        // 0 1 2
        // 3 4 5
        // 6 7 8

        // check X and O
        for(int x_or_o = 0; x_or_o <= 1; x_or_o++){

            // check lines 
            for(int row=0; row<=6; row=row+3)
                if ((board[row]==x_or_o)&&(board[row+1]==x_or_o)&&(board[row+2]==x_or_o)) gameOver = true;
            
            // check lines and cols
            for(int row=0; row<3; row++)
                if ((board[row]==x_or_o)&&(board[row+3]==x_or_o)&&(board[row+6]==x_or_o)) gameOver = true;

            //  check diagonals
            if ((board[0]==x_or_o)&&(board[4]==x_or_o)&&(board[8]==x_or_o)) gameOver = true;
            if ((board[2]==x_or_o)&&(board[4]==x_or_o)&&(board[6]==x_or_o)) gameOver = true;

            if (gameOver) {
                if (x_or_o==1){ 
                    winner = 1;
                    // cout << "We have a winner: " << "X" << endl;
                }
                else { 
                    winner = 0;
                    // cout << "We have a winner: " << "O" << endl;
                }
                gameOver = false; //reset flag
                return true;

            } 
        }
        return false;
    }

    bool isVelha(){
        for(int i=0; i<9; i++){
            if (board[i] == UNPLAYED_CELL) return false;
        }    
        return true;
    }
    int minSearch(int level){
        if (isGameOver() && winner==0) { return -10; }
        else if (isGameOver() && winner==1) { return 10; }
        else if (isVelha()) { return 0; }

        int score = 999;

        for (int i = 0; i < 9; i++){
            if (board[i] == UNPLAYED_CELL){
                board[i] = 1;
                score = min(score, maxSearch(level)-level);
                board[i] = UNPLAYED_CELL;
            }
        }
        return score;
    }

    int maxSearch(int level) {
        if (isGameOver() && winner==0) { return -10; }
        else if (isGameOver() && winner==1) { return 10; }
        else if (isVelha()) { return 0; }

        int score = -999;

        for (int i = 0; i < 9; i++){
            if (board[i] == UNPLAYED_CELL){
                board[i] = 0;
                score = max(score, minSearch(level)+level);
                board[i] = UNPLAYED_CELL;
            }
        }
        return score;
    }

    int minimax() {
        int score = -999;
        int move;
        int maxScore;
        int level = 0;
        if( isGameOver() ) return -1;
        
        for (int i = 0; i < 9; i++){
            if (board[i] == UNPLAYED_CELL){
                // AI plays
                board[i] = 1;

                // Do max search
                maxScore = maxSearch(level+1);

                // if this score is better than the best, play it
                if (maxScore > score){
                    score = maxScore;
                    move = i;
                    
                }
                cout << i << " " << score << endl;
                board[i] = UNPLAYED_CELL;
            }
        }

        return move;
    }
    
};
int main(int argc, char const *argv[])
{   
    // g++ jogo_velha.cpp -o main && ./main
    srand(time(NULL));
    Board b;
    // b.play9moves();
    b.playRandomMove(true);
    b.printBoard();

    int move;
    int turn = 0;

    while(!b.isGameOver()){
        if ((turn % 2) == 0){
            move = b.minimax();
            if (move == -1){ cout << "Velha" << endl; break;}
            else b.board[move] = 1;
        }
        else{
            b.playRandomMove(true);
        }
        
        b.printBoard();
        turn ++;
    }
    
}
