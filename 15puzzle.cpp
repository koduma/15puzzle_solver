//g++ -O2 -std=c++11 -fopenmp 15puzzle.cpp -o 15p

//#pragma warning(disable:4710)
//#pragma warning(disable:4711)
//#pragma warning(disable:4820)
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
//#include <omp.h>

#pragma GCC target ("sse4.2")

using namespace std;

#define ROW 4
#define COL 4
#define TRN 200
#define BW 45000
#define BW2 1

typedef unsigned long long ll;

struct node {
char score;
ll hash;
ll ans[(TRN/16)+1];
}fff[4*BW],ggg[4*BW2];

string bestans;

ll zoblish_field[ROW][COL][ROW*COL];

char operation(char board[ROW][COL], ll movei[(TRN/16)+1]) {
    char pos[ROW * COL] = {0};
    for (char i = 0; i < ROW * COL; i++) {
        pos[(int)board[i / COL][i % COL]] = i;
    }
    char zero_pos=pos[0];
    for (char i = 0; i <= TRN / 16; i++) {
        if (movei[i] == 0ll) {
            break;
        }
        for (int j = 0; j < 16; j++) {
            int dir = (int)(15ll & (movei[i] >> (4 * j)));
            if (dir == 0) {
                break;
            }

            int zero_row = ((int)zero_pos) / COL;
            int zero_col = ((int)zero_pos) % COL;
            int move_row = ((int)pos[dir]) / COL;
            int move_col = ((int)pos[dir]) % COL;
            
            swap(board[zero_row][zero_col], board[move_row][move_col]);

            pos[0] = pos[dir];
            pos[dir] = zero_row * COL + zero_col;

            zero_pos = (char)(move_row * COL + move_col);
        }
    }
    return zero_pos;
}

char MH_EV(char board[ROW][COL]){
    char ev=0;
    char pos[ROW*COL]={0};
    for(char i=0;i<ROW*COL;i++){
    pos[(int)board[i/COL][i%COL]]=i;    
    }
    for(char i=0;i<ROW*COL;i++){
        if(i==0){continue;}
        int correct_col=(i-1)%COL;
        int correct_row=(i-1)/COL;
        int cur_row=((int)pos[i])/COL;
        int cur_col=((int)pos[i])%COL;
        ev+=(char)(abs(cur_row-correct_row)+abs(cur_col-correct_col));
    }
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
string getans(ll movei[(TRN/16)+1]){
string ans="";
for (char i = 0; i <= TRN/16; i++) {
if (movei[i] == 0ll) { break; }
for(char k=0;k<16;k++){
int dir = (int)(15ll&(movei[i]>>(4*k)));
if (dir==0){break;}
ans+=to_string(dir)+"\n";
}
}
return ans;
}

int BEAM_SEARCH(char board[ROW][COL]) {
vector<node>dque;
node n0;
n0.hash=calc_hash(board);
for(int i=0;i<=(TRN/16);i++){
n0.ans[i]=0ll;
}
dque.push_back(n0);

int dx[4] = { -1, 0,0,1 };
int dy[4] = { 0,-1,1,0 };

unordered_map<ll, bool> visited;

for (int i = 0; i < TRN; i++) {
int ks = (int)dque.size();
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
0 <= yyy + dy[j] && yyy + dy[j] < ROW) {
char board2[ROW][COL];
memcpy(board2,temp_board,sizeof(board2));
int ny=yyy + dy[j];
int nx=xxx + dx[j];
ll z=(ll)board2[ny][nx];
cand.hash^=(zoblish_field[yyy][xxx][(int)board2[yyy][xxx]])^(zoblish_field[ny][nx][(int)board2[ny][nx]]);
cand.hash^=(zoblish_field[yyy][xxx][(int)board2[ny][nx]])^(zoblish_field[ny][nx][(int)board2[yyy][xxx]]);
cand.ans[i/16] |= z<<((4*i)%64);
swap(board2[ny][nx],board2[yyy][xxx]);
cand.score=MH_EV(board2);
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
if (fff[j].score == 0) {
bestans=getans(fff[j].ans);
return i+1;
}
vv.push_back(make_pair(fff[j].score,j));
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
vector<node>dque;
node n0;
n0.hash=calc_hash(board);
for(int i=0;i<=(TRN/16);i++){
n0.ans[i]=0ll;
}
dque.push_back(n0);
int dx[4] = { -1, 0,0,1 };
int dy[4] = { 0,-1,1,0 };
unordered_map<ll, bool> visited;
for (int i = 0; i < TRN; i++) {
int ks = (int)dque.size();
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
0 <= yyy + dy[j] && yyy + dy[j] < ROW) {
char board2[ROW][COL];
memcpy(board2,temp_board,sizeof(board2));
int ny=yyy + dy[j];
int nx=xxx + dx[j];
ll z=(ll)board2[ny][nx];
cand.hash^=(zoblish_field[yyy][xxx][(int)board2[yyy][xxx]])^(zoblish_field[ny][nx][(int)board2[ny][nx]]);
cand.hash^=(zoblish_field[yyy][xxx][(int)board2[ny][nx]])^(zoblish_field[ny][nx][(int)board2[yyy][xxx]]);
cand.ans[i/16] |= z<<((4*i)%64);
swap(board2[ny][nx],board2[yyy][xxx]);
cand.score=(char)BEAM_SEARCH(board2);
if(cand.score==-1){cand.score=125;}
if(cand.score<10){
if(MH_EV(board2)==0){
bestans=getans(cand.ans);
return i+1;
}    
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
vv.push_back(make_pair(ggg[j].score,j));
}
sort(vv.begin(),vv.end());
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

int i1, i2, i3;
for(i1=0;i1<ROW;++i1){
for(i2=0;i2<COL;++i2){
for(i3=0;i3<ROW*COL;i3++){
zoblish_field[i1][i2][i3]=xor128();
}
}
}
    

//printf("path=%d\n",BEAM_SEARCH2(board));
//printf("path=%d\n",BEAM_SEARCH(board));
BEAM_SEARCH(board);
cout<<bestans;

return 0;
}
