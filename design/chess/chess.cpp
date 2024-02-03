#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <chrono>
using namespace std;

class Piece{

    protected:

    string color;

    public:

    virtual bool isValidMove(Position* start, Position* end) = 0;
};

class Pawn : public Piece{

    public:

    Pawn(string pColor){
        Piece::color = pColor;
    }

    bool isValidMove(Position* start, Position* end){
        //implement it
        // can player put piece from start positon to end position?
    }
};

class Position{
    public:
    int x;
    int y;

    Position(int px, int py){
        x = px;
        y = py;
    }
};

class Cell{
    public:
    Position* position;
    Piece* piece;

    Cell(int px, int py, Piece* pPiece){
        position = new Position(px, py);
        piece = pPiece;
    }

    bool isValidMove(Position* p1, Position* p2){
        return piece->isValidMove(p1,p2);
    }
};

class Board{
    vector<vector<Cell*>> board;

    public:

    Board(){
        board.resize(8, vector<Cell*>(8, nullptr));
        initialiseBoard();
    }

    void initialiseBoard(){
        for(int i=0; i<8; i++){
            for(int j=0; j<8; j++){
                if(i==1){
                    Piece* piece = new Pawn("White");
                    board[i][j] = new Cell(i,j,piece);
                }
                if(i==6){
                    Piece* piece = new Pawn("Black");
                    board[i][j] = new Cell(i,j,piece);
                }
                //initiaise all the pieces and cells
            }
        }
    }

    bool move(Cell* start, Cell* end){
        if(start->isValidMove(start->position,end->position)){
            //move start to end, if end already occupied then free up and place
            //also make this cell free
            return true;
        }else{
            cout<<"Not a valid move!"<<endl;
            return false;
        }
    }

    vector<vector<Cell*>> getBoard(){
        return board;
    }
};

class Player{
    string name;
    string color;
    string rating;

    public:

    Player(string pName, string pColor, string pRating){
        name = pName;
        rating = pRating;
        color = pColor;
    }

    void setColor(string pColor){
        color = pColor;
    }

    string getColor(){
        return color;
    }
};

class Game{
    Board* board;
    Player* player1;
    Player* player2;

    public:

    Game(Player* p1, Player* p2){
        board = new Board(); //board could follow singelton design pattern
        player1 = p1;
        player2 = p2;
    }

    bool move(Player* p, Position* start, Position* end){
        //validations like if player p is moving his own pieces or not?
        //also if anything exists at start position or not?

        vector<vector<Cell*>> chessBoard = board->getBoard();
        Cell* startingCell = chessBoard[start->x][start->y];
        Cell* endingCell = chessBoard[end->x][end->y];

        if(board->move(startingCell, endingCell)){
            cout<<"Move success!!"<<endl;
            return true;
        }else{
            cout<<"Illegal Move!!"<<endl;
            return false;
        }
    }
};