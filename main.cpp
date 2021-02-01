// main.cpp
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10 in C++

// Last Modified: 1/17/2020 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2017
 Copyright 2018 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// *******Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 

// fire button connected to PE0
// pause button connected to PE1
// 32*R resistor DAC bit 0 on PB0 (least significant bit)
// 16*R resistor DAC bit 1 on PB1
// 8*R resistor DAC bit 2 on PB2
// 4*R resistor DAC bit 3 on PB3 
// 2*R resistor DAC bit 4 on PB4
// 1*R resistor DAC bit 5 on PB5 (most significant bit)

// Heartbeat LED on PE2
// Player hit indicator LED on PE3

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "PLL.h"
#include "ST7735.h"
#include "Random.h"
#include "PLL.h"
#include "Images.h"
#include "UART.h"
#include "Timer0.h"
#include "Timer1.h"
#include "initializations.h"
#include "Sound.h"
#include "Sprite.h"
#include "SlidePot.h"


SlidePot my(1500,0);

extern "C" void DisableInterrupts(void);
extern "C" void EnableInterrupts(void);
extern "C" void SysTick_Handler(void);
int language = 0; // 0 is english, 1 is spanish
char* pressText[] = {(char*)"    Press Fire    ", (char*)"  Pulse al disparar"};
char* startText[] = {(char*)"   to Start...", (char*)"para iniciar..."};
char* startAgainText[] = {(char*)"   to Start Again...", (char*)"para iniciar otra..."};
char* livesText[] = {(char*)"Lives:", (char*)"Vidas:"};
char* pointsText[] = {(char*)"Points: ", (char*)"Puntos: "};
char* gameOverText[] = {(char*)"   GAME OVER", (char*)"  FIN DEL JUEGO"};
uint32_t lastFireTime = 0;
uint32_t lastPauseTime = 0;
int8_t playerMovement = 0;
#define MAX_ENEMIES 12
uint8_t numEnemies = 2;
Sprite Enemies[MAX_ENEMIES];
Sprite EnemyMissiles[MAX_ENEMIES];
Sprite Player;
Sprite Bunker[3];
#define MAX_MISSILES 10
Sprite PlayerMissiles[MAX_MISSILES];
uint8_t PlayerMissileIndex = 0;
uint32_t kills = 0;
uint8_t killsFlag = 1;
uint8_t livesFlag = 1;
uint8_t fireFlag = 0;
uint8_t hitFlag = 0;
const uint16_t* enemyImages[] = {SmallEnemy10pointA, SmallEnemy10pointB, SmallEnemy20pointA, SmallEnemy20pointB, SmallEnemy30pointA, SmallEnemy30pointB};
const uint16_t* extraPlayerImages[] = {PlayerShip2, PlayerShip1};
const uint16_t* extraBunkerImages[] = {Bunker2, Bunker1};
uint32_t time = 0;
uint32_t flag = 1;
uint8_t waveNumber = 0;
int enemiesAlive = 0;

