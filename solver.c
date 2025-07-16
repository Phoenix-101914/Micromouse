#include "solver.h"
#include "API.h"
#include "stdlib.h"


#define MAX_SIZE 512
#define MAZE_SIZE 16

int hori_wall[MAZE_SIZE][MAZE_SIZE]={0};
int vert_wall[MAZE_SIZE][MAZE_SIZE]={0};
int man_dist[MAZE_SIZE][MAZE_SIZE];
int n=0;
Action return_arr[MAZE_SIZE*MAZE_SIZE];
int return_size=0;
int s=0;
int r=0;
int s2=0;

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
  for (int i = 0; i < MAZE_SIZE; i++){
    for (int j = 0; j < MAZE_SIZE; j++){
      if (i==MAZE_SIZE-1){
        vert_wall[i][j]=1;
      }
      
    }
  }
}

void Fill_Hori(){
  for (int i = 0; i < MAZE_SIZE; i++){
    for (int j = 0; j < MAZE_SIZE; j++){
      if (j==MAZE_SIZE-1){
        hori_wall[i][j]=1;
      }
      
    }
  }
}

void FillMan(int arr[MAZE_SIZE][MAZE_SIZE]){
    for(int i=0;i<MAZE_SIZE;i++){
        for(int j=0;j<MAZE_SIZE;j++){
          if((i==8&&j==8)||(i==7&&j==8)||(i==8&&j==7)||(i==7&&j==7)){
            arr[i][j]=0;
          }
          else{
          arr[i][j]=MAZE_SIZE*MAZE_SIZE;
          }
        }
      }
  }

void DispMan(){
    for(int i=0;i<MAZE_SIZE;i++){
        for(int j=0;j<MAZE_SIZE;j++){
            char buffer[4];
            itoa(man_dist[i][j],buffer,10);
          API_setText(i,j,buffer);
        }
      }
}

