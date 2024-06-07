#include <graphics.h>
#include <easyx.h>
#include <winuser.h>
#include<mmsystem.h>//包含多媒体设备接口头文件
#include<windows.h>
#include<istream>//txt
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;//txt

void start_cover();//开始界面
void game_part();//游戏主程序部分
void mousemeg(int* x, int* y);//读取鼠标点击的位置并赋值给x,y
int mousxy(int* x,int* y);//标准化x,y
int mousy(int y);//对y的判断
int check_map();//检查是否游戏结束
void list_of_makers();//开发者名单
int fileinput(char sen[]);//txt
void result(char sen[],int outcome, int n);//将结果存入data.txt
void recordings();//显示链表内容
struct node* add_to_record(struct node* list, int n, int steps);//链表加长函数
void record(int outcome, int steps);//将结果存入链表
//void ai_game();//ai游戏模式
//void ai(int *x, int *y);//ai下子选择
//int outcome_ai();

#pragma comment(lib,"winmm.lib")//音乐
void bgm();

int game_condition = 1;//是否重复游戏了

ExMessage m;//消息处理
int map[17][17] = {0};//初始化棋盘
int vectors[8][2] = {{1,0},{-1,0},{1,1},{-1,1},{0,1},
                     {1,-1},{0,-1},{-1,-1}};//辅助数列

struct node{
    int outcome;
    int steps;
    char time[30];
    struct node *next;
};
struct node *first = NULL;

int main()
{
	bgm();
    start_cover();//开始界面
    while(game_condition == 0){
        start_cover();
    }
    closegraph();
    return 0;
}

/*start_cover();//开始界面
    while(game_condition == 0){
        start_cover();
    }
    closegraph();
    return 0;*/

void start_cover(){
    initgraph(800,640);
    fillrectangle(300,350,500,400);
    setlinecolor(0);
    settextstyle(40, 20, _T("Consolas"));
    outtextxy(350,350,"Start");
    setlinecolor(0xFFFFFF);
    fillrectangle(300,400,500,450);
    setlinecolor(0);
    outtextxy(350,400,"Record");
    setlinecolor(0xFFFFFF);
    fillrectangle(300,450,500,500);
    setlinecolor(0);
    outtextxy(350,450,"Makers");
    setlinecolor(0xFFFFFF);
    fillrectangle(300,500,500,550);
    setlinecolor(0);
    outtextxy(350,500,"Quit");
    setlinecolor(0xFFFFFF);
    settextstyle(20, 10, _T("Consolas"));
    outtextxy(0,0,"Enter F1 to start");
    outtextxy(0,20,"Enter F2 to the temporary recordings");
    outtextxy(0,40,"Enter F3 to check the list of makers");
    outtextxy(0,80,"Enter F12 to quit");
    outtextxy(0,60,"Enter F4 to play with ai(Not finished!Enter to continue.)");
    outtextxy(0,100,R"(Enter .\data to the permanent recordings)");
    outtextxy(0,120,"Don't click your mouse too fast.");
    outtextxy(0,140,"Otherwise, the program will break down!");

    
    IMAGE img1;
    loadimage(&img1, _T("cover.jpg"),200,180);
    putimage(300,170,&img1);
    IMAGE img2;
    loadimage(&img2, _T("left.jpg"),300,640-170);
    putimage(0,170,&img2);
    IMAGE img3;
    loadimage(&img3, _T("right.jpg"),300,640-170);
    putimage(500,170,&img3);
    

    flushmessage();
    while(1){
        m = getmessage(EX_KEY);
        if (m.vkcode == 0x7B){
            game_condition = 1;
            return;
        } else if (m.vkcode == 0x70){//按F1开始游戏
            game_part();//开始游戏
            return;
        } else if (m.vkcode == 0x71){
            recordings();
            return;
        } else if (m.vkcode == 0x72){
            list_of_makers();
            return;
        } else if (m.vkcode == 0x73){
            game_condition = 0;
            return;
        }
    }
}

