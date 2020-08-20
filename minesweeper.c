/*
 * Luna Dagci
 * This program is a command-line minesweeper game, and consists of 12 functions: initialization() for setting up the game; teardown() for destroying the game,
 * update(int, int, char) for updating and calculating the state of the world; display(int) for printing out the calculated world; getCoordinates(int, int, char) for getting the
 * coordinates input; adjacentBombs() for counting bombs surrounding squares, and helper "boolean" functions: isFlagged(int, int), isAsserted(int, int) for tracking spots, isValid(int, int) for making sure that a square is in range (valid); win() for winning; reveal(int,int) for revealing squares, and printGameBoard().
 */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define TRUE  1
#define FALSE 0
int HEIGHT; //board height
int WIDTH; //board width

typedef struct minesweeper {
    int nearbyBombs; //number of bombs
    char square; // variable for plotting squares
    int bomb; // condition for bomb
    int revealed; // condition for reveal
} world;
world **gameboard; //Struct 2D Array for the game board

void initialization(); void teardown(); void update(int, int, char); void display(int); void getCoordinates(int, int, char); int isFlagged(int, int); int isAsserted(int, int); void adjacentBombs(int, int);
int isValid(int, int); void reveal(int,int); void printGameBoard(); int win();

int main(int argc, char *argv[]) {

    char input; //character input
    int x = 0, y = 0, flag_condition = FALSE; //coordinates (x is horizontal, y is vertical), and the flag condition for loop

    if (argc == 3) { //user enters 2 arguments for board size. The first arguments is for running the program (./minesweeper)
        printf("Board size is %s x %s\n", argv[1], argv[2]);
        WIDTH = atoi(argv[1]);
        HEIGHT = atoi(argv[2]); }
    else if(argc < 3){ //error handling when user enters missing arguments for board size
        printf("Board size is not specified.\nBy default board size will be 10 x 10\n");
        printf("Board size is %s x 10\n", argv[1]);
        WIDTH = 10;
        HEIGHT = 10; }
    else if (argc > 3) { //error handling when user enters too many arguments for board size
        printf("2 values are expected. The first 2 values will be taken.\n");
        printf("Board size is %s x %s\n", argv[1], argv[2]);
        WIDTH = atoi(argv[1]);
        HEIGHT = atoi(argv[2]); }

    initialization(); //set up the game
    while (flag_condition == FALSE) { //loop until conditions are met
        printf("(F) flag a spot as a mine, (R) remove a flag, (A) assert that a spot is mine free, (Q)uit\n");
        scanf(" %c", &input);
        switch (input) {
            case 'F':
            case 'R':
            case 'A':
                getCoordinates(x, y, input);
                break;
            case 'Q':
                printf("Quitting game...\n");
                flag_condition = TRUE; //ending loop with flag_condition = true
                break;
            default:   //input error handling
                printf("(F) flag a spot as a mine, (R) remove a flag, (A) assert that a spot is mine free, (Q)uit\n");
                break;
        }
    }
    teardown(); //destroy game
    return 0;
}
//setting up the game  with void returning function initialization()
void initialization() {
    printf("Setting up the game...\n");
    gameboard = malloc(WIDTH * sizeof(world *)); //dynamically allocate 2d array for game board
    for (int i = 0; i < WIDTH; ++i)
        gameboard[i] = malloc(HEIGHT * sizeof(world));

    //check if allocation is successful
    if (gameboard == NULL) {
        printf("Allocation failed.\n"); exit(1);{
        }
    }
    for (int i = 0; i < WIDTH; i++)
        for (int j = 0; j < HEIGHT; j++)
            gameboard[i][j].square = 'O'; //plotting covered squares

    srand(time(0));
    int randomrow = 0;
    for (int col = 0; col < HEIGHT; ++col) {
        randomrow = rand() % (WIDTH);
        gameboard[randomrow][col].bomb = TRUE; //create bombs in random places
    }
    for (int i = 0; i < WIDTH ; ++i)
        for (int j = 0; j < HEIGHT; ++j){
            adjacentBombs(i, j); //get (count) bombs surrounding squares.
        }
}
//setting up the game  with void returning function teardown()
void teardown() {
    printf("Destroying game...\n");
    free(gameboard); //free game board (destroy)
    exit(1); //exit game
}
// Helper input function for coordinates to avoid duplicate code and make code simple
void getCoordinates(int horiz, int vert, char input) {
    printf("Enter your horizontal coordinate (0 - %d) and press enter\n", WIDTH);
    scanf(" %d", &horiz);
    while (!(horiz >= 0 && horiz <= WIDTH)) scanf(" %d", &horiz);  //input error handling. Lets the user enter the correct input until the input is in correct range
    printf("Enter your vertical coordinate (0 - %d) and press enter\n", HEIGHT);
    scanf(" %d", &vert);
    while (!(vert >= 0 && vert <= HEIGHT)) scanf(" %d", &vert); //input error handling. Lets the user enter the correct input until the input is in correct range
    //instead of 0 and 10, put user determined board sizes
    update(horiz, vert, input); //update state of the world with given coordinates
}
//update the state of the world, with 2 integer parameters
void update(int hor, int ver, char input) {
    switch (input) {
        case 'F':
            if (isFlagged(hor, ver) == TRUE || isAsserted(hor,ver) == TRUE) {
                printf("This spot can't be flagged."); //if a square is already flagged or asserted, user can't put a flag there
                if (isFlagged(hor, ver) == TRUE) printf(" Because this spot is already flagged.\n");
                else printf(" Because this spot was already asserted\n");
            }
            else {
                gameboard[hor][ver].square = 'F'; //put a flag on the coordinate user entered
                display(FALSE);
            }
            break;
        case 'R':
            if (isFlagged(hor, ver) == FALSE) printf("This spot has no flag to remove.\n");
            else {
                gameboard[hor][ver].square = 'O'; //remove flag
                display(FALSE);
            }
            break;
        case 'A':
            if (isAsserted(hor,ver) == 1) printf("This spot was already asserted.\n");
            else if (isFlagged(hor,ver) == 1) printf("This spot is flagged.\n");
            else {
                if (gameboard[hor][ver].bomb == TRUE) {
                    gameboard[hor][ver].square = '*';
                    display(TRUE); //print out the calculated result
                }
                else {
                    reveal(hor, ver);
                    display(FALSE); //print out the calculated result
                }
            }
            break;
        default: printf("error");
            break;
    }
}
//Print the result calculated in the state of the world with function display, with int parameter condition for condition calculated in the function update()
void display(int condition) {
    if (condition == TRUE) {
        //show all bombs
        for (int i=0; i < WIDTH; ++i)
            for (int j=0; j < HEIGHT; ++j)
                if (gameboard[i][j].bomb == TRUE) gameboard[i][j].square = '*';
        printGameBoard(); //print gameboard
        printf("BOOM! Game over :(\n");
        teardown(); //User lost the game, therefore game ends.
    }
    else if (condition == FALSE) {
        printf("OK!\n");
        printGameBoard(); //print gameboard
        if (win() == TRUE) {
            printf("Congratulations!!! You won the game :)\n");
            teardown();
        }
    }
}
//if every bomb is flagged, and there's no flagged non-bomb spots, then print a win message
int win(){
    for (int i=0; i < WIDTH; ++i)
        for (int j=0; j < HEIGHT; ++j)
            if ((gameboard[i][j].bomb ==TRUE && gameboard[i][j].square == 'F')) return TRUE;
            else return FALSE;
}
int isFlagged(int x, int y){
    if(gameboard[x][y].square == 'F') return TRUE;
    else return FALSE;
}
int isAsserted(int x, int y){
    if(gameboard[x][y].square == 'X') return TRUE;
    else return FALSE;
}
//An Utility Function to check whether given cell (row, col) is a valid cell or not: A helper function derived from geeksforgeeks for counting bombs surrounding a square
int isValid(int row, int col){
    if((row >= 0) && (row < WIDTH) && (col >= 0) && (col < HEIGHT))
        return TRUE;
    else return FALSE;

}
//Count bombs surrounding a square using 1 in 4 bombs as the ratio: north, south, east, west, northeast, northwest, southeast, southwest. I used the geeksforgeeks for reference
void adjacentBombs(int row, int col){

    //north
    if (isValid (row-1, col) == TRUE) //if the spot exists
    {
        if (gameboard[row-1][col].bomb == TRUE) gameboard[row][col].nearbyBombs++; //and there is a bomb in this direction, increment number of adjacent bombs
    }
    //south
    if (isValid (row+1, col) == TRUE)
    {
        if (gameboard[row+1][col].bomb == TRUE) gameboard[row][col].nearbyBombs++;
    }
    //east
    if (isValid (row, col+1) == TRUE)
    {
        if (gameboard[row][col+1].bomb  == TRUE) gameboard[row][col].nearbyBombs++;
    }
    //west
    if (isValid (row, col-1) == TRUE)
    {
        if (gameboard[row][col-1].bomb == TRUE) gameboard[row][col].nearbyBombs++;
    }
    //northeast
    if (isValid (row-1, col+1) == TRUE)
    {
        if (gameboard[row-1][col+1].bomb == TRUE) gameboard[row][col].nearbyBombs++;
    }
    //northwest
    if (isValid (row-1, col-1) == TRUE)
    {
        if (gameboard[row-1][col-1].bomb == TRUE) gameboard[row][col].nearbyBombs++;
    }
    //southeast
    if (isValid (row+1, col+1) == TRUE)
    {
        if (gameboard[row+1][col+1].bomb == TRUE) gameboard[row][col].nearbyBombs++;
    }
    //southwest
    if (isValid (row+1, col-1) == TRUE)
    {
        if (gameboard[row+1][col-1].bomb == TRUE) gameboard[row][col].nearbyBombs++;
    }
}
/*
* ALGORITHM FOR REVEALING:
       Using a recursive approach, referencing from flood fill algorithm:
       STEP 1: If a square is valid then do the following:
       STEP 2: if surrounding squares of asserted square has 0 surrounding bombs, and is not a bomb: Reveal it.
       STEP 3: else if surrounding square of asserted square has surrounding bombs: stop, and print hint number of bombs.
       STEP 4: check next surrounding square of the asserted square
       STEP 5: if the square is already revealed, then stop.
        */
