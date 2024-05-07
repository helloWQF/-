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

GameLog logs[100];  // ���Դ洢���100����־
int logCount = 0;   // ʵ�ʴ洢����־����

// �����û��ṹ��
typedef struct {
	char username[50];
	char password[50];
} User;

// �û�ע�ắ��
void registerUser() {
	User newUser;
	FILE *fp = fopen("users.txt", "a+");
	if (fp == NULL) {
		printf("�޷����û��ļ�\n");
		exit(1);
	}

	printf("�������û���: ");
	scanf("%s", newUser.username);
	printf("����������: ");
	scanf("%s", newUser.password);

	fprintf(fp, "%s %s\n", newUser.username, newUser.password);
	fclose(fp);
	printf("ע��ɹ���\n");
}

// �û���֤����
int loginUser() {
	User user;
	char username[50], password[50];
	FILE *fp = fopen("users.txt", "r");
	if (fp == NULL) {
		printf("�޷����û��ļ�\n");
		exit(1);
	}

	printf("�������û���: ");
	scanf("%s", username);
	printf("����������: ");
	scanf("%s", password);

	while (fscanf(fp, "%s %s", user.username, user.password) != EOF) {
		if (strcmp(user.username, username) == 0 && strcmp(user.password, password) == 0) {
			fclose(fp);
			printf("��¼�ɹ���\n");
			return 1;
		}
	}

	fclose(fp);
	printf("�û������������\n");
	return 0;
}

typedef struct SNAKE { //�����һ���ڵ�
	int x;
	int y;
	struct SNAKE *next;
} snake;

//ȫ�ֱ���//
int score = 0, add = 10;//�ܵ÷���ÿ�γ�ʳ��÷֡�
int status, sleeptime = 200;//ÿ�����е�ʱ����
snake *head, * food;//��ͷָ�룬ʳ��ָ��
snake *q;//�����ߵ�ʱ���õ���ָ��
int endgamestatus = 0; //��Ϸ�����������1��ײ��ǽ��2��ҧ���Լ���3�������˳���Ϸ��

//����ȫ������//
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
void gamestart(const char *username); // �޸ĺ�������
void displayGameLogs();
void gamecircle();
void recordGameLog(const char *username, int score, time_t startTime, int duration);
void drawGame();

void Pos(int x, int y) { //���ù��λ��
	COORD pos;
	HANDLE hOutput;
	pos.X = x;
	pos.Y = y;
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOutput, pos);
}

void creatMap() { //������ͼ
	int i;
	for (i = 0; i < 58; i += 2) { //��ӡ���±߿�
		Pos(i, 0);
		printf("��");
		Pos(i, 26);
		printf("��");
	}
	for (i = 1; i < 26; i++) { //��ӡ���ұ߿�
		Pos(0, i);
		printf("��");
		Pos(56, i);
		printf("��");
	}
}

void initsnake() { //��ʼ������
	snake *tail;
	int i;
	tail = (snake *)malloc(sizeof(snake)); //����β��ʼ��ͷ�巨����x,y�趨��ʼ��λ��//
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
	while (tail != NULL) { //��ͷ��Ϊ���������
		Pos(tail->x, tail->y);
		printf("��");
		tail = tail->next;
	}
}

int biteself() { //�ж��Ƿ�ҧ�����Լ�
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
			food_1->x--; // ȷ��x����Ϊż��������ͷ����

		for (q = head; q != NULL; q = q->next) {
			if (q->x == food_1->x && q->y == food_1->y) {
				isOverlapping = 1;
				break;
			}
		}
	} while (isOverlapping); // ���ʳ���������غϣ���������

	Pos(food_1->x, food_1->y);
	food = food_1;
	printf("��");
}


void cantcrosswall() { //���ܴ�ǽ
	if (head->x == 0 || head->x == 56 || head->y == 0 || head->y == 26) {
		endgamestatus = 1;
		endgame();
	}
}

