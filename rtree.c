#include <stdio.h>
#include <stdlib.h>

#define M 6
#define m 3
typedef struct rect{
	int x1;
	int y1;
	int x2;
	int y2;

} rect;

typedef struct node{
	int id;
	struct node* child[M+1];
	struct node* parent;
	rect mbr;
	int level;
	int child_n;

} node;

//GLOBAL VARS
node *root=NULL;
int id_count=0;
int t_c = 0;
int free_c = 0;
int max_size = 1000;

node* createNode(node*);//create an empty node id is made of global int id_count incrementation
rect* createRect(int,int,int,int);//create a rect with x1,y1,x2,y2
void setNode(node,rect);//set the mbr of node

void addData(rect);//add the rect to the root(global)
node chooseLeaf(node);//choose the best node to add the data
void splitNode(node);//make a split in the node and add the new one to the parent of the original
void adjustTree(node);//adjust tree see if the node needs an overflow rearrange due to splitNode and do it in the parent if needed
void pickSeeds(node);//pick seed takes two nodes to make a split, using linear or quadratic search for the two worst pair to be together
node pickNext(node);//see where to put the left nodes after pickSeeds()

void searchData(rect);//search the data of rect if intercect some data in the rtree
int intersect(rect,rect);//search if the two rects intercects each other

int isLeaf(node);//maybe dont need, use some info in the node struct


//AUX FUNCTIONS
void separator();
void printNode(node*);
void printRect(rect*);
void test(char t[]);
void freeNode(node*);//maybe needs tail recursion
void printTree(node*);//print from root


//MAIN FUNCTION
int main(){

	root = createNode(NULL);
	root->mbr.x2 = max_size;
	root->mbr.y2 = max_size;
	int i,j,k;
	for(i = 0; i < M; i++){
		root->child[i] = createNode(root);
		root->child[i]->mbr.x1 = i*max_size/6;
		root->child[i]->mbr.y1 = i*max_size/6;
		root->child[i]->mbr.x2 = (i+1)*max_size/6;
		root->child[i]->mbr.y2 = (i+1)*max_size/6;
		for(j = 0;j < M;j++){
			root->child[i]->child[j] = createNode(root->child[i]);
			root->child[i]->child[j]->mbr.x1 = (i*max_size/6)+(j*max_size/36);
			root->child[i]->child[j]->mbr.y1 = (i*max_size/6)+(j*max_size/36);
			root->child[i]->child[j]->mbr.x2 = (i*max_size/6)+((j+1)*max_size/36);
			root->child[i]->child[j]->mbr.y2 = (i*max_size/6)+((j+1)*max_size/36);
			for(k = 0;k < M;k++){
				root->child[i]->child[j]->child[k] = createNode(root->child[i]->child[j]);
				root->child[i]->child[j]->child[k]->mbr.x1 = (i*max_size/6)+(j*max_size/36)+(k*max_size/(6*36));
				root->child[i]->child[j]->child[k]->mbr.y1 = (i*max_size/6)+(j*max_size/36)+(k*max_size/(6*36));
				root->child[i]->child[j]->child[k]->mbr.x2 = (i*max_size/6)+(j*max_size/36)+((k+1)*max_size/(6*36));
				root->child[i]->child[j]->child[k]->mbr.y2 = (i*max_size/6)+(j*max_size/36)+((k+1)*max_size/(6*36));
			}
		}
	}
	printTree(root);
	//printRect(&root->mbr);
	freeNode(root);
	printf("freed nodes: %d\n",free_c);
	return 0;
}

node* createNode(node* parent){
	//test("create node");
	node* n = malloc(sizeof(node));
	n->id = id_count++;
	if(parent == NULL){
		n->level = 0;
	}
	else{
		n->level = parent->level+1;
	}
	n->child_n = 0;
	//test("end create node");
	return n;	
}

void separator(){
	printf("\n===========================================================\n\n");
}
void printNode(node* node){
	//test("begin printnode");
	if(node == 	NULL)return;
	printf("node id : %d\nnode level : %d\nnode child_n : %d\n",node->id,node->level,node->child_n);
	printRect(&node->mbr);
}
void printRect(rect* rect){
	printf("(%d,%d)(%d,%d)\n",rect->x1,rect->y1,rect->x2,rect->y2);
}
void test(char t[]){
	printf("%d:log %s\n",t_c++,t);
}
void freeNode(node* node){
	if(node == NULL) return;
	int i= 0;
	for(i = 0; i < M+1; i++){
		if(node->child[i] != NULL)
			freeNode(node->child[i]);
	}
	//test("freeing a node");
	free_c++;
	free(node);
}
void printTree(node* node){
	//printNode(node);
	int i;
	for(i = 0;i<node->level;i++){
		printf("\t");
	}
	printf("id:%d-lv:%d-(%d,%d)(%d,%d)\n",node->id,node->level,node->mbr.x1,node->mbr.y1,node->mbr.x2,node->mbr.y2);
	i=0;
	while(node->child[i] != NULL && i<M){
			printTree(node->child[i++]);
			
	}
}