/* 
 * File:   main.cpp
 * Author: Dominik Veselý, Marek Přibáň
 *
 * Created on 29. září 2012, 17:00
 */

#include <iostream>
#include <fstream>
#include <limits>
#include <list>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <list>
#include <vector>

#include "mpi.h"
#include "SpanningTree.h"
#include "Edge.h"
#include "stdlib.h"

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
int size = 0;


//Headers
bool sendWork(int target, MPI_Request* request, int* buff, int maxbuffLenght, list<Edge*> &stack, SpanningTree* tree);
void sendFirstEdge(int target, int* buff, Edge* edge);
void receiveWork(MPI_Status* status, int* buff, int maxbuffLenght, list<Edge*> &stack, SpanningTree* tree);
void receiveFirstEdge(MPI_Status* status, int* buff, list<Edge*> &stack);
void sendNewMin(int minDegree, int rank, int processorCount);
void handleMinimum();
MatrixType** allocateArray(int size) ;
void copyArray(MatrixType** minimal, MatrixType** newMinimal) ;
MatrixType** loadGraph(FILE* file, char *path) ;
void printGraph(MatrixType **graph) ;
void destroyGraph(MatrixType **graph);


/*
 * 
 */
int main(int argc, char** argv) {
      
    //Define structures
    MatrixType **G = NULL;
    MatrixType **minimalTree = NULL;   
    FILE *file = NULL;
    int minimalDegree = INF;
    
    int flag;
    
    MPI_Status status;
    MPI_Request request;
    
    //Initialize them
    G = loadGraph(file, argv[1]);
    
    SpanningTree* st = new SpanningTree(size);
    minimalTree = allocateArray(size);   
    list<Edge*> stack;    
    Edge* currentEdge;
    
    int maxbuffLength = (size * 2 + 1) * 2 + 2;
    int* buff = new int[maxbuffLength];
    
    //Process asked for work
    bool wasRequestForWork = false;
    //Proccess id (master = 0)
    int rank;    
    //Processor count
    int processorCount;
    //counter for messages checking
    unsigned int checkCounter = 0;
    unsigned int processToAskForWork;
    
    //State,Color,Token
    int processStatus = 0;
    int color = WHITE_PROCESS;
    int token = 0;
    int  i = 0;

    
    //Token sent
    bool tokenSend = false;
    
    //If Process has result
    bool hasResult = false;
   // bool wasWorking = false;
    
    /*mereni casu */
    double time1, time2 = 0;

    
    //MPI INIT
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processorCount);
    
    
    /*cekani na vsechny procesy pred roydelenim dat*/
    MPI_Barrier(MPI_COMM_WORLD);
    cout << "MPI_Barrier START" << endl;
    
    if(rank == 0) {
        time1 = MPI_Wtime();
    }
    
    cout << "RANK:" << rank << endl;
    
    //Start of the algorithm start with 1st vertex
    int startNode = 0;
    st->vertices[startNode] = OPEN;
   
    
    //I am master I will divide initial work/or send no work keep atleast one for me.
    if(rank == 0) {
        
        
        
        int lastSentAdjacent = 1;
        vector<int> startNodeAdj;
        for(int i = 0; i < size ; i++) {
            if(G[startNode][i] == 1 && st->vertices[i] == FRESH) {
                startNodeAdj.push_back(i);
            }
        }
        
        //How many 1st tier states I have
        unsigned long startAdjacencySize = startNodeAdj.size();
        
        // Divide states except first one to other workers!
		for(i = 1; i < processorCount; i++) {            
			if(i < startAdjacencySize) {
				int adjacent = startNodeAdj[i];
				Edge* e = new Edge(startNode, adjacent);
				sendFirstEdge(i, buff, e);
				lastSentAdjacent++;
			//	cout << "Poslal jsem praci procesu " << i << endl;
			}
			else {
				MPI_Send(0, 0, MPI_INT, i, MSG_WORK_NOWORK, MPI_COMM_WORLD);
			//	cout << "Poslal jsem NO_WORK procesu " << i << endl;
			}
		}
        
        //First one goes for me!
        int adjacentForMaster = startNodeAdj[0];
		stack.push_back(new Edge(startNode, adjacentForMaster));
		//cout << "Pridelil jsem si prvniho souseda" << endl;
        
        
        // I will keep also the rest of the states which were un asigned
        while(lastSentAdjacent < startAdjacencySize){
			//adjacentForMaster =
			stack.push_back(new Edge(startNode, startNodeAdj[i]));
		//	cout << "Pridelil jsem si " << lastSentAdjacent << " souseda" << endl;
			lastSentAdjacent++;
		}
        
		processStatus = STATUS_WORKING;
        //End of master
    } else {
        
        //I am slave I am waiting what will came to me :>
        while (processStatus == 0) {
			MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
			if (flag) {                
				switch (status.MPI_TAG) {
					case MSG_WORK_NOWORK:
                        //NOWORK from master I will became IDLE for a while 
						MPI_Recv(0, 0, MPI_INT, 0, MSG_WORK_NOWORK, MPI_COMM_WORLD, &status);
						processStatus = STATUS_IDLE;
						break;
					case MSG_WORK_SENT:
                        //Master has some work for me, accept and set on working
						receiveFirstEdge(&status, buff, stack);
						processStatus = STATUS_WORKING;
						break;
				}
			}
		}
    } //End of slave
        
   processToAskForWork = (rank + 1) % processorCount;
    
    MPI_Barrier(MPI_COMM_WORLD);
	cout << "MPI_Barrier INIT WORK SENT" << endl;
    
    
    //Main Process Run!!
    while (processStatus != STATUS_FINISHED) {
        
        //If i am last processor and i am iddle ... computing ended.
        if ((rank == 0) && (processorCount == 1) && (processStatus == STATUS_IDLE)) {
            processStatus = STATUS_FINISHED;
        }
        
        
        //IDLE STATE BEHAVIOUR
        if (processStatus == STATUS_IDLE) {
            
            /*ADUV */
            if ((rank == 0) && (!tokenSend)) {
                color = WHITE_PROCESS;
                token = WHITE_TOKEN;
                MPI_Send(&token, 1, MPI_INT, 1, MSG_TOKEN, MPI_COMM_WORLD);
                tokenSend = true;
            } else if ((token != 0) && (rank != 0)) {
				int nextProcesRank = (rank + 1) % processorCount;
                MPI_Send(&token, 1, MPI_INT, nextProcesRank, MSG_TOKEN, MPI_COMM_WORLD);
				color = WHITE_PROCESS;
                token = 0;                
            }
            
            
            //I am Asking for some work!
            int target = processToAskForWork % processorCount;
            if (!wasRequestForWork) {
                if (target != rank) {
                    MPI_Send(0, 0, MPI_INT, target, MSG_WORK_REQUEST, MPI_COMM_WORLD);
                    wasRequestForWork = true;
                    
                }
            }
            
            processToAskForWork++;           
            MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);            
            if (flag) {
                
                switch (status.MPI_TAG) {
                        //Work Request I dont have any -> NO WORK
                    case MSG_WORK_REQUEST:
                        MPI_Recv(0, 0, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                        MPI_Send(0, 0, MPI_INT, status.MPI_SOURCE, MSG_WORK_NOWORK, MPI_COMM_WORLD);                       
                        break;
                        //Work has been send to me I will accept it
                    case MSG_WORK_SENT:
                        receiveWork(&status, buff, maxbuffLength, stack, st);
                        wasRequestForWork = false;
                        processStatus = STATUS_WORKING;
                        break;
                        //No Work Response remain idle
                    case MSG_WORK_NOWORK:
                        MPI_Recv(0, 0, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                        wasRequestForWork = false;
                        processStatus = STATUS_IDLE;
                        break;                        
                        //ADUV Token 
                    case MSG_TOKEN:                       
                        MPI_Recv(&token, 1, MPI_INT, status.MPI_SOURCE, MSG_TOKEN, MPI_COMM_WORLD, &status);                        
                        if ((rank == 0) && (token == BLACK_TOKEN)) {
                            cout << "MASTER PRIJAL BLACK TOKEN" << endl;
                            tokenSend = false;
                        }                        
                        if ((rank == 0) && (token == WHITE_TOKEN)) {
                            processStatus = STATUS_FINISHED;
                            cout << "MAIN PROCESS STATUS_FINISHED" << endl;                            
                        } else {
                            if (color == BLACK_PROCESS) {
                                token = BLACK_TOKEN;
                            }
                        }
                        
                        break;
                        // New Minimum came
                    case MSG_NEW_MINIMUM:
                        int message;
                        MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, MSG_NEW_MINIMUM, MPI_COMM_WORLD, &status);
                        if(message == kLowerBound){
                            hasResult = false;
							cout << "Prijat minBound procesem " << rank << endl;
                            processStatus = STATUS_FINISHED;
                        }
                        else if (message <= minimalDegree) {
                            minimalDegree = message;
                            hasResult = false;
                        }
                        break;
                        //Ending Message                        
                    case MSG_FINISH:
                        MPI_Recv(0, 0, MPI_INT, MPI_ANY_SOURCE, MSG_FINISH, MPI_COMM_WORLD, &status);
                        processStatus = STATUS_FINISHED;
                        break;
                    default:; 
                        break;
                }
            }
        }
        
        //Working Behaviour
        if (processStatus == STATUS_WORKING) {
            
            //Main while run until not ampty and not finished
            while (!stack.empty() && processStatus != STATUS_FINISHED) {
            //    wasWorking = true;
				if(processorCount > 1){
                    checkCounter++;
					if ((checkCounter % CHECK_MSG_AMOUNT) == 0) {
						MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
						if (flag) {                            
							bool workSended;
							switch (status.MPI_TAG) {
                                //Work Request came -> I have work i will share some
								case MSG_WORK_REQUEST:
									MPI_Recv(0, 0, MPI_INT, MPI_ANY_SOURCE, MSG_WORK_REQUEST, MPI_COMM_WORLD, &status);
									workSended = sendWork(status.MPI_SOURCE, &request, buff, maxbuffLength, stack, st);
                                    //change token accordingly
									if ((workSended) && (rank > status.MPI_SOURCE)) {
										color = BLACK_PROCESS;
										if (token != 0) {
											token = BLACK_TOKEN;
										}
									}
									break;
                                    //Set new minimum
								case MSG_NEW_MINIMUM:
									int message;
									MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, MSG_NEW_MINIMUM, MPI_COMM_WORLD, &status);
									if(message == kLowerBound){
										hasResult = false;
										processStatus = STATUS_FINISHED;
									}
									else if (message <= minimalDegree) {
										minimalDegree = message;
										hasResult = false;
									}
									break;
                                    
                                    //ADUV token
								case MSG_TOKEN:
									MPI_Recv(&token, 1, MPI_INT, MPI_ANY_SOURCE, MSG_TOKEN, MPI_COMM_WORLD, &status);
									if (rank == 0) {
										tokenSend = false;
									}
									break;
                                    
								default:; //chyba("neznamy typ zpravy");
									break;
							}
						}
					}
				}
                
                
                //Sequential part the main solving algorithm
                currentEdge = stack.back();
                if(st->vertices[currentEdge->to] == FRESH) {
                    st->add(currentEdge);
                    
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
                            sendNewMin(minimalDegree, rank, processorCount);
                            hasResult = true;
                        }
                        
                        //If we find best possible solution we dont continue
                        if(minimalDegree == kLowerBound) {
                            processStatus = STATUS_FINISHED;
                            break;
                        }
                       
                    } else {
                        // Add ajdents
                        for(int i = 0; i < size ; i++) {
                            //Consider only open ones!
                            if(st->vertices[i] == OPEN) {
                                for(int j = 0; j < size; j++) {
                                    if(G[i][j] == 1 && st->vertices[j] == FRESH) {
                                        //  cout << "Push: " << i << " " << j << endl;
                                        stack.push_back(new Edge(i,j));
                                    }
                                }
                            }
                        }
                    } 
                    
                } else {
                    //Remove edge, close vertex, backtrack
                    stack.pop_back();
                    st->remove(currentEdge);
                    // cout << currentEdge->to << " Closed" << endl;
                    delete currentEdge;
                }
            }
            
            //I have nothing in stack if i am not finished i am idle
            if(processStatus != STATUS_FINISHED) {
                processStatus = STATUS_IDLE;
            }
        }     
    
    }
    
    
    //Send Finish to other
    if (rank == 0 && minimalDegree != kLowerBound) {
        for (i = 1; i < processorCount; i++) {            
            MPI_Send(0, 0, MPI_INT, i, MSG_FINISH, MPI_COMM_WORLD);
        }
    }    
        
    MPI_Barrier(MPI_COMM_WORLD);
	cout << "MPI_Barrier END" << endl;
    
	
    // Master prints the time
	if(rank == 0) {
		time2 = MPI_Wtime();        
		double totalTime = time2 - time1;        
		cout << "Total Time: " << totalTime << endl;
	}
    
    if (hasResult) {
        cout << "Minimum degree: " << minimalDegree << endl;
        cout << "Spanning Tree:" << endl;
        printGraph(minimalTree);
    }
    
    
    /*
    if (wasWorking){
        cout << "PROCESOR: " <<rank<<" pracoval." <<endl;
    }*/

    
    MPI_Finalize();    
    delete st;
    destroyGraph(minimalTree);
    destroyGraph(G);
    return 0;
}

