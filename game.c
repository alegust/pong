#include <stdint.h>
#include <pic32mx.h>

int digit[10][15] = {
	{1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1}, // 0
	{0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}, // 1
	{1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1}, // 2
	{1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}, // 3
	{1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1}, // 4
	{1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1}, // 5
	{1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1}, // 6
	{1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1}, // 7
	{1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1}, // 8
	{1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}  // 9
};

uint8_t screen[128][32];

int ball_x = 0;
int ball_y = 16;
int ball_velocity_x = 1;
int ball_velocity_y = 1;

int player_height = 8;
int score_height = 5;
int score_width = 3;

int player1_y = 15;
int player1_x = 8;
int player1_score = 0;
int player1_score_pos_x = 50;
int player1_score_pos_y = 5;

int player2_y = 15;
int player2_x = 119;
int player2_score = 0;
int player2_score_pos_x = 70;
int player2_score_pos_y = 5;

void display_digit(int n, int x_pos, int y_pos) {
	int x, y;
	for(x = 0; x < score_width; x++) {
		for (y = 0; y < score_height; y++) {
			screen[x_pos+x][y_pos+y] = 0;
			screen[x_pos+x][y_pos+y] = digit[n][x+(y*3)];
		}
	}
}

void update_screen(uint8_t screen[128][32]) {
	int y;

	// ball
	screen[ball_x][ball_y] = 1;

	// player1
	for(y = 0; y < player_height; y++) {
		screen[player1_x][player1_y+y] = 1;
		screen[player1_x+1][player1_y+y] = 1;
	}

	// player2
	for(y = 0; y < player_height; y++) {
		screen[player2_x][player2_y+y] = 1;
		screen[player2_x+1][player2_y+y] = 1;
	}

	//score
	//display_digit(player1_score%10, player1_score_pos_x, player1_score_pos_y);
	//display_digit(player2_score%10, player2_score_pos_x, player2_score_pos_y);

	display_screen(screen);
}

void update_ball() {
	screen[ball_x][ball_y] = 0;
	int new_x = ball_x + ball_velocity_x;
	int new_y = ball_y + ball_velocity_y;

	if (new_y < 0 || new_y > 31) {
		ball_velocity_y = -ball_velocity_y;
	}
	if (new_x < 0 || new_x > 127) {
		ball_velocity_x = -ball_velocity_x;
	}
	if (new_x < 0) {
		player2_score++;
	}
	if (new_x > 127) {
		player1_score++;
	}

	new_x = ball_x + ball_velocity_x;
	new_y = ball_y + ball_velocity_y;
	ball_x = new_x;
	ball_y = new_y;
}

void update_player(int *x_pos, int *y_pos, int down, int up) {
	int y;
	int new_y = *y_pos;
	for(y = 0; y < player_height; y++) {
		screen[*x_pos][*y_pos+y] = 0;
		screen[*x_pos+1][*y_pos+y] = 0;
	}

	if (up) {
		new_y--;
	}
	if (down) {
		new_y++;
	}

	if (new_y > 0 &&  new_y < 31-player_height) {
		*y_pos = new_y;
	}
}

int is_player_ball_collide(int *x_pos, int *y_pos) {
	int y;
	for(y = 0; y < player_height; y++) {
		if (pos_eq(ball_x, ball_y, *x_pos, *y_pos+y)) {
			return 1;
		}
		if (pos_eq(ball_x, ball_y, *x_pos+1, *y_pos+y)) {
			return 1;
		}
	}
	return 0;
}

int pos_eq(int ax, int ay, int bx, int by) {
	if (ax == bx && ay == by) {
		return 1;
	}
	return 0;
}

void game_tick() {
		update_ball();
		update_player(&player1_x, &player1_y, getbtn4(), getbtn3());
		update_player(&player2_x, &player2_y, getbtn2(), getbtn1());

		if (is_player_ball_collide(&player1_x, &player1_y)) {
			ball_velocity_x = -ball_velocity_x;
			update_ball();
		}

		if (is_player_ball_collide(&player2_x, &player2_y)) {
			ball_velocity_x = -ball_velocity_x;
			update_ball();
		}

		if (player1_score > 9 || player2_score > 9) {
			player1_score = 0;
			player2_score = 0;
		}

		update_screen(screen);
}

void game_init() {
	int x, y;
	for(x = 0; x < 128; x++) {
		for(y = 0; y < 32; y++) {
			screen[x][y] = 0;
		}
	}
	screen[ball_x][ball_y] = 1;
	update_screen(screen);
}

int getbtn1() {
	return (PORTF >> 1) & 0x1;
}

int getbtn2( void ) {
	return (PORTD >> 5) & 0x1;
}

int getbtn3( void ) {
	return (PORTD >> 6) & 0x1;
}

int getbtn4() {
	return (PORTD >> 7) & 0x1;
}
