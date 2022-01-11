#pragma once

// 色定数
const int WHITE = GetColor(255, 255, 255);
const int BLACK = GetColor(0, 0, 0);
const int RED = GetColor(255, 0, 0);

template <typename Type>
void DrawDebugNum(Type num, int y = 0, const char* format = "%d")
{
	if (format == nullptr) { return; }
	DrawFormatString(0, y, BLACK, format, num);
}

// 連番出力で一列に画像が並んでいる時用のリソース読み込み関数
void LoadDivGraphLine(const char* fileName, int xNum, int xSize, int ySize, int* handleArray)
{
	if (fileName == nullptr || handleArray == nullptr)
	{
		return;
	}

	LoadDivGraph(fileName, xNum, xNum, 1, xSize, ySize, handleArray);
}

// 数をループさせる関数
void NumberLoop(int& num, int max, int min)
{
	if (num >= max+1) { num = min; }
	if (num < min) { num = max; }
}

// ただの2乗の関数
int Square(int num)
{
	return num * num;
}

// 引数numがminより小さければminを返す。
// 引数numがmaxより大きければmaxを返す。
// それ以外はnumを返す。
void Clamp(int& num, int min, int max)
{
	if (num < min) { num = min; }
	if (num > max) { num = max; }
}

// -1(false)か1(true)か
// 条件によって符号を変えたい時に使う
int NewFlag(bool flag)
{
	if (!flag) { return -1; }
	return 1;
}

float VCross2(VECTOR vec1, VECTOR vec2)
{
	return vec1.x * vec2.y - vec1.y * vec2.x;
}

void PlaySoundMemVer2(int soundHandle)
{
	if (!CheckSoundMem(soundHandle)) { PlaySoundMem(soundHandle, DX_PLAYTYPE_BACK); }
}