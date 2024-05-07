#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#define U 1
#define D 2
#define L 3
#define R 4

typedef struct {
	char username[50];
	int score;
	time_t startTime;
	int duration;
} GameLog;

GameLog logs[100];  // 可以存储最多100条日志
int logCount = 0;   // 实际存储的日志数量

// 定义用户结构体
typedef struct {
	char username[50];
	char password[50];
} User;

// 用户注册函数
void registerUser() {
	User newUser;
	FILE *fp = fopen("users.txt", "a+");
	if (fp == NULL) {
		printf("无法打开用户文件\n");
		exit(1);
	}

	printf("请输入用户名: ");
	scanf("%s", newUser.username);
	printf("请输入密码: ");
	scanf("%s", newUser.password);

	fprintf(fp, "%s %s\n", newUser.username, newUser.password);
	fclose(fp);
	printf("注册成功！\n");
}

// 用户验证函数
int loginUser() {
	User user;
	char username[50], password[50];
	FILE *fp = fopen("users.txt", "r");
	if (fp == NULL) {
		printf("无法打开用户文件\n");
		exit(1);
	}

	printf("请输入用户名: ");
	scanf("%s", username);
	printf("请输入密码: ");
	scanf("%s", password);

	while (fscanf(fp, "%s %s", user.username, user.password) != EOF) {
		if (strcmp(user.username, username) == 0 && strcmp(user.password, password) == 0) {
			fclose(fp);
			printf("登录成功！\n");
			return 1;
		}
	}

	fclose(fp);
	printf("用户名或密码错误！\n");
	return 0;
}

typedef struct SNAKE { //蛇身的一个节点
	int x;
	int y;
	struct SNAKE *next;
} snake;

//全局变量//
int score = 0, add = 10;//总得分与每次吃食物得分。
int status, sleeptime = 200;//每次运行的时间间隔
snake *head, * food;//蛇头指针，食物指针
snake *q;//遍历蛇的时候用到的指针
int endgamestatus = 0; //游戏结束的情况，1：撞到墙；2：咬到自己；3：主动退出游戏。

//声明全部函数//
void Pos();
void creatMap();
void initsnake();
int biteself();
void createfood();
void cantcrosswall();
void snakemove();
void pause();
void gamecircle();
void welcometogame();
void endgame();
void gamestart(const char *username); // 修改函数声明
void displayGameLogs();
void gamecircle();
void recordGameLog(const char *username, int score, time_t startTime, int duration);
void drawGame();

void Pos(int x, int y) { //设置光标位置
	COORD pos;
	HANDLE hOutput;
	pos.X = x;
	pos.Y = y;
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOutput, pos);
}

void creatMap() { //创建地图
	int i;
	for (i = 0; i < 58; i += 2) { //打印上下边框
		Pos(i, 0);
		printf("■");
		Pos(i, 26);
		printf("■");
	}
	for (i = 1; i < 26; i++) { //打印左右边框
		Pos(0, i);
		printf("■");
		Pos(56, i);
		printf("■");
	}
}

void initsnake() { //初始化蛇身
	snake *tail;
	int i;
	tail = (snake *)malloc(sizeof(snake)); //从蛇尾开始，头插法，以x,y设定开始的位置//
	tail->x = 24;
	tail->y = 5;
	tail->next = NULL;
	for (i = 1; i <= 4; i++) {
		head = (snake *)malloc(sizeof(snake));
		head->next = tail;
		head->x = 24 + 2 * i;
		head->y = 5;
		tail = head;
	}
	while (tail != NULL) { //从头到为，输出蛇身
		Pos(tail->x, tail->y);
		printf("■");
		tail = tail->next;
	}
}

int biteself() { //判断是否咬到了自己
	snake *self;
	self = head->next;
	while (self != NULL) {
		if (self->x == head->x && self->y == head->y) {
			return 1;
		}
		self = self->next;
	}
	return 0;
}

