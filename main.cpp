/* 
 * File:   main.cpp
 * Author: Dominik Veselý, Marek Přibáň
 *
 * Created on 29. září 2012, 17:00
 */

#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define DEFSIZE 2
#define MatrixType char

#define _DEBUG

using namespace std;

//=================================STACK=================================
typedef int StackElem;

struct Stack{
    //Array of elements
    StackElem *elements;
    //Size of array
    int size;
    //Index of top element in stack
    int top;
};

Stack stack;

/**
 * Check if stack is empty.
 * @return true if empty
 */
bool stackEmpty(){
    if(stack.top == -1){
        return true;
    }
    return false;
}
/** 
 * Return element on top of stack without removal.
 * @return element on top.
 */
StackElem stackTop(){
    if(!stackEmpty()){
        return stack.elements[stack.top];
    }else{
#ifdef _DEBUG
        perror("Stack is empty.\n");
#endif
    }
}
/**
 * Init stack memory.
 * @param size num of elements in stack.
 */
void stackInit(int size = DEFSIZE){
    stack.elements = (StackElem*) calloc(size,sizeof(StackElem));
    stack.size = size;
    stack.top = -1;
}
/**
 * Push element to the stack.
 * Extend stack size if necessary.
 * @param elem element pushed to stack.
 */
void stackPush(StackElem elem){
    if(stack.top == (stack.size - 1)){
#ifdef _DEBUG
        printf("Zvetsuji pamet.\n");
#endif
        //allocate new two times bigger memory
        StackElem *tmp = (StackElem*) calloc(stack.size*2,sizeof(StackElem));
        //move existing BYTES of elements from stack 
        memmove(tmp,stack.elements,sizeof(StackElem)*stack.size);
        //free old/small memory
        free(stack.elements);
        //add pointer of new bigger memory to stack
        stack.elements = tmp;
        //change stack size
        stack.size = stack.size*2;
    }
    stack.elements[++(stack.top)] = elem;
}
/**
 * Pop top/first element from the stack.
 * @return top element.
 */
StackElem stackPop(){
    if(!stackEmpty()){
        return stack.elements[(stack.top)--];
    }else{
#ifdef _DEBUG      
        perror("Stack is empty.\n");
#endif
    }
}
/**
 * Free stack memory.
 */
void stackDestroy(){
    free(stack.elements);
}
//================================LOAD GRAPH=============================
//size of matrix
int size;

/**
 * Load graph from file to memory.
 * @param file stream
 * @param path to file
 * @return array[][] with graph from file, NULL if failed.
 */
MatrixType** loadGraph(FILE* file, char *path) {
    char c;
    int col = 0, row = 0;

    MatrixType **graph;
    //open file stream
    file = fopen(path, "r");
    //check if file exists
    if (file == NULL) {
        perror("File not found.");
        exit(1);
    }
    if (fscanf(file, "%d", &size) == 1) {
        graph = (MatrixType**) calloc(size, sizeof (MatrixType*));
        while ((c = fgetc(file)) != EOF) {
            if (c == '\n' && row < size) {
                graph[row++] = (MatrixType*) calloc(size, sizeof (MatrixType));
                col = 0;
                //don't save \n char
                continue;
                //read only 50 lines
            } else if (c == '\n' && row == size) {
                break;
            } else {
                graph[row - 1][col] = c;
                col++;
            }
        }
    } else {
        perror("Error reading file.\n");
        fclose(file);
        return NULL;

    }
#ifdef _DEBUG
    printf("File readed.\n");
#endif
    //close file stream
    fclose(file);
    return graph;
}
/**
 * Free graph memory.
 * @param graph to destroy
 */
void destroyGraph(MatrixType **graph){
    //can't destroy free memory.
    if(graph == NULL)
        return;
    
    for(int i = 0; i < size; i++){
        free(graph[i]);
    }
    free(graph);
}

/*
 * 
 */
int main(int argc, char** argv) {
    /*
    stackInit();
    printf("stack size %d\n", stack.size);
    printf("stack top %d\n", stack.top);
    //prazdny?
    if(stackEmpty())
        printf("prazdny\n");
    else printf("neni prazdny \n");
    
    stackTop();
    
    stackPop();
    printf("stack top %d\n", stack.top);
    
    //vlozit hodnoty
    stackPush(1);
    printf("stack top %d\n", stack.top);

    stackPush(2);
    printf("stack top %d\n", stack.top);
    
    stackPush(3);
    printf("stack top %d\n", stack.top);
    
    stackPush(4);
    printf("stack top %d\n", stack.top);
            
    stackPush(5);
    printf("stack top %d\n", stack.top);
    
    printf("stack top %d\n",stackTop());
    printf("stack top %d\n",stackTop());
     

    printf("hodnota %d\n",stackPop());
    printf("stack top %d\n", stack.top);
    
    printf("hodnota %d\n",stackPop());
    printf("stack top %d\n", stack.top);
    
    printf("hodnota %d\n",stackPop());
    printf("stack top %d\n", stack.top);
    
    printf("hodnota %d\n",stackPop());
    printf("stack top %d\n", stack.top);
    
    printf("hodnota %d\n",stackPop());
    printf("stack top %d\n", stack.top);
    
       if(stackEmpty())
        printf("prazdny\n");
    else printf("neni prazdny \n");
    
    stackDestroy();
    */
    MatrixType **test;
    FILE *file;
    
    test = loadGraph(file, argv[1]);

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            printf("%c",test[i][j]);
        }
        printf("\n");
    }
    
    destroyGraph(test);
    return 0;
}