#pragma mark MPI FCE

void sendNewMin(int minDegree, int resultRank, int processorCount) {
    for (int i = 0; i < processorCount; i++) {
        if (i != resultRank) {
            MPI_Send(&minDegree, 1, MPI_INT, i, MSG_NEW_MINIMUM, MPI_COMM_WORLD);
        }
    }
}


bool sendWork(int target, MPI_Request* request, int* buff, int maxbuffLenght, list<Edge*> &stack, SpanningTree* tree) {
    list<Edge*>::reverse_iterator it;
    
    int numToSend = 0;
    
    it = stack.rbegin();
    while (it != stack.rend()) {
        if (tree->vertices[(*it)->to] == OPEN) {
            break;
        }
        numToSend++;
        it++;
    }
    
    if (numToSend < 2) {
        MPI_Send(0, 0, MPI_INT, target, MSG_WORK_NOWORK, MPI_COMM_WORLD);
        return false;
    }
    
    //Send just an half
    numToSend = numToSend / 2;
    
    //Stack,Tree,Delimetrx2
    int length = (numToSend * 2) + 2 + (tree->edges * 2);   
    it = stack.rbegin();
    
    int index = 0;
    for (int i = 0; i < numToSend; i++) {
        Edge* e = *it;    
        buff[index] = e->from;
        buff[index + 1] = e->to;
        stack.remove(e);
        delete e;        
        index += 2;
        it++;
    }
    buff[index] = -1;
    index++;
    int treeSize = tree->edges;
    for (int i = 0; i < treeSize; i++) {
        buff[index] = tree->vector[i]->from;
        buff[index + 1] = tree->vector[i]->to;
        index += 2;
    }
    buff[index] = -1;
    MPI_Send(buff, length, MPI_INT, target, MSG_WORK_SENT, MPI_COMM_WORLD);
    return true;
}


