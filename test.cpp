#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 420
#define SCREEN_HEIGHT 520

#define COLUMN 6
#define ROW 5

bool checkCollision(SDL_Rect rect, SDL_Point center, int rad)
{
	SDL_Rect cirRect = {center.x - rad, center.y - rad, rad * 2, rad * 2};
	if(rect.y + rect.h < cirRect.y)
		return false;
	else if(rect.y > cirRect.y + cirRect.h)
		return false;
	else if(rect.x + rect.w < cirRect.x)
		return false;
	else if(rect.x > cirRect.x + cirRect.w)
		return false;

	return true;
}

struct Brick
{
	SDL_Rect rect;
	bool isAlive;
};

int main(int argc, char* argv[])
{
	SDL_Event event;
	SDL_Rect rect;
	SDL_Point paddlePos = {280, 468};
	SDL_Point cirCenter = {315, 458};
	struct Brick bricks[ROW * COLUMN];
	float rad = 9.0f;
	float blockWidth = SCREEN_WIDTH / COLUMN;
	float blockHeight = 26;
	int speed = 5;
	bool isRunning = true;
	int vX = 1, vY = -1;
	int score = 0;
	char scoreText[16];
	sprintf(scoreText, "Score : %-4d", score);

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		perror("could not initialize sdl2\n");
		return 1;
	}

	TTF_Init();
	TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/unfonts-core/UnDotum.ttf", 24);

	SDL_Window* window = SDL_CreateWindow("Breakout Game",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	for(int y = 0; y < ROW; ++y)
	{
		for(int x = 0; x < COLUMN; ++x)
		{
			rect.x = blockWidth * x;
			rect.y = 34 + blockHeight * y;
			rect.w = blockWidth;
			rect.h = blockHeight;
			bricks[x + y * COLUMN].rect = rect;
			bricks[x + y * COLUMN].isAlive = true;
		}
	}

	while(isRunning)
	{
		if(score == (COLUMN * ROW))
		{
			isRunning = false;
			speed = 3000;
			SDL_Log("You Win!!");
			sprintf(scoreText, "You Win!!");
		}

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		for(int y = 0; y < ROW; ++y)
		{
			for(int x = 0; x < COLUMN; ++x)
			{
				if(bricks[x + y * COLUMN].isAlive)
				{
					SDL_SetRenderDrawColor(renderer, 0, 0xAF, 0xAF, 0x5F);
					SDL_RenderFillRect(renderer, &bricks[x + y * COLUMN].rect);
					SDL_SetRenderDrawColor(renderer, 0x1F, 0x1F, 0x1F, 0x1f);
					SDL_RenderDrawRect(renderer, &bricks[x + y * COLUMN].rect);
				}
			}
		}

		rect.x = paddlePos.x;
		rect.y = paddlePos.y;
		rect.w = blockWidth;
		rect.h = 20;
		SDL_SetRenderDrawColor(renderer, 0, 0, 0xAF, 0x5F);
		SDL_RenderFillRect(renderer, &rect);
		SDL_SetRenderDrawColor(renderer, 0x1F, 0x1F, 0x1F, 0x1F);
		SDL_RenderDrawRect(renderer, &rect);
		
		SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0x1F);
		for(float w = 0; w < rad * 2; ++w)
		{
			for(float h = 0; h < rad * 2; ++h)
			{
				float dx = rad - w;
				float dy = rad - h;
				if((dx * dx + dy * dy) <= (rad * rad))
				{
					SDL_RenderDrawPoint(renderer, cirCenter.x + dx, cirCenter.y + dy);
				}
			}
		}

		cirCenter.x += vX;
		cirCenter.y += vY;

		if((cirCenter.x - rad) <= 0)
		{
			cirCenter.x = rad;
			vX *= -1;
		}
		else if((cirCenter.x + rad) >= SCREEN_WIDTH)
		{
			cirCenter.x = SCREEN_WIDTH - rad;
			vX *= -1;
		}
		if((cirCenter.y - rad) <= 0)
		{
			cirCenter.y = rad;
			vY *= -1;
		}
		else if((cirCenter.y + rad) >= SCREEN_HEIGHT)
		{
			sprintf(scoreText, "GAME OVER!!");
			isRunning = false;
			speed = 3000;
		}

		if(checkCollision(rect, cirCenter, rad))
		{
			vY = -1;
		}

		for(int y = 0; y < ROW; ++y)
		{
			for(int x = 0; x < COLUMN; ++x)
			{
				if(checkCollision(bricks[x + y * COLUMN].rect, cirCenter, rad) && bricks[x + y * COLUMN].isAlive)
				{
					vY *= -1;
					score++;
					bricks[x + y * COLUMN].isAlive = false;
					SDL_Log("Score : %d", score);
					sprintf(scoreText, "Score : %-4d", score);
				}
			}
		}

		SDL_Color textColor = {0x1F, 0x1F, 0};
		SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText, textColor);
		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		int texW, texH;
		SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
		SDL_Rect dstrect = {10, 1, texW, texH};
		SDL_RenderCopy(renderer, texture, NULL, &dstrect);
		SDL_FreeSurface(surface);
		SDL_DestroyTexture(texture);

		while(SDL_PollEvent(&event))
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					isRunning = false;
					break;
				case SDLK_LEFT:
					paddlePos.x -= speed;
					break;
				case SDLK_RIGHT:
					paddlePos.x += speed;
					break;
			}

			switch(event.type)
			{
				case SDL_QUIT:
					isRunning = false;
					break;
				case SDL_MOUSEMOTION:
					paddlePos.x = event.motion.x - blockWidth / 2;
					break;
			}

			if((paddlePos.x) <= 0)
			{
				paddlePos.x = 0;
			}
			else if((paddlePos.x + blockWidth) >= SCREEN_WIDTH)
			{
				paddlePos.x = SCREEN_WIDTH - blockWidth;
			}
		}

		SDL_RenderPresent(renderer);
		SDL_Delay(speed);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	TTF_CloseFont(font);
	TTF_Quit();

	SDL_Quit();

	return 0;
}
