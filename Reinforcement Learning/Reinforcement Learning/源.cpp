#include<iostream>
#include<math.h>
#include<ctime>
#include<cstdlib>
#define _USE_MATH_DEFINES
using namespace std;
int map[20][20] = { //1��ǽ��0�ǵذ�
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} ,
		{1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1} ,
		{1,1,1,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1} ,
		{1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1} ,
		{1,0,0,0,0,0,0,1,0,0,0,1,1,1,1,0,0,0,0,1} ,
		{1,0,0,0,0,0,0,1,0,0,0,1,1,1,1,0,0,0,0,1} ,
		{1,0,0,0,0,0,0,1,0,0,0,1,1,1,1,0,0,0,0,1} ,
		{1,0,0,0,0,1,1,1,0,0,0,1,1,1,1,0,0,0,0,1} ,
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1} ,
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1} ,
		{1,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1} ,
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1} ,
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1} ,
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1} ,
		{1,0,1,0,0,0,0,0,1,1,1,1,1,0,0,1,0,0,0,1} ,
		{1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1} ,
		{1,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,1} ,
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1} ,
		{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1} ,
		{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} ,
};
double actionProb[20][20][4];//��һλ�������꣬�ڶ�λ�Ǻ����꣬��һλ���ж�����0��1��2��3�ֱ������������
int judge(int x, int y);
void move(int x, int y);
int makeDecision(int x, int y);
void updateProb(int x, int y, int decision);
void updateValue(int x, int y, int cx, int cy);
void show(int cx, int cy);
float value[20][20];
const int startX = 1;
const int startY = 1;
const int stopX = 18;
const int stopY = 18;
int currentx, currenty;//��ǰ��λ��


int main() {
	srand((int)time(0));
	for (int i = 1; i < 19; i++) {
		for (int j = 1; j < 19; j++) {
			judge(i, j);
			value[i][j] = 10;
			if (map[i][j] == 1) {
				value[i][j] = -10000;
			}
		}
	}
	value[18][18] = 100;

	currentx = startX;
	currenty = startY;

	int episode = 1;//�ӿ�ʼ��������һ��episode
	int counter = 0;//��¼һ��episode�Ĳ���

	while (episode <= 50000) {
		counter = 0;
		while (currentx != stopX || currenty != stopY) {
			move(currentx, currenty);
			counter++;
		}
		cout << counter << endl;
		episode++;
		currentx = startX;
		currenty = startY;
	}
	system("pause");
}