void snakemove() { //��ǰ��,��U,��D,��L,��R
	snake *nexthead;
	cantcrosswall();

	nexthead = (snake *)malloc(sizeof(snake));
	if (status == U) {
		nexthead->x = head->x;
		nexthead->y = head->y - 1;
		if (nexthead->x == food->x && nexthead->y == food->y) { //�����һ����ʳ��//
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("��");
				q = q->next;
			}
			score = score + add;
			createfood();
		} else {                                           //���û��ʳ��//
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q->next->next != NULL) {
				Pos(q->x, q->y);
				printf("��");
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
		if (nexthead->x == food->x && nexthead->y == food->y) { //��ʳ��
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("��");
				q = q->next;
			}
			score = score + add;
			createfood();
		} else {                           //û��ʳ��
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q->next->next != NULL) {
				Pos(q->x, q->y);
				printf("��");
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
		if (nexthead->x == food->x && nexthead->y == food->y) { //��ʳ��
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("��");
				q = q->next;
			}
			score = score + add;
			createfood();
		} else {                            //û��ʳ��
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q->next->next != NULL) {
				Pos(q->x, q->y);
				printf("��");
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
		if (nexthead->x == food->x && nexthead->y == food->y) { //��ʳ��
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q != NULL) {
				Pos(q->x, q->y);
				printf("��");
				q = q->next;
			}
			score = score + add;
			createfood();
		} else {                                     //û��ʳ��
			nexthead->next = head;
			head = nexthead;
			q = head;
			while (q->next->next != NULL) {
				Pos(q->x, q->y);
				printf("��");
				q = q->next;
			}
			Pos(q->next->x, q->next->y);
			printf("  ");
			free(q->next);
			q->next = NULL;
		}
	}
	if (biteself() == 1) {     //�ж��Ƿ��ҧ���Լ�
		endgamestatus = 2;
		endgame();
	}
}

//��ͣ
void pause() {
	time_t pauseStart = time(NULL);  // ��¼��ͣ��ʼ��ʱ��
	printf("��Ϸ����ͣ�����ո��������F5�鿴��־��\n");
	while (1) {
		Sleep(100);  // ���� CPU ʹ����
		if (GetAsyncKeyState(VK_SPACE) & 0x8000) {  // ����ٴΰ��¿ո�����������Ϸ
			break;  // ������ͣѭ��
		}
		if (GetAsyncKeyState(VK_F5) & 0x8000) {  // ������� F5������ʾ��־
			int currentDuration = (int)difftime(time(NULL), pauseStart) + logs[logCount].duration;
			system("cls");
			printf("��ǰ��Ϸ�Ự��Ϣ��\n");
			printf("�û���: %s\n�÷�: %d\n��Ϸ��ʼʱ��: %s��Ϸ����ʱ��: %d��\n",
			       logs[logCount].username, score, ctime(&logs[logCount].startTime), currentDuration);
			printf("�����������...\n");


			printf("��Ϸ����ͣ�����ո��������F5�鿴��־��\n");

		}
	}
	system("cls");
	drawGame();
}


void drawGame() {
	creatMap(); // ���´�����ͼ
	for (snake *s = head; s != NULL; s = s->next) { // ���»�������
		Pos(s->x, s->y);
		printf("��");
	}
	Pos(food->x, food->y); // ���»���ʳ��
	printf("��");
}





void gamecircle() { //������Ϸ

	Pos(64, 15);
	printf("���ܴ�ǽ������ҧ���Լ�\n");
	Pos(64, 16);
	printf("�á�.��.��.���ֱ�����ߵ��ƶ�.");
	Pos(64, 17);
	printf("F1 Ϊ���٣�F2 Ϊ����\n");
	Pos(64, 18);
	printf("��F5��ʾ��Ϸ�û���־\n");
	Pos(64, 19);
	printf("ESC ���˳���Ϸ.space����ͣ��Ϸ.");
	Pos(64, 20);
	status = R;
	while (1) {
		Pos(64, 10);
		printf("�÷֣�%d  ", score);
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
					add = 2;//��ֹ����1֮���ټӻ����д�
				}
			}
		} else if (GetAsyncKeyState(VK_F2)) {
			if (sleeptime < 350) {
				sleeptime = sleeptime + 30;
				add = add - 2;
				if (sleeptime == 350) {
					add = 1;  //��֤��ͷ�Ϊ1
				}
			}
		}
		if (GetAsyncKeyState(VK_F5) & 0x8000) {
			displayGameLogs();  // ������ʾ��־����
			Pos(64, 10);  // ���ù��λ���Լ�����Ϸ
			printf("�÷֣�%d  ", score);
		}

		Sleep(sleeptime);
		snakemove();
	}
}