void game_part(){
    //初始化棋盘
    for (int i = 0; i < 17; i++){
        for (int j = 0; j < 17; j++){
            map[i][j] = 0;
        }
    }
    if (game_condition == 0){
        cleardevice();
        game_condition = 1;
    }
    initgraph(640,640);
    setfillcolor(RGB(249,200,91));
    fillrectangle(0,0,640,640);
    setlinecolor(0);
    for (int i = 40; i < 640; i += 40){
        line(40,i,600,i);
    }
    for (int i = 40; i < 640; i+= 40){
        line(i,40,i,600);
    }
    line(35,35,35,605);
    line(35,35,605,35);
    line(605,35,605,605);
    line(35,606,605,605);
    setfillcolor(0);
    fillcircle(40*4,40*4,4);
    fillcircle(40*12,40*4,4);
    fillcircle(40*4,40*12,4);
    fillcircle(40*12,40*12,4);
    fillcircle(40*8,40*8,4);
    int countb = 0;
    int countw = 0;
    //下棋部分
    int outcome = 0;
    int flag = 1;
    while (1){
        int x, y;
        mousemeg(&x,&y);
        if (flag == 1){
            for (int i = 1; i <= 15; i++){
                for (int j = 1; j <= 15; j ++){
                    if (x == i  && y == j){
                        setlinecolor(0);
                        setfillcolor(0);
                        fillcircle(i*40,j*40,10);
                        map[i][j] = 1;
                        countb++;
                    }

                }
            }
            flag = 0;
            outcome = check_map();
        } else {
            for (int i = 1; i <= 15; i++){
                for (int j = 1; j <= 15; j++) {
                    if (x == i && y == j) {
                        setlinecolor(0);
                        setfillcolor(0xFFFFFF);
                        fillcircle(i * 40, j * 40, 10);
                        map[i][j] = 2;
                        countw++;
                    }
                }
            }
            flag = 1;
            outcome = check_map();
        }
        if (outcome == 1){
            record(1,countb);
            cleardevice();
            char sen[50] = {0};
            result(sen,outcome,countb);
            fileinput(sen);
            char black_win[20] = "Black wins!";
            settextstyle(35, 20, _T("Consolas"));
            outtextxy(80,250,black_win);
            char if_return1[30] = "Enter F6 to continue";
            outtextxy(80,320,if_return1);
            char if_return2[20] = "Enter F2 to quit";
            outtextxy(80,380,if_return2);
            flushmessage();
            while (1){
                m = getmessage(EX_KEY);
                if (m.vkcode == 0x75){
                    game_condition = 0;
                    return;
                } else if (m.vkcode == 	0x71){
                    game_condition = 1;
                    return;
                }
            }
        } else if (outcome == 2){
            record(2,countw);
            cleardevice();
            char sen[50] = {0};
            result(sen,outcome,countw);
            fileinput(sen);
            char white_win[20] = "White wins!";
            settextstyle(35, 20, _T("Consolas"));
            outtextxy(80,250,white_win);
            char if_return1[30] = "Enter F7 to continue";
            outtextxy(80,320,if_return1);
            char if_return2[20] = "Enter F2 to quit";
            outtextxy(80,380,if_return2);
            //
            flushmessage();
            while (1){
                m = getmessage(EX_KEY);
                if (m.vkcode == 0x76){
                    game_condition = 0;
                    return;
                } else if (m.vkcode == 0x71){
                    game_condition = 1;
                    return;
                }
            }
        } else if (outcome == 3){
            record(3,0);
            cleardevice();
            char sen[50] = {0};
            result(sen,outcome,countb);
            fileinput(sen);
            char black_win[20] = "Draw!";
            settextstyle(35, 20, _T("Consolas"));
            outtextxy(80,250,black_win);
            char if_return1[30] = "Enter F6 to continue";
            outtextxy(80,320,if_return1);
            char if_return2[20] = "Enter F2 to quit";
            outtextxy(80,380,if_return2);
            flushmessage();
            while (1){
                m = getmessage(EX_KEY);
                if (m.vkcode == 0x75){
                    game_condition = 0;
                    return;
                } else if (m.vkcode == 	0x71){
                    game_condition = 1;
                    return;
                }
            }
        }
    }
}

void mousemeg(int* x, int* y){
    while (true) {
        // 判断鼠标是否点击
        if (MouseHit()) {
            // 获取鼠标消息
            MOUSEMSG msg = GetMouseMsg();
            // 判断是否为左键按下事件
            if (msg.uMsg == WM_LBUTTONDOWN) {
                // 获取鼠标点击坐标
                *x = msg.x;
                *y = msg.y;
                if (mousxy(x,y) == 1 && map[*x][*y] == 0){
                    break;
                }
            }
        }
    }
    return;
}