void CalcMan(){
  FillMan(man_dist);
  enqueue(7,7,0);
  enqueue(7,8,0);
  enqueue(8,7,0);
  enqueue(8,8,0);

  while (!isEmpty()){
    struct element temp=dequeue();
      if(vert_wall[temp.x][temp.y]==0){//check walls
        if (man_dist[(temp.x)+1][temp.y]==256)
        {
          man_dist[(temp.x)+1][temp.y]=(temp.value)+1;
          enqueue((temp.x)+1,temp.y,(temp.value)+1);
        }
      }

      if(vert_wall[(temp.x)-1][temp.y]==0){//check walls
        if (man_dist[(temp.x)-1][temp.y]==256)
        {
          man_dist[(temp.x)-1][temp.y]=(temp.value)+1;
          enqueue((temp.x-1),temp.y,(temp.value)+1);
        }
      }

      if(hori_wall[temp.x][temp.y]==0){//check walls
        if (man_dist[temp.x][(temp.y)+1]==256)
        {
          man_dist[temp.x][(temp.y)+1]=(temp.value)+1;
          enqueue(temp.x,(temp.y)+1,(temp.value)+1);
        }
      }

      if(hori_wall[temp.x][(temp.y)-1]==0){//check walls
        if (man_dist[(temp.x)][(temp.y)-1]==256)
        {
          man_dist[(temp.x)][(temp.y)-1]=(temp.value)+1;
          enqueue(temp.x,(temp.y)-1,(temp.value)+1);
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
  debug_log("Heading");
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

Action back_to_start(){
  char buffer1[3];
  char buffer2[3];
  itoa(current.x,buffer1,10);
  itoa(current.y,buffer2,10);
  debug_log(buffer1);
  debug_log(buffer2);
  if(current.y==0&&current.x==0){
    debug_log("Reached");
    r=0;
    current.orient='w';
    return RIGHT;
  }

  else if(man_dist[current.x][current.y]==0){
    if(s<2){
      if(s==0){
        return_size--;
      }
      s++;
      current.orient='n';
      return RIGHT;
    }
    else{
      current.y=current.y+1;
      return FORWARD;
    }
  }
  else if(return_arr[return_size-1]==FORWARD){
    return_size--;
    switch (current.orient){
      case 'n':
        current.y++;
        break;

      case 'e':
        current.x++;
        break;
      
      case 's':
        current.y--;
        break;
      
      case 'w':
        current.x--;
        break;
    }
    debug_log("FORWARD");
    return FORWARD;
  }
  else if(return_arr[return_size-1]==RIGHT){
    return_size--;
    switch (current.orient){
      case 'n':
        current.orient='w';
        break;

      case 'e':
        current.orient='n';
        break;
      
      case 's':
        current.orient='e';
        break;
      
      case 'w':
        current.orient='s';
        break;
    }
    debug_log("LEFT");
    return LEFT;
  }
  else if(return_arr[return_size-1]==LEFT){
    return_size--;
    switch (current.orient){
      case 'n':
        current.orient='e';
        break;

      case 'e':
        current.orient='s';
        break;
      
      case 's':
        current.orient='w';
        break;
      
      case 'w':
        current.orient='n';
        break;
    }
    debug_log("RIGHT");
    return RIGHT;
  }
}

Action retAck(Heading dir){
  if(man_dist[current.x][current.y]==0){
    r=1;
    return back_to_start();
  }
  if(dir==NORTH){
    if(current.orient=='n'){
      current.y=current.y+1;
      return_arr[return_size]=FORWARD;
      return_size++;
      return FORWARD;
    }
    else if(current.orient=='e'){
      current.orient='n';
      return_arr[return_size]=LEFT;
      return_size++;
      return LEFT;
    }
    else if(current.orient=='w'){
      current.orient='n';
      return_arr[return_size]=RIGHT;
      return_size++;
      return RIGHT;
    }
    else{
      current.orient='w';
      return_arr[return_size]=RIGHT;
      return_size++;
      return RIGHT;
    }
  }
  else if(dir==WEST){
    if(current.orient=='n'){
      current.orient='w';
      return_arr[return_size]=LEFT;
      return_size++;
      return LEFT;
    }
    else if(current.orient=='e'){
      current.orient='n';
      return_arr[return_size]=LEFT;
      return_size++;
      return LEFT;
    }
    else if(current.orient=='w'){
      current.x=current.x-1;
      return_arr[return_size]=FORWARD;
      return_size++;
      return FORWARD;
    }
    else{
      current.orient='w';
      return_arr[return_size]=RIGHT;
      return_size++;
      return RIGHT;
    }
  }
  else if(dir==EAST){
    if(current.orient=='n'){
      current.orient='e';
      return_arr[return_size]=RIGHT;
      return_size++;
      return RIGHT;
    }
    else if(current.orient=='e'){
      current.x=current.x+1;
      return_arr[return_size]=FORWARD;
      return_size++;
      return FORWARD;
    }
    else if(current.orient=='w'){
      current.orient='n';
      return_arr[return_size]=RIGHT;
      return_size++;
      return RIGHT;
    }
    else{
      current.orient='e';
      return_arr[return_size]=LEFT;
      return_size++;
      return LEFT;
    }
  }
  else{
    if(current.orient=='n'){
      current.orient='e';
      return_arr[return_size]=RIGHT;
      return_size++;
      return RIGHT;
    }
    else if(current.orient=='e'){
      current.orient='s';
      return_arr[return_size]=RIGHT;
      return_size++;
      return RIGHT;
    }
    else if(current.orient=='w'){
      current.orient='s';
      return_arr[return_size]=LEFT;
      return_size++;
      return LEFT;
    }
    else{
      current.y=current.y-1;
      return_arr[return_size]=FORWARD;
      return_size++;
      return FORWARD;
    }
  }
  return IDLE;
}

void updateWalls(){
  if(API_wallFront()){
    debug_log("F");
    switch (current.orient){
      case 'n':
        if(current.y!=MAZE_SIZE-1){
          hori_wall[current.x][current.y]=1;
          break;
        }
        else{
          break;
        }

      case 'e':
        if(current.x!=MAZE_SIZE-1){
          vert_wall[current.x][current.y]=1;
          break;
        }
        else{
          break;
        }

      case 's':
        if(current.y!=0){
          hori_wall[current.x][current.y-1]=1;
          break;
        }
        else{
          break;
        }

      case 'w':
        if(current.x!=0){
          vert_wall[current.x-1][current.y]=1;
          break;
        }
        else{
          break;
        }
    }
  }
  if(API_wallLeft()){
    debug_log("L");
    switch (current.orient){
      case 'n':
        if(current.x!=0){
          vert_wall[current.x-1][current.y]=1;
          break;
        }
        else{
          break;
        }

      case 'e':
        if(current.y!=MAZE_SIZE-1){
          hori_wall[current.x][current.y]=1;
          break;
        }
        else{
          break;
        }

      case 's':
        if(current.x!=MAZE_SIZE-1){
          vert_wall[current.x][current.y]=1;
          break;
        }
        else{
          break;
        }

      case 'w':
        if(current.y!=0){
          hori_wall[current.x][current.y-1]=1;
          break;
        }
        else{
          break;
        }
    }
  }
  if(API_wallRight()){
    debug_log("R");
    switch (current.orient){
      case 'n':
        if(current.x!=MAZE_SIZE-1){
          vert_wall[current.x][current.y]=1;
          break;
        }
        else{
          break;
        }

      case 'e':
        if(current.y!=0){
          hori_wall[current.x][current.y-1]=1;
          break;
        }
        else{
          break;
        }

      case 's':
        if(current.x!=0){
          vert_wall[current.x-1][current.y]=1;
          break;
        }
        else{
          break;
        }

      case 'w':
        if(current.y!=MAZE_SIZE-1){
          hori_wall[current.x][current.y]=1;
          break;
        }
        else{
          break;
        }
    }
  }
}

Action solver() {
    return floodFill();
}

// Put your implementation of floodfill here!
Action floodFill() {
  if(n==0){
    n=1;
    Fill_Hori();
    Fill_Vert();
  }
  if(!r){
    updateWalls();
    Setwalls();
    CalcMan();
    DispMan();
    Heading dir=ret_dir();
    return retAck(dir);
  }
  else{
    return back_to_start();
  }
}
