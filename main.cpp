#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "DxLib.h"
#include "enum.h"
#include "struct.h"
#include "function.h"

// ウィンドウのサイズ
const Screen WIN_SIZE = { 950,950 };

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	ChangeWindowMode(1);
	SetWindowSizeChangeEnableFlag(1);
	SetMainWindowText("ヴァットアステロイド");
	SetGraphMode(WIN_SIZE.x, WIN_SIZE.y, 32);
	SetWindowSizeChangeEnableFlag(0);
	SetWindowSizeExtendRate(1.0);
	SetBackgroundColor(200, 0, 0);
	SetDrawScreen(DX_SCREEN_BACK);

	if (DxLib_Init() == -1) { return -1; }

	srand(time(NULL));
	// ---リソース読み込み---


	// ---フォントハンドル作成---


	// ---定数の宣言と初期化---
	const int ENEMY_NUM = 10;

	// ---変数の宣言と初期化---
	// マウス情報
	bool mouse = 0;
	bool oldmouse = 0;
	Screen mousePos[4]{};

	// 座標変数の配列
	Transform transform[2] =
	{
		{ {32,32},{WIN_SIZE.x / 2,WIN_SIZE.y / 2} },
		{ {WIN_SIZE.x / 2,WIN_SIZE.y / 2},{transform[FIELD].radius.x,transform[FIELD].radius.y} }
	};

	bool isEdgeField = 0; // 画面端にプレイヤーがいるか

	// 敵変数
	Enemy enemies[ENEMY_NUM];

	for (size_t i = 0; i < ENEMY_NUM; i++)
	{
		*(enemies + i) = *new Enemy();
	}

	int eneNum = 0; // 敵の番号,リスポーンで使う

	const int MAX_EN_WT = enemies->waitTime;
	float angle = 0; // 敵と自分の角度

	Screen scroll[2]{}; // スクロール
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

			// 画面のちらつきを防止
			if (oldmouse == 0)
			{
				mousePos[END] = mousePos[START];
			}
		}
		else
		{
			// スクロールを戻す
			mousePos[END].x -= abs(mousePos[START].x - mousePos[END].x) * 0.5f * NewFlag(mousePos[END].x - mousePos[START].x >= 0);
			mousePos[END].y -= abs(mousePos[START].y - mousePos[END].y) * 0.5f * NewFlag(mousePos[END].y - mousePos[START].y >= 0);
		}
		if (oldmouse && !mouse)
		{
			for (size_t i = 0; i < ENEMY_NUM; i++)
			{
				// 敵の押し戻し
				(enemies + i)->transform.pos = (enemies + i)->transform.pos.AddAndSub(scroll[FIELD], 0);

				// 敵にダメージを与える
				// 完全めり込みでない判定
				if (abs((enemies + i)->transform.pos.x + (enemies + i)->transform.radius.x - transform[FIELD].pos.x) >= transform[FIELD].radius.x ||
					abs((enemies + i)->transform.pos.y + (enemies + i)->transform.radius.y - transform[FIELD].pos.y) >= transform[FIELD].radius.y)
				{
					// ダメージを与える
					(enemies + i)->status->hp -= (enemies + i)->status->CalDamage(mousePos[OLD_END].AddAndSub(mousePos[END], 0));
				}
			}
		}

		// 画面スクロール値の計算
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
				// 敵が崖際で止まる
				(enemies + i)->transform.StopCliff({ 0,0 }, WIN_SIZE, scroll[FIELD], enemyScroll[i], isEdgeField);

				// 敵と自分の角度
				angle = atanf((float)(transform[PLAYER].pos.y - scroll[FIELD].y - (enemies + i)->transform.pos.y) /
					(float)(transform[PLAYER].pos.x - scroll[FIELD].x - (enemies + i)->transform.pos.x));

				// 敵速度の計算
				(enemies + i)->spd.x = ((enemies + i)->status->spdCF + abs(enemyScroll[i].x / 5)) * cosf(angle) *
					NewFlag(transform[PLAYER].pos.x - scroll[FIELD].x - (enemies + i)->transform.pos.x >= 0);
				(enemies + i)->spd.y = ((enemies + i)->status->spdCF + abs(enemyScroll[i].y / 5)) * sinf(angle) *
					NewFlag(transform[PLAYER].pos.x - scroll[FIELD].x - (enemies + i)->transform.pos.x >= 0);

				// 敵の移動
				(enemies + i)->transform.pos = (enemies + i)->transform.pos.AddAndSub((enemies + i)->spd, 0);

				// 自分と敵の当たり判定
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

		// ---描画処理---
		transform[FIELD].DrawBoxCenter(WHITE, scroll[FIELD]); // 土台
		transform[PLAYER].DrawBoxCenter(BLACK, scroll[FIELD]); // 自分
		// 敵
		for (size_t i = 0; i < ENEMY_NUM; i++)
		{
			if ((enemies + i)->state == SET)
			{
				(enemies + i)->transform.DrawBoxCenter(~RED, enemyScroll[i]);
			}
		}

		// マウス位置の表示
		if (mouse)
		{
			DrawCircle(mousePos[START].x, mousePos[START].y, 8, BLACK);
		}
		DrawCircle(mousePos[NOW].x, mousePos[NOW].y, 8, BLACK);

		// 数値確認用
		DrawDebugNum<int>(eneNum);

		ScreenFlip();
	}

	// 全リソースファイル削除
	InitGraph();
	InitSoundMem();

	DxLib_End();
	return 0;
}