int mousxy(int* x, int* y){
    int result = 0;
    for (int i = 40; i < 800; i += 40){
        if (*x >= i - 20 && *x <= i + 20){
            *x = i / 40;
            *y = mousy(*y);
            result = 1;
        }
    }
    return result;
}

int mousy(int y){
    for (int i = 40; i < 640; i+= 40){
        if (y >= i - 20 && y <= i + 20){
            return i / 40;
        }
    }
    return 0;
}

int check_map(){
    for (int i = 1; i < 16; i++){
        for (int j = 1; j < 16; j++){
            if (map[i][j] == 1){
                int ti = i;
                int tj = j;
                for (int m = 1; m < 8; m++){
                    int count = 1;
                    while (map[ti + vectors[m][0]][tj + vectors[m][1]] == 1){
                        count++;
                        ti += vectors[m][0];
                        tj += vectors[m][1];
                    }
                    if (count >= 5){
                        return 1;
                    }
                }
            }
        }
    }
    for (int i = 1; i < 16; i++){
        for (int j = 1; j < 16; j++){
            if (map[i][j] == 2){
                int ti = i;
                int tj = j;
                for (int m = 1; m < 8; m++){
                    int count = 1;
                    while (map[ti + vectors[m][0]][tj + vectors[m][1]] == 2){
                        count++;
                        ti += vectors[m][0];
                        tj += vectors[m][1];
                    }
                    if (count >= 5){
                        return 2;
                    }
                }
            }
        }
    }
    int sum = 0;
    for (int i = 1; i < 16; i++){
        for (int j = 1; j < 16; j++){
            if (map[i][j] == 1){
                sum = 1;
            }
        }
    }
    if (sum == 0){
        return 3;
    }
    return 0;
}

void list_of_makers(){
    initgraph(800,640);
    char maker1[30] = "Liang Gong";
    settextstyle(40, 20, _T("Consolas"));
    outtextxy(250,320,maker1);
    char maker2[30] = "Haoran Yuan";
    outtextxy(250,360,maker2);
    char maker3[30] = "Huijie Piao";
    outtextxy(250,400,maker3);
    char maker4[30] = "Miran Ka";
    outtextxy(250,440,maker4);
    char if_return1[21] = "Enter F5 to continue";
    outtextxy(180,0,if_return1);
    char if_return2[20] = "Enter F2 to quit";
    outtextxy(180,40,if_return2);
    flushmessage();
    while (1){
        m = getmessage(EX_KEY);
        if (m.vkcode == 0x74){
            game_condition = 0;
            return;
        } else if (m.vkcode == 	0x71){
            game_condition = 1;
            return;
        }
    }
}
/*
void ai_game() {
    //初始化棋盘
    for (int i = 1; i < 16; i++){
        for (int j = 1; j < 16; j++){
            map[i][j] = 0;
        }
    }
    for (int i = 0 ; i < 17; i++){
        map[0][i] = 3;
        map[16][i] = 3;
    }
    for (int i = 0; i < 17; i++){
        map[i][0] = 3;
        map[i][16] = 3;
    }
    if (game_condition == 0){
        cleardevice();
        game_condition = 1;
    }
    initgraph(640,640);
    setfillcolor(RGB(249,200,91));
    fillrectangle(0,0,640,640);
    setlinecolor(0);
    for (int i = 40; i < 640; i += 40){
        line(20,i,620,i);
    }
    for (int i = 40; i < 640; i+= 40){
        line(i,20,i,620);
    }
    int countb = 0;
    int countw = 0;
    int flag = 1;
    int outcome = 0;
    */
    //游戏部分
    /*setlinecolor(0xFFFFFF);
    setfillcolor(0xFFFFFF);
    fillcircle(10*40,5*40,10);
    map[5][5] = 2;*/
    /*
    while (1){
        int x,y;
        if (flag == 1){
            mousemeg(&x,&y);
            for (int i = 1; i < 16; i++){
                for (int j = 1; j < 16; j ++){
                    if (x == i  && y == j){
                        setlinecolor(0);
                        setfillcolor(0);
                        fillcircle(i*40,j*40,10);
                        map[j][i] = 1;
                        countb++;
                    }
                }
            }
            flag = 0;
            outcome = check_map();
        } else {
            ai(&x,&y);//死循环？
            setlinecolor(0xFFFFFF);
            setfillcolor(0xFFFFFF);
            fillcircle(x*40,y*40,10);
            map[x][y] = 2;
            countw++;
            flag = 1;
            outcome = check_map();
        }
        if (outcome == 1){
            cleardevice();
            char sen[50] = {0};
            result(sen,outcome,countb);
            fileinput(sen);
            char black_win[20] = "The black wins!";
            settextstyle(35, 20, _T("Consolas"));
            outtextxy(80,250,black_win);
            char if_return1[30] = "Enter F5 to continue";
            outtextxy(80,320,if_return1);
            char if_return2[20] = "Enter F2 to quit";
            outtextxy(80,380,if_return2);
            flushmessage();
            while (1){
                m = getmessage(EX_KEY);
                if (m.vkcode == 0x74){
                    game_condition = 0;
                    return;
                } else if (m.vkcode == 	0x71){
                    game_condition = 1;
                    return;
                }
            }
        } else if (outcome == 2){
            cleardevice();
            char sen[50] = {0};
            result(sen,outcome,countw);
            fileinput(sen);
            char white_win[20] = "The white wins!";
            settextstyle(35, 20, _T("Consolas"));
            outtextxy(80,250,white_win);
            char if_return1[30] = "Enter F5 to continue";
            outtextxy(80,320,if_return1);
            char if_return2[20] = "Enter F2 to quit";
            outtextxy(80,380,if_return2);
            //
            flushmessage();
            while (1){
                m = getmessage(EX_KEY);
                if (m.vkcode == 0x74){
                    game_condition = 0;
                    return;
                } else if (m.vkcode == 0x71){
                    game_condition = 1;
                    return;
                }
            }
        }
    }
}

int aix = 5;
int aiy = 5;
void ai(int *x, int *y){
    for (int i = 1; i < 16; i++){
        for (int j = 1; j < 16; j++){
            if (map[i][j] == 0){
                *x = j;
                *y = i;
                return;
            }
        }
    }
}
*/

