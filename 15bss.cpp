//g++ -O2 -std=c++11 -fopenmp -lpthread 15bss.cpp loguru.cpp -o 15bss -mcmodel=large

/*
    
14 15 8 12
10 11 9 13
2 6 5 1
3 7 4 *

path=66

*/

#pragma warning(disable:4710)
#pragma warning(disable:4711)
#pragma warning(disable:4820)
#pragma GCC optimize("unroll-loops")
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
#include "hash_map.hpp"
#include <immintrin.h>
#include <omp.h>

#pragma GCC target ("sse4.2")

using namespace std;

#define ROW 4
#define COL 4
#define TRN 126
#define BW 100000
#define BW2 300//BW2=1(115s)

typedef unsigned long long ll;

struct node {
char score;
ll hash;
ll ans[(TRN/21)+1];
char prev;
}fff[4*BW],ggg[4*BW2];

struct k2p {
    char tile[16];
    char tile_pos[16];
    char depth;
    char zpos;

    k2p(const char* s, const char* s2,char d,char zp) {
    memcpy(tile, s, sizeof(tile));
    memcpy(tile_pos, s2, sizeof(tile_pos));
    depth = d;
    zpos = zp;
    }
};

string bestans;

int shot=0;

ll zoblish_field[ROW][COL][ROW*COL];
char k5p[3][ROW*COL][ROW*COL][ROW*COL][ROW*COL][ROW*COL][ROW*COL];

char operation(char board[ROW][COL], ll movei[(TRN/21)+1]) {   
char pos[ROW * COL] = {0};
char number[ROW * COL] = {0};
for (char i = 0; i < ROW * COL; i++) {
pos[(int)board[i / COL][i % COL]] = i;
number[(int)i]=board[i/COL][i%COL];
}
char zero_pos=pos[0];
for (char i = 0; i <= TRN / 21; i++) {    
if (movei[i] == 0ll) {break;}
for (int j = 0; j < 21; j++) {
int m = (int)(7ll & (movei[i] >> (3 * j)));   
if (m == 0) {
break;
}

int dir=-1;

if(m==1){dir=(int)zero_pos-1;}
else if(m==2){dir=(int)zero_pos-COL;}
else if(m==3){dir=(int)zero_pos+COL;}
else if(m==4){dir=(int)zero_pos+1;}

int zero_row = ((int)zero_pos) / COL;
int zero_col = ((int)zero_pos) % COL;
int move_row = dir / COL;
int move_col = dir % COL;
            
swap(board[zero_row][zero_col], board[move_row][move_col]);

pos[0] = (char)dir;
pos[number[dir]] = zero_pos;

swap(number[dir], number[zero_pos]);

zero_pos = (char)dir;
}
}
return zero_pos;
}

char MH_EV(char board[ROW][COL]){
    char ev=0;
    char pos[ROW*COL]={0};
    char goalboard[ROW*COL]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0};
    bool goal=true;
    for(char i=0;i<ROW*COL;i++){
    pos[(int)board[i/COL][i%COL]]=i;
    if(board[i/COL][i%COL]!=goalboard[i]){goal=false;}
    }
    //{1,2,5,9,13},{3,4,6,7,8},{10,11,12,14,15}
    if(goal){return 0;}
    ev+=k5p[0][pos[0]][pos[1]][pos[2]][pos[5]][pos[9]][pos[13]];
    ev+=k5p[1][pos[0]][pos[3]][pos[4]][pos[6]][pos[7]][pos[8]];
    ev+=k5p[2][pos[0]][pos[10]][pos[11]][pos[12]][pos[14]][pos[15]];
    return ev;
}


ll xor128() {//xorshift整数乱数
	static unsigned long long rx = 123456789, ry = 362436069, rz = 521288629, rw = 88675123;
	ll rt = (rx ^ (rx << 11));
	rx = ry; ry = rz; rz = rw;
	return (rw = (rw ^ (rw >> 19)) ^ (rt ^ (rt >> 8)));
}

