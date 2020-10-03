#include <genesis.h>
#include <resources.h>
#include <string.h>

const int LEFT_EDGE = 0;
const int RIGHT_EDGE = 320;
const int TOP_EDGE = 0;
const int BOTTOM_EDGE = 224;

Sprite* ball;
int ball_pos_x = 100;
int ball_pos_y = 100;
int ball_vel_x = 1;
int ball_vel_y = 1;
int ball_width = 8;
int ball_height = 8;

Sprite* player;
int player_pos_x = 144;
const int player_pos_y = 200;
int player_vel_x = 0;
const int player_width = 32;
const int player_half = 16;
const int player_height = 8;

/*Score variables*/
int score = 0;
char label_score[6] = "SCORE\0";
char str_score[3] = "0";

game_on = FALSE;
char msg_start[22] = "PRESS START TO BEGIN!\0";
char msg_reset[37] = "GAME OVER! PRESS START TO PLAY AGAIN.";

//Flashing stuff
int flashing = FALSE;
int frames = 0;
int ball_color = 0;

int sign(int x) {
    return (x > 0) - (x < 0);
}

void myJoyHandler( u16 joy, u16 changed, u16 state)
{
	if (joy == JOY_1)
	{
		//Start the game when START is pressed
		if(state & BUTTON_START){
			if(!game_on){
				startGame();
			}
		}
		/*Set player velocity if left or right are pressed;
		 *set velocity to 0 if no direction is pressed */
		if (state & BUTTON_RIGHT)
		{
			player_vel_x = 3;
		}
		else if (state & BUTTON_LEFT)
		{
			player_vel_x = -3;
		} else{
			if( (changed & BUTTON_RIGHT) | (changed & BUTTON_LEFT) ){
				player_vel_x = 0;
			}
		}
	}
}

void positionPlayer(){
	/*Add the player's velocity to its position*/
	player_pos_x += player_vel_x;

	/*Keep the player within the bounds of the screen*/
	if(player_pos_x < LEFT_EDGE) player_pos_x = LEFT_EDGE;
	if(player_pos_x + player_width > RIGHT_EDGE) player_pos_x = RIGHT_EDGE - player_width;

	/*Let the Sprite engine position the sprite*/
	SPR_setPosition(player,player_pos_x,player_pos_y);
}

void moveBall(){
	//Check horizontal bounds
	if(ball_pos_x < LEFT_EDGE){
		ball_pos_x = LEFT_EDGE;
		ball_vel_x = -ball_vel_x;
	} else if(ball_pos_x + ball_width > RIGHT_EDGE){
		ball_pos_x = RIGHT_EDGE - ball_width;
		ball_vel_x = -ball_vel_x;
	}

	//Check vertical bounds
	if(ball_pos_y < TOP_EDGE){
		ball_pos_y = TOP_EDGE;
		ball_vel_y = -ball_vel_y;
	} else if(ball_pos_y + ball_height > BOTTOM_EDGE){
		endGame();
	}

	//Check for collision with paddle
	if(ball_pos_x < player_pos_x + player_width && ball_pos_x + ball_width > player_pos_x){
		if(ball_pos_y < player_pos_y + player_height && ball_pos_y + ball_height >= player_pos_y){
			
			//On collision, invert the velocity
			ball_pos_y = player_pos_y - ball_height - 1;
			ball_vel_y = -ball_vel_y;

			score++;
			updateScoreDisplay();

			if( score % 10 == 0){
				ball_vel_x += sign(ball_vel_x);
				ball_vel_y += sign(ball_vel_y);
			}

			//Make ball flash
			flashing = TRUE;
		}
	}

	//Position the ball
	ball_pos_x += ball_vel_x;
	ball_pos_y += ball_vel_y;

	SPR_setPosition(ball,ball_pos_x,ball_pos_y);
}

void updateScoreDisplay(){
	sprintf(str_score,"%d",score);
	VDP_clearText(1,2,3);
	VDP_drawText(str_score,1,2);
}

void showText(char s[]){
	VDP_drawText(s, 20 - strlen(s)/2 ,15);
}

void startGame(){

	score = 0;
	updateScoreDisplay();

	ball_pos_x = 0;
	ball_pos_y = 0;

	ball_vel_x = 1;
	ball_vel_y = 1;

	/*Clear the text from the screen*/
	VDP_clearTextArea(0,10,40,10);

	game_on = TRUE;
}

void endGame(){
	showText(msg_reset);
	game_on = FALSE;
}

int main()
{
	//Set up the controls
	JOY_init();
	JOY_setEventHandler( &myJoyHandler );

	//Load our tileset
	VDP_loadTileSet(bgtile.tileset,1,DMA);
	VDP_setPalette(PAL1, bgtile.palette->data);
	VDP_fillTileMapRect(BG_A,TILE_ATTR_FULL(PAL1,0,FALSE,FALSE,1),0,0,40,30);

	/*Draw the texts*/
	VDP_setTextPlane(BG_B);
	VDP_drawText(label_score,1,1);
	updateScoreDisplay();
	showText(msg_start);

	SPR_init(0,0,0);
	player = SPR_addSprite(&paddle, player_pos_x, player_pos_y, TILE_ATTR(PAL1, 0, FALSE, FALSE));
	ball = SPR_addSprite(&imgball, 100,100,TILE_ATTR(PAL1,0, FALSE, FALSE));

	//Store the ball color
	ball_color = VDP_getPaletteColor(22);

	while(1)
	{
		//Do cool stuff in a loop!
		if(game_on == TRUE){
			moveBall();
			positionPlayer();

			//Handle the flashing of the ball
			if( flashing == TRUE ){
				//Count frames
				frames++;

				//Leave the color for 2 frames, then toggle it
				if( frames % 4 == 0 ){
					VDP_setPaletteColor(22,ball_color);
				} else if( frames % 2 == 0){
					VDP_setPaletteColor(22,RGB24_TO_VDPCOLOR(0xffffff));
				}

				//After 30 frames stop flashing
				if(frames > 30){
					flashing = FALSE;
					frames = 0;
					VDP_setPaletteColor(22,ball_color);
				}
			}

			
			
		}
		SPR_update();
		VDP_waitVSync();


		
	}

	//Stop doing cool stuff :(
	return(0);
}

