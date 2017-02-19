#include <utility>      // std::pair, std::make_pair
#include <vector>
using namespace std;

struct point
{
   int i, j;
};


int d4 = 4;
long neighbors4[][2] = {{0,1}, {-1,0}, {1,0}, {0,-1} };

int d8 = 8;
long neighbors8[][2] = { {-1,1}, {0,1}, {1,1}, {-1,0}, {1,0}, {-1,-1},{0,-1},{1,-1} };

enum side {
  right     = 0,
  left      = 1,
  top       = 2,
  bottom    = 3,
};

bool containsPoint (vector<point> pixList, int i, int j) {
   for (int i2=0; i2<pixList.size(); i2++)
      if (pixList[i2].i == i && pixList[i2].j == j)
         return true;
   return false;
}

bool isInRange(int i, int j, int row, int column) {
   return i >= 0 && j >=0 && i < row && j < column;
}

void movePrim (int &i, int &j, enum side s) {
   if (s == top)     i++;
   if (s == bottom)  i--;
   if (s == left)    j++;
   if (s == right)   j--;
}

void moveSuperpixel (int &i, int &j, enum side s) {
   if (s == top)     i--;
   if (s == bottom)  i++;
   if (s == left)    j--;
   if (s == right)   j++;
}

bool checkCondition (int i, int j, int label, enum side s, MATRIX *rightPrims, MATRIX *leftPrims, MATRIX *topPrims, MATRIX *bottomPrims, int minr, int maxr, int minc, int maxc, MATRIX *mask) {
   if (isInRange (i, j, rightPrims->row, rightPrims->column) && mask->data[i][j]) {
      if (s == right) {
         if (j < minc)
            return false;
         if ((rightPrims->data[i][j] != label && rightPrims->data[i][j] != 0) || leftPrims->data[i][j] > 0 || topPrims->data[i][j] > 0 || bottomPrims->data[i][j] > 0)
            return false;
         else return true;
      }
      else if (s == left) {
         if (j > maxc)
            return false;
         if ((leftPrims->data[i][j] != label && leftPrims->data[i][j] != 0) || rightPrims->data[i][j] > 0 || topPrims->data[i][j] > 0 || bottomPrims->data[i][j] > 0)
            return false;
         else return true;
      }
      else if (s == top) {
         if (i > maxr)
            return false;
         if ((topPrims->data[i][j] != label && topPrims->data[i][j] != 0) || bottomPrims->data[i][j] > 0 || rightPrims->data[i][j] > 0 || leftPrims->data[i][j] > 0)
            return false;
         else return true;
      }
      else {
         if (i < minr)
            return false;
         if ((bottomPrims->data[i][j] != label && bottomPrims->data[i][j] != 0) || topPrims->data[i][j] > 0 || rightPrims->data[i][j] > 0 || leftPrims->data[i][j] > 0)
            return false;
         else return true; 
      }
   }
   else return false;
}

bool checkCondition4Superpixel (int i, int j, int d, int maxDist, enum side s, MATRIX *rightPrims, MATRIX *leftPrims, MATRIX *topPrims, MATRIX *bottomPrims, MATRIX *mask) {
   /*if (isInRange (i, j, rightPrims->row, rightPrims->column) && (maxDist == -1 || d <= maxDist) && mask->data[i][j]) {
      if (s == right || s == left) {
         if (rightPrims->data[i][j] > 0 || leftPrims->data[i][j] > 0)
            return false;
         else return true;
      }
      else {
         if (topPrims->data[i][j] > 0 || bottomPrims->data[i][j] > 0)
            return false;
         else return true;
      }
   }
   else return false;*/

   if (isInRange (i, j, rightPrims->row, rightPrims->column) && (maxDist == -1 || d <= maxDist)) {
      if (s == right) {
         if (rightPrims->data[i][j] > 0 || leftPrims->data[i][j] > 0 || topPrims->data[i][j] > 0 || bottomPrims->data[i][j] > 0)
            return false;
         else return true;
      }
      else if (s == left) {
         if (leftPrims->data[i][j] > 0 || rightPrims->data[i][j] > 0 || topPrims->data[i][j] > 0 || bottomPrims->data[i][j] > 0)
            return false;
         else return true;
      }
      else if (s == top) {
         if (topPrims->data[i][j] > 0 || bottomPrims->data[i][j] > 0 || rightPrims->data[i][j] > 0 || leftPrims->data[i][j] > 0)
            return false;
         else return true;
      }
      else {
         if (bottomPrims->data[i][j] > 0 || topPrims->data[i][j] > 0 || rightPrims->data[i][j] > 0 || leftPrims->data[i][j] > 0)
            return false;
         else return true; 
      }
   }
   else return false;
}

