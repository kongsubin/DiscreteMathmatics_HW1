#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv){
    char buffer[51][51];
    char *splitPtr;
    int rowcount = 0;         	  	        // input row
    int colcount = 0;                       // input column
    int rows, cols = 0;
    int whiteInfo[51];
    int blackInfo[51];

    if(argc==1){
	  printf("put the input filename");
	  return -1;
    }    
    // stdin to buffer
    FILE *fp = fopen(argv[1], "r");

    // Count rows and Save the file text
    while (!feof(fp)){
        fgets(buffer[rowcount], sizeof(buffer), fp);
        rowcount++;
    }
    fclose(fp);
    rows = rowcount - 2;

    // we have to make 'split' by " "
    // Save the blackInfo
    splitPtr = strtok(buffer[0], " ");
    while (splitPtr != NULL){
        blackInfo[colcount] = atoi(splitPtr);
        splitPtr = strtok(NULL, " ");
        colcount++;
    }
    cols = colcount;
    
    // remove ' ' and save board info
    int **board;
    board = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++){
        board[i] = (int *)malloc(cols * sizeof(int *));
    }
    for (int i = 1; i <= rows; i++){
        int j = 0;
        int k = 0;
        while (j < cols){
            if (buffer[i][k] == '\n'){
                break;
            }
            if (buffer[i][k] != ' '){
                board[i-1][j] = buffer[i][k] - '0';
                j++;
            }
            k++;
        }
    }

    int count = 0;
    // Save the whiteInfo
    for (int i = 1; i <= rows; i++){
        splitPtr = strtok(buffer[i], " ");
        count = 0;
        while (splitPtr != NULL){
            if(count == cols)
                whiteInfo[i-1] = atoi(splitPtr); // end number of row 
            splitPtr = strtok(NULL, " ");
            count++;
        }
    }

    // making formula
    fp = fopen("formula", "w");

    // //declare pro
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            fprintf(fp, "(declare-const p%d_%d Int)\n", i, j);

    //Q1. all proposition has 0 or 1 value.
    fprintf(fp, "(assert (and ");
    for (int i = 0; i < rows; i++){
      fprintf(fp, "(and ");
      for (int j = 0; j < cols; j++){
        fprintf(fp, "(or (= p%d_%d 1) (= p%d_%d 0))", i, j,  i, j);
      }
      fprintf(fp, ")");
    }
    fprintf(fp, "))\n");

    //Q2. Check the column and Find the black cell 
    fprintf(fp, "(assert (and ");
    for(int j=0; j<cols; j++){
       fprintf(fp, "(= (+ ");
       for(int i=0; i<rows; i++){
           fprintf(fp, "(* p%d_%d %d) ", i, j,  board[i][j]);
       }
       fprintf(fp, " ) %d) ", blackInfo[j]);
    }
    fprintf(fp, "))\n");

    //Q3. Check the row and Find the white cell 
    fprintf(fp, "(assert (and ");
    for(int i=0; i<rows; i++){
       fprintf(fp, "(= (- 0 (+ ");
       for(int j=0; j<cols; j++){
           fprintf(fp, "(* (- p%d_%d 1) %d) ", i, j, board[i][j]);
       }
       fprintf(fp, " ) ) %d) ", whiteInfo[i]);
    }
    fprintf(fp, "))\n");

    fprintf(fp, "(check-sat)\n");
    fprintf(fp, "(get-model)\n");
    fclose(fp);

    //z3 formula command execute
    FILE *fin = popen("z3 formula ", "r");
    char buf[128];
    char b[128];
    char s[128];
    char t[128];
    int** result = (int**) malloc(sizeof(int*) * rows);
    for(int i = 0; i < rows; i++) {
        result[i] = (int*) malloc(sizeof(int) * cols);
    }

    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            result[i][j] = 0;
        }
    }
    fscanf(fin, "%s", buf);

    // no solution
    if (strcmp(buf, "unsat") == 0){
        printf("No solution\n");
        free(board);
        free(result);
        return 0;
    }
    int i, j, k;
    fscanf(fin, "%s", buf);
    while (!feof(fin)){
        fscanf(fin, "%s %s %s %s %s", b, s, b, b, t);
        sscanf(s, "p%d_%d", &i, &j);
        if (strcmp(t, "0)") != 0){
            result[i][j] = 1;
        }
    }

    // print solution
    for (int x = 0; x < rows; x++){
        for (int y = 0; y < cols; y++){
            printf("%d ", result[x][y]);
        }
        printf("\n");
    }
    printf("\n");
    free(board);
    free(result);
    return 0;
}