void createfood() {
	int isOverlapping;
	snake *food_1;
	srand((unsigned)time(NULL));
	food_1 = (snake *)malloc(sizeof(snake));

	do {
		isOverlapping = 0;
		food_1->x = rand() % 52 + 2;
		food_1->y = rand() % 24 + 1;
		if ((food_1->x % 2) != 0)
			food_1->x--; // 确保x坐标为偶数，与蛇头对齐

		for (q = head; q != NULL; q = q->next) {
			if (q->x == food_1->x && q->y == food_1->y) {
				isOverlapping = 1;
				break;
			}
		}
	} while (isOverlapping); // 如果食物与蛇身重合，重新生成

	Pos(food_1->x, food_1->y);
	food = food_1;
	printf("■");
}


void cantcrosswall() { //不能穿墙
	if (head->x == 0 || head->x == 56 || head->y == 0 || head->y == 26) {
		endgamestatus = 1;
		endgame();
	}
}

void snakemove() { //蛇前进,上U,下D,左L,右R
	snake *nexthead;
	cantcrosswall();

	nexthead = (snake *)malloc(sizeof(snake));
	if (status == U) {
		nexthead->x = head->x;
		nexthead->y = head->y - 1;
		if (nexthead->x == food->x && nexthead->y == food->y) { //如果下一个有食物//
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("■");
				q = q->next;
			}
			score = score + add;
			createfood();
		} else {                                           //如果没有食物//
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q->next->next != NULL) {
				Pos(q->x, q->y);
				printf("■");
				q = q->next;
			}
			Pos(q->next->x, q->next->y);
			printf("  ");
			free(q->next);
			q->next = NULL;
		}
	}
	if (status == D) {
		nexthead->x = head->x;
		nexthead->y = head->y + 1;
		if (nexthead->x == food->x && nexthead->y == food->y) { //有食物
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("■");
				q = q->next;
			}
			score = score + add;
			createfood();
		} else {                           //没有食物
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q->next->next != NULL) {
				Pos(q->x, q->y);
				printf("■");
				q = q->next;
			}
			Pos(q->next->x, q->next->y);
			printf("  ");
			free(q->next);
			q->next = NULL;
		}
	}
	if (status == L) {
		nexthead->x = head->x - 2;
		nexthead->y = head->y;
		if (nexthead->x == food->x && nexthead->y == food->y) { //有食物
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("■");
				q = q->next;
			}
			score = score + add;
			createfood();
		} else {                            //没有食物
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q->next->next != NULL) {
				Pos(q->x, q->y);
				printf("■");
				q = q->next;
			}
			Pos(q->next->x, q->next->y);
			printf("  ");
			free(q->next);
			q->next = NULL;
		}
	}
	if (status == R) {
		nexthead->x = head->x + 2;
		nexthead->y = head->y;
		if (nexthead->x == food->x && nexthead->y == food->y) { //有食物
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("■");
				q = q->next;
			}
			score = score + add;
			createfood();
		} else {                                     //没有食物
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q->next->next != NULL) {
				Pos(q->x, q->y);
				printf("■");
				q = q->next;
			}
			Pos(q->next->x, q->next->y);
			printf("  ");
			free(q->next);
			q->next = NULL;
		}
	}
	if (biteself() == 1) {     //判断是否会咬到自己
		endgamestatus = 2;
		endgame();
	}
}

//暂停
void pause() {
	time_t pauseStart = time(NULL);  // 记录暂停开始的时间
	printf("游戏已暂停。按空格键继续，F5查看日志。\n");
	while (1) {
		Sleep(100);  // 减少 CPU 使用率
		if (GetAsyncKeyState(VK_SPACE) & 0x8000) {  // 如果再次按下空格键，则继续游戏
			break;  // 结束暂停循环
		}
		if (GetAsyncKeyState(VK_F5) & 0x8000) {  // 如果按下 F5，则显示日志
			int currentDuration = (int)difftime(time(NULL), pauseStart) + logs[logCount].duration;
			system("cls");
			printf("当前游戏会话信息：\n");
			printf("用户名: %s\n得分: %d\n游戏开始时间: %s游戏持续时间: %d秒\n",
			       logs[logCount].username, score, ctime(&logs[logCount].startTime), currentDuration);
			printf("按任意键返回...\n");


			printf("游戏已暂停。按空格键继续，F5查看日志。\n");

		}
	}
	system("cls");
	drawGame();
}


