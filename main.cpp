#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "DxLib.h"
#include "enum.h"
#include "struct.h"
#include "function.h"

// �E�B���h�E�̃T�C�Y
const Screen WIN_SIZE = { 950,950 };

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	ChangeWindowMode(1);
	SetWindowSizeChangeEnableFlag(1);
	SetMainWindowText("���@�b�g�A�X�e���C�h");
	SetGraphMode(WIN_SIZE.x, WIN_SIZE.y, 32);
	SetWindowSizeChangeEnableFlag(0);
	SetWindowSizeExtendRate(1.0);
	SetBackgroundColor(200, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	if (DxLib_Init() == -1) { return -1; }

	srand(time(NULL));
	// ---���\�[�X�ǂݍ���---


	// ---�t�H���g�n���h���쐬---


	// ---�萔�̐錾�Ə�����---
	const int ENEMY_NUM = 10;

	// ---�ϐ��̐錾�Ə�����---
	// �}�E�X���
	bool mouse = 0;
	bool oldmouse = 0;
	Screen mousePos[4]{};

	// ���W�ϐ��̔z��
	Transform transform[2] =
	{
		{ {32,32},{WIN_SIZE.x / 2,WIN_SIZE.y / 2} },
		{ {WIN_SIZE.x / 2,WIN_SIZE.y / 2},{transform[FIELD].radius.x,transform[FIELD].radius.y} }
	};

	bool isEdgeField = 0; // ��ʒ[�Ƀv���C���[�����邩

	// �G�ϐ�
	Enemy enemies[ENEMY_NUM];

	for (size_t i = 0; i < ENEMY_NUM; i++)
	{
		*(enemies + i) = *new Enemy();
	}

	int eneNum = 0; // �G�̔ԍ�,���X�|�[���Ŏg��

	const int MAX_EN_WT = enemies->waitTime;
	float angle = 0; // �G�Ǝ����̊p�x

	Screen scroll[2]{}; // �X�N���[��
	Screen enemyScroll[ENEMY_NUM]{};


	while (!(ProcessMessage() == -1 || CheckHitKey(KEY_INPUT_ESCAPE)))
	{
		ClearDrawScreen();
		GetMousePoint(&mousePos[NOW].x, &mousePos[NOW].y);

		oldmouse = mouse;
		mouse = GetMouseInput() & MOUSE_INPUT_LEFT;
		mousePos[OLD_END] = mousePos[END];
		isEdgeField = 0;

		if (mouse)
		{
			GetMousePoint(&mousePos[oldmouse].x, &mousePos[oldmouse].y);

			// ��ʂ̂������h�~
			if (oldmouse == 0)
			{
				mousePos[END] = mousePos[START];
			}
		}
		else
		{
			// �X�N���[����߂�
			mousePos[END].x -= abs(mousePos[START].x - mousePos[END].x) * 0.5f * NewFlag(mousePos[END].x - mousePos[START].x >= 0);
			mousePos[END].y -= abs(mousePos[START].y - mousePos[END].y) * 0.5f * NewFlag(mousePos[END].y - mousePos[START].y >= 0);
		}
		if (oldmouse && !mouse)
		{
			for (size_t i = 0; i < ENEMY_NUM; i++)
			{
				// �G�̉����߂�
				(enemies + i)->transform.pos = (enemies + i)->transform.pos.AddAndSub(scroll[FIELD], 0);

				// �G�Ƀ_���[�W��^����
				// ���S�߂荞�݂łȂ�����
				if (abs((enemies + i)->transform.pos.x + (enemies + i)->transform.radius.x - transform[FIELD].pos.x) >= transform[FIELD].radius.x ||
					abs((enemies + i)->transform.pos.y + (enemies + i)->transform.radius.y - transform[FIELD].pos.y) >= transform[FIELD].radius.y)
				{
					// �_���[�W��^����
					(enemies + i)->status->hp -= (enemies + i)->status->CalDamage(mousePos[OLD_END].AddAndSub(mousePos[END], 0));
				}
			}
		}

		// ��ʃX�N���[���l�̌v�Z
		scroll[FIELD] = mousePos[START].AddAndSub(mousePos[END], 1);

		for (size_t i = 0; i < ENEMY_NUM; i++)
		{
			if ((enemies + i)->state == DEAD)
			{
				if (i == eneNum)
				{
					(enemies + i)->Respawn(MAX_EN_WT, scroll[FIELD], 0);
				}
				if ((enemies + eneNum)->state == SET)
				{
					eneNum++;
				}

				NumberLoop(eneNum, 9);
			}

			if ((enemies + i)->state == SET)
			{
				// �G���R�ۂŎ~�܂�
				(enemies + i)->transform.StopCliff({ 0,0 }, WIN_SIZE, scroll[FIELD], enemyScroll[i], isEdgeField);

				// �G�Ǝ����̊p�x
				angle = atanf((float)(transform[PLAYER].pos.y - scroll[FIELD].y - (enemies + i)->transform.pos.y) /
					(float)(transform[PLAYER].pos.x - scroll[FIELD].x - (enemies + i)->transform.pos.x));

				// �G���x�̌v�Z
				(enemies + i)->spd.x = ((enemies + i)->status->spdCF + abs(enemyScroll[i].x / 5)) * cosf(angle) *
					NewFlag(transform[PLAYER].pos.x - scroll[FIELD].x - (enemies + i)->transform.pos.x >= 0);
				(enemies + i)->spd.y = ((enemies + i)->status->spdCF + abs(enemyScroll[i].y / 5)) * sinf(angle) *
					NewFlag(transform[PLAYER].pos.x - scroll[FIELD].x - (enemies + i)->transform.pos.x >= 0);

				// �G�̈ړ�
				(enemies + i)->transform.pos = (enemies + i)->transform.pos.AddAndSub((enemies + i)->spd, 0);

				// �����ƓG�̓����蔻��
				if (transform[PLAYER].BoxCollision({ (enemies + i)->transform.radius,(enemies + i)->transform.pos.AddAndSub(scroll[FIELD],0) }))
				{
					(enemies + i)->state = DEAD;
				}

				if ((enemies + i)->status->hp <= 0)
				{
					(enemies + i)->state = DEAD;
				}
			}

		}

		// ---�`�揈��---
		transform[FIELD].DrawBoxCenter(WHITE, scroll[FIELD]); // �y��
		transform[PLAYER].DrawBoxCenter(BLACK, scroll[FIELD]); // ����
		// �G
		for (size_t i = 0; i < ENEMY_NUM; i++)
		{
			if ((enemies + i)->state == SET)
			{
				(enemies + i)->transform.DrawBoxCenter(~RED, enemyScroll[i]);
			}
		}

		// �}�E�X�ʒu�̕\��
		if (mouse)
		{
			DrawCircle(mousePos[START].x, mousePos[START].y, 8, BLACK);
		}
		DrawCircle(mousePos[NOW].x, mousePos[NOW].y, 8, BLACK);

		// ���l�m�F�p
		DrawDebugNum<int>(eneNum);

		ScreenFlip();
	}

	// �S���\�[�X�t�@�C���폜
	InitGraph();
	InitSoundMem();

	DxLib_End();
	return 0;
}
