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
void pickSeeds(node* n,int split[]);//pick seed takes two nodes to make a split, using linear or quadratic search for the two worst pair to be together
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
void freeNode(node**);//maybe needs tail recursion
void printTree(node*);//print from root
int areaMbr(rect*,rect*);//calculate the area of the mbr of two rects
int area(rect*);

void test1(node**,node**);//receives two pointers of pointers and swap the adress of these two 
void test2(node*);//create two children in node and calls test1, compare before and after 
void test3(node*,node*);//receive two pointers of node and make a change in the content of the nodes (mbr in this case)
void test4(node*);//create two children in node and calls test3, compare before and after 
//this test shows that some functions will need to use pointer of pointer of node (when need to swap an adress of a pointer, for example)
//when other functions will need only to use a pointer of node (like change a var content which is not related with the address, like the node->mbr.x1)
void test5(int[]);//receive a int[] and modify, create another int[] and calls test6
void test6(int[],int[]);//receive two int[] and swap their values and return
//the int[] works like a adr pointer aparently


//MAIN FUNCTION
int main(){

/* 	root = createNode(NULL);
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
	freeNode(&root);
	printf("freed nodes: %d\n",free_c);

	
	node* testNode = createEmptyNode();
	printf("test node: %p\n",testNode);
	printTree(testNode);
	test4(testNode);
	printTree(testNode);

	freeNode(&testNode);
	separator();
	int a[2];
	a[0] = -1;
	a[1] = -2;
	printf("main:a[0]: %d, a[1]: %d\n",a[0],a[1]);
	test5(a);
	printf("main:a[0]: %d, a[1]: %d\n",a[0],a[1]); */

	start = clock();
	root = createNode(NULL);
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
	int i;
	for(i = 0; i < M+1; i++){
		n->child[i] = NULL;
	}
	n->mbr.x1 = n->mbr.x2 = n->mbr.y1 = n->mbr.y2  = 0;
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
	int i;
	for(i = 0; i < M+1; i++){
		n->child[i] = NULL;
	}
	n->mbr.x1 = n->mbr.x2 = n->mbr.y1 = n->mbr.y2  = 0;
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
	printf("node id : %d\nnode level : %d\nadr: %p\n",node->id,node->level,node);
	printRect(&node->mbr);

}
void printRect(rect* rect){
	printf("(%d,%d)(%d,%d)\n",rect->x1,rect->y1,rect->x2,rect->y2);
}
void test(char t[]){
	printf("%d:log %s\n",t_c++,t);
}
void freeNode(node** node){
	if(node == NULL) return;
	int i = 0;
	for(i = 0; i < M+1; i++){
		if((*node)->child[i] != NULL)
			freeNode(&(*node)->child[i]);
	}
	free(*node);
	*node = NULL;
	free_c++;
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


void test1(node** n1,node** n2){//this one works to swap the children in test2
	node* aux = *n1;
	*n1=*n2;
	*n2=aux;
}
void test2(node* node){
	node->child[0] = createNode(node);
	node->child[1] = createNode(node);
	printTree(node);
	test1(&node->child[0],&node->child[1]);
	printTree(node);	
}
void test3(node* n1,node* n2){
	n1->mbr.x1 = 3;
	n2->mbr.y1 = 6;
	
}
void test4(node* node){
	node->child[0] = createNode(node);
	node->child[1] = createNode(node);
	printTree(node);
	test3(node->child[0],node->child[1]);
	printTree(node);
}
void test5(int a[]){
	printf("t5:a[0]: %d, a[1]: %d\n",a[0],a[1]);
	a[0] = 0;
	a[1] = 1;
	printf("t5:a[0]: %d, a[1]: %d\n",a[0],a[1]);
	int b[2];
	b[0] = 9;
	b[1] = 8;
	printf("t5:b[0]: %d, b[1]: %d\n",b[0],b[1]);
	test6(a,b);
	printf("t5:a[0]: %d, a[1]: %d\n",a[0],a[1]);
	printf("t5:b[0]: %d, b[1]: %d\n",b[0],b[1]);
}
void test6(int a[],int b[]){
	printf("t6:a[0]: %d, a[1]: %d\n",a[0],a[1]);
	printf("t6:b[0]: %d, b[1]: %d\n",b[0],b[1]);
	int c[2];
	c[0] = a[0];
	a[0] = b[0];
	c[1] = b[1];
	b[1] = a[1];
	b[0] = c[0];
	a[1] = c[1];
	printf("t6:a[0]: %d, a[1]: %d\n",a[0],a[1]);
	printf("t6:b[0]: %d, b[1]: %d\n",b[0],b[1]);
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
		if(i>=M+1){
			printf("erro insertion on M+1 node\n");
			return;
		}
		i++;
	}
	selected->child[i]=data;
	data->parent=selected;
	data->level = selected->level+1;
	if(i==M){
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
	while(i < M && node->child[i] != NULL){//run until M-1, this node doesnt supose to has a M+1 child(child[M])
		//calcular area e comparar com area_diff
		int area_calc = areaMbr(&node->child[i]->mbr,dataRect) - area(&node->child[i]->mbr);//test if can get negative, but probably doesnt
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
void splitNode(node* n){ //it is possible that this function needs a pointer of pointer aproach //CRITICAL//CRITICAL//CRITICAL
	if (n == NULL) return;
	if(n->child[M] == NULL) return;//this means the node doesnt have M+1 children
	int i;
	int split[2];
	pickSeeds(n,split);//CRITICAL
	
	node* aux = createEmptyNode();
	node* aux2 = createEmptyNode();
	int index_a[m],index_b[m],ia=1,ib=1;
	index_a[0] = split[0];
	aux->child[0] = n->child[split[0]];
	index_b[0] = split[1];
	aux2->child[0] = n->child[split[1]];
	rect rect_a,rect_b;
	rect_a = n->child[split[0]]->mbr;//CRITICAL
	rect_b = n->child[split[1]]->mbr;//CRITICAL
	for(i=0;i<M+1;i++){
		if(i != split[0] && i != split[1]){//to not count the two seeds
			if(ia<m){
				if(areaMbr(&rect_a,&n->child[i]->mbr) < areaMbr(&rect_b,&n->child[i]->mbr) || ib >=m){//CRITICAL
					//index_a[ia] = i;
					aux->child[ia] = n->child[i];//TODO adjust mbr
					ia++;
				}
				else{
					//index_b[ib] = i;
					aux2->child[ib] = n->child[i];//TODO adjust mbr
					ib++;
				}
			}
			else{
				//index_b[ib] = i;
				aux2->child[ib] = n->child[i];//TODO adjust mbr
				ib++;
			}
		}		
	}
	if(n == root){//need to confirm if this does what i really think it does //CRITICAL
		free(root);
		root = createEmptyNode();
		root->child[0] = aux;
		root->child[1] = aux2;
		//adjust the mbr
		
	}
	else{
		aux->parent = n->parent;//CRITICAL
		aux2->parent = n->parent;//CRITICAL
		while(i < M+1 && aux->parent->child[i] != NULL){//CRITICAL
			if(aux->parent->child[i] == n){//put aux in the place of n
				free(n);
				aux->parent->child[i] = aux;
			}
			i++;
		}
		aux->parent->child[i] = aux2;//put aux2 in the place o last empty child place
		if(i>=M){//this means that aux2 is the M+1 esim child wich means the parent need a split
			splitNode(aux->parent);
		}
	}


}

void adjustTree(node* node){
	//maybe adjust need to pass the child that will make the adjust, but need to see if need to adjust if split but i guess will not
}
void pickSeeds(node* n,int split[]){
	if(n == NULL) return;
	int i, i1 = 0, i2 = 1;
	if(areaMbr(&n->child[0]->mbr,&n->child[1]->mbr) > areaMbr(&n->child[0]->mbr,&n->child[2]->mbr)){
		if(areaMbr(&n->child[0]->mbr,&n->child[1]->mbr) > areaMbr(&n->child[1]->mbr,&n->child[2]->mbr)){
			i1=0;
			i2=1;
		}
		else{
			i1=1;
			i2=2;
		}
	}
	else{
		if(areaMbr(&n->child[0]->mbr,&n->child[2]->mbr) > areaMbr(&n->child[1]->mbr,&n->child[2]->mbr)){
			i1=0;
			i2=2;
		}
		else{
			i1=1;
			i2=2;
		}
	}
	int area = areaMbr(&n->child[i1]->mbr,&n->child[i2]->mbr);
	for(i=3;i<M+1;i++){
		int a1 = areaMbr(&n->child[i1]->mbr,&n->child[i]->mbr);
		int a2 = areaMbr(&n->child[i2]->mbr,&n->child[i]->mbr);
		if(a1>a2){
			if(a1>area){
				i2=i;
				area = areaMbr(&n->child[i1]->mbr,&n->child[i2]->mbr);
			}
		}
		else{
			if(a2>area){
				i1=i;
				area = areaMbr(&n->child[i1]->mbr,&n->child[i2]->mbr);
			}
		}
		
	}
	split[0] = i1;
	split[1] = i2;

}
node* pickNext(node* n){
	return n;
}