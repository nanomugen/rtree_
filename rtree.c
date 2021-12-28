#include <stdio.h>
#include <stdlib.h>
#include <time.h>


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
node *root=NULL;//root node, global address
int id_count=0;//counter of ids to assign, later the ids will be used to store the data structure to fast recover(instead of add all the data in the tree again)
int t_c = 0;//counter of test logs used
int free_c = 0;//counter of freed nodes with the function freeNode
int max_size = 1000;//temp size of a bound to the mbr of root and the children
//CLOCK VARS
clock_t start, end;
double cpu_time_used;

//INSERT FUNCIONS
void Insert(int,int,int,int);//add the rect of the coord params to the root(global)
node* chooseLeaf(node*,rect*);//choose the best node to insertion
void splitNode(node*);//make a split in the node and add the new one to the parent of the original
void adjustTree(node*);//adjust tree see if the node needs an overflow rearrange due to splitNode and do it in the parent if needed
void pickSeeds(node*);//pick seed takes two nodes to make a split, using linear or quadratic search for the two worst pair to be together
node* pickNext(node*);//see where to put the left nodes after pickSeeds()

void search(rect);//search the data of rect if intercect some data in the rtree
int intersect(rect*,rect*);//search if the two rects intercects each other: 1 true 0 false

int isLeaf(node*);//maybe dont need, use some info in the node struct: 1 true 0 false


//AUX FUNCTIONS
node* createEmptyNode();
node* createNode(node*);//create an empty node id is made of global int id_count incrementation
void setMbr(node*,int,int,int,int);
void separator();
void printNode(node*);
void printRect(rect*);
void test(char t[]);
void freeNode(node*);//maybe needs tail recursion
void printTree(node*);//print from root
int areaMbr(rect*,rect*);//calculate the area of the mbr of two rects
int area(rect*);

//MAIN FUNCTION
int main(){
	start = clock();
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
	freeNode(root);
	printf("freed nodes: %d\n",free_c);
	end = clock();
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("time elapsed: %f\n",cpu_time_used);
	return 0;
}


//===============================================================================================
//AUX FUNCIONS
//===============================================================================================
node* createEmptyNode(){
	//test("create node");
	node* n = malloc(sizeof(node));
	n->id = id_count++;
	n->level = 0;
	n->child_n = 0;
	return n;	
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
		n->parent = parent;
	}
	n->child_n = 0;
	//test("end create node");
	return n;	
}
void setMbr(node* node,int x1, int y1, int x2, int y2){
	node->mbr.x1 = x1;
	node->mbr.y1 = y1;
	node->mbr.x2 = x2;
	node->mbr.y2 = y2;
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
int area(rect* r){
	if(r == NULL) return -1;
	return (r->x2-r->x1)*(r->y2-r->y1);
}

void printTree(node* node){
	//printNode(node);
	int i;
	for(i = 0;i<node->level;i++){
		printf("\t");
	}
	printf("id:%d-lv:%d-(%d,%d)(%d,%d)-adr:%p-dad:%p\n",node->id,node->level,node->mbr.x1,node->mbr.y1,node->mbr.x2,node->mbr.y2,node,node->parent);
	i=0;
	while(node->child[i] != NULL && i<M){
			printTree(node->child[i++]);
			
	}
}

int areaMbr(rect* r1,rect* r2){
	if(r1 == NULL || r2 == NULL) return -1;
	int x1,x2,y1,y2;
	if(r1->x1 < r2->x1) x1 = r1->x1;
	else x1 = r2->x1;
	if(r1->x2 > r2->x2) x2 = r1->x2;
	else x2 = r2->x2;
	if(r1->y2 > r2->y2) y2 = r1->y2;
	else y2 = r2->y2;
	if(r1->y1 < r2->y1) y1 = r1->y1;
	else y1 = r2->y1;
	return (y2-y1)*(x2-x1);
}

//===============================================================================================
//INSERT FUNCIONS
//===============================================================================================

void Insert(int x1, int y1, int x2, int y2){
	node* data = createEmptyNode();
	setMbr(data,x1,y1,x2,y2);
	node* selected = chooseLeaf(root,&data->mbr);
	int i = 0;
	while(selected->child[i]!=NULL){
		if(i>=M+2){
			printf("erro insertion on M+2 node\n");
			return;
		}
		i++;
	}
	selected->child[i]=data;
	data->parent=selected;
	if(i==M+1){
		splitNode(selected);
	}
	adjustTree(selected);
	

}
node* chooseLeaf(node* node, rect* dataRect){
	if(node == NULL) return NULL;
	if(root == node && root->child[0]==NULL) return root;//this means root is leaf and has zero data entries
	if(node->child[0]->child[0]==NULL) return node;//this means this node has a child and this child doesnt has a child, which means it is data, wich menas the node is leaf
	//if is not leaf need to find the least increasing node to add the data
	int area_diff=-1;
	int i = 0, node_choosen = 0;
	while(i < M && node->child[i] != NULL){
		//calcular area e comparar com area_diff
		int area_calc = areaMbr(&node->child[i]->mbr,dataRect) - area(&node->child[i]->mbr);
		if(area_diff == -1){
			area_diff = area_calc;

		}
		else{
			if(area_calc < area_diff){
				area_diff = area_calc;
				node_choosen = i;
			}
		}
		i++;
	}
	return chooseLeaf(node->child[node_choosen],dataRect);
}
void splitNode(node* node){

}
void adjustTree(node* node){
	
}
void pickSeeds(node* node){
	
}
node* pickNext(node* node){
	return node;
}