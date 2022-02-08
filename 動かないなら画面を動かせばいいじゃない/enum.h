#pragma once
enum Scene
{
	TITLE, STAGE_SELECT, PLAY, CLEAR, GAMEOVER,
	SET, // énÇﬂÇÈÇΩÇﬂÇ…ïœêîÇèëÇ´ä∑Ç¶ÇÈ
};

enum MouseState
{
	START, END, OLD_END, NOW
};

enum ObjEnum
{
	PLAYER, FIELD, STAGE
};

enum EnemyType
{
	INIT = -1,
	NORMAL, HARD, TOUGH, FAST, STRONG,
	ENEMY_KINDS
};

enum Color
{
	BLACK,
	WHITE,
	RED,
	GREEN,
	YELLOW,
	CYAN,
	MAGENTA,
	GRAY,
	COLOR_NUM
};

enum HandState
{
	GU, PA, YUBI
};

enum Derection
{
	LEFT, UP, RIGHT, DOWN
};

enum Bg
{
	UCHU, MOKUME, UMI, UMI2
};