#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#define FPS 60.0

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 480

#define PADDLE_LEFT 0
#define PADDLE_RIGHT 1

#define PADDLE_POS_X  30
#define PADDLE_POS_Y  (DISPLAY_HEIGHT/2)
#define PADDLE_SIZE_X 25
#define PADDLE_SIZE_Y 120
#define PADDLE_SPEED 4

#define BALL_SIZE 20
#define BALL_SPEED 3

#define LEFT_PADDLE_COLLISION 1
#define RIGHT_PADDLE_COLLISION 2
#define UPPER_COLLISION 3
#define BOTTOM_COLLISION 4
#define LEFT_COLLISION 5
#define RIGHT_COLLISION 6

#define INPUT_NONE 0
#define INPUT_DOWN 1
#define INPUT_UP 2

typedef struct paddle_s {
	int pos_x;
	int pos_y;
	int size_x;
	int size_y;
	int speed;

	int touch_top;
	int touch_down;

} paddle_t;

void init_paddle(paddle_t *p_paddle, int pos_x, int pos_y, int size_x, int size_y, int speed)
{
	p_paddle->pos_x = pos_x;
	p_paddle->pos_y = pos_y;
	p_paddle->size_x = size_x;
	p_paddle->size_y = size_y;
	p_paddle->speed = speed;
}

void up_paddle(paddle_t *p_paddle)
{
	p_paddle->pos_y-=p_paddle->speed;

	p_paddle->touch_down = 0;

	if((p_paddle->pos_y-(p_paddle->size_y/2))<=0)
	{
		p_paddle->pos_y=(p_paddle->size_y/2);
		p_paddle->touch_top = 1;
	}
}

void down_paddle(paddle_t *p_paddle)
{
	p_paddle->pos_y+=p_paddle->speed;

	p_paddle->touch_top=0;

	if(p_paddle->pos_y>=(DISPLAY_HEIGHT-(p_paddle->size_y/2)))
	{
		p_paddle->pos_y=(DISPLAY_HEIGHT-(p_paddle->size_y/2));
		p_paddle->touch_down = 1;
	}
}

void draw_paddle(paddle_t *p_paddle)
{
	al_draw_filled_rectangle(p_paddle->pos_x-(p_paddle->size_x/2), 
		                     p_paddle->pos_y-(p_paddle->size_y/2), 
		                     p_paddle->pos_x+(p_paddle->size_x/2), 
		                     p_paddle->pos_y+(p_paddle->size_y/2), 
		                     al_map_rgb(255, 255, 255));
}


typedef struct ball_s {
	int pos_x;
	int pos_y;
	int size;
	
	int mov_x;
	int mov_y;

} ball_t;

void init_ball(ball_t *p_ball, int pos_x, int pos_y, int size, int speed)
{
	p_ball->pos_x = pos_x;
	p_ball->pos_y = pos_y;
	p_ball->size = size;

	p_ball->mov_x = 0;
	p_ball->mov_y = 0;
}

void set_ball_vector(ball_t *p_ball, int x_vec, int y_vec)
{
	p_ball->mov_x = x_vec;
	p_ball->mov_y = y_vec;
}

void go_ball(ball_t *p_ball)
{
	p_ball->pos_x += p_ball->mov_x;
	p_ball->pos_y += p_ball->mov_y;
}

void draw_ball(ball_t *p_ball)
{
	al_draw_filled_rectangle(p_ball->pos_x-(p_ball->size/2), 
		                     p_ball->pos_y-(p_ball->size/2), 
		                     p_ball->pos_x+(p_ball->size/2), 
		                     p_ball->pos_y+(p_ball->size/2), 
		                     al_map_rgb(255, 255, 255));
}


/* Overall game class */
typedef struct pong_game_s {	
	paddle_t paddle_left;
	paddle_t paddle_right;

	ball_t ball;

	int points_left;
	int points_right;

	int speed_up;

} pong_game_t;

