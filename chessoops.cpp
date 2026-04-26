#include <iostream>
// #include <filesystem>
#include <cmath>
using namespace std;

enum piececolor
{
    black,
    white,
    none,
};
class Chessboard;

class piece

{
    char character;

protected:
    piececolor color;

public:
    piece()
    {
        character = '.';
        color = none;
    }
    piece(piececolor col, char c)
    {
        character = c;
        color = col;
    }
    virtual ~piece() {}
    virtual char getchar() { return character; }
    bool iswhite() { return color == white; }
    bool isblack() { return color == black; }
    virtual bool movementlogic(int scrrow, int scrcol, int desrow, int descol, Chessboard &obj) = 0;
};
class Chessboard
{
    piece *board[8][8];

public:
    Chessboard()
    {

        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                board[i][j] = nullptr;
    }
    piece *ispieceat(int desrow, int descol)
    {
        return board[desrow][descol];
    }
    void setPieceAt(int row, int col, piece *p)
    {
        board[row][col] = p;
    }

    void display()
    {
        string alpha = "   a b c d e f g h";
        cout << alpha << endl;
        cout << " +-----------------+" << endl;

        for (int i = 0; i < 8; i++)
        {
            cout << 8 - i << "| ";
            for (int j = 0; j < 8; j++)
            {
                if (board[i][j] != nullptr)
                    cout << board[i][j]->getchar() << " ";
                else
                    cout << ". ";
            }
            cout << "|" << 8 - i << endl;
        }

        cout << " +-----------------+" << endl;
        cout << alpha << endl;
    }
    bool move(int scrrows, int scrcol, int desrow, int descol)
    {

        if (board[scrrows][scrcol] == nullptr)
            return false;
        delete board[desrow][descol];
        board[desrow][descol] = board[scrrows][scrcol];
        board[scrrows][scrcol] = nullptr;
        return true;
    }
    piece *getPieceAt(int row, int col)
    {

        return board[row][col];
    }
};

class king : public piece
{
public:
    king(piececolor col) : piece(col, (col == white) ? 'K' : 'k')
    {
    }
    ~king() {}
    bool movementlogic(int scrrow, int scrcol, int desrow, int descol, Chessboard &obj)
    {
        int dx = abs(desrow - scrrow);
        int dy = abs(descol - scrcol);
        if ((dx <= 1 && dy <= 1) && !(dx == 0 && dy == 0))
        {
            piece *despostion = obj.ispieceat(desrow, descol);
            if (despostion != nullptr && (iswhite() && isupper(despostion->getchar()) || (isblack() && islower(despostion->getchar()))))
            {
                cout << "cant capture own piece ";
                return false;
            }
            cout << "move valid";
            obj.move(scrrow, scrcol, desrow, descol);
            return true;
        }
        else
        {
            cout << "invalid";
            return false;
        }
    }
};
class Rook : public piece
{
public:
    Rook() {}
    Rook(piececolor col) : piece(col, (col == white) ? 'R' : 'r')
    {
    }
    bool movementlogic(int scrrow, int scrcol, int desrow, int descol, Chessboard &obj)
    {
        if (scrrow == desrow)
        {
            int minval = min(scrcol, descol);
            int maxval = max(scrcol, descol);

            // Check all squares between source and destination
            for (int i = minval + 1; i < maxval; i++)
            {
                if (obj.getPieceAt(desrow, i) != nullptr)
                {
                    cout << "Cannot jump over the piece.\n";
                    return false;
                }
            }

            // After the loop, check the destination
            piece *destPiece = obj.ispieceat(desrow, descol);
            if ((iswhite() && isupper(destPiece->getchar())) || (isblack() && islower(destPiece->getchar())))
            {
                cout << "Cannot capture your own piece.\n";
                return false;
            }

            cout << "Correct move\n";
            obj.move(scrrow, scrcol, desrow, descol);
            return true;
        }

        else if (scrcol == descol) // Vertical movement
        {
            int minval = min(scrrow, desrow);
            int maxval = max(scrrow, desrow);

            for (int i = minval + 1; i < maxval; i++)
            {
                if (obj.getPieceAt(i, scrcol) != nullptr)
                {
                    cout << "Cannot jump over a piece.\n";
                    return false;
                }
            }

            piece *destPiece = obj.ispieceat(desrow, descol);
            if ((iswhite() && isupper(destPiece->getchar())) || (isblack() && islower(destPiece->getchar())))
            {
                cout << "Cannot capture your own piece.\n";
                return false;
            }

            cout << "Valid vertical move\n";
            obj.move(scrrow, scrcol, desrow, descol);
            return true;
        }
        else
        {
            cout << "Invalid rook move: must move in straight line.\n";
            return false;
        }
    }
};
class Bishop : public piece
{
public:
    Bishop() {}
    Bishop(piececolor col) : piece(col, (col == white) ? 'B' : 'b') {}
    bool movementlogic(int scrrow, int scrcol, int desrow, int descol, Chessboard &obj)
    {
        if (abs(desrow - scrrow) == abs(descol - scrcol))
        {
            int rowstep = (scrrow < desrow) ? 1 : -1;
            int colstep = (scrcol < descol) ? 1 : -1;
            int i = scrrow + rowstep;
            int j = scrcol + colstep;
            for (; i != desrow && j != descol; i += rowstep, j += colstep)
            {
                if (obj.getPieceAt(i, j) != nullptr)
                {
                    cerr << "can not pass over a piece \n";
                    return false;
                }
            }
            piece *destPiece = obj.ispieceat(desrow, descol);
            if ((iswhite() && isupper(destPiece->getchar())) || (isblack() && islower(destPiece->getchar())))
            {
                cerr << "Cannot capture your own piece.\n";
                return false;
            }

            cout << "Valid diagonal move\n";
            obj.move(scrrow, scrcol, desrow, descol);
            return true;
        }
        else
        {
            return false;
        }
    }
};
class Queen : public piece
{
public:
    Queen(piececolor col) : piece(col, (col == white) ? 'Q' : 'q') {}
    ~Queen() {}
    bool movementlogic(int scrrow, int scrcol, int desrow, int descol, Chessboard &obj)
    {
        Rook r(color);
        Bishop b(color);
        if (b.movementlogic(scrrow, scrcol, desrow, descol, obj))
        {
            return true;
        }
        else if (r.movementlogic(scrrow, scrcol, desrow, descol, obj))
        {
            return true;
        }
        else

            return false;
    }
};

