#include "solver.h"
#include "API.h"
#include "stdlib.h"


#define MAX_SIZE 512
#define MAZE_SIZE 16

int hori_wall[MAZE_SIZE][MAZE_SIZE]={0};
int vert_wall[MAZE_SIZE][MAZE_SIZE]={0};
int man_dist[MAZE_SIZE][MAZE_SIZE];

int r=0;

struct position{
  int x;
  int y;
  char orient;
};

struct position current={0,0,'n'};

struct element{
  int x;
  int y;
  int value;
};

int man_n=0;
int man_w=0;
int man_e=0;
int man_s=0;

struct element queue[MAX_SIZE];
int front = -1, rear = -1;

int isEmpty() {
    return front == -1;
}

int isFull() {
  return (rear + 1) % MAX_SIZE == front;
}

void enqueue(int data1,int data2,int value) {
    if (isFull()) {
        return;
    }
    if (front == -1) {
        front = 0;
    }
    rear = (rear + 1) % MAX_SIZE;
    queue[rear].x = data1;
    queue[rear].y = data2;
    queue[rear].value=value;
}

struct element dequeue() {
    struct element data = queue[front];
    if (front == rear) {
        front = rear = -1;
    } else {
        front = (front + 1) % MAX_SIZE;
    }
    return data;
}

void Fill_Vert(){
  for (int j = 0; j < MAZE_SIZE; j++){
    vert_wall[MAZE_SIZE-1][j]=1;
  }
}

void Fill_Hori(){
  for (int j = 0; j < MAZE_SIZE; j++){
    hori_wall[j][MAZE_SIZE-1]=1;
  }
}

void FillMan(int arr[MAZE_SIZE][MAZE_SIZE]){
  if(r == 0){
    enqueue(MAZE_SIZE/2 - 1, MAZE_SIZE/2 - 1, 0);
    enqueue(MAZE_SIZE/2 - 1, MAZE_SIZE/2, 0);
    enqueue(MAZE_SIZE/2, MAZE_SIZE/2 - 1, 0);
    enqueue(MAZE_SIZE/2, MAZE_SIZE/2, 0);
  }
  else{
    enqueue(0,0,0);
  }
  for (int i = 0; i < MAZE_SIZE; i++){
    for (int j = 0; j < MAZE_SIZE; j++){
      if(r==0){
        if ((i == MAZE_SIZE/2 && j == MAZE_SIZE/2) || (i == MAZE_SIZE/2 - 1 && j == MAZE_SIZE/2) || (i == MAZE_SIZE/2 && j == MAZE_SIZE/2 - 1) || (i == MAZE_SIZE/2 - 1 && j == MAZE_SIZE/2 - 1)){
          arr[i][j] = 0;
        }
        else{
          arr[i][j] = MAZE_SIZE * MAZE_SIZE;
        }
      }
      else{
        if (i == 0 && j == 0){
          arr[i][j] = 0;
        }
        else{
          arr[i][j] = MAZE_SIZE * MAZE_SIZE;
        }
      }
    }
  }
}

void DispMan(){
  for (int i = 0; i < MAZE_SIZE; i++){
    for (int j = 0; j < MAZE_SIZE; j++){
      char buffer[4];
      itoa(man_dist[i][j], buffer, 10);
      API_setText(i, j, buffer);
    }
  }
}