void welcometogame() { //��ʼ����
	Pos(40, 12);
	printf("��ӭ����̰ʳ����Ϸ��");
	Pos(40, 25);
	system("pause");
	system("cls");
	Pos(25, 12);
	printf("�á�.��.��.���ֱ�����ߵ��ƶ��� F1 Ϊ���٣�2 Ϊ����\n");
	Pos(25, 13);
	printf("���ٽ��ܵõ����ߵķ�����\n");
	Pos(25, 14);
	printf("��ѡ��̰���ߵ���ɫ��\n");
	printf("1. ��ɫ\n");
	printf("2. ��ɫ\n");
	printf("3. ��ɫ\n");
	printf("4. ��ɫ\n");
	printf("5. �ۺ�ɫ\n");
	printf("6. ��ɫ\n");
	printf("7. ����ɫ\n");
	printf("8. ��ɫ\n");
	int colorChoice;
	printf("������ѡ��Ĭ��Ϊ1����");
	scanf("%d", &colorChoice);
	switch (colorChoice) {
		case 1:
			system("color 0F"); // ���ÿ���̨��ɫΪ�ڵ�����
			break;
		case 2:
			system("color 0C"); // ���ÿ���̨��ɫΪ�ڵ׺���
			break;
		case 3:
			system("color 09"); // ���ÿ���̨��ɫΪ�ڵ�����
			break;
		case 4:
			system("color 0A"); // ���ÿ���̨��ɫΪ�ڵװ���
			break;
		case 5:
			system("color 0D"); // ���ÿ���̨��ɫΪ�ڵ׷ۺ���
			break;
		case 6:
			system("color 05"); // ���ÿ���̨��ɫΪ�ڵ�����
			break;
		case 7:
			system("color 0B"); // ���ÿ���̨��ɫΪ�ڵ�������
			break;
		case 8:
			system("color 06"); // ���ÿ���̨��ɫΪ�ڵ׳�ɫ��
			break;
		default:
			system("color 0F"); // Ĭ�ϰ�ɫ
			break;
	}
	system("pause");
	system("cls");
}

void displayGameLogs() {
	// ���� logs ���Ѿ��������ݵ���־����
	system("cls"); // �����Ա�鿴��־
	printf("��Ϸ��־:\n");
	for (int i = 0; i < logCount; i++) {
		printf("%d. �û���: %s, �÷�: %d, ��Ϸ��ʼʱ��: %s, ����ʱ��: %d��\n",
		       i + 1, logs[i].username, logs[i].score, ctime(&logs[i].startTime), logs[i].duration);
	}
	system("pause"); // ��ͣ�鿴��־
	system("cls"); // �����󷵻���Ϸ
	Pos(64, 15);
	printf("���ܴ�ǽ������ҧ���Լ�\n");
	Pos(64, 16);
	printf("�á�.��.��.���ֱ�����ߵ��ƶ�.");
	Pos(64, 17);
	printf("F1 Ϊ���٣�F2 Ϊ����\n");
	Pos(64, 18);
	printf("��F5��ʾ��Ϸ�û���־\n");
	Pos(64, 19);
	printf("ESC ���˳���Ϸ.space����ͣ��Ϸ.");
	Pos(64, 20);
	creatMap(); // ���´�����Ϸ��ͼ
	for (snake *s = head; s != NULL; s = s->next) { // ���»�������
		Pos(s->x, s->y);
		printf("��");
	}
	Pos(food->x, food->y); // ���»���ʳ��
	printf("��");
}

void recordGameLog(const char *username, int score, time_t startTime, int duration) {
	if (logCount < 100) { // ȷ�����������鷶Χ
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
	printf("*** %s ������Ϸ�� ***\n", username);

	// ��ʼ����־
	strcpy(logs[logCount].username, username);
	logs[logCount].startTime = time(NULL);
	logs[logCount].score = 0;  // ��ʼ�÷�Ϊ0
	logs[logCount].duration = 0;  // ��ʼ����ʱ��Ϊ0
}

void endgame() {
	int duration = (int)difftime(time(NULL), logs[logCount].startTime);  // ����������Ϸ�Ự�ĳ���ʱ��
	logs[logCount].duration = duration;
	logs[logCount].score = score;

	system("cls");
	Pos(24, 12);
	if (endgamestatus == 1) {
		printf("�Բ�����ײ��ǽ�ˡ���Ϸ����!");
	} else if (endgamestatus == 2) {
		printf("�Բ�����ҧ���Լ��ˡ���Ϸ����!");
	} else if (endgamestatus == 3) {
		printf("���Ѿ���������Ϸ��");
	}
	Pos(24, 13);
	printf("���ĵ÷���%d\n", score);

	// ������־�������Ա���һ����Ϸʹ��
	logCount++;

	displayGameLogs();  // ��ʾ��־���˳�
	exit(0);
}

int main() {
	system("cls");
	int choice;
	char username[50];  // �洢�û���

	printf("1. ע��\n");
	printf("2. ��¼\n");
	printf("��ѡ�����: ");
	scanf("%d", &choice);

	if (choice == 1) {
		registerUser();
	} else if (choice == 2) {
		if (loginUser()) {
			printf("��¼�ɹ���\n");
			printf("�������û���: ");
			scanf("%s", username);
			gamestart(username);  // ���û������ݸ���Ϸ��ʼ����
			gamecircle();
			endgame();
		} else {
			// ��¼ʧ�ܣ��˳���Ϸ
			return 0;
		}
	} else {
		printf("��Ч��ѡ��\n");
		return 0;
	}

	return 0;
}