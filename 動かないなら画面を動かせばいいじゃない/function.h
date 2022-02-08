#pragma once
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
void NumberLoop(int& num, int max, int min = 0)
{
	if (num > max) { num = min; }
	if (num < min) { num = max; }
}

// ただの2乗の関数
int Square(int num)
{
	return num * num;
}

// 配列の0番目から配列のn番目までの合計
// 配列全体の合計を求めるには n に arrayNum の値を入れる
int Sum(int* array, size_t arrayNum, size_t n)
{
	if (array == nullptr || arrayNum < n) { return (int)nullptr; }

	int sum = 0;

	for (size_t i = 0; i < n; i++)
	{
		sum += array[i];
	}

	return sum;
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

void PlaySoundMemLoop(int soundHandle)
{
	if (!CheckSoundMem(soundHandle)) { PlaySoundMem(soundHandle, DX_PLAYTYPE_LOOP); }
}

void PlaySoundMemBack(int soundHandle)
{
	PlaySoundMem(soundHandle, DX_PLAYTYPE_BACK);
}

void PlaySoundMemBack2(int soundHandle)
{
	if (!CheckSoundMem(soundHandle)) { PlaySoundMem(soundHandle, DX_PLAYTYPE_BACK); }
}

void DrawTime(int x,int y,int time,int fontHandle)
{
	if (time <= 60000)
	{
		time = 60000 - time;
		int s = (time % 60000) / 1000;
		int cs = (time % 1000) / 10;

		DrawFormatStringToHandle(x, y, GetColor(255, 255, 255), fontHandle, "TIME\n%02d.%02d", s, cs);
	}
	else
	{
		DrawFormatStringToHandle(x, y, GetColor(255, 255, 255), fontHandle, "TIME\n00:00");
	}
}