class Superpixel
{
public:
   int label;
   vector<point> pixList;
   vector<int> adjList;
   int minc, minr, maxr, maxc;
   point meanP;
   int isMergable;
   bool isMerged;
   bool isCell;

   pair<int, double> rightVotingPrim;
   pair<int, double> leftVotingPrim;
   pair<int, double> topVotingPrim;
   pair<int, double> bottomVotingPrim;

   Superpixel () {
   	rightVotingPrim 	= make_pair (-1, 0.0);
   	leftVotingPrim 	= make_pair (-1, 0.0);
   	topVotingPrim 		= make_pair (-1, 0.0);
   	bottomVotingPrim 	= make_pair (-1, 0.0);
   	isMergable = 1;
      isMerged = false;
      isCell = false;
   }

   void removeNeighbor (int nglabel) {
		vector<int>::iterator it = find (adjList.begin(), adjList.end(), nglabel); 
		while (it != adjList.end()) {
			adjList.erase(it);
         it = find (adjList.begin(), adjList.end(), nglabel); 
		}
   }

   void addNeighbor (int nglabel) {
      if (label != nglabel && nglabel > 0) {
   		vector<int>::iterator it = find (adjList.begin(), adjList.end(), nglabel); 
   		if (it == adjList.end()) {
   			adjList.push_back(nglabel);
   		}
      }
   }

   void setMeanPoint () {
   	int totI = 0, totJ = 0;
   	for(int i=0; i<pixList.size(); i++) {
   		totI += pixList[i].i;
   		totJ += pixList[i].j;
   	}
   	meanP.i = totI / pixList.size();
   	meanP.j = totJ / pixList.size();
   }

   int getArea() {
   	return pixList.size();
   }

   bool isNeighbor (int label) {
      return find (adjList.begin(), adjList.end(), label) != adjList.end(); 
   }

   bool containsPoint (int i, int j) {
      if (i < minr || i > maxr || j < minc || j > maxc)
         return false; 
      for (int i2=0; i2<pixList.size(); i2++)
         if (pixList[i2].i == i && pixList[i2].j == j)
            return true;
      return false;
   }

   void defineVotingPrims (MATRIX *rightPrims, MATRIX *leftPrims, MATRIX *topPrims, MATRIX *bottomPrims, MATRIX *mask, int maxPrimLabel, enum side s, int maxDist) {
      double overlaps[maxPrimLabel+1];
      for (int j=0; j<maxPrimLabel+1; j++)
         overlaps[j] = 0;
      
      int columnNo;
      if (s == left || s == right)
         columnNo = maxr-minr+1;
      else
         columnNo = maxc-minc+1;
      int visited[columnNo];
      for (int i=0; i<columnNo; i++)
         visited[i] = 0;

      vector <point> points = pixList;
      for (int i=0; i<points.size(); i++) {
         int i2 = points[i].i;
         int j2 = points[i].j;
         int d = 1;

         //be careful that this time we are moving from the superpixel not from a primivite
         moveSuperpixel (i2, j2, s);

         while ((maxDist == -1 || d <= maxDist) && isInRange(i2, j2, rightPrims->row, rightPrims->column)) {
            if (((s == left || s == right) && visited[i2-minr]) || ((s==top || s==bottom) && visited[j2-minc]))
               break;
            
            if (!checkCondition4Superpixel (i2, j2, d, maxDist, s, rightPrims, leftPrims, topPrims, bottomPrims, mask)) {
               if (s == right && rightPrims->data[i2][j2]) {
                  overlaps[rightPrims->data[i2][j2]]++;
                  visited[i2-minr] = 1;
               }
               else if (s == left && leftPrims->data[i2][j2]) {
                  overlaps[leftPrims->data[i2][j2]]++;
                  visited[i2-minr] = 1;
               }
               else if (s == top && topPrims->data[i2][j2]) {
                  overlaps[topPrims->data[i2][j2]]++;
                  visited[j2-minc] = 1;
               }
               else if (s == bottom && bottomPrims->data[i2][j2]) {
                  overlaps[bottomPrims->data[i2][j2]]++;
                  visited[j2-minc] = 1;
               }
               break;
            }

            //be careful that this time we are moving from the superpixel not from a primivite
            moveSuperpixel (i2, j2, s);
            d++;
         }
      }

      double maxVal = 0;
      int maxInd = -1;
      for (int j=1; j<maxPrimLabel+1; j++) {
         if (overlaps[j] > maxVal) {
            maxVal = overlaps[j];
            maxInd = j;
         }
      }
      if (s == right)   rightVotingPrim   = make_pair (maxInd, maxVal);
      if (s == left)    leftVotingPrim    = make_pair (maxInd, maxVal);
      if (s == top)     topVotingPrim     = make_pair (maxInd, maxVal);
      if (s == bottom)  bottomVotingPrim  = make_pair (maxInd, maxVal);
   }

};