void CalcMan(){

  FillMan(man_dist);
  while (!isEmpty()){
    struct element temp = dequeue();
    if (vert_wall[temp.x][temp.y] == 0){ // check walls
      if (man_dist[(temp.x) + 1][temp.y] == MAZE_SIZE * MAZE_SIZE){
        man_dist[(temp.x) + 1][temp.y] = (temp.value) + 1;
        enqueue((temp.x) + 1, temp.y, (temp.value) + 1);
      }
    }

    if (vert_wall[(temp.x) - 1][temp.y] == 0){ // check walls
      if (man_dist[(temp.x) - 1][temp.y] == MAZE_SIZE * MAZE_SIZE){
        man_dist[(temp.x) - 1][temp.y] = (temp.value) + 1;
        enqueue((temp.x - 1), temp.y, (temp.value) + 1);
      }
    }

    if (hori_wall[temp.x][temp.y] == 0){ // check walls
      if (man_dist[temp.x][(temp.y) + 1] == MAZE_SIZE * MAZE_SIZE){
        man_dist[temp.x][(temp.y) + 1] = (temp.value) + 1;
        enqueue(temp.x, (temp.y) + 1, (temp.value) + 1);
      }
    }

    if (hori_wall[temp.x][(temp.y) - 1] == 0){ // check walls
      if (man_dist[(temp.x)][(temp.y) - 1] == MAZE_SIZE * MAZE_SIZE){
        man_dist[(temp.x)][(temp.y) - 1] = (temp.value) + 1;
        enqueue(temp.x, (temp.y) - 1, (temp.value) + 1);
      }
    }
  }
}

void Setwalls(){
    for(int i=0;i<MAZE_SIZE;i++){
      API_setWall(0,i,'w');
      API_setWall(i,0,'s');
      API_setWall(i,MAZE_SIZE-1,'n');
      API_setWall(MAZE_SIZE-1,i,'e');
      }

    for(int i=0;i<MAZE_SIZE;i++){//setting vertical
      for(int j=0;j<MAZE_SIZE;j++){
        if(vert_wall[i][j]==1){
          API_setWall(i,j,'e');
         }
       }
    }

    for(int i=0;i<MAZE_SIZE;i++){//setting horizontal
      for(int j=0;j<MAZE_SIZE;j++){
        if(hori_wall[i][j]==1){
          API_setWall(i,j,'n');
         }
       }
    }
}

Heading ret_dir(){
  if(current.y==MAZE_SIZE-1||hori_wall[current.x][current.y]==1){
    man_n=MAZE_SIZE*MAZE_SIZE;
  }
  else{
    man_n=man_dist[current.x][current.y+1];
  }

  if(current.y==0||hori_wall[current.x][current.y-1]==1){
    man_s=MAZE_SIZE*MAZE_SIZE;
  }
  else{
    man_s=man_dist[current.x][current.y-1];
  }

  if(current.x==0||vert_wall[current.x-1][current.y]==1){
    man_w=MAZE_SIZE*MAZE_SIZE;
  }
  else{
    man_w=man_dist[current.x-1][current.y];
  }

  if(current.x==MAZE_SIZE-1||vert_wall[current.x][current.y]==1){
    man_e=MAZE_SIZE*MAZE_SIZE;
  }
  else{
    man_e=man_dist[current.x+1][current.y];
  }
      if(man_n<=man_w&&man_n<=man_e&&man_n<=man_s){
        debug_log("NORTH");
        return NORTH;
      }
      if(man_e<=man_s&&man_e<=man_w&&man_e<=man_n){
        debug_log("EAST");
        return EAST;
      }
      if(man_s<=man_n&&man_s<=man_w&&man_s<=man_e){
        debug_log("SOUTH");
        return SOUTH;
      }
    else{
      debug_log("WEST");
      return WEST;
    }
}

/*void changeTarget(){

}*/

