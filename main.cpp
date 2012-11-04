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
#include "mpi.h"

#define MatrixType char
#define kLowerBound 2
#define INF 1<<16

//How often I will check for messages
#define CHECK_MSG_AMOUNT  100

//Messages types
#define MSG_WORK_REQUEST 1000
#define MSG_WORK_SENT    1001
#define MSG_WORK_NOWORK  1002
#define MSG_TOKEN        1003
#define MSG_FINISH       1004
#define MSG_NEW_MINIMUM  1005

//Process Statuses
#define STATUS_WORKING  2001
#define STATUS_IDLE     2002
#define STATUS_FINISHED 2999

//Process colors
#define WHITE_PROCESS 3001
#define BLACK_PROCESS 3002

//Tokens
#define WHITE_TOKEN 3003
#define BLACK_TOKEN 3004


using namespace std;
#pragma mark 
#pragma mark Graphs

int size = 0;


//================================LOAD GRAPH=============================
//size of matrix

MatrixType** allocateArray(int size) {
    MatrixType **graph = (MatrixType**) calloc(size, sizeof (MatrixType*));
    for(int i = 0 ; i < size ; i++) {
        graph[i] = (MatrixType*) calloc(size, sizeof (MatrixType));
    }
    return graph;
}

void copyArray(MatrixType** minimal, MatrixType** newMinimal) {
    for ( int i = 0; i < size; ++i ){        
      memcpy(minimal[i], newMinimal[i], sizeof (MatrixType) * size);
    }
}

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
      
    //Define structures
    MatrixType **G = NULL;
    MatrixType **minimalTree = NULL;   
    FILE *file = NULL;
    int minimalDegree = INF;
    
    //Initialize them
    G = loadGraph(file, argv[1]);    
    SpanningTree* st = new SpanningTree(size);
    minimalTree = allocateArray(size);
   
    // Stack
    stack<Edge*> stack;
    
    /* ptsl jsem se na praci */
    bool wasRequestForWork = false;
    /*Posílaná data - mozno zmenit dle implementace na jiny typ podporovany MPI*/
    int message[100];
    /*Cislo meho procesu - 0 je master*/
    int my_rank;
    /*Pocet vsech procesu*/
    int processorCount;
    /*Slouzi pro pravidelne kontrolovani prijatych zprav*/
    unsigned int checkCounter;
    /*Koho se mam zeptat na praci*/
    unsigned int processToAskForWork;
    /*Aktualni stav procesu*/
    int processStatus = 0;
    /*Ukoncovaci token*/
    int token = 0;
    /*Naanaanananaaa*/
   // int color = WHITE_PROCESS;
    /**/
    bool vectorSent = false;
    /**/
    bool isResult = false;
    bool wasWorking = false;
    
    /*mereni casu */
    double time1, time2 = 0;

    
    
    /* Inicializace MPI prostředí */
    MPI_Init(&argc, &argv);
    
    /*INICIALIZACE PROCESŮ - ZÁKLADNÍ ROZDĚLENÍ PRÁCE*/
    
    /* Zjistí jaký jsem proces a uloží do my_rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    /*Zjistí kolik procesoru se ucastni vypoctu*/
    MPI_Comm_size(MPI_COMM_WORLD, &processorCount);
    
    
    /*cekani na vsechny procesy pred roydelenim dat*/
    MPI_Barrier(MPI_COMM_WORLD);
    cout << "MPI_Barrier START" << endl;
    
    if(my_rank == 0) {
        time1 = MPI_Wtime();
    }
    
    cout << "RANK:" << my_rank << endl;
    
    //Start of the algorithm start with 1st vertex
    int startNode = 0;
    st->vertices[startNode] = OPEN;
   
    //Push startNode Adjents to the stack !
    for(int i = 0; i < size ; i++) {
        //Consider only open ones!
        if(G[startNode][i] == 1 && st->vertices[i] == FRESH) {
       //     cout << "Init Push: " << startNode << " " << i << endl;
            stack.push(new Edge(startNode,i));
        }
    }
    
    
    
    
    
    
    
    //BruteForce DFS!
    while (!stack.empty()) {
        
        Edge* currentEdge = stack.top();
      //  cout << "toped: " << currentEdge->from << " " << currentEdge->to << endl;
        
        //If Vertex is opened -> proceed
        if(st->vertices[currentEdge->to] == FRESH) {            
            st->add(currentEdge);
        //    cout << currentEdge->to << " Opened" << endl;
            
            //if we have worse solution than current best abandon branch
            if(minimalDegree < st->degree) {
              //  cout << "Branch not perspective" << endl;
                continue;
            }        
            
            //Perform check for SpaningTree
            if(st->isValid()) {
                
                //If we find new minimum we will asign it
                if(minimalDegree > st->degree) {
                    minimalDegree = st->degree;
                    copyArray(minimalTree, st->matrix);
                    cout << "I have got tree with degree: " << st->degree << endl;
                }
                
                //If we find best possible solution we dont continue
                if(minimalDegree == kLowerBound) {
                //    cout << "Found lower boundary" << endl;
                    break;
                }
                
//               cout << "I have got tree with degree: " << st->degree << endl;
              // printGraph(st->matrix);
            } else {
               // Add ajdents
                for(int i = 0; i < size ; i++) {
                    //Consider only open ones!
                    if(st->vertices[i] == OPEN) {                        
                        for(int j = 0; j < size; j++) {                       
                            if(G[i][j] == 1 && st->vertices[j] == FRESH) {
                              //  cout << "Push: " << i << " " << j << endl;
                                stack.push(new Edge(i,j));
                            }
                        }
                    }
                }
            }
            
        } else {
            //Remove edge, close vertex, backtrack
            stack.pop();
            st->remove(currentEdge);
           // cout << currentEdge->to << " Closed" << endl;
            delete currentEdge;
        }
        
    }
    
    //Print result
     cout << "Best solution has degree: " << minimalDegree << endl;
     printGraph(minimalTree);

    //clean up
    delete st;
    destroyGraph(minimalTree);
    destroyGraph(G);
    return 0;
}