ll calc_hash(char board[ROW][COL]){
ll hash=0ll;
for (int row = 0; row < ROW; row++) {
for (int col = 0; col < COL; col++) {
char num = board[row][col];
hash ^= zoblish_field[row][col][(int)num];
}
}
return hash;
}

void bfs1(){

    queue<k2p>pq;
    //{1,2,5,9,13},{3,4,6,7,8},{10,11,12,14,15}
    //char tile[16];
    //char tile_pos[16];    
    //char depth;
    //char zpos;
    char tile[16];
    char tile_pos[16];
    for(char i=0;i<ROW*COL;i++){
    tile[(int)i]=16;
    tile_pos[(int)i]=-1;
    }
    tile[0]=1;
    tile[1]=2;
    tile[4]=5;
    tile[8]=9;
    tile[12]=13;
    tile[15]=0;
    tile_pos[1]=0;
    tile_pos[2]=1;
    tile_pos[5]=4;
    tile_pos[9]=8;
    tile_pos[13]=12;
    tile_pos[0]=15;
    char zps=15;
    k2p kpt = {tile,tile_pos, 0,zps};
    pq.push(kpt);
    int dx[4] = { -1, 0,0,1 };
    int dy[4] = { 0,-1,1,0 };
    memset(k5p, -1, sizeof(k5p));
    k5p[0][15][0][1][4][8][12]=0;
    while(!pq.empty()){
        k2p tp = pq.front();pq.pop();
        int y=((int)tp.zpos)/COL;
        int x=((int)tp.zpos)%COL;
        for (int j = 0; j < 4; j++) {
        if (0 <= x + dx[j] && x + dx[j] < COL &&
            0 <= y + dy[j] && y + dy[j] < ROW) {
            k2p next = tp;
            int ny = y + dy[j];
            int nx = x + dx[j];
            int d = static_cast<int>(next.depth);
            next.depth = (char)(d - 1);
            char tileA = next.tile[next.zpos];
            char tileB = next.tile[(ny * COL) + nx];
            swap(next.tile[next.zpos], next.tile[(ny * COL) + nx]);
            if (tileB != 16) {
                swap(next.tile_pos[0], next.tile_pos[(int)tileB]);
            } else {
                next.tile_pos[0] = (ny * COL) + nx;
            }
            next.zpos = (ny * COL) + nx;
            if (k5p[0][(int)next.zpos][(int)next.tile_pos[1]][(int)next.tile_pos[2]]
                    [(int)next.tile_pos[5]][(int)next.tile_pos[9]][(int)next.tile_pos[13]]==-1) {
                k5p[0][(int)next.zpos][(int)next.tile_pos[1]][(int)next.tile_pos[2]]
                    [(int)next.tile_pos[5]][(int)next.tile_pos[9]][(int)next.tile_pos[13]] = -next.depth;   
                pq.push(next);
            }
        }
    }
}
}

void bfs2(){

    queue<k2p>pq;
    //{1,2,5,9,13},{3,4,6,7,8},{10,11,12,14,15}
    //char tile[16];
    //char tile_pos[16];    
    //char depth;
    //char zpos;
    char tile[16];
    char tile_pos[16];
    for(char i=0;i<ROW*COL;i++){
    tile[(int)i]=16;
    tile_pos[(int)i]=-1;
    }
    tile[2]=3;
    tile[3]=4;
    tile[5]=6;
    tile[6]=7;
    tile[7]=8;
    tile[15]=0;
    tile_pos[3]=2;
    tile_pos[4]=3;
    tile_pos[6]=5;
    tile_pos[7]=6;
    tile_pos[8]=7;
    tile_pos[0]=15;
    char zps=15;
    k2p kpt = {tile,tile_pos, 0,zps};
    pq.push(kpt);
    int dx[4] = { -1, 0,0,1 };
    int dy[4] = { 0,-1,1,0 };
    k5p[1][15][2][3][5][6][7]=0;
    while(!pq.empty()){
        k2p tp = pq.front();pq.pop();
        int y=((int)tp.zpos)/COL;
        int x=((int)tp.zpos)%COL;
        for (int j = 0; j < 4; j++) {
        if (0 <= x + dx[j] && x + dx[j] < COL &&
            0 <= y + dy[j] && y + dy[j] < ROW) {
            k2p next = tp;
            int ny = y + dy[j];
            int nx = x + dx[j];
            int d = static_cast<int>(next.depth);
            next.depth = (char)(d - 1);
            char tileA = next.tile[next.zpos];
            char tileB = next.tile[(ny * COL) + nx];
            swap(next.tile[next.zpos], next.tile[(ny * COL) + nx]);
            if (tileB != 16) {
                swap(next.tile_pos[0], next.tile_pos[(int)tileB]);
            } else {
                next.tile_pos[0] = (ny * COL) + nx;
            }
            next.zpos = (ny * COL) + nx;
            if (k5p[1][(int)next.zpos][(int)next.tile_pos[3]][(int)next.tile_pos[4]]
                    [(int)next.tile_pos[6]][(int)next.tile_pos[7]][(int)next.tile_pos[8]]==-1) {
                k5p[1][(int)next.zpos][(int)next.tile_pos[3]][(int)next.tile_pos[4]]
                    [(int)next.tile_pos[6]][(int)next.tile_pos[7]][(int)next.tile_pos[8]] = -next.depth;   
                pq.push(next);
            }
        }
    }
}
}