void drawGame() {
	creatMap(); // 重新创建地图
	for (snake *s = head; s != NULL; s = s->next) { // 重新绘制蛇体
		Pos(s->x, s->y);
		printf("■");
	}
	Pos(food->x, food->y); // 重新绘制食物
	printf("■");
}





void gamecircle() { //控制游戏

	Pos(64, 15);
	printf("不能穿墙，不能咬到自己\n");
	Pos(64, 16);
	printf("用↑.↓.←.→分别控制蛇的移动.");
	Pos(64, 17);
	printf("F1 为加速，F2 为减速\n");
	Pos(64, 18);
	printf("按F5显示游戏用户日志\n");
	Pos(64, 19);
	printf("ESC ：退出游戏.space：暂停游戏.");
	Pos(64, 20);
	status = R;
	while (1) {
		Pos(64, 10);
		printf("得分：%d  ", score);
		if (GetAsyncKeyState(VK_UP) && status != D) {
			status = U;
		} else if (GetAsyncKeyState(VK_DOWN) && status != U) {
			status = D;
		} else if (GetAsyncKeyState(VK_LEFT) && status != R) {
			status = L;
		} else if (GetAsyncKeyState(VK_RIGHT) && status != L) {
			status = R;
		} else if (GetAsyncKeyState(VK_SPACE)) {
			pause();
		} else if (GetAsyncKeyState(VK_ESCAPE)) {
			endgamestatus = 3;
			break;
		} else if (GetAsyncKeyState(VK_F1)) {
			if (sleeptime >= 50) {
				sleeptime = sleeptime - 30;
				add = add + 2;
				if (sleeptime == 320) {
					add = 2;//防止减到1之后再加回来有错
				}
			}
		} else if (GetAsyncKeyState(VK_F2)) {
			if (sleeptime < 350) {
				sleeptime = sleeptime + 30;
				add = add - 2;
				if (sleeptime == 350) {
					add = 1;  //保证最低分为1
				}
			}
		}
		if (GetAsyncKeyState(VK_F5) & 0x8000) {
			displayGameLogs();  // 调用显示日志函数
			Pos(64, 10);  // 重置光标位置以继续游戏
			printf("得分：%d  ", score);
		}

		Sleep(sleeptime);
		snakemove();
	}
}

void welcometogame() { //开始界面
	Pos(40, 12);
	printf("欢迎来到贪食蛇游戏！");
	Pos(40, 25);
	system("pause");
	system("cls");
	Pos(25, 12);
	printf("用↑.↓.←.→分别控制蛇的移动， F1 为加速，2 为减速\n");
	Pos(25, 13);
	printf("加速将能得到更高的分数。\n");
	Pos(25, 14);
	printf("请选择贪吃蛇的颜色：\n");
	printf("1. 白色\n");
	printf("2. 红色\n");
	printf("3. 蓝色\n");
	printf("4. 绿色\n");
	printf("5. 粉红色\n");
	printf("6. 紫色\n");
	printf("7. 亮蓝色\n");
	printf("8. 橙色\n");
	int colorChoice;
	printf("请输入选择（默认为1）：");
	scanf("%d", &colorChoice);
	switch (colorChoice) {
		case 1:
			system("color 0F"); // 设置控制台颜色为黑底绿字
			break;
		case 2:
			system("color 0C"); // 设置控制台颜色为黑底红字
			break;
		case 3:
			system("color 09"); // 设置控制台颜色为黑底蓝字
			break;
		case 4:
			system("color 0A"); // 设置控制台颜色为黑底白字
			break;
		case 5:
			system("color 0D"); // 设置控制台颜色为黑底粉红字
			break;
		case 6:
			system("color 05"); // 设置控制台颜色为黑底紫字
			break;
		case 7:
			system("color 0B"); // 设置控制台颜色为黑底亮蓝字
			break;
		case 8:
			system("color 06"); // 设置控制台颜色为黑底橙色字
			break;
		default:
			system("color 0F"); // 默认白色
			break;
	}
	system("pause");
	system("cls");
}