void pong_game_init(pong_game_t *pong_game)
{
	pong_game->points_left = 0;
	pong_game->points_right = 0;
	pong_game->speed_up = 0;

	init_paddle(&pong_game->paddle_left, PADDLE_POS_X, PADDLE_POS_Y, PADDLE_SIZE_X, PADDLE_SIZE_Y, PADDLE_SPEED);
	init_paddle(&pong_game->paddle_right, (DISPLAY_WIDTH-PADDLE_POS_X), PADDLE_POS_Y, PADDLE_SIZE_X, PADDLE_SIZE_Y, PADDLE_SPEED);
	init_ball(&pong_game->ball, (DISPLAY_WIDTH/2), DISPLAY_HEIGHT/2, BALL_SIZE, BALL_SPEED);

	/* set initial direction */
	set_ball_vector(&pong_game->ball, -3, 1);

	draw_paddle(&pong_game->paddle_left);
	draw_paddle(&pong_game->paddle_right);
	draw_ball(&pong_game->ball);
}

int pong_game_check_collision(pong_game_t *pong_game)
{
	paddle_t *p_paddle_left = &pong_game->paddle_left;
	paddle_t *p_paddle_right = &pong_game->paddle_right;
	ball_t *p_ball = &pong_game->ball;

	//printf("ball: %d %d %d %d %d\n", p_ball->pos_x, p_ball->pos_y, p_paddle_left->pos_x, (p_paddle_left->pos_y+(p_paddle_left->size_y/2)), (p_paddle_left->pos_y-(p_paddle_left->size_y/2)));

	/* LEFT PADDLE COLLISION */
	if((p_ball->pos_x <= (p_paddle_left->pos_x+(p_paddle_left->size_x/2))) &&
	   (p_ball->pos_y <= (p_paddle_left->pos_y+(p_paddle_left->size_y/2))) &&
	   (p_ball->pos_y >= (p_paddle_left->pos_y-(p_paddle_left->size_y/2))))
	{
		return LEFT_PADDLE_COLLISION;
	}
	/* RIGHT PADDLE COLLISION */
	else if((p_ball->pos_x >= (p_paddle_right->pos_x-(p_paddle_right->size_x/2))) &&
	   	    (p_ball->pos_y <= (p_paddle_right->pos_y+(p_paddle_right->size_y/2))) &&
	        (p_ball->pos_y >= (p_paddle_right->pos_y-(p_paddle_right->size_y/2))))
	{
		return RIGHT_PADDLE_COLLISION;
	}
	/* UPPER COLLISION */
	else if(p_ball->pos_y <= 0)
	{
		return UPPER_COLLISION;
	}
	/* BOTTOM COLLISION */
	else if(p_ball->pos_y >= DISPLAY_HEIGHT)
	{
		return BOTTOM_COLLISION;
	}
	/* BALL out of game on the left side */
	else if(p_ball->pos_x <= 0)
	{
		return LEFT_COLLISION;
	}
	/* BALL out of game on the right side */
	else if(p_ball->pos_x >= DISPLAY_WIDTH)
	{
		return RIGHT_COLLISION;
	}

	return 0;
}

void pong_paddle_ai(pong_game_t *pong_game, int paddle)
{
	ball_t *p_ball = &pong_game->ball;

	if((p_ball->mov_x < 0) && (paddle == PADDLE_LEFT))
	{
		if((p_ball->mov_y > 0) && (p_ball->pos_y >= pong_game->paddle_left.pos_y))
			down_paddle(&pong_game->paddle_left);
		else if(p_ball->mov_y < 0 && (p_ball->pos_y <= pong_game->paddle_left.pos_y))
			up_paddle(&pong_game->paddle_left);
	}

	if((p_ball->mov_x > 0) && (paddle == PADDLE_RIGHT))
	{
		if((p_ball->mov_y > 0) && (p_ball->pos_y >= pong_game->paddle_right.pos_y))
			down_paddle(&pong_game->paddle_right);
		else if(p_ball->mov_y < 0 && (p_ball->pos_y <= pong_game->paddle_right.pos_y))
			up_paddle(&pong_game->paddle_right);
	}
}