void bfs3(){

    queue<k2p>pq;
    //{1,2,5,9,13},{3,4,6,7,8},{10,11,12,14,15}
    //char tile[16];
    //char tile_pos[16];    
    //char depth;
    //char zpos;
    char tile[16];
    char tile_pos[16];
    for(char i=0;i<ROW*COL;i++){
    tile[(int)i]=16;
    tile_pos[(int)i]=-1;
    }
    tile[9]=10;
    tile[10]=11;
    tile[11]=12;
    tile[13]=14;
    tile[14]=15;
    tile[15]=0;
    tile_pos[10]=9;
    tile_pos[11]=10;
    tile_pos[12]=11;
    tile_pos[14]=13;
    tile_pos[15]=14;
    tile_pos[0]=15;
    char zps=15;
    k2p kpt = {tile,tile_pos, 0,zps};
    pq.push(kpt);
    int dx[4] = { -1, 0,0,1 };
    int dy[4] = { 0,-1,1,0 };
    k5p[2][15][9][10][11][13][14]=0;
    while(!pq.empty()){
        k2p tp = pq.front();pq.pop();
        int y=((int)tp.zpos)/COL;
        int x=((int)tp.zpos)%COL;
        for (int j = 0; j < 4; j++) {
        if (0 <= x + dx[j] && x + dx[j] < COL &&
            0 <= y + dy[j] && y + dy[j] < ROW) {
            k2p next = tp;
            int ny = y + dy[j];
            int nx = x + dx[j];
            int d = static_cast<int>(next.depth);
            next.depth = (char)(d - 1);
            char tileA = next.tile[next.zpos];
            char tileB = next.tile[(ny * COL) + nx];
            swap(next.tile[next.zpos], next.tile[(ny * COL) + nx]);
            if (tileB != 16) {
                swap(next.tile_pos[0], next.tile_pos[(int)tileB]);
            } else {
                next.tile_pos[0] = (ny * COL) + nx;
            }
            next.zpos = (ny * COL) + nx;
            if (k5p[2][(int)next.zpos][(int)next.tile_pos[10]][(int)next.tile_pos[11]]
                    [(int)next.tile_pos[12]][(int)next.tile_pos[14]][(int)next.tile_pos[15]]==-1) {
                k5p[2][(int)next.zpos][(int)next.tile_pos[10]][(int)next.tile_pos[11]]
                    [(int)next.tile_pos[12]][(int)next.tile_pos[14]][(int)next.tile_pos[15]] = -next.depth;   
                pq.push(next);
            }
        }
    }
}
}

