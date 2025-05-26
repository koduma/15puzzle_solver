//g++ -O2 -std=c++11 check.cpp -o check

#include <vector>
#include <cfloat>
#include <cstdio>
#include <tuple>
#include <cstring>
#include <climits>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <string>
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <cassert>
#include <random>
#include <queue>
#include <deque>
#include <list>
#include <map>
#include <array>
#include <chrono>
#include <fstream>
#include <functional>
#include <unordered_map>


using namespace std;


int board[4][4]={
{14,15,8,12},
{10,11,9,13},
{2,6,5,1},
{3,7,4,0}
};
int ck[66]={1,13,12,8,9,11,6,5,4,1,13,12,11,4,1,7,5,6,15,9,4,15,6,1,7,13,12,7,15,6,1,2,10,14,9,1,2,5,3,10,14,9,1,2,5,3,13,15,3,14,10,13,14,10,9,5,6,3,7,11,8,4,3,7,11,12};


int main(){

    int zero_pos;

    for(int i=0;i<16;i++){
        if(board[i/4][i%4]==0){zero_pos=i;break;}
    }

    for(int i=0;i<66;i++){
        int pos=ck[i];
        for(int j=0;j<16;j++){
            if(board[j/4][j%4]==pos){
                if((j==zero_pos-1)||(j==zero_pos+1)||(j==zero_pos+4)||(j==zero_pos-4));
                else{printf("error!\n");}
                swap(board[j/4][j%4],board[zero_pos/4][zero_pos%4]);
                zero_pos=j;
                break;
            }
        }
    }

    for(int i=0;i<4;i++){
    for(int j=0;j<4;j++){
        printf("%d,",board[i][j]);
    }
    printf("\n");
    }

    printf("\n");
    return 0;
}
