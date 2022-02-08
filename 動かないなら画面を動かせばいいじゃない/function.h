#pragma once
// �A�ԏo�͂ň��ɉ摜������ł��鎞�p�̃��\�[�X�ǂݍ��݊֐�
void LoadDivGraphLine(const char* fileName, int xNum, int xSize, int ySize, int* handleArray)
{
	if (fileName == nullptr || handleArray == nullptr)
	{
		return;
	}

	LoadDivGraph(fileName, xNum, xNum, 1, xSize, ySize, handleArray);
}

// �������[�v������֐�
void NumberLoop(int& num, int max, int min = 0)
{
	if (num > max) { num = min; }
	if (num < min) { num = max; }
}

// ������2��̊֐�
int Square(int num)
{
	return num * num;
}

// �z���0�Ԗڂ���z���n�Ԗڂ܂ł̍��v
// �z��S�̂̍��v�����߂�ɂ� n �� arrayNum �̒l������
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

// ����num��min��菬�������min��Ԃ��B
// ����num��max���傫�����max��Ԃ��B
// ����ȊO��num��Ԃ��B
void Clamp(int& num, int min, int max)
{
	if (num < min) { num = min; }
	if (num > max) { num = max; }
}

// -1(false)��1(true)��
// �����ɂ���ĕ�����ς��������Ɏg��
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