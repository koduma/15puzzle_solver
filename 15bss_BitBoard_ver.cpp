
//Linux:g++ -O2 -std=c++11 -fopenmp -lpthread 15bss_BitBoard_ver.cpp loguru.cpp -o 15bss_BitBoard_ver -mcmodel=large -ldl
//Windows10,Windows11:g++ -O2 -std=c++11 -fopenmp -lpthread 15bss_BitBoard_ver.cpp loguru.cpp -o 15bss_BitBoard_ver -mcmodel=large

/*
    
* 12 9 13
15 11 10 14
3 7 2 5
4 8 6 1

=ok

* 12 10 13
15 11 14 9
3 7 2 5
4 8 6 1

=ok

* 11 9 13
12 15 10 14
3 7 6 2
4 8 5 1

=ok

* 15 9 13
11 12 10 14
3 7 6 2
4 8 5 1

=ok

* 12 9 13
15 11 10 14
3 7 6 2
4 8 5 1

=WA(82),use meta=ok

* 12 14 13
15 11 9 10
3 7 6 2
4 8 5 1

=WA(82),use meta=ok

* 12 10 13
15 11 14 9
3 7 6 2
4 8 5 1

=ok

* 12 11 13
15 14 10 9
3 7 6 2
4 8 5 1

=WA(82),use meta=ok

* 12 10 13
15 11 9 14
7 3 6 2
4 8 5 1

=ok

* 12 9 13
15 11 14 10
3 8 6 2
4 7 5 1

=ok


* 12 9 13
15 11 10 14
8 3 6 2
4 7 5 1

=ok


* 12 14 13
15 11 9 10
8 3 6 2
4 7 5 1

=ok

* 12 9 13
15 11 10 14
7 8 6 2
4 3 5 1

=ok

* 12 10 13
15 11 14 9
7 8 6 2
4 3 5 1

=ok

* 12 9 13
15 8 10 14
11 7 6 2
4 3 5 1

=ok

* 12 9 13
15 11 10 14
3 7 5 6
4 8 2 1

=ok


* 12 9 13
15 11 10 14
7 8 5 6
4 3 2 1

=ok


path=80

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
#define BW 10000
#define BW2 10

typedef unsigned long long ll;
typedef unsigned char uc;

struct node {
uc score;
ll hash;
ll ans[(TRN/32)+1];
char prev;
}fff[4*BW],ggg[4*BW2];

struct k2p {
    long long tile;      // 盤面
    long long tile_pos;  // 各番号の位置
    char depth;
    char zpos;
    k2p(long long t, long long tp, char d, char zp)
      : tile(t), tile_pos(tp), depth(d), zpos(zp) {}
};

inline char get_tile(long long tile, int i) {
    return (tile >> (i * 4)) & 0xF;
}
inline void set_tile(long long& tile, int i, char val) {
    tile &= ~(0xFLL << (i * 4));
    tile |= (static_cast<long long>(val & 0xF) << (i * 4));
}

void exchange_ll(int posA, int posB, ll* field) {
    int shiftA = posA * 4;
    int shiftB = posB * 4;

    ll nA = (*field >> shiftA) & 0xFULL;
    ll nB = (*field >> shiftB) & 0xFULL;

    *field &= ~(0xFULL << shiftA);
    *field &= ~(0xFULL << shiftB);

    *field |= (nA << shiftB) | (nB << shiftA);
}

string bestans;

int shot=0;

ll zoblish_field[ROW][COL][ROW*COL];
char k5p[3][ROW*COL][ROW*COL][ROW*COL][ROW*COL][ROW*COL][ROW*COL];

char operation(char board[ROW][COL], ll movei[(TRN/32)+1],char pos[ROW * COL],ll* bboard,int lim) {   
char number[ROW * COL] = {0};
for (char i = 0; i < ROW * COL; i++) {
pos[(int)board[((int)i) / COL][((int)i) % COL]] = i;
number[(int)i]=board[((int)i)/COL][((int)i)%COL];
}
char zero_pos=pos[0];
ll bboard2=*bboard;    
for (char i = 0; i <= TRN / 32; i++) {
if(lim==0){break;}    
for (int j = 0; j < 32; j++) {
lim--;
if(lim==0){break;}    
int m = (int)(3ll & (movei[(int)i] >> (2 * j)));    

int dir=-1;

if(m==0){dir=(int)zero_pos-1;}
else if(m==1){dir=(int)zero_pos-COL;}
else if(m==2){dir=(int)zero_pos+COL;}
else if(m==3){dir=(int)zero_pos+1;}

int zero_row = ((int)zero_pos) / COL;
int zero_col = ((int)zero_pos) % COL;
int move_row = dir / COL;
int move_col = dir % COL;
	
char tmp=board[zero_row][zero_col];
board[zero_row][zero_col]=board[move_row][move_col];
board[move_row][move_col]=tmp;
    
exchange_ll(((move_row*COL)+move_col),(zero_row*COL)+zero_col,&bboard2);

pos[0] = (char)dir;
pos[(int)number[dir]] = zero_pos;
tmp=number[dir];
number[dir]=number[(int)zero_pos];
number[(int)zero_pos]=tmp;

zero_pos = (char)dir;
}
}
*bboard=bboard2;    
return zero_pos;
}

unsigned char MH_EV(ll goalboard,ll bboard,char pos[ROW*COL]){
    if(goalboard==bboard){return 0;}
    uc ev=0;
    //{1,2,5,9,13},{3,4,6,7,8},{10,11,12,14,15}
    ev+=k5p[0][(int)pos[0]][(int)pos[1]][(int)pos[2]][(int)pos[5]][(int)pos[9]][(int)pos[13]];
    ev+=k5p[1][(int)pos[0]][(int)pos[3]][(int)pos[4]][(int)pos[6]][(int)pos[7]][(int)pos[8]];
    ev+=k5p[2][(int)pos[0]][(int)pos[10]][(int)pos[11]][(int)pos[12]][(int)pos[14]][(int)pos[15]];    
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

void bfs(int key1,int key2,int key3,int key4,int key5,int pattern){
    queue<k2p> pq;
    long long tile = 0, tile_pos = 0;
    int same1,same2;
    if (key1==1){
        same1=8;
        same2=6;
    }
    else if(key1==3){
        same1=9;
        same2=9;
    }
    else if(key1==10){
        same1=13;
        same2=8;
    }
    for (int i = 0; i < ROW * COL; ++i) {
        set_tile(tile, i, same1);
        set_tile(tile_pos, i, same2);
    }
    set_tile(tile, key1 - 1, key1);
    set_tile(tile, key2 - 1, key2);
    set_tile(tile, key3 - 1, key3);
    set_tile(tile, key4 - 1, key4);
    set_tile(tile, key5 - 1, key5);
    set_tile(tile, 15, 0);

    set_tile(tile_pos, key1, key1 - 1);
    set_tile(tile_pos, key2, key2 - 1);
    set_tile(tile_pos, key3, key3 - 1);
    set_tile(tile_pos, key4, key4 - 1);
    set_tile(tile_pos, key5, key5 - 1);
    set_tile(tile_pos, 0, 15);

    char zps = 15;
    k2p kpt(tile, tile_pos, 0, zps);
    pq.push(kpt);

    int dx[4] = { -1, 0, 0, 1 };
    int dy[4] = { 0, -1, 1, 0 };
    memset(k5p[pattern], -1, sizeof(k5p[pattern]));
    k5p[pattern][15][key1 - 1][key2 - 1][key3 - 1][key4 - 1][key5 - 1] = 0;

    while (!pq.empty()) {
        k2p tp = pq.front(); pq.pop();
        int y = tp.zpos / COL;
        int x = tp.zpos % COL;
        for (int j = 0; j < 4; j++) {
            int nx = x + dx[j];
            int ny = y + dy[j];
            if (0 <= nx && nx < COL && 0 <= ny && ny < ROW) {
                k2p next = tp;
                int d = static_cast<int>(next.depth);
                next.depth = (char)(d - 1);
                int nzpos = ny * COL + nx;
                char tileB = get_tile(next.tile, nzpos);
                char tpm = get_tile(next.tile, next.zpos);
                set_tile(next.tile, next.zpos, get_tile(next.tile, nzpos));
                set_tile(next.tile, nzpos, tpm);
                if (tileB != same1) {
                    char tpm2 = get_tile(next.tile_pos, 0);
                    set_tile(next.tile_pos, 0, get_tile(next.tile_pos, tileB));
                    set_tile(next.tile_pos, tileB, tpm2);
                } else {
                    set_tile(next.tile_pos, 0, nzpos);
                }
                next.zpos = nzpos;
                char* state_ptr = &k5p[pattern][(int)next.zpos][(int)get_tile(next.tile_pos, key1)][(int)get_tile(next.tile_pos, key2)][(int)get_tile(next.tile_pos, key3)][(int)get_tile(next.tile_pos, key4)][(int)get_tile(next.tile_pos, key5)];
                if (*state_ptr == (char)-1) {
                    *state_ptr = -next.depth;
                    pq.push(next);
                }
            }
        }
    }
}

string getans(char board[ROW][COL],ll movei[(TRN/32)+1],int lim){
string ans="";
char pos[ROW * COL] = {0};
char number[ROW * COL] = {0};
for (char i = 0; i < ROW * COL; i++) {
pos[(int)board[((int)i)/ COL][((int)i) % COL]] = i;
number[(int)i]=board[((int)i)/COL][((int)i)%COL];
}
char zero_pos=pos[0];
for (char i = 0; i <= TRN / 32; i++) {
if(lim==0){break;}     
for (int j = 0; j < 32; j++) {
lim--;
if(lim==0){break;}

int m = (int)(3ll & (movei[(int)i] >> (2 * j)));

int dir=-1;

if(m==0){dir=(int)zero_pos-1;}
else if(m==1){dir=(int)zero_pos-COL;}
else if(m==2){dir=(int)zero_pos+COL;}
else if(m==3){dir=(int)zero_pos+1;}

int zero_row = ((int)zero_pos) / COL;
int zero_col = ((int)zero_pos) % COL;
int move_row = dir / COL;
int move_col = dir % COL;

ans+=to_string((int)board[move_row][move_col])+",";

char tmp=board[zero_row][zero_col];
board[zero_row][zero_col]=board[move_row][move_col];
board[move_row][move_col]=tmp;

pos[0] = (char)dir;
pos[(int)number[dir]] = zero_pos;

tmp=number[dir];
number[dir]=number[(int)zero_pos];
number[(int)zero_pos]=tmp;

zero_pos = (char)dir;
    
}
}
return ans;
}

int BEAM_SEARCH(ll goalboard,ll bboard,char board[ROW][COL],char pos[ROW*COL],int TURN) {
if(MH_EV(goalboard,bboard,pos)==0){return 0;}
if(shot%1000==0){printf("shot=%d\n",shot);}
shot++;     
vector<node>dque;
node n0;
n0.hash=calc_hash(board);
for(int i=0;i<=(TRN/32);i++){
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
char temp_pos[ROW*COL];    
ll tmp_bboard=bboard;
memcpy(temp_board, board, sizeof(temp_board));   
char zero_pos=operation(temp_board, temp.ans,temp_pos,&tmp_bboard,i+1);  
for (int j = 0; j < 4; j++) {
node cand = temp;
int xxx=((int)zero_pos)%COL;
int yyy=((int)zero_pos)/COL;
if (0 <= xxx + dx[j] && xxx + dx[j] < COL &&
0 <= yyy + dy[j] && yyy + dy[j] < ROW && cand.prev+j!=3) {
ll board2=tmp_bboard;  
int ny=yyy + dy[j];
int nx=xxx + dx[j];
int shiftA =((yyy*COL)+xxx)*4;
int shiftB=((ny*COL)+nx)*4;    
int nA = (int)((board2 >> shiftA) & 0xFULL);
int nB = (int)((board2 >> shiftB) & 0xFULL);  
cand.hash^=(zoblish_field[yyy][xxx][nA])^(zoblish_field[ny][nx][nB]);
cand.hash^=(zoblish_field[yyy][xxx][nB])^(zoblish_field[ny][nx][nA]);
cand.ans[i/32] |= (((ll)(j))<<((2*i)%64));
exchange_ll((yyy*COL)+xxx,(ny*COL)+nx,&board2);
char pos2[ROW*COL];
memcpy(pos2,temp_pos,sizeof(pos2));    
char abb=pos2[nA];
pos2[nA]=pos2[nB];
pos2[nB]=abb;    
cand.score=MH_EV(goalboard,board2,pos2);   
cand.prev=j;
fff[(4 * k) + j] = cand;
}
else{
cand.score=200;
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
if(fff[j].score<200){
vv.push_back(make_pair((int)fff[j].score,j));
}
}
sort(vv.begin(),vv.end());
int push_node=0;
for (int j = 0; push_node < BW ;j++) {  
if(j>=(int)vv.size()){break;}
int p=vv[j].second;
node n1=fff[p];
if (i < TURN - 1) {
if(!visited[n1.hash]){
visited[n1.hash]=true;
dque.push_back(n1);
push_node++;
}
}
}
}
return 200;
}

int BEAM_SEARCH2(ll goalboard,ll bboard,char board[ROW][COL],char pos[ROW*COL],int TURN) {
if(MH_EV(goalboard,bboard,pos)==0){return 0;}	
int LIM=TURN;
vector<int>pro_league;
vector<node>dque;
node n0;
n0.hash=calc_hash(board);
for(int i=0;i<=(TRN/32);i++){
n0.ans[i]=0ll;
}
n0.prev=-1;    
dque.push_back(n0);
int dx[4] = { -1, 0,0,1 };
int dy[4] = { 0,-1,1,0 };
emilib::HashMap<ll, bool> visited;
for (int i = 0; i < TURN; i++) {
int ks = (int)dque.size();
pro_league.clear();	
//#pragma omp parallel for
for (int k = 0; k < ks; k++) {
node temp = dque[k];
char temp_board[ROW][COL];
char temp_pos[ROW*COL];    
ll tmp_bboard=bboard;
memcpy(temp_board, board, sizeof(temp_board));   
char zero_pos=operation(temp_board, temp.ans,temp_pos,&tmp_bboard,i+1);  
for (int j = 0; j < 4; j++) {
node cand = temp;
int xxx=((int)zero_pos)%COL;
int yyy=((int)zero_pos)/COL;
if (0 <= xxx + dx[j] && xxx + dx[j] < COL &&
0 <= yyy + dy[j] && yyy + dy[j] < ROW && cand.prev+j!=3) {
ll board2=tmp_bboard;
char board3[ROW][COL];
memcpy(board3,temp_board,sizeof(board3));    
int ny=yyy + dy[j];
int nx=xxx + dx[j];
int shiftA =((yyy*COL)+xxx)*4;
int shiftB=((ny*COL)+nx)*4;    
int nA = (int)((board2 >> shiftA) & 0xFULL);
int nB = (int)((board2 >> shiftB) & 0xFULL);  
cand.hash^=(zoblish_field[yyy][xxx][nA])^(zoblish_field[ny][nx][nB]);
cand.hash^=(zoblish_field[yyy][xxx][nB])^(zoblish_field[ny][nx][nA]);
cand.ans[i/32] |= (((ll)(j))<<((2*i)%64));
exchange_ll((yyy*COL)+xxx,(ny*COL)+nx,&board2);
char pos2[ROW*COL];
memcpy(pos2,temp_pos,sizeof(pos2));    
char abb=pos2[nA];
pos2[nA]=pos2[nB];
pos2[nB]=abb;
char abd=board3[yyy][xxx];
board3[yyy][xxx]=board3[ny][nx];
board3[ny][nx]=abd;    
int lim=LIM;
if((int)pro_league.size()>=BW2){lim=pro_league[BW2-1];LIM=lim;}
cand.score=(uc)BEAM_SEARCH(goalboard,board2,board3,pos2,lim-1);
if(MH_EV(goalboard,board2,pos2)==0){
    bestans=getans(board,cand.ans,i+2);
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
cand.score=200;
ggg[(4 * k) + j] = cand;
}
}
}
dque.clear();
vector<pair<int,int> >vv;
for (int j = 0; j < 4 * ks; j++) {
if(ggg[j].score<200){
vv.push_back(make_pair((int)ggg[j].score,j));
}
}
if((int)vv.size()==0){
    cout<<"impossible"<<endl;
    exit(0);
}	
sort(vv.begin(),vv.end());
printf("depth=%d/%d,score=%d\n",i+1,TURN,vv[0].first);    
int push_node=0;
for (int j = 0; push_node < BW2 ;j++) {  
if(j>=(int)vv.size()){break;}
int p=vv[j].second;
node n1=ggg[p];
if (i < TURN - 1) {
if(!visited[n1.hash]){
visited[n1.hash]=true;
dque.push_back(n1);
push_node++;
}
}
}
}
return 200;
}

bool judge(char board[ROW][COL],vector<int>ck){

    char goalboard[ROW][COL];

    for(int i=1;i<=ROW*COL;i++){
        goalboard[(i-1)/COL][(i-1)%COL]=(char)(i%(ROW*COL));
    }

    int zero_pos;

    for(int i=0;i<ROW*COL;i++){
        if(board[i/COL][i%COL]==0){zero_pos=i;break;}
    }

    for(int i=0;i<(int)ck.size();i++){
        int pos=ck[i];
        for(int j=0;j<ROW*COL;j++){
            if(board[j/COL][j%COL]==pos){
                if((j==zero_pos-1)||(j==zero_pos+1)||(j==zero_pos+COL)||(j==zero_pos-COL));
                else{printf("move_error!\n");return false;}
				char tmp=board[j/COL][j%COL];
				board[j/COL][j%COL]=board[zero_pos/COL][zero_pos%COL];
				board[zero_pos/COL][zero_pos%COL]=tmp;
                zero_pos=j;
                break;
            }
        }
    }

    for(int i=0;i<ROW*COL;i++){
        if(board[i/COL][i%COL]!=goalboard[i/COL][i%COL]){printf("board_error!\n");return false;}
    }
    printf("state:AC\n");
    return true;
}

int main() {    
    
char board[ROW][COL];
string a[ROW][COL];
char pos[ROW*COL];
char init_board[ROW][COL];

for(int i=0; i<ROW; i++){
for(int j=0; j<COL; j++) {
cin >> a[i][j];
if(a[i][j]=="*"){board[i][j]=0;}
else{board[i][j]=stoi(a[i][j]);}
pos[(int)board[i][j]]=(i*COL)+j;    
}
}

auto start = chrono::high_resolution_clock::now();

memcpy(init_board,board,sizeof(board));

int i1, i2, i3;
for(i1=0;i1<ROW;++i1){
for(i2=0;i2<COL;++i2){
for(i3=0;i3<ROW*COL;i3++){
zoblish_field[i1][i2][i3]=xor128();
}
}
}

ll goalboard=0ll;
ll bboard=0ll;    

for(int i=1;i<=ROW*COL;i++){    
goalboard |= ((ll)(i % (ROW * COL))) << (4 * (i - 1));
bboard |= ((ll)board[(i - 1) / COL][(i - 1) % COL]) << (4 * (i - 1));
}

memset(k5p, -1, sizeof(k5p));

//{1,2,5,9,13},{3,4,6,7,8},{10,11,12,14,15}

int key[3][5]={
    {1,2,5,9,13},
    {3,4,6,7,8},
    {10,11,12,14,15}
};

#pragma omp parallel for
for(int i=0;i<3;i++){    
bfs(key[i][0],key[i][1],key[i][2],key[i][3],key[i][4],i);
}
    

printf("path_length:%d\n",BEAM_SEARCH2(goalboard,bboard,board,pos,TRN));

auto end = chrono::high_resolution_clock::now();
auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

cout << "calc_time: " << duration/1000.0 << "s" << endl;

vector<int>ck;

if (!bestans.empty()) {
bestans.pop_back();
}

string ddd="";

for(int i=0;i<(int)bestans.size();i++){
if(bestans[i]==','){
    ck.push_back(stoi(ddd));
    ddd="";
    continue;
}
ddd+=bestans[i];
}
if((int)ddd.size()>0){
ck.push_back(stoi(ddd));
}

if(!judge(init_board,ck)){
return 0;    
}

cout<<"route:{"<<bestans<<"}"<<endl;    

return 0;
}
