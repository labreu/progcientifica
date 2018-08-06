#include<iostream>
#include<time.h>
#include<map>
#include<list>
#include<string>
#include<vector>

#define CELULA_NULA -1  // CELULA QUE É POSSIVEL MOVIMENTO 
#define CELULA_INVALIDA -2  // CELULA QUE É IMPOSSIVEL MOVIMENTO 
#define BOARD_SIZE 3  //LARGURA DO TABULEIRO QUADRADO

using namespace std;


class Cell{
    public:
        int i, valor;
        int vizinhos[4];
        
    Cell(){}
    Cell(int i, int valor): i(i), valor(valor){}
    
    bool isnull(){
        // CELULA QUE É POSSIVEL MOVIMENTO 
        return valor == CELULA_NULA;
    }
    
    bool isValidCell(int i){
        // CELULA QUE É IMPOSSIVEL MOVIMENTO 
        return vizinhos[i] != CELULA_INVALIDA;
    }

    string toString(){
        // MONTA STRING QUE REPRESENTA A CELULA
        return "Cell[" +  to_string(i) + "] = " + to_string(valor);
    }

    void imprime(){
        cout << toString() << endl;
    }  

    void getPossibleSwaps(){
        // verifica pra quais celulas vizinhas (cima, baixo, direita e esquerda) é possível fazer a troca
        // BOARD_SIZE = 3;
        // posicoes
        //  0 1 2
        //  3 4 5
        //  6 7 8

        if (i % BOARD_SIZE == 0) vizinhos[0] = CELULA_INVALIDA;
        else vizinhos[0] = i - 1;

        if ((i+1) % BOARD_SIZE == 0) vizinhos[1] = CELULA_INVALIDA;
        else vizinhos[1] = i + 1;

        if ((i + BOARD_SIZE) >= BOARD_SIZE*BOARD_SIZE) vizinhos[2] = CELULA_INVALIDA;
        else vizinhos[2] = i + BOARD_SIZE;

        if ((i - BOARD_SIZE) < 0) vizinhos[3] = CELULA_INVALIDA;
        else vizinhos[3] = i - BOARD_SIZE;
    }
};

class Board{
    public:
        Cell board[9];
        Cell celulaNula;
        int mann_distance;
        int levelArvore;

        Board(): mann_distance(999), levelArvore(0){}
        Board(string tipoInicializacao){
            mann_distance = 999;
            levelArvore = 0;
            if (tipoInicializacao == "fix"){
                initializeFixBoard();
            }
            else if(tipoInicializacao == "target"){
                initializeTargetBoard();
            }
            else{
                // cout << "Tabuleiro iniciado aleatoriamente" << endl;
                initializeRandomBoard();
            }
            // printBoard();
        }

        bool checaMovimentoValido(int i){
            return celulaNula.isValidCell(i);
        }

        void initializeTargetBoard(){
            // target
            //  1 | 2 | 3
            //  4 | 5 | 6
            //  7 | 8 | -
            int n = 1;
            for(int i=0; i<BOARD_SIZE*BOARD_SIZE; i++){
                if(i==8) board[i] = Cell(i, CELULA_NULA);
                else {board[i] = Cell(i, n);
                n++;}
            }
            
        }
        void initializeFixBoard(){
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
            board[7] = Cell(7, CELULA_NULA);
            board[8] = Cell(8, 6);
        }

        void initializeRandomBoard(){
            int n = 1;
            int nullcell_pos = rand() % (BOARD_SIZE*BOARD_SIZE);

            for(int i=0; i<BOARD_SIZE*BOARD_SIZE; i++){
                if(i==nullcell_pos) board[i] = Cell(i, CELULA_NULA);
                else {
                    board[i] = Cell(i, n);
                    n++;
                }   
            }

        }

        string toString(){
            string s = "";
            for(int i=0; i<BOARD_SIZE*BOARD_SIZE; i++){
                if (board[i].isnull()) s +=  " | -";
                else s += " | " + to_string(board[i].valor);
                if (((i+1) % BOARD_SIZE) == 0) s += "\n";
            }
            return s;
        }
        
        void printBoard(){
            cout << toString() << endl;
        }

        void localizaCelulaNula(){
            for(int i=0; i<BOARD_SIZE*BOARD_SIZE; i++)
                if (board[i].isnull()){
                    celulaNula = board[i];
                    celulaNula.getPossibleSwaps();
                }
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

        void manhattan_distance(Board b2, bool heuristicaG){
            //CALCULA DISTANCIA |X1-X2| + |Y1-Y2|
            
            levelArvore += 1;
            if (heuristicaG)  mann_distance = levelArvore;
            else mann_distance = 0;

            for(int i=0; i<BOARD_SIZE*BOARD_SIZE; i++){
                for(int j=0; j<BOARD_SIZE*BOARD_SIZE; j++){
                    if (board[i].isnull()) continue;
                    if (board[i].valor==b2.board[j].valor){
                        int x1 = i / BOARD_SIZE;
                        int x2 = j / BOARD_SIZE;
                        int y1 = i % BOARD_SIZE;
                        int y2 = j % BOARD_SIZE;
                        int dist = abs(x1-x2) + abs(y1-y2);
                        mann_distance += dist;
                    }
                }
            }
        }
};

class SalvaDados{
    public:
        string dados;
        SalvaDados():dados(""){}