void makeSprites() {
	waveNumber++;
	for (int i = 0; i < numEnemies; i++) {
		if(i < 6)
		{
		Enemies[i].init(20*i,28,enemyImages[Random()%6],Black,16,10,1,0);
		Enemies[i].setFireRate(waveNumber);
		enemiesAlive++;
		}
		else
		{
		Enemies[i].init(20*(i-6),45,enemyImages[Random()%6],Black,16,10,1,0);
		Enemies[i].setFireRate(waveNumber);
		enemiesAlive++;
		}
	}
	if (numEnemies < MAX_ENEMIES-1) {
		numEnemies++;
	}
	if(waveNumber % 5 == 0){
	Bunker[0].init(16, 151, Bunker0, BlackBunker, 18, 5, 0, 0); //bunkers respawning after every 5th wave
	Bunker[1].init(53, 151, Bunker0, BlackBunker, 18, 5, 0, 0);
	Bunker[2].init(90, 151, Bunker0, BlackBunker, 18, 5, 0, 0);
	}
}
void SysTick_Handler(void){
	if (killsFlag) { // if # of kills have changed, update scoreboard
		ST7735_SetCursor(8, 0);
		ST7735_OutUDec(kills);
		killsFlag = 0;
	} 
	if (livesFlag) { // if # of player lives have changed, update scoreboard
		ST7735_SetCursor(19, 0);
		ST7735_OutUDec(Player.getLives());
		livesFlag = 0;
	}
	Player.Move();
	Player.Draw();
	for(int i=0; i<3; i++){
	if(Bunker[i].isAlive()){
		Bunker[i].Redraw();
	}
	}
	for (int i = 0; i < MAX_MISSILES; i++) { // moves and draws each player missile
		if(PlayerMissiles[i].isAlive()) {
			PlayerMissiles[i].Move();
			for (int j = 0; j < numEnemies; j++) {
				if(PlayerMissiles[i].collisionCheck(Enemies[j])) {
					enemiesAlive--;
					Enemies[j].Draw();
					kills++;
					Sound_Killed();
					killsFlag = 1;
					break;
				}
			}
			PlayerMissiles[i].Draw();
		}
	}
	if (enemiesAlive) { // checks if any enemies are alive and moves and draws them 
		for (int i = 0; i < numEnemies; i++) {
			if (Enemies[i].isAlive()) {
				int8_t shoot = Enemies[i].Move();
				if (shoot < 0) { // this condition checks if move() returned a negative value which indicates game over
					flag = 0;
					return;
				}
				if (!Enemies[i].isAlive()) {
					enemiesAlive--;
				}
				// the following condition checks whether the enemy is supposed to shoot and whether it still has a missile alive
				if (shoot && !EnemyMissiles[i].isAlive()) { 
					EnemyMissiles[i].init(Enemies[i].getX(),Enemies[i].getY(),Missile,BlackMissile,2,6,1,1);
					Sound_Shoot();
				}
				Enemies[i].Draw();
			}
			if (EnemyMissiles[i].isAlive()) { // moves and draws each enemy missile
				EnemyMissiles[i].Move();
				if (EnemyMissiles[i].collisionCheck(Player) && Player.getLives() > 0) {
					Player.setImage(extraPlayerImages[Player.getLives() - 1]);
					Sound_Explosion();
					livesFlag = 1;
					hitFlag = 1;
				}
				for(int j=0; j<3; j++){
				if (EnemyMissiles[i].collisionCheck(Bunker[j])) {
					if(Bunker[j].getLives() == 0){
						Bunker[j].setImage(BlackBunker);
						Bunker[j].Redraw(); //dead bunker so cover it up
					}
					else 
					{
						Bunker[j].setImage(extraPlayerImages[Bunker[j].getLives() - 1]);
					}
					Sound_Explosion();
				}
			}
				EnemyMissiles[i].Draw();
			}
		}
	}
	else { 
		makeSprites(); // this is executed when the player has killed all enemies on the screen
	}
	flag = Player.isAlive(); // if the player loses all his/her lives, the game ends
}