string getans(char board[ROW][COL],ll movei[(TRN/21)+1]){
string ans="";
char pos[ROW * COL] = {0};
char number[ROW * COL] = {0};
for (char i = 0; i < ROW * COL; i++) {
pos[(int)board[i / COL][i % COL]] = i;
number[(int)i]=board[i/COL][i%COL];
}
char zero_pos=pos[0];
for (char i = 0; i <= TRN / 21; i++) {
if (movei[i] == 0ll) {break;}
for (int j = 0; j < 21; j++) {
int m = (int)(7ll & (movei[i] >> (3 * j)));
if (m == 0) {
break;
}

int dir=-1;

if(m==1){dir=(int)zero_pos-1;}
else if(m==2){dir=(int)zero_pos-COL;}
else if(m==3){dir=(int)zero_pos+COL;}
else if(m==4){dir=(int)zero_pos+1;}

int zero_row = ((int)zero_pos) / COL;
int zero_col = ((int)zero_pos) % COL;
int move_row = dir / COL;
int move_col = dir % COL;

ans+=to_string((int)board[move_row][move_col])+"\n";    
            
swap(board[zero_row][zero_col], board[move_row][move_col]);

pos[0] = (char)dir;
pos[number[dir]] = zero_pos;

swap(number[dir], number[zero_pos]);

zero_pos = (char)dir;
    
}
}
return ans;
}

int BEAM_SEARCH(char board[ROW][COL],int TURN) {
if(MH_EV(board)==0){return 0;}
if(shot%1000==0){printf("shot=%d\n",shot);}
//printf("shot=%d\n",shot);
shot++;    
vector<node>dque;
node n0;
n0.hash=calc_hash(board);
for(int i=0;i<=(TRN/21);i++){
n0.ans[i]=0ll;
}
n0.prev=-1;    
dque.push_back(n0);

int dx[4] = { -1, 0,0,1 };
int dy[4] = { 0,-1,1,0 };

emilib::HashMap<ll, bool> visited;

for (int i = 0; i < TURN; i++) {
int ks = (int)dque.size();
#pragma omp parallel for
for (int k = 0; k < ks; k++) {
node temp = dque[k];
char temp_board[ROW][COL];
memcpy(temp_board, board, sizeof(temp_board));
char zero_pos=operation(temp_board, temp.ans);
for (int j = 0; j < 4; j++) {
node cand = temp;
int xxx=((int)zero_pos)%COL;
int yyy=((int)zero_pos)/COL;
if (0 <= xxx + dx[j] && xxx + dx[j] < COL &&
0 <= yyy + dy[j] && yyy + dy[j] < ROW && cand.prev+j!=3) {
char board2[ROW][COL];
memcpy(board2,temp_board,sizeof(board2));
int ny=yyy + dy[j];
int nx=xxx + dx[j];
cand.hash^=(zoblish_field[yyy][xxx][(int)board2[yyy][xxx]])^(zoblish_field[ny][nx][(int)board2[ny][nx]]);
cand.hash^=(zoblish_field[yyy][xxx][(int)board2[ny][nx]])^(zoblish_field[ny][nx][(int)board2[yyy][xxx]]);
cand.ans[i/21] |= (((ll)(j+1))<<((3*i)%63));
swap(board2[ny][nx],board2[yyy][xxx]);
cand.score=MH_EV(board2);    
cand.prev=j;
fff[(4 * k) + j] = cand;
}
else{
cand.score=125;
fff[(4 * k) + j] = cand;
}
}
}   
dque.clear();
vector<pair<int,int> >vv;
for (int j = 0; j < 4 * ks; j++) {
if(fff[j].score==0){
    return i+1;
}
if(fff[j].score<125){
vv.push_back(make_pair((int)fff[j].score,j));
}
}
sort(vv.begin(),vv.end());
int push_node=0;
for (int j = 0; push_node < BW ;j++) {  
if(j>=(int)vv.size()){break;}
int p=vv[j].second;
node n1=fff[p];
if (i < TRN - 1) {
if(!visited[n1.hash]){
visited[n1.hash]=true;
dque.push_back(n1);
push_node++;
}
}
}
}
return -1;
}