int judge(int x, int y) {//��(x,y)���ڵ�ͼ����Χ���ϰ���״��
	int down = map[x + 1][y];
	int up = map[x - 1][y];
	int left = map[x][y - 1];
	int right = map[x][y + 1];
	if (right == 0 && left == 0 && down == 0 && up == 0) {
		actionProb[x][y][0] = 0.25; actionProb[x][y][1] = 0.25; actionProb[x][y][2] = 0.25; actionProb[x][y][3] = 0.25;
		return 0;//û���ϰ���
	}
	if (right == 0 && left == 0 && down == 0 && up == 1) {
		actionProb[x][y][0] = 0; actionProb[x][y][1] = 1 / 3.0; actionProb[x][y][2] = 1 / 3.0; actionProb[x][y][3] = 1 / 3.0;
		return 1;//��
	}
	if (right == 0 && left == 0 && down == 1 && up == 0) {
		actionProb[x][y][1] = 0; actionProb[x][y][0] = 1 / 3.0; actionProb[x][y][2] = 1 / 3.0; actionProb[x][y][3] = 1 / 3.0;
		return 2;//��
	}
	if (right == 0 && left == 1 && down == 0 && up == 0) {
		actionProb[x][y][2] = 0; actionProb[x][y][1] = 1 / 3.0; actionProb[x][y][0] = 1 / 3.0; actionProb[x][y][3] = 1 / 3.0;
		return 3;//��
	}
	if (right == 1 && left == 0 && down == 0 && up == 0) {
		actionProb[x][y][3] = 0; actionProb[x][y][1] = 1 / 3.0; actionProb[x][y][2] = 1 / 3.0; actionProb[x][y][0] = 1 / 3.0;
		return 4;//��
	}
	if (right == 0 && left == 0 && down == 1 && up == 1) {
		actionProb[x][y][0] = 0; actionProb[x][y][1] = 0; actionProb[x][y][2] = 1 / 2.0; actionProb[x][y][3] = 1 / 2.0;
		return 5;//����
	}
	if (right == 0 && left == 1 && down == 0 && up == 1) {
		actionProb[x][y][0] = 0; actionProb[x][y][2] = 0; actionProb[x][y][1] = 1 / 2.0; actionProb[x][y][3] = 1 / 2.0;
		return 6;//����
	}
	if (right == 1 && left == 0 && down == 0 && up == 1) {
		actionProb[x][y][0] = 0; actionProb[x][y][3] = 0; actionProb[x][y][2] = 1 / 2.0; actionProb[x][y][1] = 1 / 2.0;
		return 7;//����
	}
	if (right == 0 && left == 1 && down == 1 && up == 0) {
		actionProb[x][y][2] = 0; actionProb[x][y][1] = 0; actionProb[x][y][0] = 1 / 2.0; actionProb[x][y][3] = 1 / 2.0;
		return 8;//����
	}
	if (right == 1 && left == 0 && down == 1 && up == 0) {
		actionProb[x][y][3] = 0; actionProb[x][y][1] = 0; actionProb[x][y][0] = 1 / 2.0; actionProb[x][y][2] = 1 / 2.0;
		return 9;//����
	}
	if (right == 1 && left == 1 && down == 0 && up == 0) {
		actionProb[x][y][2] = 0; actionProb[x][y][3] = 0; actionProb[x][y][0] = 1 / 2.0; actionProb[x][y][1] = 1 / 2.0;
		return 10;//����
	}
	if (right == 0 && left == 1 && down == 1 && up == 1) {
		actionProb[x][y][2] = 0; actionProb[x][y][1] = 0; actionProb[x][y][0] = 0; actionProb[x][y][3] = 1;
		return 11;//������
	}
	if (right == 1 && left == 0 && down == 1 && up == 1) {
		actionProb[x][y][2] = 1; actionProb[x][y][1] = 0; actionProb[x][y][0] = 0; actionProb[x][y][3] = 0;
		return 11;//������
	}
	if (right == 1 && left == 1 && down == 0 && up == 1) {
		actionProb[x][y][2] = 0; actionProb[x][y][1] = 1; actionProb[x][y][0] = 0; actionProb[x][y][3] = 0;
		return 11;//������
	}
	if (right == 0 && left == 1 && down == 1 && up == 1) {
		actionProb[x][y][2] = 0; actionProb[x][y][1] = 0; actionProb[x][y][0] = 1; actionProb[x][y][3] = 0;
		return 11;//������
	}
}

void move(int x, int y) {
	int decision = makeDecision(x, y);
	switch (decision) {
	case 0:currentx = x - 1; break;
	case 1:currentx = x + 1; break;
	case 2:currenty = y - 1; break;
	case 3:currenty = y + 1; break;
	}
	updateValue(x, y, currentx, currenty);
}

int makeDecision(int x, int y) {
	float epsilon = 0.1;
	int randomNumber = rand();
	if ((randomNumber / (double)RAND_MAX) <= 1 - epsilon) {
		if (value[x + 1][y] >= value[x - 1][y] && value[x + 1][y] >= value[x][y + 1] && value[x + 1][y] >= value[x][y - 1] && map[x + 1][y] != 1) {
			return 1;
		}
		else if (value[x - 1][y] >= value[x + 1][y] && value[x - 1][y] >= value[x][y + 1] && value[x - 1][y] >= value[x][y - 1] && map[x - 1][y] != 1) {
			return 0;
		}
		else if (value[x][y + 1] >= value[x - 1][y] && value[x][y + 1] >= value[x + 1][y] && value[x][y + 1] >= value[x][y - 1] && map[x][y + 1] != 1) {
			return 3;
		}
		else if (value[x][y - 1] >= value[x - 1][y] && value[x][y - 1] >= value[x + 1][y] && value[x][y - 1] >= value[x][y + 1] && map[x][y - 1] != 1) {
			return 2;
		}
	}
	else {
		while (true) {
			int randomNumber2 = rand() % 4;
			if (actionProb[x][y][randomNumber2] > 0.0001) {
				return randomNumber2;
			}
		}
	}
}

void updateValue(int x, int y, int cx, int cy) {
	const float alpha = 0.01;//ѧϰ����
	const float gamma = 0.99;//˥������
	if (x != stopX || y != stopY) {
		value[x][y] = value[x][y] + alpha * (-1 + gamma * value[cx][cy] - value[x][y]);
	}
	else {
		value[x][y] = value[x][y] + alpha * (100 + gamma * value[cx][cy] - value[x][y]);
	}
}