void clock(void){
  time++; //time is in ms 
}
int main(void){
	DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Output_Init();
  Timer1_Init(&clock,80000); // 1000 Hz
	generalIOInit();
	SysTick_Init();
	ADC_Init();
	
	ST7735_SetCursor(8, 4);
  ST7735_OutString((char*)"Select");
  ST7735_SetCursor(7, 5);
	ST7735_OutString((char*)"Language:");
	ST7735_SetCursor(8, 8);
	ST7735_OutString((char*)"English");
	ST7735_SetCursor(8, 10);
	ST7735_OutString((char*)"Espa\xA4ol");
	while (!(GPIO_PORTE_DATA_R&0x1)) {
		int slider = getMovementDirection();
		if (slider <= 0) {
			language = 0;
			ST7735_SetCursor(6, 8);
			ST7735_OutString((char*)">");
			ST7735_SetCursor(6, 10);
			ST7735_OutString((char*)" ");
		}
		else {
			language = 1;
			ST7735_SetCursor(6, 10);
			ST7735_OutString((char*)">");
			ST7735_SetCursor(6, 8);
			ST7735_OutString((char*)" ");
		}
	}
	while (GPIO_PORTE_DATA_R&0x1); // wait for user to let go
	ST7735_FillScreen(0x0000);
	ST7735_SetCursor(8, 4);
  ST7735_OutString((char*)"SPACE");
  ST7735_SetCursor(6, 5);
	ST7735_OutString((char*)"INVADERS");
	ST7735_SetCursor(7, 6);
	ST7735_OutString((char*)"Remake");
	ST7735_SetCursor(0, 9);
	ST7735_OutString(pressText[language]);
	ST7735_SetCursor(3, 10);
	ST7735_OutString(startText[language]);
	while (!(GPIO_PORTE_DATA_R&0x1));
	while (GPIO_PORTE_DATA_R&0x1); // wait for user to let go
	while(1){
	Random_Init(NVIC_ST_CURRENT_R);
	ST7735_FillScreen(0x0000);
	Player.init(53, 159, PlayerShip0, BlackPlayer, 18, 8, 0, 0);
	Player.Draw();
	Bunker[0].init(16, 151, Bunker0, BlackBunker, 18, 5, 0, 0);
	Bunker[1].init(53, 151, Bunker0, BlackBunker, 18, 5, 0, 0);
	Bunker[2].init(90, 151, Bunker0, BlackBunker, 18, 5, 0, 0);
	for(int i=0; i<3; i++){
		Bunker[i].Draw();
	}
	flag = 1;
	kills = 0;
	killsFlag = 1;
	livesFlag = 1;
	time = 0;
	enemiesAlive = 0;
	numEnemies = 1;
	waveNumber = 0;
	makeSprites();
	ST7735_SetCursor(0, 0);
	ST7735_OutString(pointsText[language]);
	ST7735_SetCursor(12, 0);
	ST7735_OutString(livesText[language]);
	EnableInterrupts();
	while(flag){
		if (fireFlag == 1) {
			uint32_t firePressTime = time;
			GPIO_PORTE_DATA_R |= 0x4;
			while(time - firePressTime <= 100){ //press fire button and LED stays on for 0.1 seconds
				__nop();
			}
			fireFlag = 0;
		}
		GPIO_PORTE_DATA_R &= ~0x4; //fire LED off otherwise
		if (hitFlag == 1) {
			uint32_t hitTime = time;
			GPIO_PORTE_DATA_R |= 0x8;
			while(time - hitTime <= 500){ //hit indicator LED stays on for 0.5 seconds
				__nop();
			}
			hitFlag = 0;
		}
		GPIO_PORTE_DATA_R &= ~0x8; //hit LED off otherwise
	}
	DisableInterrupts();
	for(int i=0; i<numEnemies; i++){ //game is over, kill everything off
		Enemies[i].hit();
	}
  ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_SetCursor(3, 6);
  ST7735_OutString(gameOverText[language]);
  ST7735_SetCursor(5, 8);
  ST7735_SetTextColor(ST7735_WHITE);
  ST7735_OutString(pointsText[language]);
	ST7735_OutUDec(kills);
	ST7735_SetCursor(1, 9);
	ST7735_OutString(pressText[language]);
	ST7735_SetCursor(0, 10);
	ST7735_OutString(startAgainText[language]);
	while (!(GPIO_PORTE_DATA_R&0x1));
	while (GPIO_PORTE_DATA_R&0x1); // wait for user to let go and then start the game over again
}
}
extern "C" void GPIOE_Handler(void){
	uint32_t interruptCode = GPIO_PORTE_RIS_R;
	if((interruptCode & 0x1) == 0x1){
		GPIO_PORTE_ICR_R = 0x1;
		if(time - lastFireTime >= 250)
		{
			lastFireTime = time;
			fireFlag = 1;
			PlayerMissiles[PlayerMissileIndex].init(Player.getX(),Player.getY(),Missile,BlackMissile,2,6,0,1);
			PlayerMissileIndex = (PlayerMissileIndex+1)%10;
			Sound_Shoot();
		}
	}
	else if((interruptCode & 0x2) == 0x2){
		GPIO_PORTE_ICR_R = 0x2;
		if(time - lastPauseTime >= 100)
		{
			lastPauseTime = time;
			DisableInterrupts();
			while(GPIO_PORTE_DATA_R == 0x2){
				__nop(); //wait for player to release the pause button
			}
			while(GPIO_PORTE_DATA_R != 0x2){
				__nop(); //wait for player to press the pause button again to unpause
			}
			EnableInterrupts();
		}
	}
	else{
		GPIO_PORTE_ICR_R = 0x3;
	}
}
