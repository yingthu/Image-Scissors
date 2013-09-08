/* iScissor.cpp */
/* Main file for implementing project 1.  See TODO statments below
 * (see also correlation.cpp and iScissor.h for additional TODOs) */

#include <assert.h>

#include "correlation.h"
#include "iScissor.h"
#include <queue>
using namespace std;

const double linkLengths[8] = { 1.0, SQRT2, 1.0, SQRT2, 1.0, SQRT2, 1.0, SQRT2 };

// two inlined routines that may help;

inline Node& NODE(Node* n, int i, int j, int width)
{
    return *(n + j * width + i);
}

inline unsigned char PIXEL(const unsigned char* p, int i, int j, int c, int width)
{
    return *(p + 3 * (j * width + i) + c);
}

/************************ TODO 1 ***************************
 *InitNodeBuf
 *	INPUT:
 *		img:	a RGB image of size imgWidth by imgHeight;
 *		nodes:	a allocated buffer of Nodes of the same size, one node corresponds to a pixel in img;
 *  OUPUT:
 *      initializes the column, row, and linkCost fields of each node in the node buffer.
 */

void InitNodeBuf(Node* nodes, const unsigned char* img, int imgWidth, int imgHeight)
{
	// Initializes the column and row
	// nodes size: imgWidth*imgHeight, img size: 3*imgWidth*imgHeight
	for (int row = 0; row < imgHeight; row++)
		for (int column = 0; column < imgWidth; column++)
		{
				nodes[row*imgWidth+column].column = column;
				nodes[row*imgWidth+column].row = row;
		}

	// Initializes the linkCost
	double maxD = 0.0000;
	double *rsltImg;
	for (int knlNum = 0; knlNum < 8; knlNum++)
	{
		rsltImg = new double[imgWidth*imgHeight*3];
		image_filter(rsltImg, img, NULL, imgWidth, imgHeight, kernels[knlNum], 3, 3, 1, 0);
		for (int row = 0; row < imgHeight; row++)
			for (int column = 0; column < imgWidth; column++)
			{
				int rPos = 3*(row*imgWidth+column);
				double Dlink = sqrt((rsltImg[rPos]*rsltImg[rPos]+rsltImg[rPos+1]*rsltImg[rPos+1]+rsltImg[rPos+2]*rsltImg[rPos+2]) / 3);
				nodes[rPos/3].linkCost[knlNum] = Dlink;
				if (Dlink > maxD) maxD = Dlink;
			}
		delete [] rsltImg;
	}
	for (int knlNum = 0; knlNum < 8; knlNum++)
		for (int i = 0; i < imgWidth*imgHeight; i++)
			nodes[i].linkCost[knlNum] = (maxD - nodes[i].linkCost[knlNum]) * linkLengths[knlNum];
}
/************************ END OF TODO 1 ***************************/

static int offsetToLinkIndex(int dx, int dy)
{
    int indices[9] = { 3, 2, 1, 4, -1, 0, 5, 6, 7 };
    int tmp_idx = (dy + 1) * 3 + (dx + 1);
    assert(tmp_idx >= 0 && tmp_idx < 9 && tmp_idx != 4);
    return indices[tmp_idx];
}

/************************ TODO 4 ***************************
 *LiveWireDP:
 *	INPUT:
 *		seedX, seedY:	seed position in nodes
 *		nodes:			node buffer of size width by height;
 *      width, height:  dimensions of the node buffer;
 *		selection:		if selection != NULL, search path only in the subset of nodes[j*width+i] if selection[j*width+i] = 1;
 *						otherwise, search in the whole set of nodes.
 *		numExpanded:		compute the only the first numExpanded number of nodes; (for debugging)
 *	OUTPUT:
 *		computes the minimum path tree from the seed node, by assigning
 *		the prevNode field of each node to its predecessor along the minimum
 *		cost path from the seed to that node.
 */
struct pqnode
{
    int idx;
    double key;
    pqnode(int a=0, double b=0):idx(a), key(b){}
};

struct cmp
{
    bool operator()(pqnode a, pqnode b)
	{
        return a.key > b.key;
    }
};

int indexingNeighbors(int i,int j)
{
	int result;
	if((i==-1)&&(j==-1))
		result=3;
	if((i==0)&&(j==-1))
		result=2;
	if((i==1)&&(j==-1))
		result=1;
	if((i==-1)&&(j==0))
		result=4;
	if((i==1)&&(j==0))
		result=0;
	if((i==-1)&&(j==1))
		result=5;
	if((i==0)&&(j==1))
		result=6;
	if((i==1)&&(j==1))
		result=7;
	return result;
}