void reveal(int row, int col) {
    if (isValid(row, col) == TRUE) { //base case : for recursion: invalid coordinates
        if (gameboard[row][col].revealed == TRUE) return; //base case : if already revealed: stop
        if (gameboard[row][col].bomb == TRUE || gameboard[row][col].nearbyBombs != 0) { //base case : if there is bomb and bomb near the coordinates: reveal and stop
            gameboard[row][col].revealed = TRUE;
            return;
        } else {
            gameboard[row][col].revealed = TRUE; //reveal square
            // north row-1, col
            reveal(row - 1, col);
            // south row+1, col
            reveal(row + 1, col);
            // east row, col+1
            reveal(row, col + 1);
            // west row, col-1
            reveal(row, col - 1);
            // northeast row-1, col+1
            reveal(row - 1, col + 1);
            // northwest row-1, col-1
            reveal(row - 1, col - 1);
            // southeast row+1, col+1
            reveal(row + 1, col + 1);
            // southwest row+1, col-1
            reveal(row + 1, col - 1);
        }
    }   else
        return;
}
void printGameBoard(){
    for (int i=0; i < WIDTH; ++i)
    {
        for (int j=0; j < HEIGHT; ++j) {
            if (gameboard[i][j].revealed == TRUE && gameboard[i][j].nearbyBombs != 0){ // if the square is revealed and has surrounding bombs
                gameboard[i][j].square = gameboard[i][j].nearbyBombs + '0'; //print how many bombs there are around that square (hint numbers)
            } else if(gameboard[i][j].revealed == TRUE && gameboard[i][j].nearbyBombs == 0) //if the square is revealed and has no surrounding bombs
                gameboard[i][j].square = 'X'; //reveal that square

            printf("%c ", gameboard[i][j].square);
        }
        printf("\n");
    }
}