    void log(string s){
        dados += s ;
    }

};


class Solver{
    public:
        int count, movimentos;
        Board solution;
        map<string, string> cache_map;
        Board target;
        list<Board> F;
        SalvaDados sd;
        clock_t t1, t2;
        double differenceSeconds;

        Solver(): count(0), movimentos(0) {
            // Inicializa Target
            target = Board("target");    
        }

        bool checkFim(Board& b){
            // Checa se o board recebido é o target desejado
            if (b==target){
                solution = b;
                // cout << "Solved! " << count << " iterations." << endl;
                return true;
            }
            return false;
        }
        bool inCache(Board& b, Board& pai){
            // Checa se o board recebido já foi visitado (está no cache). Se nao, guarda no cache o board e o Pai no cache 
            string s = b.toString();

            if (cache_map[s]==""){
                cache_map[s] = pai.toString();
                return false;
            }
            return true;
        }
        
        void log_data(){
            sd.log(to_string(differenceSeconds) + "," + to_string(movimentos) + "," + to_string(count));
        }

        void showSolution(Board inicial, bool printSolution){
            string reprInicial = inicial.toString();
            string repr = solution.toString();
            movimentos = 0;

            if (printSolution){
                cout << endl << "Partindo do target ate o estado inicial" << endl;
                cout << "=== Printing solution ===" << endl << repr << endl;
            }
                
            while (repr!=reprInicial){
                string pai_repr = cache_map[repr];
                if (printSolution) cout << pai_repr << endl;
                repr = pai_repr;
                movimentos++;
            }
            // cout << "FIM -- " << movimentos << " movimentos." << endl;
            log_data();
        }

        Board getMenorHeuristica(){
            int menorHeuristica = 9999;
            int auxHeuristica;
            Board boardMenorHeuristica;
            list<Board>::iterator it2;

            for (list<Board>::iterator it=F.begin(); it != F.end(); ++it){
                auxHeuristica = (*it).mann_distance;
                if( auxHeuristica < menorHeuristica){
                    boardMenorHeuristica = *it;
                    menorHeuristica = auxHeuristica;
                    it2 = it;
                }
            }
            F.erase(it2);
            return boardMenorHeuristica;
        }
        bool solveBoardBFS(Board b, char opcao, bool heuristicaG){
            t1 = clock();

            // Checa fim e adiciona ao cache
            if (checkFim(b) || inCache(b, b)) {
                differenceSeconds = ( ( (float) clock() ) - ( (float) t1 ) )/(double) CLOCKS_PER_SEC; 
                return true;
            }

            // Add a fila/pilha dependendo do argumento useBfs
            F.push_back(b);

            // Enquanto a fila/pilha nao esta vazia
            Board board;
            while(!F.empty()){
                // cout << "Tamanho fila/pilha: " << F.size() << endl;

                // Coleta um novo "No" da fila/pilha
                switch(opcao){
                    case 'b':{
                        board = F.front();
                        F.pop_front();
                        break;
                    }
                    case 'd':{
                        board = F.back();
                        F.pop_back();
                        break;
                    }
                    case 'a':
                        board = getMenorHeuristica(); 
                        break;
                    case 'f':
                        board = getMenorHeuristica(); 
                        break;
                    default:
                        cout << "Opcao invalida" << endl;
                        return false;
                }

                // Checa iteracoes
                count++;
                if (count > 140000){
                    solution = board;
                    // cout << "Parando execucao apos 150k nós gerados" << endl;
                    break;
                }
                
                // Localiza celula vazia
                board.localizaCelulaNula();

                // Para cada posicao vazia em torno da celula vazia
                // 4 posicoes = cima, baixo, direita e esquerda
                for (int i=0; i<4; i++){ 
                    if (board.checaMovimentoValido(i)){
                        // Troca celula nula com o vizinho com movimento valido
                        Board nextBoard = board;
                        nextBoard.swap(nextBoard.celulaNula.i, nextBoard.celulaNula.vizinhos[i]);
                        
                        // Checa cache
                        if (inCache(nextBoard, board)) continue;
                        

                        // Checa fim
                        if (checkFim(nextBoard)) {
                            differenceSeconds = ( ( (float) clock() ) - ( (float) t1 ) )/(double) CLOCKS_PER_SEC; 
                            return true;
                        };
                        
                        // Calcula heuristica
                        if (opcao=='a' || opcao=='f') nextBoard.manhattan_distance(target, heuristicaG);
                        
                        // Coloca na fila/pilha
                        F.push_back(nextBoard);
                    }
                    
                }
            }
            
            differenceSeconds = ( ( (float) clock() ) - ( (float) t1 ) )/(double) CLOCKS_PER_SEC; 
            return false;
        }
};



int main(int argc, char const *argv[])
{
    srand(time(NULL));
    Solver s;
    char metodos[] = {'b', 'd', 'a', 'f'};
    bool heuristicaG;
    
    for(int j=0; j<200; j++){
        Board b = Board("random"); //random, fix or target
        if (b==s.target) continue;

        for(int k=0; k<4; k++){
            
            if (metodos[k] == 'a') heuristicaG = true;
            else heuristicaG = false;

            s = Solver();
            s.solveBoardBFS(b, metodos[k], heuristicaG);  
            s.showSolution(b, false);
            cout << metodos[k] << "," << s.sd.dados << endl;
        }
    }  
}   