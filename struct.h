#pragma once
int Square(int num);
void NumberLoop(int& num, int max, int min = 0);

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

		NumberLoop(animeNum, picNum);
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

	bool CircleCollision(Transform obj)
	{
		return
			Square(pos.x - obj.pos.x) + Square(pos.y - obj.pos.y) <=
			radius.x + obj.radius.x;
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

	void DrawGraphCenter(int grHandle)
	{
		DrawExtendGraph(pos.x - radius.x, pos.y - radius.y,
			pos.x + radius.x, pos.y + radius.y, grHandle, 1);
	}

	// ���S�_�Ɣ��a�ɂ��w��
	void DrawBoxCenter(size_t color, Screen scroll = { 0,0 })
	{
		DrawFillBox(pos.x - radius.x - scroll.x, pos.y - radius.y - scroll.y,
			pos.x + radius.x - scroll.x, pos.y + radius.y - scroll.y, color);
	}
};

class EnemyStatus
{
public:
	int spdCF; // CoeFficient:�W��
	int Atk, def, hp;

	EnemyStatus(int enemyType)
	{
		// �G�X�e�[�^�X�e�[�u��
		// �s�͓G�̎�ށA��͊e�X�e�[�^�X��\��
		int statusTable[STRONG + 1][4] =
		{
			{3,1,2,5},  // NORMAL
			{3,1,5,1},  // HARD
			{3,1,2,20}, // TOUGH
			{6,1,2,5},  // FAST
			{3,2,2,5}   // STRONG
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
	}

	int CalDamage(Screen scrSpd)
	{
		int damage;

		damage = sqrtf(Square(scrSpd.x) + Square(scrSpd.y)) / 5 / def - def;
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
	int waitTime = 100;
	int state = DEAD;

	Enemy()
	{
		transform.radius = { 32,32 };
		spd = {};
		status = {};
		status = new EnemyStatus(INIT);
	}

	void Respawn(const int WAIT_TIME, Screen scroll, int enemyType)
	{
		if (state == DEAD)
		{
			waitTime--;

			if (waitTime < 0)
			{
				waitTime = WAIT_TIME;
				*status = EnemyStatus(enemyType);
				transform.pos = { 200 - scroll.x,200 - scroll.y };
				state = SET;
			}
		}
	}
};

// �V�F�C�N�Ɏg��
class Shake
{
public:
	int range;	  // �V�F�C�N�͈̔͋L�^
	bool isShake = 0; // �V�F�C�N����?
	Screen num = { 0,0 };   // �V�F�C�N�l�L�^
	int time = 0;	  // �o�ߎ��ԋL�^

	void Func(const int SHAKE_RANGE, const int SHAKE_INTERVAL)
	{
		if (!isShake) return;

		// �V�F�C�N����莞�Ԃ��ƂɊɂ₩�ɂȂ�
		time++;
		if (time >= SHAKE_INTERVAL)
		{
			time = 0;
			range /= 2;
		}
		// �V�F�C�N�l����
		num.x = rand() % (range + 1) - range / 2;
		num.y = rand() % (range + 1) - range / 2;
		// �V�F�C�N�I���
		if (range == 0) { isShake = 0; }
	}
};