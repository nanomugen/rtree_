#include <stdio.h>
#include <stdlib.h>

#define M 6
#define m 3
typedef struct rect{
	int x1;
	int x2;
	int y1;
	int y2;

} rect;

typedef struct node{
	int id;
	struct node* child;
	struct node* parent;
	rect mbr;
	int level;
	int child_n;

} node;

node *root=NULL;
int id_count=0;

node createNode();//create an empty node id is made of global int id_count incrementation
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


int main(){
	rect r;
	r.x1=10;
	r.x2=20;
	printf("%d e %d e %d\n",r.x1,r.x2,r.y1);
	node n1;
	n1.child = malloc(sizeof(node));
	n1.id = 222;
	n1.child->id=333;
	printf("%d e %d e %d e %d\n",n1.id,n1.child->id,n1.child,M);
	free(n1.child);

	return 0;
}
