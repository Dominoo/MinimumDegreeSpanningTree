/* 
 * File:   main.cpp
 * Author: Dominik Veselý, Marek Přibáň
 *
 * Created on 29. září 2012, 17:00
 */

#include <cstdlib>
#include <stdio.h>
#include <string.h>


#define DEFSIZE 2

using namespace std;

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

/*
 * Check if stack is empty.
 * @return true if empty
 */
bool stackEmpty(){
    if(stack.top == -1){
        return true;
    }
    return false;
}
/* 
 * Return element on top of stack without removal.
 * @return element on top.
 */
StackElem stackTop(){
    if(!stackEmpty()){
        return stack.elements[stack.top];
    }else{
        perror("Stack is empty.\n");
    }
}
/*
 * Init stack memory.
 * @param size num of elements in stack.
 */
void stackInit(int size = DEFSIZE){
    stack.elements = (StackElem*) calloc(size,sizeof(StackElem));
    stack.size = size;
    stack.top = -1;
}
/*
 * Push element to the stack.
 * Extend stack size if necessary.
 * @param elem element pushed to stack.
 */
void stackPush(StackElem elem){
    if(stack.top == (stack.size - 1)){
        printf("Zvetsuji pamet.\n");
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
/*
 * Pop top/first element from the stack.
 * @return top element.
 */
StackElem stackPop(){
    if(!stackEmpty()){
        return stack.elements[(stack.top)--];
    }else{
        perror("Stack is empty.\n");
    }
}
/*
 * 
 */
int main(int argc, char** argv) {
    
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
    
    return 0;
}