int BEAM_SEARCH2(char board[ROW][COL]) {
if(MH_EV(board)==0){return 0;}	
int LIM=TRN;
vector<int>pro_league;
vector<node>dque;
node n0;
n0.hash=calc_hash(board);
for(int i=0;i<=(TRN/21);i++){
n0.ans[i]=0ll;
}
n0.prev=-1;    
dque.push_back(n0);
int dx[4] = { -1, 0,0,1 };
int dy[4] = { 0,-1,1,0 };
emilib::HashMap<ll, bool> visited;	
for (int i = 0; i < TRN; i++) {
int ks = (int)dque.size();
pro_league.clear();	
//#pragma omp parallel for
for (int k = 0; k < ks; k++) {
node temp = dque[k];
char temp_board[ROW][COL];
memcpy(temp_board, board, sizeof(temp_board));
char zero_pos=operation(temp_board, temp.ans);
for (int j = 0; j < 4; j++) {
node cand = temp;
int xxx=((int)zero_pos)%COL;
int yyy=((int)zero_pos)/COL;
if (0 <= xxx + dx[j] && xxx + dx[j] < COL &&
0 <= yyy + dy[j] && yyy + dy[j] < ROW && cand.prev+j!=3) {
char board2[ROW][COL];
memcpy(board2,temp_board,sizeof(board2));
int ny=yyy + dy[j];
int nx=xxx + dx[j];
cand.hash^=(zoblish_field[yyy][xxx][(int)board2[yyy][xxx]])^(zoblish_field[ny][nx][(int)board2[ny][nx]]);
cand.hash^=(zoblish_field[yyy][xxx][(int)board2[ny][nx]])^(zoblish_field[ny][nx][(int)board2[yyy][xxx]]);
cand.ans[i/21] |= (((ll)(j+1))<<((3*i)%63));
swap(board2[ny][nx],board2[yyy][xxx]);
int lim=LIM;
if((int)pro_league.size()>=BW2){lim=pro_league[BW2-1];LIM=lim;}	
cand.score=(char)BEAM_SEARCH(board2,lim-1);  
if(cand.score==-1){cand.score=124;}
if(MH_EV(board2)==0){
    bestans=getans(board,cand.ans);
    return i+1;
}
cand.prev=j;
int cs=(int)cand.score;
if(cs<lim){	
pro_league.push_back(cs);
sort(pro_league.begin(),pro_league.end());
}	
ggg[(4 * k) + j] = cand;
}
else{
cand.score=125;
ggg[(4 * k) + j] = cand;
}
}
}
dque.clear();
vector<pair<int,int> >vv;
for (int j = 0; j < 4 * ks; j++) {
if(ggg[j].score<125){
vv.push_back(make_pair((int)ggg[j].score,j));
}
}
sort(vv.begin(),vv.end());
printf("depth=%d/%d,score=%d\n",i+1,TRN,vv[0].first);    
int push_node=0;
for (int j = 0; push_node < BW2 ;j++) {  
if(j>=(int)vv.size()){break;}
int p=vv[j].second;
node n1=ggg[p];
if (i < TRN - 1) {
if(!visited[n1.hash]){
visited[n1.hash]=true;
dque.push_back(n1);
push_node++;
}
}
}
}
return -1;
}


int main() {
    
char board[ROW][COL]={0};
string a[ROW][COL];

for(int i=0; i<ROW; i++){
for(int j=0; j<COL; j++) {
cin >> a[i][j];
if(a[i][j]=="*"){board[i][j]=0;}
else{board[i][j]=stoi(a[i][j]);}
}
}

auto start = chrono::high_resolution_clock::now();

int i1, i2, i3;
for(i1=0;i1<ROW;++i1){
for(i2=0;i2<COL;++i2){
for(i3=0;i3<ROW*COL;i3++){
zoblish_field[i1][i2][i3]=xor128();
}
}
}

bfs1();
bfs2();
bfs3();    

printf("path=%d\n",BEAM_SEARCH2(board));


auto end = chrono::high_resolution_clock::now();
//printf("path=%d\n",BEAM_SEARCH(board,TRN));
//BEAM_SEARCH2(board);
cout<<bestans;

auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

cout << "time: " << duration/1000.0 << "s" << endl;

return 0;
}