void LiveWireDP(int seedX, int seedY, Node* nodes, int width, int height, const unsigned char* selection, int numExpanded)
{
	//state:INITIAL:0,EXPANDED:1,ACTIVE:2
	pqnode current;
	int currentindex;
	int neighborList[8];
	int currentx,currenty;
	int rx,ry;
	int rindex;	
	pqnode tmpnode;
	
	priority_queue<pqnode,vector<pqnode>,cmp> pq;
	priority_queue<pqnode,vector<pqnode>,cmp> tmp;
	//initialize each node to INITIAL
	for(int i=0;i<(width*height);i++)
		nodes[i].state=0;
	//set totalCost of seed be 0
	nodes[seedY*width+seedX].totalCost=0.0;
	//insert seed into pq
	pq.push(pqnode(seedY*width+seedX,0.0));
	while(!pq.empty())
	{
		//extract node q(current node)
		current=pq.top();
		pq.pop();
		currentindex=current.idx;
		//make q as EXPANDED
		nodes[currentindex].state=1;
		currentx=currentindex%width;
		currenty=(currentindex-currentx)/width;
		//for each existing neighbor r of q
		for(int i=-1;i<=1;i++)
		{
			for(int j=-1;j<=1;j++)
			{
				//make sure r exists
				if(((currentx+i)>=0)&&((currenty+j)>=0)&&((currentx+i)<width)&&((currenty+j)<height))
				{
					rx=currentx+i;
					ry=currenty+j;
					rindex=ry*width+rx;
					//if r has not been EXPANDED
					if(nodes[rindex].state!=1)
					{
						//if r is still INITIAL
						if(nodes[rindex].state==0)
						{
							pq.push(pqnode(rindex,nodes[currentindex].totalCost+nodes[currentindex].linkCost[indexingNeighbors(i,j)]));
							nodes[rindex].totalCost=nodes[currentindex].totalCost+nodes[currentindex].linkCost[indexingNeighbors(i,j)];
							nodes[rindex].state=2;
						}
						else
						{
							//if r is ACTIVE
							//this is the stupid part, here if the "less" criterion is met, we need to: 1)update r's totalCost in nodes,
							//and 2)update the key of corresponding pqnode of r in pq to the new totalCost
							//so how can we find the corresponding pqnode of r in pq?
							//my way is to pop pqnodes from pq one by one until the right pqnode is found, and then
							//push back all the poped pqnodes...
							//this is not the most efficient way, but i can only think of this using the priority_queue in STL
							if(nodes[rindex].state==2)
							{
								if((nodes[currentindex].totalCost+nodes[currentindex].linkCost[indexingNeighbors(i,j)])<nodes[rindex].totalCost)
								{
									nodes[rindex].totalCost=nodes[currentindex].totalCost+nodes[currentindex].linkCost[indexingNeighbors(i,j)];
									while(true)
									{
										tmpnode=pq.top();
										pq.pop();
										if(tmpnode.idx==rindex)
											break;
										else
											tmp.push(pqnode(tmpnode.idx,tmpnode.key));
									}
									pq.push(pqnode(rindex,nodes[rindex].totalCost));
									while(!tmp.empty())
									{
										tmpnode=tmp.top();
										tmp.pop();
										pq.push(pqnode(tmpnode.idx,tmpnode.key));
									}
								}
							}
						}
					}
				}
			}
		}				
	}
	
	printf("TODO: %s:%d\n", __FILE__, __LINE__); 
}
/************************ END OF TODO 4 ***************************/

/************************ TODO 5 ***************************
 *MinimumPath:
 *	INPUT:
 *		nodes:				a node buffer of size width by height;
 *		width, height:		dimensions of the node buffer;
 *		freePtX, freePtY:	an input node position;
 *	OUTPUT:
 *		insert a list of nodes along the minimum cost path from the seed node to the input node.
 *		Notice that the seed node in the buffer has a NULL predecessor.
 *		And you want to insert a *pointer* to the Node into path, e.g.,
 *		insert nodes+j*width+i (or &(nodes[j*width+i])) if you want to insert node at (i,j), instead of nodes[nodes+j*width+i]!!!
 *		after the procedure, the seed should be the head of path and the input code should be the tail
 */

void MinimumPath(CTypedPtrDblList <Node>* path, int freePtX, int freePtY, Node* nodes, int width, int height)
{
printf("TODO: %s:%d\n", __FILE__, __LINE__); 

}
/************************ END OF TODO 5 ***************************/

/************************ An Extra Credit Item ***************************
 *SeeSnap:
 *	INPUT:
 *		img:				a RGB image buffer of size width by height;
 *		width, height:		dimensions of the image buffer;
 *		x,y:				an input seed position;
 *	OUTPUT:
 *		update the value of x,y to the closest edge based on local image information.
 */

void SeedSnap(int& x, int& y, unsigned char* img, int width, int height)
{
    printf("SeedSnap in iScissor.cpp: to be implemented for extra credit!\n");
}

//generate a cost graph from original image and node buffer with all the link costs;
void MakeCostGraph(unsigned char* costGraph, const Node* nodes, const unsigned char* img, int imgWidth, int imgHeight)
{
    int graphWidth = imgWidth * 3;
    int graphHeight = imgHeight * 3;
    int dgX = 3;
    int dgY = 3 * graphWidth;

    int i, j;
    for (j = 0; j < imgHeight; j++) {
        for (i = 0; i < imgWidth; i++) {
            int nodeIndex = j * imgWidth + i;
            int imgIndex = 3 * nodeIndex;
            int costIndex = 3 * ((3 * j + 1) * graphWidth + (3 * i + 1));

            const Node* node = nodes + nodeIndex;
            const unsigned char* pxl = img + imgIndex;
            unsigned char* cst = costGraph + costIndex;

            cst[0] = pxl[0];
            cst[1] = pxl[1];
            cst[2] = pxl[2];

            //r,g,b channels are grad info in seperate channels;
            DigitizeCost(cst	   + dgX, node->linkCost[0]);
            DigitizeCost(cst - dgY + dgX, node->linkCost[1]);
            DigitizeCost(cst - dgY      , node->linkCost[2]);
            DigitizeCost(cst - dgY - dgX, node->linkCost[3]);
            DigitizeCost(cst	   - dgX, node->linkCost[4]);
            DigitizeCost(cst + dgY - dgX, node->linkCost[5]);
            DigitizeCost(cst + dgY	   ,  node->linkCost[6]);
            DigitizeCost(cst + dgY + dgX, node->linkCost[7]);
        }
    }
}

