#pragma once

// ���W�p�ϐ�
typedef struct Screen
{
	int x;
	int y;

	// calFlag == 0:�����Z calFlag == 1:�����Z 
	Screen AddAndSub(Screen Vec, bool calFlag)
	{
		Screen answer;

		if (!calFlag)
		{
			answer.x = x + Vec.x;
			answer.y = y + Vec.y;
		}
		else
		{
			answer.x = x - Vec.x;
			answer.y = y - Vec.y;
		}

		return answer;
	}
}Screen;

// �E�B���h�E�̃T�C�Y
const Screen WIN_SIZE = { 950,950 };

// �����A�j���[�V�����`��Ɏg���ϐ�
// ��{�I�ɂ� picNum �� timer �̒l��������������΂����B
class Anime
{
public:
	int picNum; // �摜�̖���
	int timer; // 1���̕`�掞��(�t���[��)
	int nextTimer = timer;	// �^�C�}�[�����l�̋L�^�p
	int animeNum = 0;	// ���݂̉摜�z����L�^���邽�߂̕ϐ�

	// �A�j���[�V�����X�V�֐�
	void Update()
	{
		timer--;
		NumberLoop(timer, nextTimer);

		if (timer == 0) { animeNum++; }

		NumberLoop(animeNum, picNum - 1);
	}
};

class Transform
{
public:
	Screen radius;
	Screen pos;

	bool BoxCollision(Transform obj)
	{
		return
			abs(pos.x - obj.pos.x) <= radius.x + obj.radius.x &&
			abs(pos.y - obj.pos.y) <= radius.y + obj.radius.y;
	}

	bool PointCollision(Screen pointPos)
	{
		return
			pos.x - radius.x <= pointPos.x && pos.y - radius.y <= pointPos.y &&
			pos.x + radius.x >= pointPos.x && pos.y + radius.y >= pointPos.y;
	}

	// �w��ʒu�Ŏ~�܂�
	void StopCliff(const Screen scrMin, const Screen scrMax, Screen& fieldScroll, Screen& objScroll, bool& isEdgeField)
	{
		// �E�[
		if (pos.x + radius.x + fieldScroll.x >= scrMax.x)
		{
			objScroll.x = pos.x + radius.x + fieldScroll.x - scrMax.x;
			isEdgeField = 1;
		}
		// ���[
		if (pos.x - radius.x + fieldScroll.x <= scrMin.x)
		{
			objScroll.x = pos.x - radius.x + fieldScroll.x - scrMin.x;
			isEdgeField = 1;
		}
		// ���[
		if (pos.y + radius.y + fieldScroll.y >= scrMax.y)
		{
			objScroll.y = pos.y + radius.y + fieldScroll.y - scrMax.y;
			isEdgeField = 1;
		}
		// ��[
		if (pos.y - radius.y + fieldScroll.y <= scrMin.y)
		{
			objScroll.y = pos.y - radius.y + fieldScroll.y - scrMin.y;
			isEdgeField = 1;
		}
		// �X�N���[���l���Z�b�g
		if (!isEdgeField)
		{
			objScroll = { 0,0 };
		}
	}

	void DrawGraphCenter(int grHandle, Screen scroll = { 0,0 }, Screen turnFlag = { 0,0 })
	{
		DrawExtendGraph(pos.x - radius.x * NewFlag(turnFlag.x == 0) - scroll.x, pos.y - radius.y * NewFlag(turnFlag.y == 0) - scroll.y,
			pos.x + radius.x * NewFlag(turnFlag.x == 0) - scroll.x, pos.y + radius.y * NewFlag(turnFlag.y == 0) - scroll.y, grHandle, 1);
	}

	// ���S�_�Ɣ��a�ɂ��w��
	void DrawBoxCenter(size_t color, Screen scroll = { 0,0 }, int fillFlag = 1)
	{
		DrawBox(pos.x - radius.x - scroll.x, pos.y - radius.y - scroll.y,
			pos.x + radius.x - scroll.x, pos.y + radius.y - scroll.y, color, fillFlag);
	}

	void DrawHpBar(int hp, int maxHp, size_t color, Screen scroll = { 0,0 })
	{
		DrawFillBox(pos.x - radius.x - scroll.x,
			pos.y - radius.y - scroll.y,
			(pos.x - radius.x) + radius.x *
			2 * (float)hp / (float)maxHp - scroll.x,
			pos.y + radius.y - scroll.y, color);
		DrawBoxCenter(GetColor(0, 0, 0), scroll, 0);
	}
};

class EnemyStatus
{
public:
	int spdCF; // CoeFficient:�W��
	int Atk, def, hp;
	int maxHp = hp;

	EnemyStatus(int enemyType)
	{
		// �G�X�e�[�^�X�e�[�u��
		// �s�͓G�̎�ށA��͊e�X�e�[�^�X��\��
		int statusTable[ENEMY_KINDS][4] =
		{
			{2,1,5,50},  // NORMAL
			{2,1,26,1},  // HARD
			{2,1,5,200}, // TOUGH
			{4,1,5,30},  // FAST
			{2,2,5,50}   // STRONG
		};

		switch (enemyType)
		{
			case NORMAL:
			case HARD:
			case TOUGH:
			case FAST:
			case STRONG:
				spdCF = statusTable[enemyType][0];
				Atk = statusTable[enemyType][1];
				def = statusTable[enemyType][2];
				hp = statusTable[enemyType][3];
				break;
			default:
				spdCF = 0;
				Atk = 0;
				def = 0;
				hp = 0;
				break;
		}
		maxHp = hp;
	}

	int CalDamage(Screen scrSpd)
	{
		int damage;

		damage = sqrtf(Square(scrSpd.x) + Square(scrSpd.y)) / def - def / 2;
		if (damage < 0) { damage = 0; }

		return damage;
	}
};

class Enemy
{
public:
	Transform transform;
	EnemyStatus* status;
	Screen spd;
	int waitTime;
	int isAlive = 0;
	int deathFrame = 0;
	int type = 0;

	Enemy()
	{
		transform.radius = { 32,32 };
		spd = {};
		status = new EnemyStatus(INIT);
	}

	void Respawn(const int WAIT_TIME, Screen scroll, Screen& enemyScroll, int enemyTypeRate[ENEMY_KINDS])
	{
		int RateSum = 0;
		int sum[ENEMY_KINDS];
		float angleRand = 0.0f;
		int randNum = rand() % 100;

		RateSum += Sum(enemyTypeRate, ENEMY_KINDS, ENEMY_KINDS);
		for (size_t i = 0; i < ENEMY_KINDS; i++)
		{
			sum[i] = Sum(enemyTypeRate, ENEMY_KINDS, i + 1);
		}

		if (RateSum != 100) { return; }

		for (size_t i = 0; i < ENEMY_KINDS; i++)
		{
			if (randNum < sum[i])
			{
				type = i;
				break;
			}
		}

		if (!isAlive)
		{
			waitTime--;

			if (waitTime < 0)
			{
				waitTime = WAIT_TIME;
				*status = EnemyStatus(type);
				angleRand = rand();
				transform.pos.x = WIN_SIZE.x / 2 + (WIN_SIZE.x / 2 - transform.radius.x) * cosf(angleRand);
				transform.pos.y = WIN_SIZE.y / 2 + (WIN_SIZE.y / 2 - transform.radius.y) * sinf(angleRand);
				isAlive = 1;
				enemyScroll = {};
			}
		}
	}
};