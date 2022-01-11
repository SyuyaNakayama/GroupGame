#pragma once
enum Scene
{
	TITLE, HOW_TO_PLAY, PLAY, CLEAR, GAMEOVER
};

enum MouseState
{
	START, END, OLD_END,NOW
};

enum ObjEnum
{
	PLAYER, FIELD
};

enum EnemyState
{
	DEAD, SET
};

enum EnemyType
{
	INIT = -1,
	NORMAL,HARD,TOUGH,FAST,STRONG
};