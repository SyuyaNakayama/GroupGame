#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "DxLib.h"
#include "enum.h"
#include "function.h"
#include "struct.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	ChangeWindowMode(1);
	SetWindowSizeChangeEnableFlag(1);
	SetMainWindowText("3120_動かないなら画面を動かせばいいじゃない");
	SetGraphMode(WIN_SIZE.x + 200, WIN_SIZE.y, 32);
	SetWindowSizeChangeEnableFlag(0);
	SetWindowSizeExtendRate(1.0);
	SetDrawScreen(DX_SCREEN_BACK);
	srand(time(NULL));
	if (DxLib_Init() == -1) { return -1; }
	SetMouseDispFlag(false); //マウスポインタ―が消える関数

	// ---画像読み込み---
	int bgPic[4] =
	{
		LoadGraph("resources/picture/BackGround.png"),
		LoadGraph("resources/picture/BackGroundmokume.png"),
		LoadGraph("resources/picture/BackGroundumi.png"),
		LoadGraph("resources/picture/BackGroundumi2.png")
	};
	int titlePic[6]{};
	LoadDivGraphLine("resources/picture/title.png", 6, 950, 950, titlePic);
	int stagePic[5];
	LoadDivGraphLine("resources/picture/stage.png", 5, 960, 960, stagePic);
	int rulePic[5];
	LoadDivGraphLine("resources/picture/rule.png", 5, 128, 128, rulePic);

	int handsPic[3] =
	{
		LoadGraph("resources/picture/hand.png"),  // 手（グー）
		LoadGraph("resources/picture/hand2.png"), // 手（パー）
		LoadGraph("resources/picture/hand3.png")  // 手（人差し指）
	};

	int enemyPic[7][3][6]{}; // 添え字は左から、ステージ・敵の種類・アニメーションナンバー
	int picMem[12]{}; // 画像退避用ハンドル

	LoadDivGraphLine("resources/picture/enemy/imomusi.png", 3, 128, 64, enemyPic[0][0]);

	LoadDivGraphLine("resources/picture/enemy/gokiburi.png", 3, 64, 64, enemyPic[1][0]);

	LoadDivGraphLine("resources/picture/enemy/corona.png", 3, 64, 64, picMem);
	for (size_t i = 0; i < 3; i++) { enemyPic[2][i][0] = picMem[i]; }

	LoadDivGraph("resources/picture/enemy/debu.png", 8, 4, 2, 96, 128, picMem);
	for (size_t i = 0; i < 8; i++) { enemyPic[3][i > 3][i % 4] = picMem[i]; }

	LoadDivGraph("resources/picture/enemy/shark.png", 12, 6, 2, 128, 128, picMem);
	for (size_t i = 0; i < 12; i++) { enemyPic[4][i > 5][i % 6] = picMem[i]; }

	enemyPic[5][0][0] = LoadGraph("resources/picture/enemy/daily.png");

	LoadDivGraphLine("resources/picture/enemy/meteor.png", 3, 64, 64, picMem);
	for (size_t i = 0; i < 3; i++) { enemyPic[6][i][0] = picMem[i]; }

	int playerPic[7][6] =
	{
		{LoadGraph("resources/picture/player/leaf.png")},
		{LoadGraph("resources/picture/player/banana.png")},
		{LoadGraph("resources/picture/player/japan.png")},
		{LoadGraph("resources/picture/player/potechi.png")},
		{LoadGraph("resources/picture/player/kaizokusen.png")},
		{LoadGraph("resources/picture/player/child.png")}
	};
	LoadDivGraphLine("resources/picture/player/earth.png", 6, 64, 64, playerPic[6]);
	int firePic = LoadGraph("resources/picture/player/playerDeath.png");

	// ---音読み込み---
	int gameBgm = LoadSoundMem("resources/sound/gamebgm.mp3");
	int titleBgm = LoadSoundMem("resources/sound/titlebgm.mp3");
	int clearSe[2] =
	{
		LoadSoundMem("resources/sound/gameclear_1.mp3"),
		LoadSoundMem("resources/sound/gameclear_2.mp3")
	};
	int hitSe[2] =
	{
		LoadSoundMem("resources/sound/麻痺・しびれ.mp3"),
		LoadSoundMem("resources/sound/katikatihitsound.mp3")
	};
	int gameoverSe = LoadSoundMem("resources/sound/gameover.mp3");
	int damageSe = LoadSoundMem("resources/sound/tyuuoudamage.mp3");
	int decisionSe = LoadSoundMem("resources/sound/決定ボタン.mp3");

	// ---フォントハンドル作成---
	int fontHandle[] =
	{
		CreateFontToHandle("ＭＳ 明朝", 96, 6),
		CreateFontToHandle("ＭＳ 明朝", 64, 6),
		CreateFontToHandle("ＭＳ 明朝", 48, 6),
		CreateFontToHandle("ＭＳ 明朝", 20, 6),
		CreateFontToHandle("ＭＳ 明朝", 232, 6)
	};

	// ---定数の宣言と初期化---
	// 色定数
	const int COLOR[COLOR_NUM]
	{
		GetColor(0, 0, 0),GetColor(255, 255, 255),
		GetColor(200, 0, 0),GetColor(0, 200, 0),
		GetColor(200, 200, 0),GetColor(0, 200, 200),
		GetColor(200, 0, 200),GetColor(127, 127, 127)
	};

	const int HANDS_RADIUS[3] = { 16,32,32 };
	const int THANDER_TIME = 5;
	const int ENEMY_NUM = 100;
	const int PLAYER_MAX_HP = 25;
	const int STAGE_NUM = 7; // ステージの数(チュートリアル含む)
	const int MAX_RAND_NUM = 8; // 乱数の最大値
	// 文字列
	const char* SELECT_STR[11]
	{
		"りすたーと","せれくとへ","たいとるへ"
	};
	// ---変数の宣言と初期化---
	// マウス情報
	bool mouse = 0;
	bool oldmouse = 0;
	Screen mousePos[4]{};
	int handState = 0;

	int scene = TITLE;
	int stage = 0;
	int isGameover = 0;
	int bgNum = 0;

	int startTime = 0;
	int gameoverTime = 0;
	bool isThanderExist[4]{};
	int thanderTimer[4]{};
	// 座標変数
	Transform transform[3] =
	{
		{ {32,32},{WIN_SIZE.x / 2,WIN_SIZE.y / 2} },
		{ {WIN_SIZE.x / 2,WIN_SIZE.y / 2},{transform[FIELD].radius.x,transform[FIELD].radius.y} },
		{ {WIN_SIZE.x / 2 + 5,WIN_SIZE.y / 2 + 5},{transform[FIELD].radius.x,transform[FIELD].radius.y} }
	};
	Transform messageBox[2] =
	{
		{256,48,WIN_SIZE.x / 2,WIN_SIZE.y / 2},
		{256,48,WIN_SIZE.x / 2,WIN_SIZE.y / 2 + messageBox[1].radius.y * 4}
	};
	Transform selectBox[6]{};
	Transform hpBox[ENEMY_NUM + 1]{};

	bool isEdgeField = 0; // 画面端にプレイヤーがいるか
	int playerHp = PLAYER_MAX_HP;

	// 敵変数
	Enemy* enemies = new Enemy[ENEMY_NUM]();
	int eneNum = 0; // 敵の番号,リスポーンで使う

	// 敵の出現バランス
	// 通常, 硬い, 体力, 速い, 強い　の順
	int enemyAppearRate[7][ENEMY_KINDS] =
	{
		{0,0,100,0,0},   // 0ステージ 葉っぱ&蟲
		{100,0,0,0,0},   // 1ステージ バナナ&G
		{20,0,0,50,30},  // 2ステージ 人&コロナ
		{0,50,50,0,0},   // 3ステージ ポテチ&デブ
		{0,0,0,30,70},   // 4ステージ 船&サメ
		{25,25,25,0,25}, // 5ステージ 少年&宿題
		{0,35,30,0,35},  // 6ステージ 地球&隕石
	};
	int enemyWT = 0;
	float angle = 0; // 敵と自分の角度
	int killCount = 0;
	Screen shake[ENEMY_NUM]{}; // シェイク値
	Screen turnFlag[ENEMY_NUM]{}; // 反転するか
	int enemyType[ENEMY_NUM]{}; // 敵の種類

	// スクロール
	Screen fieldScroll{};
	Screen enemyScroll[ENEMY_NUM]{};

	// アニメーション変数
	Anime enemyAnime[7] =
	{
		{3,5},
		{3,1},
		{1,0},
		{4,5},
		{6,3},
		{1,0},
		{1,0}
	};
	Anime playerAnime[7] =
	{
		{1,0},
		{1,0},
		{1,0},
		{1,0},
		{1,0},
		{1,0},
		{6,6}
	};
	Anime tutorialAnime = { 5,6 };
	Anime titleAnime = { 6,30 };

	// 変数に値を代入
	hpBox[ENEMY_NUM].pos = { WIN_SIZE.x / 2,WIN_SIZE.y / 2 - transform[PLAYER].radius.y - 24 }; // プレイヤーのHPバー
	for (int i = 0; i < 6; i++)
	{
		selectBox[i].radius = { 96,96 };
		selectBox[i].pos =
		{
			WIN_SIZE.x / 2 + selectBox[i].radius.x * 3 * ((i % 3) - 1),
			WIN_SIZE.y / 2 + selectBox[i].radius.y * 3 / 2 * NewFlag(i > 2)
		};
	}
	for (size_t i = 0; i < ENEMY_NUM + 1; i++)
	{
		hpBox[i].radius.y = 6;
	}

	ChangeVolumeSoundMem(150, gameBgm);

	while (!(ProcessMessage() == -1 || CheckHitKey(KEY_INPUT_ESCAPE)))
	{
		ClearDrawScreen();
		GetMousePoint(&mousePos[NOW].x, &mousePos[NOW].y);

		oldmouse = mouse;
		mouse = GetMouseInput() & MOUSE_INPUT_LEFT;
		mousePos[OLD_END] = mousePos[END];

		handState = PA;

		DrawGraph(0, 0, bgPic[MOKUME], 1);

		if (scene != PLAY)
		{
			PlaySoundMemLoop(titleBgm);
			StopSoundMem(gameBgm);
		}
		else
		{
			PlaySoundMemLoop(gameBgm);
			StopSoundMem(titleBgm);
		}

		switch (scene)
		{
			case TITLE:
				titleAnime.Update();
				// チュートリアルへ翔ぶ
				if (messageBox[0].PointCollision(mousePos[NOW]))
				{
					handState = YUBI;

					if (mouse && !oldmouse)
					{
						scene = SET;
						startTime = GetNowCount();
						stage = 0;
						PlaySoundMemBack(decisionSe);
					}
				}

				DrawGraph(0, 0, titlePic[titleAnime.animeNum], 1);
				messageBox[0].DrawBoxCenter(COLOR[WHITE + 3 * messageBox[0].PointCollision(mousePos[NOW])]);
				DrawStringToHandle(messageBox[0].pos.x - messageBox[0].radius.x + 64, messageBox[0].pos.y - messageBox[0].radius.y,
					"すたーと", COLOR[BLACK], fontHandle[0]);
				break;
			case STAGE_SELECT:
				for (size_t i = 0; i < STAGE_NUM - 1; i++)
				{
					if (selectBox[i].PointCollision(mousePos[NOW]))
					{
						handState = YUBI;

						if (mouse && !oldmouse)
						{
							stage = i + 1;
							scene = SET;
							PlaySoundMemBack(decisionSe);
						}
					}

					selectBox[i].DrawBoxCenter(COLOR[WHITE + 2 * selectBox[i].PointCollision(mousePos[NOW])]);
					DrawFormatStringToHandle(
						selectBox[i].pos.x - 24, selectBox[i].pos.y - 48,
						COLOR[BLACK], fontHandle[0], "%d", i + 1);
					PlaySoundMemBack(decisionSe);
				}
				break;
			case SET:
				for (size_t i = 0; i < ENEMY_NUM; i++)
				{
					enemies[i].waitTime = 30;
					enemies[i].transform.radius = { 32,32 };
					enemies[i].isAlive = 0;
					switch (stage)
					{
						case 0:
							enemies[i].transform.radius = { 32,16 };
							break;
						case 5:
						case 3:
							enemies[i].transform.radius = { 24,32 };
							break;
					}
					enemies[i].transform.radius.x *= 1.5;
					enemies[i].transform.radius.y *= 1.5;
					hpBox[i].radius.x = enemies[i].transform.radius.x;
				}
				transform[PLAYER].radius = { 32,32 };
				bgNum = MOKUME;
				switch (stage)
				{
					case 0:
						transform[PLAYER].radius = { 28,30 };
						break;
					case 1:
						transform[PLAYER].radius = { 32,28 };
						break;
					case 2:
					case 4:
						bgNum = UMI;
						break;
					case 5:
						transform[PLAYER].radius = { 16,32 };
						break;
					case 6:
						bgNum = UCHU;
						break;
				}
				transform[PLAYER].radius.x *= 1.5;
				transform[PLAYER].radius.y *= 1.5;
				hpBox[ENEMY_NUM].radius.x = transform[PLAYER].radius.x;
				scene = PLAY;
				killCount = 0;
				playerHp = PLAYER_MAX_HP;
				startTime = GetNowCount();
				enemyWT = enemies->waitTime;
				PlaySoundMemBack(decisionSe);
				break;
			case PLAY:
				if (GetNowCount() - startTime <= 60000 && isGameover == 0 || stage == 0)
				{
					isEdgeField = 0;
					for (size_t i = 0; i < 4; i++)
					{
						if (--thanderTimer[i] == 0)
						{
							isThanderExist[i] = 0;
						}
					}

					// 画面スクロール値の計算
					fieldScroll = mousePos[START].AddAndSub(mousePos[END], 1);

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

					for (size_t i = 0; i < ENEMY_NUM; i++)
					{
						if (enemies[i].isAlive == 0)
						{
							// 敵の出現
							if (i == eneNum)
							{
								enemies[i].Respawn(enemyWT, fieldScroll, enemyScroll[i], enemyAppearRate[stage]);
							}
							if (enemies[eneNum].isAlive == 1)
							{
								NumberLoop(++eneNum, ENEMY_NUM - 1);
							}
						}
						if (enemies[i].isAlive == 1)
						{
							shake[i] = {};
							enemyType[i] = {};
							turnFlag[i] = {};

							if (oldmouse && !mouse)
							{
								// 敵の押し戻し
								enemies[i].transform.pos = enemies[i].transform.pos.AddAndSub(fieldScroll, 0);

								if (abs(enemies[i].transform.pos.x - transform[FIELD].pos.x) >= transform[FIELD].radius.x ||
									abs(enemies[i].transform.pos.y - transform[FIELD].pos.y) >= transform[FIELD].radius.y)
								{
									enemyScroll[i] = {};
									if (enemies[i].transform.pos.x - transform[FIELD].pos.x - transform[FIELD].radius.x >= 0)
									{
										isThanderExist[RIGHT] = 1;
										thanderTimer[RIGHT] = THANDER_TIME;
									}
									if (transform[FIELD].pos.x - enemies[i].transform.pos.x - transform[FIELD].radius.x >= 0)
									{
										isThanderExist[LEFT] = 1;
										thanderTimer[LEFT] = THANDER_TIME;
									}
									if (enemies[i].transform.pos.y - transform[FIELD].pos.y - transform[FIELD].radius.y >= 0)
									{
										isThanderExist[DOWN] = 1;
										thanderTimer[DOWN] = THANDER_TIME;
									}
									if (transform[FIELD].pos.y - enemies[i].transform.pos.y - transform[FIELD].radius.y >= 0)
									{
										isThanderExist[UP] = 1;
										thanderTimer[UP] = THANDER_TIME;
									}
									// 敵にダメージを与える
									enemies[i].status->hp -= enemies[i].status->CalDamage(mousePos[OLD_END].AddAndSub(mousePos[END], 1));
									PlaySoundMemBack(hitSe[enemies[i].type == HARD]);
								}
							}

							// ステージごとの変化
							switch (stage)
							{
								case 0:
									turnFlag[i] = { enemies[i].transform.pos.x + fieldScroll.x >= transform[PLAYER].pos.x,0 };
									playerHp = PLAYER_MAX_HP;
									// チュートリアルクリア時
									if (killCount >= 5)
									{
										StopSoundMem(gameBgm);
										scene = STAGE_SELECT;
									}
									break;
								case 1:
									turnFlag[i] = { 0,enemies[i].transform.pos.y + fieldScroll.y < transform[PLAYER].pos.y };
									break;
								case 2:
									shake[i] = { GetRand(MAX_RAND_NUM * 2) - MAX_RAND_NUM,GetRand(MAX_RAND_NUM * 2) - MAX_RAND_NUM };
									if (enemies[i].type == NORMAL) { enemyType[i] = 0; }
									if (enemies[i].type == STRONG) { enemyType[i] = 1; }
									if (enemies[i].type == FAST) { enemyType[i] = 2; }
									break;
								case 6:
									if (enemies[i].type == STRONG) { enemyType[i] = 2; }
								case 3:
									if (enemies[i].type == TOUGH) { enemyType[i] = 0; }
									if (enemies[i].type == HARD) { enemyType[i] = 1; }
									break;
								case 4:
									turnFlag[i] = { enemies[i].transform.pos.x + fieldScroll.x < transform[PLAYER].pos.x,0 };
									if (enemies[i].type == FAST) { enemyType[i] = 0; }
									if (enemies[i].type == STRONG) { enemyType[i] = 1; }
									break;
							}

							// HPバーの移動
							hpBox[i].pos = { enemies[i].transform.pos.x,enemies[i].transform.pos.y - enemies[i].transform.radius.y - 16 };

							// 敵が崖際で止まる
							enemies[i].transform.StopCliff({ 0,0 }, WIN_SIZE, fieldScroll, enemyScroll[i], isEdgeField);

							// 敵と自分の角度
							angle = atanf((float)(transform[PLAYER].pos.y - fieldScroll.y - enemies[i].transform.pos.y) /
								(float)(transform[PLAYER].pos.x - fieldScroll.x - enemies[i].transform.pos.x));

							// 張り付きを防止　超重要！！！
							Clamp(enemies[i].transform.pos.x, enemies[i].transform.radius.x,
								WIN_SIZE.x - enemies[i].transform.radius.x);
							Clamp(enemies[i].transform.pos.y, enemies[i].transform.radius.y,
								WIN_SIZE.y - enemies[i].transform.radius.y);

							// 敵速度の計算
							enemies[i].spd.x = (enemies[i].status->spdCF) * cosf(angle) *
								NewFlag(transform[PLAYER].pos.x - fieldScroll.x - enemies[i].transform.pos.x >= 0);
							enemies[i].spd.y = (enemies[i].status->spdCF) * sinf(angle) *
								NewFlag(transform[PLAYER].pos.x - fieldScroll.x - enemies[i].transform.pos.x >= 0);

							// 敵の移動
							enemies[i].transform.pos = enemies[i].transform.pos.AddAndSub(enemies[i].spd, 0);

							// 自分と敵の当たり判定
							if (transform[PLAYER].BoxCollision({ enemies[i].transform.radius,enemies[i].transform.pos.AddAndSub(fieldScroll,0) }))
							{
								playerHp -= enemies[i].status->Atk;
								enemies[i].isAlive = 0;
								PlaySoundMemBack(damageSe);
							}

							// 敵の死亡
							if (enemies[i].status->hp <= 0)
							{
								enemies[i].isAlive = 2;
								killCount++;
							}

							// チュートリアルの時のみ一体のみ出現
							if (stage == 0) { break; }
						}
						if (enemies[i].isAlive == 2)
						{
							if (++enemies[i].deathFrame >= 15)
							{
								enemies[i].deathFrame = 0;
								enemies[i].isAlive = 0;
							}
						}
					}
					// やられた
					if (playerHp <= 0)
					{
						isGameover = 1;
						gameoverTime = GetNowCount();
						StopSoundMem(damageSe);
						PlaySoundMemBack(gameoverSe);
					}
				}
				//ゲームオーバー画面へ
				if (isGameover == 1)
				{
					StopSoundMem(gameBgm);
					if (GetNowCount() - gameoverTime >= 4000)
					{
					scene = GAMEOVER;
					isGameover = 0;
					}
				}
				// クリア
				if (GetNowCount() - startTime >= 60000)
				{
					StopSoundMem(gameBgm);
					if (GetNowCount() - startTime <= 60200)
					{
						PlaySoundMemBack2(clearSe[0]);
						PlaySoundMemBack2(clearSe[1]);
					}
				}
				if (GetNowCount() - startTime >= 64000 && stage != 0)
				{
					scene = CLEAR;
				}

				if (GetMouseInput() & MOUSE_INPUT_LEFT) { handState = GU; }

				// アニメーション更新
				if (stage == 0) { tutorialAnime.Update(); }
				enemyAnime[stage].Update();
				playerAnime[stage].Update();

				// ---描画処理---
				// 背景
				if (bgNum == UMI)
				{
					transform[FIELD].DrawGraphCenter(bgPic[UMI2]);
				}
				else
				{
					transform[FIELD].DrawGraphCenter(bgPic[bgNum]);
				}
				transform[FIELD].DrawGraphCenter(bgPic[bgNum], fieldScroll); // 土台
				// 自分
				if (playerHp > 0)
				{
					transform[PLAYER].DrawGraphCenter(playerPic[stage][playerAnime[stage].animeNum], fieldScroll);
				}
				else
				{
					transform[PLAYER].DrawGraphCenter(firePic, fieldScroll);
				}
				// 敵
				for (size_t i = 0; i < ENEMY_NUM; i++)
				{
					switch (enemies[i].isAlive)
					{
						case 1:
							if (GetNowCount() - startTime < 60000)
							{
								hpBox[i].DrawHpBar(enemies[i].status->hp, enemies[i].status->maxHp, COLOR[GREEN], enemyScroll[i]);
							}
						case 2:
							if (GetNowCount() - startTime < 60000)
							{
								if (enemies[i].deathFrame % 3 == 0)
								{
									enemies[i].transform.DrawGraphCenter(
										enemyPic[stage][enemyType[i]][enemyAnime[stage].animeNum],
										enemyScroll[i].AddAndSub(shake[i], 1), turnFlag[i]);
								}
							}
							else
							{
								enemies[i].transform.DrawGraphCenter(firePic, enemyScroll[i]);
							}
							break;
					}
				}
				// デンリュウ
				transform[STAGE].DrawGraphCenter(stagePic[0], fieldScroll);
				for (size_t i = 0; i < 4; i++)
				{
					if (isThanderExist[i])
					{
						transform[STAGE].DrawGraphCenter(stagePic[1 + i], fieldScroll);
					}
				}

				if (stage == 0)
				{
					DrawExtendGraph(20, 20, 212, 212, rulePic[tutorialAnime.animeNum], 1);
				}
				if (playerHp > 0) { hpBox[ENEMY_NUM].DrawHpBar(playerHp, PLAYER_MAX_HP, COLOR[GREEN], fieldScroll); }
				if (isGameover == 1) { DrawStringToHandle(0, 347, "GAMEOVER", COLOR[RED], fontHandle[4]); }
				if (GetNowCount() - startTime >= 60000 && stage != 0) { DrawStringToHandle(58, 347, " CLEAR", COLOR[GREEN], fontHandle[4]); }
				break;
			case CLEAR:
			case GAMEOVER:
				for (size_t i = 0; i < 2; i++)
				{
					if (messageBox[i].PointCollision(mousePos[NOW]))
					{
						handState = YUBI;

						if (mouse && !oldmouse)
						{
							scene = (4 * scene - 11) * (i == 0);
							PlaySoundMemBack(decisionSe);
							if (i == 1) { titleAnime.animeNum = 0; }
							break;
						}
					}
					messageBox[i].DrawBoxCenter(COLOR[WHITE + 2 * messageBox[i].PointCollision(mousePos[NOW])]);

					DrawStringToHandle(messageBox[i].pos.x - messageBox[i].radius.x + 8,
						messageBox[i].pos.y - messageBox[i].radius.y,
						SELECT_STR[(1 + i) * (scene == CLEAR) + (2 * i) * (scene == GAMEOVER)],
						COLOR[BLACK], fontHandle[0]);
				}

				DrawFillBox(120, 110, 830, 380, COLOR[WHITE]);
				DrawFillBox(130, 120, 820, 370, COLOR[YELLOW]);
				if (scene == CLEAR)
				{
					DrawStringToHandle(115, 130, "クリア\n", COLOR[BLACK], fontHandle[4]);
				}
				if (scene == GAMEOVER)
				{
					DrawStringToHandle(115, 150, "　　 ここが貴様の\n　 　墓場となった\n　１から出直して来い", COLOR[BLACK], fontHandle[1]);
				}
				break;
		}

		DrawGraph(mousePos[NOW].x - HANDS_RADIUS[handState], mousePos[NOW].y - HANDS_RADIUS[handState], handsPic[handState], 1);
		DrawFillBox(WIN_SIZE.x, 0, WIN_SIZE.x + 200, WIN_SIZE.y, COLOR[BLACK]);

		switch (scene)
		{
			case TITLE:
				DrawStringToHandle(955, 350, "「すたーと」の枠を\n左クリックすると\nチュートリアルです", COLOR[WHITE], fontHandle[3]);
				break;
			case PLAY:
				DrawFormatStringToHandle(980, 30, COLOR[WHITE], fontHandle[2], "STAGE\n%d", stage);
				DrawTime(980, 350, (GetNowCount() - startTime) * (stage != 0), fontHandle[2]);
				DrawFormatStringToHandle(980, 670, COLOR[WHITE], fontHandle[2], "KILL\n%d", killCount);
				if (stage == 0)
				{
					// チュートリアル説明文
					DrawStringToHandle(975, 30 + 96, "↑現在の\nステージ", COLOR[WHITE], fontHandle[3]);
					DrawStringToHandle(965, 350 + 96,
						"↑残り時間\n0秒になるまで\n耐久したらクリア",
						COLOR[WHITE], fontHandle[3]);
					DrawStringToHandle(950, 670 + 96,
						"↑敵を倒した数\nこの値が5になったら\nチュートリアル終了",
						COLOR[WHITE], fontHandle[3]);
				}
				break;
		}

		ScreenFlip();
	}

	// 全リソースファイル削除
	InitGraph();
	InitSoundMem();
	InitFontToHandle();

	DxLib_End();
	return 0;
}