Action retAck(Heading dir){
  if((current.x == MAZE_SIZE/2 - 1 || current.x == MAZE_SIZE/2) && (current.y == MAZE_SIZE/2 - 1 || current.y == MAZE_SIZE/2)){
    if(r == 0){
      debug_log(&current.orient);
      r=1;
      if(current.orient == 's'){
        current.orient = 'w';
      }
      else if(current.orient == 'n'){
        current.orient = 'e';
      }
      else if(current.orient == 'w'){
        current.orient = 'n';
      }
      else{
        current.orient = 's';
      }
      debug_log(&current.orient);
      return RIGHT;
    }
  }

  if(current.x == 0 && current.y == 0){
    if(r == 1){
      r=0;
      if(current.orient == 's'){
        current.orient = 'w';
      }
      else if(current.orient == 'n'){
        current.orient = 'e';
      }
      else if(current.orient == 'w'){
        current.orient = 'n';
      }
      else{
        current.orient = 's';
      }
      debug_log(&current.orient);
      return RIGHT;
    }
  }
  if(dir==NORTH){
    if(current.orient=='n'){
      current.y=current.y+1;
      return FORWARD;
    }
    else if(current.orient=='e'){
      current.orient='n';
      return LEFT;
    }
    else if(current.orient=='w'){
      current.orient='n';
      return RIGHT;
    }
    else{
      current.orient='w';
      return RIGHT;
    }
  }
  else if(dir==WEST){
    if(current.orient=='n'){
      current.orient='w';
      return LEFT;
    }
    else if(current.orient=='e'){
      current.orient='n';
      return LEFT;
    }
    else if(current.orient=='w'){
      current.x=current.x-1;
      return FORWARD;
    }
    else{
      current.orient='w';
      return RIGHT;
    }
  }
  else if(dir==EAST){
    if(current.orient=='n'){
      current.orient='e';
      return RIGHT;
    }
    else if(current.orient=='e'){
      current.x=current.x+1;
      return FORWARD;
    }
    else if(current.orient=='w'){
      current.orient='n';
      return RIGHT;
    }
    else{
      current.orient='e';
      return LEFT;
    }
  }
  else{
    if(current.orient=='n'){
      current.orient='e';

      return RIGHT;
    }
    else if(current.orient=='e'){
      current.orient='s';
      return RIGHT;
    }
    else if(current.orient=='w'){
      current.orient='s';
      return LEFT;
    }
    else{
      current.y=current.y-1;
      return FORWARD;
    }
  }
}

void updateWalls(){
  if(API_wallFront()){
    debug_log("F");
    switch (current.orient){
      case 'n':
        if(current.y!=MAZE_SIZE-1){
          hori_wall[current.x][current.y]=1;
        }
        break;

      case 'e':
        if(current.x!=MAZE_SIZE-1){
          vert_wall[current.x][current.y]=1;
        }
        break;

      case 's':
        if(current.y!=0){
          hori_wall[current.x][current.y-1]=1;
        }
        break;

      case 'w':
        if(current.x!=0){
          vert_wall[current.x-1][current.y]=1;
        }
        break;
    }
  }
  if(API_wallLeft()){
    debug_log("L");
    switch (current.orient){
      case 'n':
        if(current.x!=0){
          vert_wall[current.x-1][current.y]=1;
        }
        break;

      case 'e':
        if(current.y!=MAZE_SIZE-1){
          hori_wall[current.x][current.y]=1;
        }
        break;

      case 's':
        if(current.x!=MAZE_SIZE-1){
          vert_wall[current.x][current.y]=1;
        }
        break;


      case 'w':
        if(current.y!=0){
          hori_wall[current.x][current.y-1]=1;
        }
        break;
    }
  }
  if(API_wallRight()){
    debug_log("R");
    switch (current.orient){
      case 'n':
        if(current.x!=MAZE_SIZE-1){
          vert_wall[current.x][current.y]=1;
        }
        break;

      case 'e':
        if(current.y!=0){
          hori_wall[current.x][current.y-1]=1;
        }
        break;

      case 's':
        if(current.x!=0){
          vert_wall[current.x-1][current.y]=1;
        }
        break;

      case 'w':
        if(current.y!=MAZE_SIZE-1){
          hori_wall[current.x][current.y]=1;
        }
        break;
    }
  }
}

Action solver() {
    return floodFill();
}

// Put your implementation of floodfill here!
Action floodFill() {
    Fill_Hori();
    Fill_Vert();
    updateWalls();
    Setwalls();
    CalcMan();
    DispMan();
    Heading dir=ret_dir();
    return retAck(dir);
}