void pong_game_loop(pong_game_t *pong_game, int left_player_input)
{
	int collision = 0;

	paddle_t *p_paddle_left = &pong_game->paddle_left;
	paddle_t *p_paddle_right = &pong_game->paddle_right;
	ball_t *p_ball = &pong_game->ball;

	go_ball(&pong_game->ball);

	if(left_player_input == INPUT_DOWN)
		down_paddle(p_paddle_left);
	else if(left_player_input == INPUT_UP)
		up_paddle(p_paddle_left);

	//pong_paddle_ai(pong_game, PADDLE_LEFT);
	pong_paddle_ai(pong_game, PADDLE_RIGHT);

	collision = pong_game_check_collision(pong_game);
	if(collision == LEFT_PADDLE_COLLISION)
	{
		set_ball_vector(&pong_game->ball, (BALL_SPEED+pong_game->speed_up), pong_game->ball.mov_y);
		pong_game->speed_up++;
	}
	else if(collision == RIGHT_PADDLE_COLLISION)
	{
		set_ball_vector(&pong_game->ball, -(BALL_SPEED+pong_game->speed_up), pong_game->ball.mov_y);	
		pong_game->speed_up++;
	}
	else if(collision == UPPER_COLLISION)
	{
		set_ball_vector(&pong_game->ball, pong_game->ball.mov_x, BALL_SPEED);	
	}
	else if(collision == BOTTOM_COLLISION)
	{
		set_ball_vector(&pong_game->ball, pong_game->ball.mov_x, -BALL_SPEED);	
	}
	else if(collision == LEFT_COLLISION)
	{
		pong_game->points_right++;
		pong_game->speed_up = 0;
		init_ball(&pong_game->ball, (DISPLAY_WIDTH/2), DISPLAY_HEIGHT/2, BALL_SIZE, BALL_SPEED);
		set_ball_vector(&pong_game->ball, BALL_SPEED, -1);
	}
	else if(collision == RIGHT_COLLISION)
	{
		pong_game->points_left++;
		pong_game->speed_up = 0;
		init_ball(&pong_game->ball, (DISPLAY_WIDTH/2), DISPLAY_HEIGHT/2, BALL_SIZE, BALL_SPEED);
		set_ball_vector(&pong_game->ball, -BALL_SPEED, 1);
	}

	draw_paddle(p_paddle_right);
	draw_paddle(p_paddle_left);
	draw_ball(p_ball);
}


int main()
{
	al_init();
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_primitives_addon();

	int key_input = INPUT_NONE;
	int redraw = 1;
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_FONT *font = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_EVENT ev;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_KEYBOARD_STATE keyState;

	char tmp_buffer[16];

	if(!al_install_keyboard()) {
		fprintf(stderr, "failed to initialize the keyboard!\n");
		return -1;
	}

	timer = al_create_timer(1.0 / FPS);
	if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	font = al_create_builtin_font();
	if(!font) {
		fprintf(stderr, "failed to create font!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	event_queue = al_create_event_queue();
	if(!event_queue) {
	  fprintf(stderr, "failed to create event_queue!\n");
	  al_destroy_font(font);
	  al_destroy_display(display);
	  al_destroy_timer(timer);
	  return -1;
	}

	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_clear_to_color(al_map_rgb(0,0,0));
    al_flip_display();

	al_start_timer(timer);

	pong_game_t pong_game;
	pong_game_init(&pong_game);

	while (true) {
        al_wait_for_event(event_queue, &ev);
		if(ev.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;
		}
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}


		if(redraw && al_is_event_queue_empty(event_queue)) {
			redraw = 0;
			al_clear_to_color(al_map_rgb(0, 0, 0));
			snprintf(tmp_buffer, 16, "%d : %d", pong_game.points_left, pong_game.points_right);
			al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, tmp_buffer);

			al_get_keyboard_state(&keyState);
			if (al_key_down(&keyState, ALLEGRO_KEY_UP))
				key_input = INPUT_UP;
			else if (al_key_down(&keyState, ALLEGRO_KEY_DOWN))
				key_input = INPUT_DOWN;
			else key_input = INPUT_NONE;
			
			pong_game_loop(&pong_game, key_input);

			al_flip_display();
		}
	}

	al_destroy_font(font);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}
