#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <termios.h>

#define HEIGHT 11
#define WIDTH 31

struct Pixel {
	char symbol;
	int foregroundRGB[3];
	int backgroundRGB[3];
};
struct Pixel screen[HEIGHT][WIDTH];
struct Pixel spacePixel = {' ',{255,255,255},{0,0,0}};
struct Pixel playerPixel = {'#',{255,255,255},{0,0,0}};

char getch()
{
	struct termios old, new;
	char ch;
	tcgetattr(0, &old); /* grab old terminal i/o settings */
	new = old; /* make new settings same as old settings */
	new.c_lflag &= ~ICANON; /* disable buffered i/o */
	new.c_lflag &= ~ECHO; /* set echo mode */
	tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
	ch = getchar();
	tcsetattr(0, TCSANOW, &old); /* set terminal i/o settings back to old */
	return ch;
}
int msleep(long msec)
{
	struct timespec ts;
	int res;
	 if (msec < 0)
	{
		errno = EINVAL;
		return -1;
	}
	ts.tv_sec = msec / 1000;
	ts.tv_nsec = (msec % 1000) * 1000000;
	do {
		res = nanosleep(&ts, &ts);
	} while (res && errno == EINTR);
	return res;
}
void printScreen() {
	printf("\033c");
	for(int i=0; i < HEIGHT; i++){
		for(int j=0; j < WIDTH; j++){
			printf("\033[38;2;%d;%d;%dm" // set foreground color
				"\033[48;2;%d;%d;%dm" // set background color
				"%c" // print character
				"\033[?25l", // hide cursor
				screen[i][j].foregroundRGB[0],screen[i][j].foregroundRGB[1],screen[i][j].foregroundRGB[2],
				screen[i][j].backgroundRGB[0],screen[i][j].backgroundRGB[1],screen[i][j].backgroundRGB[2],
				screen[i][j].symbol);

		}
	printf("\n"); // print new line
	}
	printf("\033[97;40" // revert color to default
		"\033[?25h"); // show the cursor
}
void addText(char* str, int size, int y) {
	int initial = (30 - size) / 2;
	for(int i = initial; i < initial+size; i++) screen[y-1][i] = (struct Pixel) {str[i-initial],{255,255,255},{0,0,0}};
}
void intro2(int opacity){
	if(opacity >= 255) return;
	for(int i = 10; i < 20; i++) {
		 screen[4][i] = (struct Pixel) {"A White In"[i-10],{255-opacity,255-opacity,255-opacity},{0,0,0}};
	}
	for(int i = 11; i < 19; i++) {
		 screen[5][i] = (struct Pixel) {"The Hood"[i-11],{255-opacity,255-opacity,255-opacity},{0,0,0}};
	}
	printScreen();
	msleep(100);
	intro2(opacity+10);
}
void intro(int opacity){
	if(opacity >= 255) return intro2(0);
	for(int i = 9; i < 21; i++) {
		 screen[4][i] = (struct Pixel) {"NoobJsPerson"[i-9],{255-opacity,255-opacity,255-opacity},{0,0,0}};
	}
	for(int i = 11; i < 19; i++) {
		screen[5][i] = (struct Pixel) {"Presents"[i-11],{255-opacity,255-opacity,255-opacity},{0,0,0}};
	}
	printScreen();
	msleep(100);
	intro(opacity+10);
}
void clearScreen() {
	for(int i=0; i < HEIGHT; i++){
		for(int j=0; j < WIDTH; j++){
			screen[i][j] = spacePixel;
		}
	}
}
void gameLoop() {
	static int playerPosition[2] = {4,15};
	static int health = 10;
	clearScreen();
	// set player pixel on the screen
	screen[playerPosition[0]][playerPosition[1]] = playerPixel;
	// set health pixels
	for(int i = 0; i < health; i++) screen[0][i] = (struct Pixel) {' ',{255,0,0},{255,0,0}};
	printScreen();
	char action = getch();
	switch(action) {
		case 'w':
		if(playerPosition[0] > 0) playerPosition[0]--;
		break;
		case 's':
		if(playerPosition[0] < HEIGHT-1) playerPosition[0]++;
		break;
		case 'a':
		if(playerPosition[1] > 0) playerPosition[1]--;
		break;
		case 'd':
		if(playerPosition[1] < WIDTH-1) playerPosition[1]++;
		break;
	}
	gameLoop();
}
void mainMenu(int option) {
	int cursorPositions[] = {11,10,12};
	screen[(option+1)*3-1][cursorPositions[option]] = (struct Pixel) {'>',{255,255,255},{0,0,0}};
	addText("Start", 5, 3);
	addText("Options", 7, 6);
	addText("Quit", 4, 9);
	printScreen();
	char movement = getch();
	screen[(option+1)*3-1][cursorPositions[option]] = (struct Pixel) {' ',{255,255,255},{0,0,0}};
	switch(movement) {
		case 'w':
		option = option == 0 ? 2 : option - 1;
		break;
		case 's':
		option = option == 2 ? 0 : option + 1;
		break;
		case '\n':
			switch(option) {
				case 0:
				spacePixel.symbol = '_';
				return gameLoop();
				break;
				case 2:
				exit(0);
			}
	}
	mainMenu(option);
}
int main(int argc, char** argv){
	clearScreen();
	intro(0);
	mainMenu(0);
}
