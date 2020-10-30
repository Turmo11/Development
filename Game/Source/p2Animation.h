#ifndef __P2ANIMATION_H__
#define __P2ANIMATION_H__

#include "SDL/include/SDL_rect.h"
#include "p2Point.h"
#define MAX_FRAMES 25

#include "Application.h"
#include "PugiXml\src\pugixml.hpp"
#include "Map.h"
#include "p2List.h"
#include "p2SString.h"
#include "Render.h"
#include "Textures.h"
#include "p2Log.h"

class Animation
{
public:
	float speed = 1.0f;
	SDL_Rect frames[MAX_FRAMES];
	iPoint offset[MAX_FRAMES];

	bool loop = true;
	float current_frame;
	int last_frame = 0;


	bool lock = false;

	

private:
	int loops = 0;
public:

	bool DrawAnimation(const char* name)
	{
		App->map->data;


		return true;
	}


	void PushBack(const SDL_Rect& rect, int offsetX = 0, int offsetY = 0)
	{
		frames[last_frame] = rect;
		offset[last_frame].x = offsetX;
		offset[last_frame].y = offsetY;
		last_frame++;
	}

	//NEW-------------------------------------
	void SetOffset(int frame, int offsetX = 0, int offsetY = 0) {
		offset[frame].x = offsetX;
		offset[frame].y = offsetY;
	}

	void SetSpeed(float new_speed) {
		speed = new_speed;
	}
	//---------------------------


	SDL_Rect& GetCurrentFrame()
	{
		current_frame += speed;
		if (current_frame >= last_frame)
		{
			current_frame = (loop) ? 0.0f : last_frame - 1;
			if (loop)
				loops++;

		}
		return frames[(int)current_frame];
	}

	iPoint& GetOffset()
	{

		return offset[(int)current_frame];
	}

	bool Finished() const
	{
		return loops > 0;
	}

	void Reset()
	{
		current_frame = 0;
		loops = 0;
	}
};

#endif