class pawn : public piece

{
public:
    pawn(piececolor col) : piece(col, (col == white) ? 'P' : 'p') {}
    ~pawn() {}
    bool movementlogic(int scrrow, int scrcol, int desrow, int descol, Chessboard &obj)
    {
        int rowstep = (color == white) ? -1 : 1; // -1 for white and 1 for balck to move moveforward in array
        int startrow = (color == white) ? 6 : 1;
        piece *destination = obj.getPieceAt(desrow, descol);

        if (scrcol == descol && desrow == scrrow + rowstep && destination == nullptr)
        {

            obj.move(scrrow, scrcol, desrow, descol);
            return true;
        }

        if (scrcol == descol && scrrow == startrow && desrow == scrrow + 2 * rowstep && destination == nullptr && destination == nullptr)
        {
            obj.move(scrrow, scrcol, desrow, descol);
            return true;
        }
        // disgnal capture colums  between diff 1 ,, destination position can not be '.'
        if (abs(descol - scrcol) == 1 && desrow == scrrow + rowstep && destination != nullptr)
        {
            if (iswhite() && isupper(destination->getchar()) || isblack() && islower(destination->getchar()))
            {
                cout << "can not capture own piece \n";
                return false;
            }

            obj.move(scrrow, scrcol, desrow, descol);
            return true;
        }
        // promotion logic
        delete obj.getPieceAt(desrow, descol);
        if (iswhite() && desrow == 0 || isblack() && desrow == 7)
        {

            char choice;
            cout << "tell your choice \n";
            cin >> choice;
            delete obj.getPieceAt(desrow, descol);
            switch (choice)
            {
            case 'Q':
                obj.setPieceAt(desrow, descol, new Queen(color));
                break;
            case 'R':
                obj.setPieceAt(desrow, descol, new Rook(color));
                break;
            case 'B':
                obj.setPieceAt(desrow, descol, new Bishop(color));
                break;
            case 'N':
                // board[desrow][descol] = new Knight(pawn_color);
                break;
            default:
                obj.setPieceAt(desrow, descol, new Queen(color)); // default to Queen
            }
        }
    }
};

int main()
{
}
