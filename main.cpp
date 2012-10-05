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
#include <stack>
#include "SpanningTree.h"
#include "Edge.h"


#define MatrixType char



#define _DEBUG

using namespace std;

#pragma mark 
#pragma mark Graphs

int size = 0,currentDegree = 0,maxNodes = 0;


//================================LOAD GRAPH=============================
//size of matrix

MatrixType** allocateArray(int size) {
    MatrixType **graph = (MatrixType**) calloc(size, sizeof (MatrixType*));
    for(int i = 0 ; i < size ; i++) {
        graph[i] = (MatrixType*) calloc(size, sizeof (MatrixType));
       // memset(graph[i], 0, sizeof(MatrixType) * size);
    }
    return graph;
}

void copyArray(MatrixType** minimal, MatrixType** newMinimal) {
    for ( int i = 0; i < size; ++i ){        
      memcpy(minimal[i], newMinimal[i], sizeof (MatrixType) * size * size);
    }
}

bool isSpanningTree(MatrixType** tree) {
  //  if(maxNodes == 0 && )
}

//int getDegree(Matrix)



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
        graph = allocateArray(size);
        
        while ((c = fgetc(file)) != EOF) {
            if (c == '\n' && row < size) {
                col = 0;
                row++;
                continue;
            } else if (c == '\n' && row == size) {
                break;
            } else {
                graph[row - 1][col] = atoi(&c);
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

void printGraph(MatrixType **graph) {
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            printf("%d",graph[i][j]);
        }
        printf("\n");
    }
    printf("\n");

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
      
    
    MatrixType **G = NULL;
    MatrixType **minimalTree = NULL;
    
    
    FILE *file = NULL;
    int minimalDegree = 0;
    int currentDegree = 0;
    G = loadGraph(file, argv[1]);
    SpanningTree* st = new SpanningTree(size);
    minimalTree = allocateArray(size);
   

    stack<Edge*> stack;
    
    int startNode = 0;
    st->vertices[startNode] = OPEN;
   
    //Push startNode Adjents to the stack !
    for(int i = 0; i < size ; i++) {
        //Consider only open ones!
        if(G[startNode][i] == 1 && st->vertices[i] == FRESH) {
            cout << "Init Push: " << startNode << " " << i << endl;
            stack.push(new Edge(startNode,i));
        }
    }
    
    //BruteForce DFS!
    while (!stack.empty()) {
        
        Edge* currentEdge = stack.top();
        cout << "toped: " << currentEdge->from << " " << currentEdge->to << endl;

        
        if(st->vertices[currentEdge->to] == FRESH) {
            
            st->add(currentEdge);
            cout << currentEdge->to << " Opened" << endl;
            
            
            //Perform check for SpaningTree
            if(st->isValid()) {
                cout << "I have got tree" << endl;
                printGraph(st->matrix);
            } else {
               // Add ajdents
                for(int i = 0; i < size ; i++) {
                    //Consider only open ones!
                    if(st->vertices[i] == OPEN) {                        
                        for(int j = 0; j < size; j++) {                       
                            if(G[i][j] == 1 && st->vertices[j] == FRESH) {
                                cout << "Push: " << i << " " << j << endl;
                                stack.push(new Edge(i,j));
                            }
                        }
                    }
                }
            }
            
        } else {
            stack.pop();
            st->remove(currentEdge);
            cout << currentEdge->to << " Closed" << endl;

            delete currentEdge;
        }
        
        
    }
 
    
    
  //  printGraph(G);
  //  printGraph(spanningTree);
  //  copyArray(spanningTree,G);
   // printGraph(spanningTree);

    delete st;

    
    destroyGraph(G);
   // free(vertices);
    return 0;
}


