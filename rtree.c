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

node *root=NULL;
int id_count=0;
int t_c = 0;

node* createNode();//create an empty node id is made of global int id_count incrementation
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
void freeNode(node*);


//MAIN FUNCTION
int main(){
	rect r;
	r.x1=10;
	r.x2=20;
	printf("%d e %d e %d\n",r.x1,r.x2,r.y1);
	node n1;
	n1.child[0] = malloc(sizeof(node));
	n1.id = 222;
	n1.child[0]->id=333;
	printf("%d e %d e %d e %d\n",n1.id,n1.child[0]->id,n1.child,M);
	separator();
	n1.child[0]->child[0] = createNode();
	n1.child[0]->child[0]->child[0] = createNode();
	//test("main <createnode & printnode>");
	printNode(n1.child[0]->child[0]);
	printf("\n");
	printNode(n1.child[0]->child[0]->child[0]);




	freeNode(n1.child[0]);

	return 0;
}

node* createNode(){
	//test("create node");
	node* n = malloc(sizeof(node));
	n->id = id_count++;
	n->level = 0;
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
	printf("x1 : %d, y1 : %d, x2 : %d, y2 : %d\n",rect->x1,rect->y1,rect->x2,rect->y2);
}
void test(char t[]){
	printf("%d:log %s\n",t_c++,t);
}
void freeNode(node* node){
	if(node == NULL) return;
	int i= 0;
	for(i = 0;i < M+1;i++){
		if(node->child[i] != NULL)
			freeNode(node->child[i]);
	}
	test("freeing a node");
	free(node);
}