void displayGameLogs() {
	// 假设 logs 是已经存有数据的日志数组
	system("cls"); // 清屏以便查看日志
	printf("游戏日志:\n");
	for (int i = 0; i < logCount; i++) {
		printf("%d. 用户名: %s, 得分: %d, 游戏开始时间: %s, 持续时间: %d秒\n",
		       i + 1, logs[i].username, logs[i].score, ctime(&logs[i].startTime), logs[i].duration);
	}
	system("pause"); // 暂停查看日志
	system("cls"); // 清屏后返回游戏
	Pos(64, 15);
	printf("不能穿墙，不能咬到自己\n");
	Pos(64, 16);
	printf("用↑.↓.←.→分别控制蛇的移动.");
	Pos(64, 17);
	printf("F1 为加速，F2 为减速\n");
	Pos(64, 18);
	printf("按F5显示游戏用户日志\n");
	Pos(64, 19);
	printf("ESC ：退出游戏.space：暂停游戏.");
	Pos(64, 20);
	creatMap(); // 重新创建游戏地图
	for (snake *s = head; s != NULL; s = s->next) { // 重新绘制蛇体
		Pos(s->x, s->y);
		printf("■");
	}
	Pos(food->x, food->y); // 重新绘制食物
	printf("■");
}

void recordGameLog(const char *username, int score, time_t startTime, int duration) {
	if (logCount < 100) { // 确保不超出数组范围
		strcpy(logs[logCount].username, username);
		logs[logCount].score = score;
		logs[logCount].startTime = startTime;
		logs[logCount].duration = duration;
		logCount++;
	}
}



void gamestart(const char *username) {
	system("mode con cols=100 lines=30");
	welcometogame();
	creatMap();
	initsnake();
	createfood();
	Pos(0, 28);
	printf("*** %s 正在游戏中 ***\n", username);

	// 初始化日志
	strcpy(logs[logCount].username, username);
	logs[logCount].startTime = time(NULL);
	logs[logCount].score = 0;  // 初始得分为0
	logs[logCount].duration = 0;  // 初始持续时间为0
}

void endgame() {
	int duration = (int)difftime(time(NULL), logs[logCount].startTime);  // 计算整个游戏会话的持续时间
	logs[logCount].duration = duration;
	logs[logCount].score = score;

	system("cls");
	Pos(24, 12);
	if (endgamestatus == 1) {
		printf("对不起，您撞到墙了。游戏结束!");
	} else if (endgamestatus == 2) {
		printf("对不起，您咬到自己了。游戏结束!");
	} else if (endgamestatus == 3) {
		printf("您已经结束了游戏。");
	}
	Pos(24, 13);
	printf("您的得分是%d\n", score);

	// 增加日志计数器以备下一次游戏使用
	logCount++;

	displayGameLogs();  // 显示日志并退出
	exit(0);
}

int main() {
	system("cls");
	int choice;
	char username[50];  // 存储用户名

	printf("1. 注册\n");
	printf("2. 登录\n");
	printf("请选择操作: ");
	scanf("%d", &choice);

	if (choice == 1) {
		registerUser();
	} else if (choice == 2) {
		if (loginUser()) {
			printf("登录成功！\n");
			printf("请输入用户名: ");
			scanf("%s", username);
			gamestart(username);  // 将用户名传递给游戏开始函数
			gamecircle();
			endgame();
		} else {
			// 登录失败，退出游戏
			return 0;
		}
	} else {
		printf("无效的选择\n");
		return 0;
	}

	return 0;
}