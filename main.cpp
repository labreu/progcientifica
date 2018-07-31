#include<iostream>
#include<time.h>
#include<map>
#include<list>
#include<string>

#define NULL_CELL -1
#define CELULA_INVALIDA -2
#define BOARD_SIZE 3

using namespace std;
    // g++ main.cpp -o main && ./main

    // posicoes
    //  0 1 2
    //  3 4 5
    //  6 7 8

    // movimentos
    // n - e b d c (anti horario)
    // 0 -   3 1
    // 1 - 0 4 2 
    // 2 - 1 5 
    // 3 -   6 4 0
    // 4 - 3 7 5 1
    // 5 - 4 8
    // 6 -     7 3
    // 7 - 6   8 4
    // 8 - 7     5

class Cell{
    public:
        int i, valor;
        int posicoes[4];
        
    Cell(){}
    Cell(int i, int valor): i(i), valor(valor){}

    void imprime(){
        cout << toString() << endl;
    }  

    string toString(){
        return "Cell[" +  to_string(i) + "] = " + to_string(valor);
    }

    void get_possible_swaps(){
        if (i % BOARD_SIZE == 0) posicoes[0] = CELULA_INVALIDA;
        else posicoes[0] = i - 1;

        if ((i+1) % BOARD_SIZE == 0) posicoes[1] = CELULA_INVALIDA;
        else posicoes[1] = i + 1;

        if ((i + BOARD_SIZE) >= BOARD_SIZE*BOARD_SIZE) posicoes[2] = CELULA_INVALIDA;
        else posicoes[2] = i + BOARD_SIZE;

        if ((i - BOARD_SIZE) < 0) posicoes[3] = CELULA_INVALIDA;
        else posicoes[3] = i - BOARD_SIZE;

        // cout << "Cell[" << i << "] = " << valor << " | E:" << posicoes[0] << " B:" << posicoes[1] << " D:"<< posicoes[2] << " C:" << posicoes[3] << endl;
    }
};

class Board{
    public:
        Cell board[9];
        Cell nc;
        Board(){}

        void initialize_target_board(){
            // target
            //  1 | 2 | 3
            //  4 | 5 | 6
            //  7 | 8 | -
            int n = 1;
            for(int i=0; i<BOARD_SIZE*BOARD_SIZE; i++){
                if(i==8) board[i] = Cell(i, NULL_CELL);
                else board[i] = Cell(i, n);
                n++;
            }
        }
        void initialize_fix_board(){
            // | 1 | 2 | 3
            // | 7 | 4 | 5
            // | 8 | - | 6
            board[0] = Cell(0, 1);
            board[1] = Cell(1, 2);
            board[2] = Cell(2, 3);
            board[3] = Cell(3, 7);
            board[4] = Cell(4, 4);
            board[5] = Cell(5, 5);
            board[6] = Cell(6, 8);
            board[7] = Cell(7, NULL_CELL);
            board[8] = Cell(8, 6);
        }

        void initialize_random_board(){
            int n = 1;
            int nullcell_pos = rand() % (BOARD_SIZE*BOARD_SIZE);

            for(int i=0; i<BOARD_SIZE*BOARD_SIZE; i++){
                if(i==nullcell_pos) board[i] = Cell(i, NULL_CELL);
                else {
                    board[i] = Cell(i, n);
                    n++;
                }   
            }
        }

        string toString(){
            string s = "";
            for(int i=0; i<BOARD_SIZE*BOARD_SIZE; i++) 
                s += "|" + board[i].toString();
            return s;
        }

        void get_null_cell(){
            for(int i=0; i<BOARD_SIZE*BOARD_SIZE; i++)
                if (board[i].valor == NULL_CELL){
                    nc = board[i];
                    nc.get_possible_swaps();
                }
        }

        void print_board(){
            for(int i=0; i<BOARD_SIZE*BOARD_SIZE; i++){
                if (board[i].valor == NULL_CELL) cout << " | " << "-";
                else cout << " | " << board[i].valor;
                if (((i+1) % 3) == 0) cout << endl;
            }
            cout << endl;
        }
        bool operator==(const Board& b){
            for(int i=0; i<BOARD_SIZE*BOARD_SIZE; i++)
                if (board[i].valor != b.board[i].valor) 
                    return false;
            return true;
        }
        void swap(int pos1, int pos2){
            Cell swp = board[pos2];
            swp.i = pos1;

            board[pos2] = board[pos1];
            board[pos2].i = pos2;

            board[pos1] = swp;
        }
};


class Solver{
    public:
        int count;
        Board solution;
        map<string, bool> cache_map;

        Solver(): count(0){}

        bool check_end(Board& target, Board& b){
            if (b==target){
                solution = b;
                cout << "Solved! " << count << " iterations." << endl;
                return true;
            }
            return false;
        }
        bool in_cache(Board b){
            string s = b.toString();
            if (!cache_map[s]){
                cache_map[s] = true;
                return false;
            }
            return true;
        }
        
        bool solve_board_bfs(Board& target, Board b, bool use_bfs){
            if(use_bfs) cout << "BFS: " ;
            else cout << "DFS: " ;

            list<Board> F;
            Board board;
            
            // Check end
            if (check_end(target, b)) return true;
            
            // Check cache
            in_cache(b);
            
            // Add to tree
            F.push_back(b);

            while(!F.empty()){

                if(use_bfs){
                    board = F.front();
                    F.pop_front();
                }
                else{
                    board = F.back();
                    F.pop_back();
                }
             
                // Check iterations
                count++;
                if (count > 500000){
                    solution = board;
                    cout << "Ending after 500k iterations" << endl;
                    break;
                }
                
                // Get current null cell
                board.get_null_cell();

                // For each available move (up down left right)
                for (int i=0; i<4; i++){
                    if (board.nc.posicoes[i] != CELULA_INVALIDA){
                        Board tmp = board;
                        
                        // Swap null with choice
                        tmp.swap(tmp.nc.i, tmp.nc.posicoes[i]);
                        
                        // Check cache
                        if (in_cache(tmp)) continue;

                        // Check end
                        if (check_end(target, tmp)) return true;

                        // Add to tree
                        F.push_back(tmp);
                    }
                }
            }
            return false;
        }
};


int main(int argc, char const *argv[])
{
    srand(time(NULL));

    // Puzzle Inicial
    cout << "Puzzle Inicial" << endl;
    Board b = Board();    
    b.initialize_random_board();
    b.print_board();

    // Target
    Board target = Board();    
    target.initialize_target_board();
    target.print_board();

    cout << target.toString() << endl;
    // BFS
    Solver s;
    s.solve_board_bfs(target, b, true);

    // DFS
    Solver s2;
    s2.solve_board_bfs(target, b, false);
    // s.solution.print_board();

}   