void receiveWork(MPI_Status* status, int* buff, int maxbuffLenght, list<Edge*> &stack, SpanningTree* tree) {
    tree->reset();
    
    //MPI Receive
    MPI_Recv(buff, maxbuffLenght, MPI_INT, MPI_ANY_SOURCE, MSG_WORK_SENT, MPI_COMM_WORLD, status);
    //MPI get_Count
    int dataLenght = maxbuffLenght;
    MPI_Get_count(status, MPI_INT, &dataLenght);
    
    bool delimeter = false;
    for (int i = 2; i < dataLenght; i += 2) {
        if (delimeter) {
            tree->add(new Edge(buff[i - 2], buff[i - 1]));
        } else {
            stack.push_back(new Edge(buff[i - 2], buff[i - 1]));
        }
        
        if (buff[i] == -1) {
            if (i == dataLenght - 1) {
                break;
            }
            i++;
            if (buff[i] == -1) {
                break;
            }
            delimeter = true;
        }
    }    
}


void sendFirstEdge(int target, int* buff, Edge* edge) {
    buff[0] = edge->from;
    buff[1] = edge->to;
    MPI_Send(buff, 2, MPI_INT, target, MSG_WORK_SENT, MPI_COMM_WORLD);
    delete edge;
}

void receiveFirstEdge(MPI_Status* status, int* buff, list<Edge*> &stack) {
    MPI_Recv(buff, 2, MPI_INT, MPI_ANY_SOURCE, MSG_WORK_SENT, MPI_COMM_WORLD, status);
    stack.push_back(new Edge(buff[0], buff[1]));
}

#pragma mark
#pragma mark GRAPH Functions

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
    
    
    
   // printf("File readed.\n");
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


