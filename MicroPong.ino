#include <MicroView.h>

const int sensorPin = A1;
const float sensorMaxValue = 1024.0;

const int renderDelay = 16; // About 60hz
const int startDelay = 4000;
const int gameOverDelay = 6000;

const int scoreToWin = 10;
int playerScore = 0;
int enemyScore = 0;

const float paddleWidth = LCDWIDTH/16.0;
const float paddleHeight = LCDHEIGHT/3.0;
const float halfPaddleWidth = paddleWidth/2.0;
const float halfPaddleHeight = paddleHeight/2.0;

float playerPosX = 1.0 + halfPaddleWidth;
float playerPosY = 0.0;
float enemyPosX = LCDWIDTH - 1.0 - halfPaddleWidth;
float enemyPosY = 0.0;
float enemyVelY = 0.5;

const float ballRadius = 2.0;
const float ballSpeedX = 1.0;
float ballPosX = LCDWIDTH/2.0;
float ballPosY = LCDHEIGHT/2.0;
float ballVelX = -1.0 * ballSpeedX;
float ballVelY = 0;

void setup()
{
	initializeGraphics();
 	initializeInput();
	displayGameStart();
}

void resetGame()
{
	enemyScore = 0;
	playerScore = 0;
	enemyPosY = 0.0;
	ballPosX = LCDWIDTH/2.0;
	ballPosY = LCDHEIGHT/2.0;
	ballVelX = -1.0 * ballSpeedX;
	ballVelY = 0.0;
}

void initializeGraphics()
{
	uView.begin();
	uView.setFontType(1);
}

void initializeInput()
{
	digitalWrite(sensorPin, HIGH);
	pinMode(sensorPin, INPUT);
}

void displayGameStart()
{
	uView.clear(PAGE);
	renderString(20,10, "Get");
	renderString(10,30, "Ready!");
	uView.display();
	delay(startDelay);
}

void loop()
{
	updateGame();
	renderGame();
	
	if (playerScore >= scoreToWin)
	{
		gameOver(true);
	}
	else if (enemyScore >= scoreToWin)
	{
		gameOver(false);
	}
}

void updateGame()
{
	updatePlayer();
	updateEnemy();
	updateBall();
}

float clampPaddlePosY(float paddlePosY)
{
	float newPaddlePosY = paddlePosY;
	
	if (paddlePosY - halfPaddleHeight < 0)
	{
		newPaddlePosY = halfPaddleHeight;
	}
	else if (paddlePosY + halfPaddleHeight > LCDHEIGHT)
	{
		newPaddlePosY = LCDHEIGHT - halfPaddleHeight;
	}
	
	return newPaddlePosY;
}

void updatePlayer()
{
	float knobValue = analogRead(sensorPin) / sensorMaxValue;
	playerPosY = clampPaddlePosY(knobValue * LCDHEIGHT);
}

void updateEnemy()
{
	// Follow the ball at a set speed
	if (enemyPosY < ballPosY)
	{
		enemyPosY += enemyVelY;
	}
	else if(enemyPosY > ballPosY)
	{
		enemyPosY -= enemyVelY;
	}
	
	enemyPosY = clampPaddlePosY(enemyPosY);
}

void updateBall()
{
	ballPosY += ballVelY;
	ballPosX += ballVelX;
	
	// Top and bottom wall collisions
	if (ballPosY < ballRadius)
	{
		ballPosY = ballRadius;
		ballVelY *= -1.0;
	}
	else if (ballPosY > LCDHEIGHT - ballRadius)
	{
		ballPosY = LCDHEIGHT - ballRadius;
		ballVelY *= -1.0;
	}
	
	// Left and right wall collisions
	if (ballPosX < ballRadius)
	{
		ballPosX = ballRadius;
		ballVelX = ballSpeedX;
		enemyScore++;
	}
	else if (ballPosX > LCDWIDTH - ballRadius)
	{
		ballPosX = LCDWIDTH - ballRadius;
		ballVelX *= -1.0 * ballSpeedX;
		playerScore++;
	}
	
	// Paddle collisions
	if (ballPosX < playerPosX + ballRadius + halfPaddleWidth)
	{
		if (ballPosY > playerPosY - halfPaddleHeight - ballRadius && 
			ballPosY < playerPosY + halfPaddleHeight + ballRadius)
		{
			ballVelX = ballSpeedX;
			ballVelY = 2.0 * (ballPosY - playerPosY) / halfPaddleHeight;
		}
	}
	else if (ballPosX > enemyPosX - ballRadius - halfPaddleWidth)
	{
		if (ballPosY > enemyPosY - halfPaddleHeight - ballRadius && 
			ballPosY < enemyPosY + halfPaddleHeight + ballRadius)
		{
			ballVelX = -1.0 * ballSpeedX;
			ballVelY = 2.0 * (ballPosY - enemyPosY) / halfPaddleHeight;
		}
	}
}

void renderGame()
{
	uView.clear(PAGE);
	
	renderScores(playerScore, enemyScore);
	renderPaddle(playerPosX, playerPosY);
	renderPaddle(enemyPosX, enemyPosY);
	renderBall(ballPosX, ballPosY);

	uView.display();
	delay(renderDelay);
}

void renderString(int x, int y, String string)
{
	uView.setCursor(x,y);
	uView.print(string);
}

void renderPaddle(int x, int y)
{
	uView.rect(
		x - halfPaddleWidth, 
		y - halfPaddleHeight, 
		paddleWidth, 
		paddleHeight);
}

void renderBall(int x, int y)
{
	uView.circle(x, y, 2);	
}

void renderScores(int firstScore, int secondScore)
{
	renderString(10, 0, String(firstScore));
	renderString(LCDWIDTH - 14, 0, String(secondScore));
}

void gameOver(bool didWin)
{
	if (didWin)
	{
		renderString(20,10, "You");
		renderString(20,30, "Win!");
	}
	else
	{
		renderString(20,10, "You");
		renderString(15,30, "Lose!");
	}
	
	uView.display();
	delay(gameOverDelay);
	
	// Get ready to start the game again.
    resetGame();
	displayGameStart();
}