void bgm()
{
    //打开音乐
    mciSendString("open ./su.mp3", 0, 0, 0);//mci:多媒体设备接口  send：发送	string:字符串
    //播放音乐	repeat：重复播放
    mciSendString("play ./su.mp3", 0, 0, 0);
}


int fileinput(char sen[])
{
    FILE  *p1=fopen("data.txt","a+");
    fprintf(p1,sen);
    fclose(p1);
    return 0;
}

void result(char sen[],int outcome,int n){
    if (outcome == 1) {
        strcpy(sen, "Black wins using");
    } else if (outcome == 2){
        strcpy(sen, "White wins using");
    }
    sen[16] = ' ';
    char tem[3];
    strcat(sen,itoa(n,tem,10));
    strcat(sen," steps at ");
    time_t t;
    struct tm *local_time;
    time(&t);
    local_time = localtime(&t);
    strcat(sen,asctime(local_time));
    strcat(sen,"\n");
    return;
}

void recordings(){
    cleardevice();
    outtextxy(260,0,"Enter F5 to continue");
    outtextxy(260,20,"Enter F3 to quit");
    int rank = 0;
    struct node* temp = first;
    while (temp != NULL){
        char sen[50] = {0};
        if (temp->outcome == 1){
            strcpy(sen,"Black wins using");
        } else {
            strcpy(sen,"White wins using");
        }
        sen[16] = ' ';
        char tem[3] = {0};
        strcat(sen,itoa(temp->steps,tem,10));
        strcat(sen," steps at ");
        strcat(sen,temp->time);
        temp = temp->next;
        outtextxy(0,(rank+2) * 20, sen);
        rank++;
    }
    while (1){
        m = getmessage(EX_KEY);
        if (m.vkcode == 0x74){
            game_condition = 0;
            return;
        } else if (m.vkcode == 	0x72){
            game_condition = 1;
            return;
        }
    }
}

void record(int outcome, int steps){
    first = add_to_record(first,outcome,steps);
}

struct node* add_to_record(struct node* list, int n, int steps){
    struct node* new_node;
    new_node = static_cast<node *>(malloc(sizeof(struct node)));
    if (new_node == NULL){
        printf("Error: malloc failed in add_to_list\n");
        exit(EXIT_FAILURE);
    }
    new_node->outcome = n;
    new_node->steps = steps;
    time_t t;
    struct tm *local_time;
    time(&t);
    gmtime(&t);
    local_time = localtime(&t);
    strcpy(new_node->time,asctime(local_time));
    new_node->next = list;
    return new_node;
}