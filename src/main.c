#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include "rlgl.h"
#include <stdint.h>
#include <string.h>
#include "raymath.h"

#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

void UpdateDrawFrame(void);
Texture2D GenCollisionMask(Texture2D s);

Texture2D aTex, bTex, aMask, bMask;
Vector2 aPos, bPos;
Vector2 aLocalPos, bLocalPos;
Camera2D camera = {0};

bool boxCollision = false;
bool pixelCollision = false;
Rectangle overlap = {0};

const int screenWidth = 240;
const int screenHeight = 135;
float rows, columns;

int main(void)
{
	InitWindow(screenWidth, screenHeight, "Pixel Perfect Collision");
	camera.zoom = 1.f;

#if defined(PLATFORM_WEB)
	emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
	SetTargetFPS(60);
	aTex = LoadTexture("assets/skull.png");
	bTex = LoadTexture("assets/rock.png");
	aMask = GenCollisionMask(aTex);
	bMask = GenCollisionMask(bTex);

	aPos = (Vector2){screenWidth/4.f - aTex.width/2.f,
		screenHeight/2.f - aTex.height/2.f};
	bPos = (Vector2){screenWidth/2.f - bTex.width/2.f,
		screenHeight/2.f - bTex.height/2.f};

	while (!WindowShouldClose())
	{
		UpdateDrawFrame();
	}
#endif
	CloseWindow();

	return 0;
}

Texture2D GenCollisionMask(Texture2D s)
{
	Image image = LoadImageFromTexture(s);
	rlPixelFormat inFmt = RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32;
	rlPixelFormat outFmt = RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
	Vector4 *pixels = rlReadTexturePixels(s.id, s.width, s.height, inFmt);
	int nPixels = s.width * s.height;

	free(image.data);
	image.data = NULL;
	image.format = outFmt;

	int bufSize = nPixels * sizeof(uint8_t);
	image.data = malloc(bufSize);

	uint8_t *outBuf = malloc(bufSize);
	for(int i = 0; i < nPixels; ++i) {
		if(pixels[i].w < 0.1f) {
			outBuf[i] = (uint8_t)0;
		} else {
			outBuf[i] = (uint8_t)255;
		}
	}
	memcpy(image.data, outBuf, bufSize);
	free(outBuf);

	Texture2D c = LoadTextureFromImage(image);
	free(image.data);
	free(pixels);
	return c;
}

void UpdateDrawFrame(void)
{
	// Update
	//----------------------------------------------------------------------------------
	float scaleFactor = (GetScreenWidth()/(float)screenWidth);
	camera.zoom = scaleFactor;
	overlap = (Rectangle){0};
	pixelCollision = false;
	Vector2 collisionPixel = {0};

	// quick movement stuff
	if(IsKeyDown(KEY_W)) { aPos.y -= 1; }
	if(IsKeyDown(KEY_S)) { aPos.y += 1; }
	if(IsKeyDown(KEY_D)) { aPos.x += 1; }
	if(IsKeyDown(KEY_A)) { aPos.x -= 1; }

	Rectangle r1 = (Rectangle){aPos.x, aPos.y, aTex.width, aTex.height};
	Rectangle r2 = (Rectangle){bPos.x, bPos.y, bTex.width, bTex.height};

	boxCollision = CheckCollisionRecs(r1, r2);
	if(boxCollision) {
		overlap = GetCollisionRec(r1, r2);
		rows = overlap.width;
		columns = overlap.height;
	} else {
		rows = 0;
		columns = 0;
	}

	// translating the overlap position to the corresponding starting points
	aLocalPos = (Vector2) {overlap.x - aPos.x, overlap.y - aPos.y};
	bLocalPos = (Vector2) {overlap.x - bPos.x, overlap.y - bPos.y};

	// getting the pixel data from the GPU
	uint8_t *aP = rlReadTexturePixels(aMask.id, aMask.width, aMask.height,
			RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);
	uint8_t *bP = rlReadTexturePixels(bMask.id, bMask.width, bMask.height,
			RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE);

	for(int y = 0; y < rows; ++y) {
		for(int x = 0; x < columns; ++x) {
			if(	aP[(int)((aLocalPos.x + x) + (aLocalPos.y + y) * aMask.width)] &&
				bP[(int)((bLocalPos.x + x) + (bLocalPos.y + y) * bMask.width)])
			{
				pixelCollision = true;
				collisionPixel = (Vector2){x, y};
				break;
			}
		}
		if(pixelCollision) { break; }
	}

	//----------------------------------------------------------------------------------

	// Draw
	//----------------------------------------------------------------------------------
	BeginDrawing();

	BeginMode2D(camera);
		ClearBackground(GetColor(0x2d2d2dFF));
		
		// Debug - drawing the generated masks
		DrawTexture(aMask, 0, 0, WHITE);
		DrawTexture(bMask, aMask.width, 0, WHITE);

		// Drawing the sprites
		DrawTextureV(bTex, bPos, WHITE);
		DrawRectangleLines(r2.x, r2.y, r2.width, r2.height, GRAY);

		DrawTextureV(aTex, aPos, WHITE);
		DrawRectangleLines(r1.x, r1.y, r1.width, r1.height, GRAY);

		Color c = pixelCollision == true ? GREEN : RED;

		// World overlap position
		DrawRectangleLinesEx(overlap, 1.f, c);

		// Local overlap positions
		DrawRectangleLines(aLocalPos.x, aLocalPos.y, overlap.width, overlap.height, c);
		DrawRectangleLines(bLocalPos.x + aMask.width, bLocalPos.y, overlap.width, overlap.height, c);

		if(pixelCollision) {
			// Drawing collision pixel against the 'a' sprite mask
			DrawRectangle(	collisionPixel.x + aLocalPos.x,
							collisionPixel.y + aLocalPos.y,
							1, 1, MAGENTA);

			// Drawing collision pixel against the 'b' sprite mask
			DrawRectangle(	collisionPixel.x + aMask.width + bLocalPos.x,
							collisionPixel.y + bLocalPos.y,
							1, 1, RED);

			// Drawing collision pixel realtive to the world
			DrawRectangle(	collisionPixel.x + overlap.x,
							collisionPixel.y + overlap.y,
							1, 1, YELLOW );
		}
	EndMode2D();

	EndDrawing();
	//----------------------------------------------------------------------------------
}
