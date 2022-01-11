#pragma once

// �F�萔
const int WHITE = GetColor(255, 255, 255);
const int BLACK = GetColor(0, 0, 0);
const int RED = GetColor(255, 0, 0);

template <typename Type>
void DrawDebugNum(Type num, int y = 0, const char* format = "%d")
{
	if (format == nullptr) { return; }
	DrawFormatString(0, y, BLACK, format, num);
}

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
void NumberLoop(int& num, int max, int min)
{
	if (num >= max+1) { num = min; }
	if (num < min) { num = max; }
}

// ������2��̊֐�
int Square(int num)
{
	return num * num;
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

float VCross2(VECTOR vec1, VECTOR vec2)
{
	return vec1.x * vec2.y - vec1.y * vec2.x;
}

void PlaySoundMemVer2(int soundHandle)
{
	if (!CheckSoundMem(soundHandle)) { PlaySoundMem(soundHandle, DX_PLAYTYPE_BACK); }
}