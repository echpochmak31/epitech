#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MIN_MAZE_LENGTH 1
#define MAX_MAZE_LENGTH 10000
#define WALL_CHAR 'X'
#define SPACE_CHAR '*'
#define PATH_CHAR 'o'
#define DEAD_END '#'
#define MAX_NEIGHBOURS 4


void dispose(char** charMatrix) {
    for(int i = 0; i < MAX_MAZE_LENGTH; i++)
        free(charMatrix[i]);
    free(charMatrix);
}

char** newCharMatrix() {
    char ** matrix = calloc(MAX_MAZE_LENGTH, sizeof(char*));

    for (int i = 0; i < MAX_MAZE_LENGTH; i++) {
        matrix[i] = calloc(MAX_MAZE_LENGTH, sizeof(char));
    }

    return matrix;
}

typedef struct {
    int Row;
    int Column;
} Postition;


bool tracingDFS(char** maze, int height, int width, Postition position, Postition* path, int * pathLength) {

    maze[position.Row][position.Column] = PATH_CHAR;

    // check if the finish is reached 
    if (position.Row == height - 1 && position.Column == width - 1) {
        path[*pathLength] = position;
        (*pathLength)++;

        return true;
    }

    Postition* neighbours = calloc(MAX_NEIGHBOURS, sizeof(Postition));
    int validNeighbours = 0;

    // go right
    if (position.Column < width && maze[position.Row][position.Column + 1] == SPACE_CHAR) {
        Postition neighbour;
        neighbour.Row = position.Row;
        neighbour.Column = position.Column + 1;

        neighbours[validNeighbours] = neighbour;
        validNeighbours++;
    }

    // go up
    if (position.Row < height && maze[position.Row + 1][position.Column] == SPACE_CHAR) {
        Postition neighbour;
        neighbour.Row = position.Row + 1;
        neighbour.Column = position.Column;

        neighbours[validNeighbours] = neighbour;
        validNeighbours++;
    }

    // go left
    if (position.Column > 0 && maze[position.Row][position.Column - 1] == SPACE_CHAR) {
        Postition neighbour;
        neighbour.Row = position.Row;
        neighbour.Column = position.Column - 1;

        neighbours[validNeighbours] = neighbour;
        validNeighbours++;
    }

    // go down
    if (position.Row > 0 && maze[position.Row - 1][position.Column] == SPACE_CHAR) {
        Postition neighbour;
        neighbour.Row = position.Row - 1;
        neighbour.Column = position.Column;

        neighbours[validNeighbours] = neighbour;
        validNeighbours++;
    }

    for (int i = 0; i < validNeighbours; i++) {
        bool solved = tracingDFS(maze, height, width, neighbours[i], path, pathLength);
        if (solved) {
            path[*pathLength] = position;
            (*pathLength)++;
            free(neighbours);

            return true;
        }
    }
    
    maze[position.Row][position.Column] = DEAD_END;
    free(neighbours);

    return false;
}


int main(int argc, char ** argv) {

    FILE *file = fopen(argv[1], "r");
    
    if (file == NULL) {
        printf("Failed to open file.\n");

        return 1;
    }
    
    char buffer[MAX_MAZE_LENGTH + 1]; // + 1 due to '\n'

    char ** maze = newCharMatrix();
    
    int columns = -1;
    int rows = 0;

    // read maze
    while (fgets(buffer, MAX_MAZE_LENGTH + 1, file) != NULL) {

        if (columns == -1) {
            columns = strlen(buffer) - 1;
        }
        strncpy(maze[rows], buffer, columns);

        rows++;
    }

    Postition start;
    start.Row = 0;
    start.Column = 0;
    
    Postition * path = calloc(rows * columns, sizeof(Postition));
    int pathLength = 0;

    // run dfs
    bool solved = tracingDFS(maze, rows, columns, start, path, & pathLength);

    if (solved == false) {
        printf("no solution found\n");

        return 0;
    }


    // clean up dead end labels    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            if (maze[i][j] == DEAD_END) {
                maze[i][j] = SPACE_CHAR;
            }
        }
    }

    // print result
    for (int i = 0; i < rows; i++) {
        printf("%s\n", maze[i]);
    }

    fclose(file);
    dispose(maze);
    free(path);

    return 0;
}