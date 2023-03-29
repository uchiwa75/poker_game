#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define N 5 //手札の枚数.

#define M 20 // 初期のチップ枚数

int pre_deck[N * 4];	// ランダムな値を格納するための配列. 構造体に変換後別の配列に移す.
			// 二人のプレイヤーに必要なカードはN * 4までなので[52]である必要はない.

char *suits[4] = {" クラブ  ",
                  " ダイヤ  ", 
                  " ハート  ", 
                  "スペード "}; // トランプのスートをポインタの配列に格納.

char *hands[10] = { "なし", "ワンペア", 
		    "ツーペア", "スリーカード", 
		    "ストレート", "フラッシュ", "フルハウス", 
		    "フォーカード", "ストレートフラッシュ", 
		    "ロイヤルストレートフラッシュ" }; // ポーカーの役も同様.

typedef struct card {
	int num;
	int value;
	char* suit;
}Card; 	// カードを番号(num)、価値(value)、スート(suit)の3つから構成される構造体とする.
	// numが同じであってもvalueに差を持たせるのは手札のソートのため.

Card deck[N * 4]; // pre_deckに格納した数字をCard型構造体に変換した後に格納するための配列.
Card player_hand[N * 2]; // 一旦deckに格納した後でプレイヤーとnpcに割り振る.
Card npc_hand[N * 2];

int player_rate = 0; // プレイヤーの手札でできている役を評価する点数.
int npc_rate = 0; // npcの手札でできている役を評価する点数.

int npc_exchange_count = 0; // npcが交換したカードの枚数.
int include_number = 0; // // プレイヤーが交換したカードの枚数.

int judgement = 2; // 0はプレイヤーの勝利, 1はnpcの勝利, 2は引き分け.

int show_card_p = 0; // プレイヤーのカードをオープンにするかのフラグ. 1 ならオープン.
int show_card_n = 0; // npcのカードをオープンにするかのフラグ. 1 ならオープン.

int win_count = 0; // 勝利カウンター.
int lose_count = 0; // 敗北カウンター.
int chop_count = 0; // 引き分けカウンター.
int player_fold_count = 0; // プレイヤーが勝負を降りた回数.
int npc_fold_count = 0; // npcが勝負を降りた回数.
int games = 0; // 終了したゲーム数.

int player_chips = M; // プレイヤーのチップ数.
int player_bet = 0; // プレイヤーのベット数.
int npc_bet = 0;  // npcのベット数.
int raised_chips = 0; // プレイヤーとnpcのベット数の差.

int npc_anxiety = 0; // npcの不安度. 大きくなるほど勝負を降りやすくなる.

int npc_tendency = 0; // npcのレイズ数の傾向. 0 or 1. 1なら多め. ゲームごとにランダム.

int npc_fold = 0; // npcが勝負を降りるかのフラグ, 1 なら降りる.

char yn[10]; // y/n入力待ち用文字列.


// プロトタイプ宣言
void title(void);
void shuffle(void);
void conbert(void);
void deal(void);
void show_card(Card x[], Card y[]);
void show_hand(int x, int y);
void player_exchange(void);
void npc_exchange(void);
void sort(Card x[]);
void rate(Card x[], int *y);
void show_down(void);
void judge(void);
void compare(void);
void bet_situation(void);
int fold_or_not(int x);
void npc_decision(void);
void init(void);
void ui(void);
void enter(void);




int main(void)
{	

	int continue_game = 0; // ゲームを継続するかの指標を初期化.
	init();
	title();
	
	ui();
	printf("カードが配られました.\n");
	printf("ENTERキーで手札を見ます.");
	enter();
	
	srand((unsigned int)time(NULL));
	int first_move = rand() % 2; //  初戦の先手と後手をランダムに決定する
    
	//first_move = 1; // デバッグ用　0:プレイヤー先手固定,   1:npc先手固定.
	
	while (continue_game == 0 && player_chips > 0) {

		shuffle(); // 重複のない乱数を生成して
		conbert(); // カードに変換して
		deal(); // プレイヤーとnpcに分配する
		
		sort(player_hand); // プレイヤーの手札を整理
		rate(player_hand, &player_rate); // プレイヤーの手札を評価

		sort(npc_hand); // npcの手札を整理
		rate(npc_hand, &npc_rate); // npcの手札を評価

		if (npc_rate >= 6) {
			npc_anxiety -= 20;
		} // フルハウス以上なら勝負を降りることはない

		if (games >= 4) { // プレイヤーがブラフで勝ち続けるのを防ぐ
			if (npc_fold_count > 0) {
				if (player_fold_count == 0 || (player_fold_count != 0 && npc_fold_count / player_fold_count >= 3)) {
					npc_anxiety -= 5;
				}
			}
		}
			
		npc_tendency = rand() % 2; // npcのレイズ傾向を決定 
		
		show_card_p = 1; // プレイヤーの手札をオープンにする
		
		ui();

		// プレイヤー先手
		if (first_move == 0) {

			printf("あなたの番です.\n");

			// ファーストベット
			raised_chips = 0;

			printf("初めにベットするチップの枚数を決めてください.(1 ～ 5) : ");

			char buf[100];
			int temp = 0;
			while (raised_chips <= 0 || raised_chips > 5) {

				fgets(buf, sizeof(buf), stdin);
				sscanf(buf, "%d", &temp);

				if (temp >= 1 && temp <= 5 && temp <= player_chips) {
					raised_chips = temp;
				} else if (temp >= 1 && temp <= 5 && temp > player_chips) {
					printf("チップが足りません. 入力しなおしてください. : ");
				} else {
					printf(" 1 ～ 5 の数字を入力してください. : ");
				}
			}

			if (raised_chips <= 2) {
				npc_anxiety -= 2;
			} else if (raised_chips == 5) {
				npc_anxiety += 1;
			} // プレイヤーのベット額によって不安度を変化させる

			player_chips -= raised_chips;   // 所持枚数を減らす
			player_bet += raised_chips;        // プレイヤーの賭け枚数を増やす

			ui();
			printf(" %d 枚ベットしました.", raised_chips);
			enter();


			// 相手のベット
			ui();
			printf("相手の番です.");
			enter();

			if (player_bet >= 4) {
				if (npc_rate == 0 && npc_hand[0].value < 41) {
					ui();
					npc_fold_count++;
					player_chips++;

					printf("相手は勝負を降りました.\n");
					printf("チップを %d 枚獲得します.", 1);
					enter();

					//show_card_n = 1; // 動作確認用
					//ui();
					//printf("デバッグのため相手の手札を表示しています");
					//enter();

					goto game_end;
				}
			} // プレイヤーのベットが4枚以上でnpcがハイカード J 未満ならチップを 1 枚支払って降りる


			npc_bet = player_bet;

			raised_chips = 0;

			if (npc_tendency == 1 && npc_rate >= 2) {  
				raised_chips = 2;
			} else if (npc_tendency == 0 && npc_rate >= 3) {    
				raised_chips = 2;
			} else if (npc_tendency == 1 && (npc_rate >= 1 || npc_hand[0].value >= 49)) { //value>=49 は K 以上    
				raised_chips = 1;
			} else if (npc_tendency == 0 && npc_rate >= 2) {    
				raised_chips = 1;
			} // 何枚レイズするかの判断

			while (raised_chips > player_chips) {
				raised_chips--;
			} // レイズがプレイヤーのチップ数を超えないように

			npc_bet += raised_chips;

			ui();

			if (raised_chips == 2 || raised_chips == 1) {
				printf("相手はベットを受け入れ, さらに %d 枚レイズしました.", raised_chips);
				enter();
			} else {   		    
				printf("コールしました.");
				enter();
			}

			ui();

			if (raised_chips > 0) {
				printf("コールしますか ? (y/n) : ");

				do {
					fgets(yn, sizeof(yn), stdin);
					if (strcmp(yn, "n\n") == 0) {

						ui();
						printf("勝負を降りました.\n");
						player_fold_count++;
						printf(" %d 枚のチップを失いました.", player_bet);
						enter();

						goto game_end;

					} else if (strcmp(yn, "y\n") == 0) {

						player_chips -= raised_chips;
						player_bet += raised_chips;

						ui();
						printf("ベットを %d 枚増やしました.", raised_chips);
						enter();

					} else {
						printf("y/n で入力してください.");
					}

				} while (strcmp(yn, "y\n") *  strcmp(yn, "n\n") != 0);

			}

			ui();

			player_exchange(); // 一度だけカードを交換する
			sort(player_hand); // カード整理
			rate(player_hand, &player_rate); // 評価


			if (include_number == 0) {
				npc_anxiety += 3;
			} else if (include_number == 1) {
				npc_anxiety += 1;
			} else if (include_number >= 4) {
				npc_anxiety -= 5;
			} // プレイヤーの交換枚数で不安度を変化させる

			if (include_number > 0) {
				ui();
				printf(" %d 枚交換しました.", include_number);
				enter();
			}


			npc_exchange();	// npcも一度だけカードを交換する		
			sort(npc_hand);		
			rate(npc_hand, &npc_rate);

			if (npc_rate >= 6) {			
				npc_anxiety -= 20;
			} // フルハウス以上なら勝負を降りることはない

			ui();
			printf("相手の番です.\n");

			if (npc_exchange_count == 0) {
				printf("相手はカードを交換しませんでした.");
				enter();
			} else if (npc_exchange_count > 0) {
				printf("カードを %d 枚交換しました.", npc_exchange_count);
				enter();
			}

			// カード交換後ベット
			// プレイヤーのベット
			raised_chips = 0;

			ui();
			printf("あなたの番です.\n");

			if (player_chips == 0) {
				printf("レイズできないのでチェックします.");
				enter();
			} else if (player_chips > 0) {
				printf("チェックなら 0 を, レイズなら上乗せする枚数を入力してください.(0 ～ 2) : ");

				temp = 10; // うまく書き換わらなければ下のwhile文へ入る
				fgets(buf, sizeof(buf), stdin);
				sscanf(buf, "%d", &temp);
				while (temp > player_chips || (temp < 0 || temp > 2)) {
					if (temp > player_chips && temp >= 0 && temp <= 2) {
						printf("チップが足りません. 入力しなおしてください. : ");
					} else if (temp < 0 || temp > 2) {
						printf(" 0 ～ 2 の数字を入力してください. : ");
					}

					fgets(buf, sizeof(buf), stdin);
					sscanf(buf, "%d", &temp);
				}

				raised_chips = temp;

				if (raised_chips == 0) {
					ui();
					printf("チェックしました.");
					enter();

				} else if (raised_chips > 0) {

					player_chips -= raised_chips; // 所持枚数を減らす
					player_bet += raised_chips; // プレイヤーの賭け枚数を増やす

					ui();
					printf(" %d 枚レイズしました.", raised_chips);
					enter();

				}
			}


			// 相手のベット
			ui();

			//printf("(デバッグ用ここでの不安度を表示します:%d)\n", npc_anxiety);

			printf("相手の番です.");
			enter();

			npc_decision();

			if (npc_fold == 1) { // 勝負を降りる場合

				ui();

				npc_fold_count++;

				player_chips += player_bet + npc_bet;

				printf("相手は勝負を降りました.\n");
				printf("チップを %d 枚獲得します.", npc_bet);
				enter();

				//show_card_n = 1; // 動作確認用
				//ui();
				//printf("デバッグのため相手の手札を表示しています");
				//enter();

				goto game_end;

			} else if (npc_fold == 0) { // 勝負を受ける場合

				npc_bet += raised_chips; // いったん相手のベットをプレイヤーにそろえて

				if (raised_chips == 0) { // プレイヤーがチェックした場合

					if (npc_tendency == 1) { // npc側のレイズ額を決めていく
						if (npc_rate >= 1) {
							raised_chips = 2;
						} else {
							raised_chips = 0;
						}
					} else if (npc_tendency == 0) {
						if (npc_rate >= 2) {
							raised_chips = 2;
						} else if (npc_rate == 1) {
							raised_chips = 1;
						}
					} 

					while (raised_chips > player_chips) {
						raised_chips--;
					} // プレイヤーのチップ数を超えないように

					npc_bet += raised_chips;  // npcのベット額を増やす

					if (raised_chips >= 1) {
						ui();
						printf("相手は %d 枚レイズしました.", raised_chips);
						enter();
					} else {
						ui();
						printf("相手もチェックしました.");
						enter();
					}

				} else if (raised_chips >= 1) { // プレイヤーがレイズした場合

					raised_chips = 0; // プレイヤーのレイズ分をリセット

					if (npc_tendency == 1) {
						if (npc_rate >= 3) {
							raised_chips = 2;
						} else if (npc_rate == 2) {
							raised_chips = 1;
						}
					} else if (npc_tendency == 0) {
						if (npc_rate >= 4) {
							raised_chips = 2;
						} else if (npc_rate == 3) {
							raised_chips = 1;
						} // レイズの傾向と手の強さによっていくらレイズするか決める
					}

					while (raised_chips > player_chips) {  // プレイヤーのチップ数を超えないように
						raised_chips--;
					}

					npc_bet += raised_chips; // npcがレイズした分

					if (raised_chips >= 1) {
						ui();
						printf("相手はレイズを受け入れ, さらに %d 枚レイズしました.", raised_chips);
						enter();
					} else {
						ui();
						printf("相手はコールしました.");
						enter();
					}
				 }
			}

			if (raised_chips > 0) {

				ui();
				printf("コールしますか ? (y/n) : ");

				do {
					fgets(yn, sizeof(yn), stdin);
					if (strcmp(yn, "n\n") == 0) {
						ui();
						printf("勝負を降りました.\n");
						player_fold_count++;
						printf(" %d 枚のチップを失いました.", player_bet);
						enter();

						goto game_end;

					} else if (strcmp(yn, "y\n") == 0) {

						player_chips -= raised_chips;
						player_bet += raised_chips;

						ui();
						printf("ベットを %d 枚増やしました.", raised_chips);
						enter();

					} else {
						printf("y/n で入力してください.");
					}
				} while (strcmp(yn, "y\n") *  strcmp(yn, "n\n") != 0);

			}
	
    		} else if (first_move == 1) { // npc先手

			printf("オープンしました.");
			enter();
    		
			char buf[100];
			int temp = 0;
			temp = rand() % 10 + 1;
    		
			if (npc_rate >= 2) { //  最初のベット枚数をカードの強さと乱数を用いて決定する
    				if (temp >= 9 && temp <= 10) {
    		        		raised_chips = 5;
				} else if (temp >= 5 && temp <= 8) {
					raised_chips = 4;
    		    		} else if (temp >= 1 && temp <= 4) {
    		     			raised_chips = 3;
    		    		} 
    			} else if (npc_rate == 1) {
    		    		if (temp == 10) {
    		        		raised_chips = 5;
				} else if (temp == 9) {
					raised_chips = 4;
				} else if (temp >= 5 && temp <= 8) {
    		        		raised_chips = 3;
    		   		 } else if (temp >= 1 && temp <= 4) {
    		        		raised_chips = 2;
    		    		} 
    			} else {
				if (npc_hand[1].value >= 45) { // 2番目に強いカードが Q 以上
					 raised_chips = 3;
				} else {
    		        		raised_chips = 2;
    		    		} 
    			}
			
			while (raised_chips > player_chips) {
				raised_chips--;
			} // プレイヤーのチップ数を超えないように
			

    			npc_bet += raised_chips;
    		
			ui();
			printf("相手の番です.\n");
			printf(" %d 枚ベットしました.", raised_chips);
			enter();

			ui();
			printf("コールしますか ?\n");
			printf("('n'を選ぶとチップを 1 枚払って勝負を終えます) (y/n) : ");

			do {
				fgets(yn, sizeof(yn), stdin);
				if (strcmp(yn, "n\n") == 0) {

					ui();
					printf("勝負を降りました.\n");
					player_fold_count++;
					printf(" %d 枚のチップを失いました.", 1);
					enter();

					goto game_end;
				} else if (strcmp(yn, "y\n") == 0) { // 勝負を受けてさらに上乗せするかどうか
					
					player_chips -= raised_chips;
					player_bet += raised_chips;

					if (player_chips == 0) { // チップが足りないのでコールまで
						
						ui();
						printf("コールしました.");
						enter();
						
					} else if (player_chips > 0) {
						
						ui();
						printf("さらにレイズすることもできます.\n");
						printf("このままなら 0 を, レイズなら上乗せする枚数を入力してください.(0 ～ 2) : ");

						temp = 10;  // うまく書き換わらなければ下のwhile文へ入る
						fgets(buf, sizeof(buf), stdin);
						sscanf(buf, "%d", &temp);

						while (temp > player_chips || (temp < 0 || temp > 2)) {
							if (temp > player_chips && temp >= 0 && temp <= 2) {
								printf("チップが足りません. 入力しなおしてください. : ");
							}
							else if (temp < 0 || temp > 2) {
								printf(" 0 ～ 2 の数字を入力してください. : ");
							}

							fgets(buf, sizeof(buf), stdin);
							sscanf(buf, "%d", &temp);
						}

						raised_chips = temp;

						if (raised_chips == 0) {
							ui();
							printf("コールしました.");
							enter();
						}
						else if (raised_chips > 0) {

							player_chips -= raised_chips;   // 所持枚数を減らす
							player_bet += raised_chips;     // プレイヤーの賭け枚数を増やす

							ui();
							printf(" %d 枚レイズしました.", raised_chips);
							enter();
							
						}
					}
				} else {
					printf("y/n で入力してください.");
				}

			} while (strcmp(yn, "y\n") *  strcmp(yn, "n\n") != 0);
			
			if (raised_chips == 2) {
				if (npc_rate == 0 && npc_hand[0].value < 41) {
			        
					ui();

					npc_fold_count++;
					player_chips += player_bet + npc_bet;

					printf("相手は勝負を降りました.\n");
					printf("チップを %d 枚獲得します.", npc_bet);
					enter();

					//show_card_n = 1; // 動作確認用
					//ui();
					//printf("デバッグのため相手の手札を表示しています");
					//enter();

					goto game_end;
			    	}
			} // npcが J 未満のハイカードでプレイヤーが 2 枚レイズしたとき降りる
			
		
		
			if (npc_rate == 0 && npc_hand[0].value < 45) {
				npc_anxiety += raised_chips;
			} // npcがQ未満のハイカードならプレイヤーの上乗せ分不安にする
			
			if (raised_chips > 0) {
				npc_bet += raised_chips;

				ui();
				printf("相手はコールしました.");
				enter();
			}

			npc_exchange();
			sort(npc_hand);
			rate(npc_hand, &npc_rate);

			if (npc_rate >= 6) {
				npc_anxiety -= 20;
			} // フルハウス以上なら勝負を降りることはない

			ui();
			printf("相手の番です.\n");
			
			if (npc_exchange_count == 0) {
				printf("相手はカードを交換しませんでした.");
				enter();
			} else if (npc_exchange_count > 0) {
				printf("カードを %d 枚交換しました.", npc_exchange_count);
				enter();
			}
    		
			ui();
            
			player_exchange(); // 一度だけカードを交換する
			sort(player_hand); // カード整理
			rate(player_hand, &player_rate); // 評価

			if (include_number == 0) {
				npc_anxiety += 3;
			} else if (include_number == 1) {
				npc_anxiety += 1;
			} else if (include_number >= 4) {
				npc_anxiety -= 5;
			} // プレイヤーの交換枚数で不安度を変化させる

			if (include_number > 0) {
				ui();
				printf(" %d 枚交換しました.", include_number);
				enter();
			}
    		
			// ここから交換後のベット
			
			raised_chips = 0; // リセット

			int dice = rand() % 10 + 1;
			
			if (npc_tendency == 1) { // 強気の場合
				if ((dice <= 8) && ((npc_anxiety < 3 && npc_rate == 2) || npc_rate >= 3)) {
					raised_chips = 2;
				} else if ((dice <= 1) && npc_rate >= 1) { // ブラフ
					raised_chips = 2;
				} else if ((npc_anxiety < 3 && npc_rate >= 1) || (npc_anxiety >= 3 && npc_rate >= 2)) {
					raised_chips = 1;
				}
			} else if (npc_tendency == 0) { // 弱気の場合
				if ((dice <= 8) && ((npc_anxiety < 3 && npc_rate == 4) || npc_rate >= 5)) {
					raised_chips = 2;
				} else if ((npc_anxiety < 3 && npc_rate >= 2) || (npc_anxiety >= 3 && npc_rate >= 3)) {
					raised_chips = 1;
				}
			}
			
			while (raised_chips > player_chips) {
				raised_chips--;
			} // プレイヤーのチップ数を超えないように

			npc_bet += raised_chips;

			ui();
			printf("相手の番です.\n");

			if (raised_chips > 0) {
				printf(" %d 枚レイズしました.", raised_chips);
				enter();
			} else if (raised_chips == 0){
				printf("チェックしました.");
				enter();
			}
			
			ui();
			printf("あなたの番です.\n");
			
			if (raised_chips > 0) {
				
				printf("コールしますか ? (y/n) : ");
				
				do {
					fgets(yn, sizeof(yn), stdin);

					if (strcmp(yn, "n\n") == 0) {

						ui();
						printf("勝負を降りました.\n");
						player_fold_count++;
						printf(" %d 枚のチップを失いました.", player_bet);
						enter();

						goto game_end;
						
					} else if (strcmp(yn, "y\n") == 0) { // 勝負を受けてさらに上乗せするかどうか

						player_chips -= raised_chips;
						player_bet += raised_chips;

						if (player_chips == 0) { // チップが足りないのでコールまで
							ui();
							printf("コールしました.");
							enter();
						} else if (player_chips > 0) {

							ui();
							printf("さらにレイズすることもできます.\n");
							printf("このままなら 0 を, レイズなら上乗せする枚数を入力してください.(0 ～ 2) : ");

							temp = 10;  // うまく書き換わらなければ下のwhile文へ入る
							fgets(buf, sizeof(buf), stdin);
							sscanf(buf, "%d", &temp);

							while (temp > player_chips || (temp < 0 || temp > 2)) {
								if (temp > player_chips && temp >= 0 && temp <= 2) {
									printf("チップが足りません. 入力しなおしてください. : ");
								} else if (temp < 0 || temp > 2) {
									printf(" 0 ～ 2 の数字を入力してください. : ");
								}

								fgets(buf, sizeof(buf), stdin);
								sscanf(buf, "%d", &temp);
							}

							raised_chips = temp;

							if (raised_chips == 0) {
								ui();
								printf("コールしました.");
								enter();
							} else if (raised_chips > 0) {

								player_chips -= raised_chips; // 所持枚数を減らす
								player_bet += raised_chips; // プレイヤーの賭け枚数を増やす

								ui();
								printf(" %d 枚レイズしました.", raised_chips);
								enter();
							}
						}
					} else {
						printf("y/n で入力してください.");
					}
					
				} while (strcmp(yn, "y\n") *  strcmp(yn, "n\n") != 0);

			} else if (raised_chips == 0) { // 相手がチェックのとき

				if (player_chips == 0) {
					printf("レイズできないのでチェックします.");
					enter();
				} else if (player_chips > 0) {

					printf("チェックなら 0 を, レイズなら上乗せする枚数を入力してください.(0 ～ 2) : ");

					temp = 10;  // うまく書き換わらなければ下のwhile文へ入る
					fgets(buf, sizeof(buf), stdin);
					sscanf(buf, "%d", &temp);

					while (temp > player_chips || (temp < 0 || temp > 2)) {
						if (temp > player_chips && temp >= 0 && temp <= 2) {
							printf("チップが足りません. 入力しなおしてください. : ");
						} else if (temp < 0 || temp > 2) {
							printf(" 0 ～ 2 の数字を入力してください. : ");
						}

						fgets(buf, sizeof(buf), stdin);
						sscanf(buf, "%d", &temp);
					}

					raised_chips = temp;

					if (raised_chips == 0) {
						ui();
						printf("チェックしました.");
						enter();
					} else if (raised_chips > 0) {
						player_chips -= raised_chips; // 所持枚数を減らす
						player_bet += raised_chips; // プレイヤーの賭け枚数を増やす

						ui();
						printf(" %d 枚レイズしました.", raised_chips);
						enter();
					}
				}	
			}

			if ((raised_chips == 2 || (raised_chips == 1 && player_chips == 0)) && npc_rate < 2) {	
				npc_anxiety += 1;	
			} // プレイヤーの強気のレイズで不安になる. 勝負を降りる割合が多ければここ消すかも
			
			if (raised_chips > 0) { // プレイヤーがレイズした場合npcがコールするか決める
				ui();
    				printf("相手の番です.");
				//printf("\n(デバッグのためここでの不安度を表示します:%d)", npc_anxiety);
				enter();
        		
        			npc_decision();
        		
    				if (npc_fold == 1) { // 勝負を降りる場合

					ui();

					npc_fold_count++;
					player_chips += player_bet + npc_bet;

					printf("相手は勝負を降りました.\n");
					printf("チップを %d 枚獲得します.", npc_bet);
					enter();

					//show_card_n = 1; // 動作確認用
					//ui();
					//printf("デバッグのため相手の手札を表示しています");
					//enter();

					goto game_end;

        			} else {
					npc_bet += raised_chips;
					ui();
					printf("コールしました.");
					enter();
        		    
        			}
			}
    		}
		
		show_down(); // 相手のカードをオープンして手札を比べる
		
		judge(); // 勝敗を決定する

        	game_end: // 勝負を降りた時のgoto先
        
        
        	if (player_chips <= 0) {
			
			ui();
			printf("チップがなくなりました...\n\n");
           		printf("GAME OVER");
			enter();

		} else {
			printf("\n\t次のゲームに進みますか ? (y/n) : ");  	// ゲームを続けるか聞く
			
			do {
				fgets(yn, sizeof(yn), stdin);

				if (strcmp(yn, "n\n") == 0) {
					continue_game = 1;
				} else if (strcmp(yn, "y\n") == 0) {
					games++;
				    	init();

					ui();
					printf("新しいカードを配りました.\n");
					printf("ENTERキーで手札を見ます.");
					enter();
					
					if (first_move == 0) {  // 先手後手を入れ替える
					    first_move = 1;
					} else {
					    first_move = 0;
					}

				} else {
					printf("y/n で入力してください.");
				}

			} while (strcmp(yn, "y\n") *  strcmp(yn, "n\n") != 0);
        	}
	}

	init();

	ui();
	
	printf("Thank you for playing !");	//  ゲームの終了
	enter();
	
	return 0;
}





///// title ///// タイトル画面とNOTICE.
void title(void) {
	printf("\n << POKER >>\n\n");
	printf(" ** NOTICE **\n");
	printf("数字や y/n の入力が求められない限り, ENTERキーで先に進みます.\n");
	printf("所持チップが 0 枚の状態で勝負が終わるとGAME OVERとなります.\n");
	printf("所持チップがなくならない限り, 勝負はいつまでも続けることができます.(相手のチップに制限はありません)\n");
	printf("勝負の途中で所持チップが 0 枚になってもその勝負は継続できます.(ただし自分も相手もレイズはできません)");
	printf("\n\n\n\n Press ENTER to Start");
	enter();
}

///// shuffle ///// 1から52の間で重複の無いように乱数を生成してpre_deck[]に格納する.
void shuffle(void) 
{
	int m = rand() % 52 + 1;
	pre_deck[0] = m;
	int i = 1;
	
	while (i < N * 4) {	
		int index = 1;	
		m = rand() % 52 + 1;	
		
		for (int j = 0; j < i; j++) {	
			if (m == pre_deck[j]) {	
				index *= 0;	
				break;
			}
		}
		
		if (index == 1) {
			pre_deck[i] = m;
			i++;
		}
	}
}

//// conbert /////// pre_deck[]に格納した乱数をCard型構造体に変換して, deck[]に格納する.
void conbert(void)
{
	for (int i = 0; i < N * 4; i++) {		
		int mod = pre_deck[i] % 4;
		deck[i].num = pre_deck[i] % 13 + 1;
		
		if (deck[i].num == 1) {			
			deck[i].value = 53 + mod;	
		} else {	
			deck[i].value = (deck[i].num - 1) * 4 + mod + 1;	
		}

		deck[i].suit = suits[mod];
		
	}
}

///// deal ///// deckに格納したCard型構造体をプレイヤー用とnpc用に分かれた配列に分配する.
void deal(void)
{
	int j = 0;
	
	for (int i = 0; i < N * 2; i++) {    
		player_hand[i] = deck[j];		
		j++;	
		npc_hand[i] = deck[j];	
		j++;	
	}
}

///// show_card ///// 手札を画面に表示する.
void show_card(Card x[], Card y[])
{
	printf("\n\n\n\t<<あなたの手札>>         <<相手の手札>>\n");
	
	if (show_card_p == 0 && show_card_n == 0) {    
		for (int i = 0; i < N; i++) {	    
			printf("\t[***********]:%d          [***********]\n", i + 1);		
		}
	} else if (show_card_p == 1 && show_card_n == 0) {
		for (int i = 0; i < N; i++) {
			if (x[i].num == 1) {
				printf("\t[%s A]:%d          [***********]\n", x[i].suit, i + 1);
			}
			else if (x[i].num == 13) {
				printf("\t[%s K]:%d          [***********]\n", x[i].suit, i + 1);
			}
			else if (x[i].num == 12) {
				printf("\t[%s Q]:%d          [***********]\n", x[i].suit, i + 1);
			}
			else if (x[i].num == 11) {
				printf("\t[%s J]:%d          [***********]\n", x[i].suit, i + 1);
			}
			else {
				printf("\t[%s%2d]:%d          [***********]\n", x[i].suit, x[i].num, i + 1);
			}
		}
	} else if (show_card_p == 1 && show_card_n == 1) {	    
		for (int i = 0; i < N; i++) {		    
			if (x[i].num == 1) {			    
				printf("\t[%s A]:%d          ", x[i].suit, i + 1);				
				if (y[i].num == 1) {
					printf("[%s A]\n", y[i].suit);
				}
				else if (y[i].num == 13) {
					printf("[%s K]\n", y[i].suit);
				}
				else if (y[i].num == 12) {
					printf("[%s Q]\n", y[i].suit);
				}
				else if (y[i].num == 11) {
					printf("[%s J]\n", y[i].suit);
				}
				else {
					printf("[%s%2d]\n", y[i].suit, y[i].num);
				}
			} else if (x[i].num == 13) {
				printf("\t[%s K]:%d          ", x[i].suit, i + 1);
				if (y[i].num == 1) {
					printf("[%s A]\n", y[i].suit);
				}
				else if (y[i].num == 13) {
					printf("[%s K]\n", y[i].suit);
				}
				else if (y[i].num == 12) {
					printf("[%s Q]\n", y[i].suit);
				}
				else if (y[i].num == 11) {
					printf("[%s J]\n", y[i].suit);
				}
				else {
					printf("[%s%2d]\n", y[i].suit, y[i].num);
				}
			} else if (x[i].num == 12) {
				printf("\t[%s Q]:%d          ", x[i].suit, i + 1);
				if (y[i].num == 1) {
					printf("[%s A]\n", y[i].suit);
				}
				else if (y[i].num == 13) {
					printf("[%s K]\n", y[i].suit);
				}
				else if (y[i].num == 12) {
					printf("[%s Q]\n", y[i].suit);
				}
				else if (y[i].num == 11) {
					printf("[%s J]\n", y[i].suit);
				}
				else {
					printf("[%s%2d]\n", y[i].suit, y[i].num);
				}
			} else if (x[i].num == 11) {
				printf("\t[%s J]:%d          ", x[i].suit, i + 1);
				if (y[i].num == 1) {
					printf("[%s A]\n", y[i].suit);
				}
				else if (y[i].num == 13) {
					printf("[%s K]\n", y[i].suit);
				}
				else if (y[i].num == 12) {
					printf("[%s Q]\n", y[i].suit);
				}
				else if (y[i].num == 11) {
					printf("[%s J]\n", y[i].suit);
				}
				else {
					printf("[%s%2d]\n", y[i].suit, y[i].num);
				}
			} else {
				printf("\t[%s%2d]:%d          ", x[i].suit, x[i].num, i + 1);
				if (y[i].num == 1) {
					printf("[%s A]\n", y[i].suit);
				}
				else if (y[i].num == 13) {
					printf("[%s K]\n", y[i].suit);
				}
				else if (y[i].num == 12) {
					printf("[%s Q]\n", y[i].suit);
				}
				else if (y[i].num == 11) {
					printf("[%s J]\n", y[i].suit);
				}
				else {
					printf("[%s%2d]\n", y[i].suit, y[i].num);
				}
			}
		}
	}
}

///// show_hand ///// 手札内で成立している役名を表示する. 引数にとるintはrate関数で算出した値.
void show_hand(int x, int y)
{
	if (show_card_p == 0) {
		printf("\t(役 : ??)\n\n\n");
	}
	else if (show_card_p == 1 && show_card_n == 0) {
		printf("\t(役 : %s)\n\n\n", hands[x]);
	} else if (show_card_p == 1 && show_card_n == 1) {
	    	printf("\t(役 : %s)", hands[x]);
	    	int length = strlen(hands[x]);
	    	for (int i = 0; i < 17 - length; i++) {
	        	printf(" ");
	    	}
	    	printf(" (役 : %s)\n\n\n", hands[y]);
	}
}

///// player_exchange ///// プレイヤーのカード交換を受け付ける.
void player_exchange(void)
{
	int exclude_number = 0;
	include_number = 0;

	printf("あなたの番です.\n");
	printf("カードを交換しますか ? (y/n) : ");

	do {
		fgets(yn, sizeof(yn), stdin);

		if (strcmp(yn, "n\n") == 0) {
			
			ui();	
			printf("では手札はそのままです.");
			enter();
			
			return;
		} else if (strcmp(yn, "y\n") == 0) {

			ui();
			
			char ex[100];
			
			printf("捨てたいカードの右にある 1 ～ 5 の番号を全て入力してください. \n");
			printf("区切りは必要ありません. ENTERキーで確定します. : ");
			
			fgets(ex, sizeof(ex), stdin);
			
			int len = strlen(ex);
			int ex_index; // 入力した数字に重複がないかを調べるための指標

			for (int i = 0; i < len; i++) {				
				if (ex[i] >= 49 && ex[i] <= 53) { // 1 ～ 5 の数字 であるか
					ex_index = 1;
					int j = 0;
					while (j < i) {
						ex_index *= (ex[i] - ex[j]);
						j++;
					}

					if (ex_index != 0) {
						exclude_number = ex[i] - 48;
						player_hand[exclude_number - 1] = player_hand[N + include_number];
						include_number++;
					}
				}
			}
			if (include_number == 0) {
			    printf("番号が確認できませんでした. 交換せずに進みます.");
			    enter();
			}
		} else {	
			printf("y/n で入力してください. : ");
		}
	} while (strcmp(yn, "y\n") *  strcmp(yn, "n\n") != 0);

}

///// npc_exchange ///// 手札の内容に応じてnpcにカード交換をさせる.
void npc_exchange(void)
{
	npc_exchange_count = 0;

	// High Card
	if (npc_rate == 0) {  // あと1枚でFlashが完成する場合はFlashの完成を目指す
	    for (int a = 0; a < N; a++) {
	        for (int b = 0; b < N; b++) {
	            for (int c = 0; c < N; c++) {
	                for (int d = 0; d < N; d++) {
	                    if (strcmp(npc_hand[a].suit, npc_hand[b].suit) == 0 && a != b) {
	                        if (strcmp(npc_hand[b].suit, npc_hand[c].suit) == 0 && b != c) {
	                            if (strcmp(npc_hand[c].suit, npc_hand[d].suit) == 0 && c != d) {
	                                if (strcmp(npc_hand[d].suit, npc_hand[a].suit) == 0 && d != a) {
	                                    if (strcmp(npc_hand[a].suit, npc_hand[c].suit) == 0 && a != c) {
	                                        if (strcmp(npc_hand[b].suit, npc_hand[d].suit) == 0 && b != d) {
	                                            for (int i = 0; i < N; i++) {
	                                                if (i != a && i != b && i != c && i != d) {  // 1枚だけスート違いの i
	                                                    if (npc_hand[i].num != 1 && npc_hand[i].num != 13) {  // A や K を捨ててまでは目指さない
    	                                                    npc_hand[i] = npc_hand[N];
    	                                                    npc_exchange_count++;
    	                                                    return;  // スート違いの1枚を交換して終了
	                                                    }
	                                                }
	                                            }
	                                        }
	                                    }
	                                }
	                            }
	                        }
	                    }
	                }
	            }
	        }
	    }
	}
	if (npc_rate == 0) { // Straightまであと1枚で, かつ一番強いカードが10未満ならStraightを目指す
	    if (npc_hand[0].num < 10 && npc_hand[0].num > 1) {
	        if (npc_hand[0].num - npc_hand[N - 2].num == 3 || npc_hand[0].num - npc_hand[N - 2].num == 4) { // 上から4枚があと1枚でStraightになる条件
	            npc_hand[N - 1] = npc_hand[N];
	            npc_exchange_count++;
	            return;
	        }
	        if (npc_hand[1].num - npc_hand[N - 1].num == 3 || npc_hand[1].num - npc_hand[N - 1].num == 4) { // 下から4枚があと1枚でStraightになる条件
	            npc_hand[0] = npc_hand[N];
	            npc_exchange_count++;
	            return;
	        }
	    }
	}
	if (npc_rate == 0) {
		for (int i = 0; i < N; i++) {
			if (npc_hand[i].num < 7 && npc_hand[i].num != 1) {
				npc_hand[i] = npc_hand[N + i];  // 7未満はすべて交換
				npc_exchange_count++;
			}
		}
		if (npc_exchange_count == 0) {      // High Cardで1枚も交換しないのはおかしいので少なくとも2枚は交換させる
		    npc_hand[N - 1] = npc_hand[N];
		    npc_exchange_count++;
		    npc_hand[N - 2] = npc_hand[N + 1];
		    npc_exchange_count++;
		} else if (npc_exchange_count == 1) {
		    npc_hand[N - 2] = npc_hand[N + 1];
		    npc_exchange_count++;
		}
	}
	// One Pair
	if (npc_rate == 1) {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				if (npc_hand[i].num == npc_hand[j].num && i != j) {
					// ペアを構成しているのは i と j なので交換するカードはそれ以外から選ぶ
					if (npc_hand[N - 1].num >= 6) {  // 一番弱いカードが6以上の場合は一番強いカードは残して交換
						for (int k = 0; k < N; k++) {
							if (i == 0) {
								if (k > 2) {
									npc_hand[k] = npc_hand[N + k];  // N + k < 9 になるので大丈夫
									npc_exchange_count++;
								}
							}
							else if (k != 0 && k != i && k != j) {
								npc_hand[k] = npc_hand[N + k];  // N + k < 9
								npc_exchange_count++;
							}
						}
						return;  //交換終了
					}
					else {
						for (int k = 0; k < N; k++) {
							if (k != i && k != j) { // kはペアを構成しているカードではない
							    if (npc_hand[N - 1].num == npc_hand[i].num) {  // 一番弱いカードがペアの時2枚交換
							        npc_hand[1] = npc_hand[N];                 // このブロックは2のワンペアでカードを1枚も交換しなかった
							        npc_exchange_count++;                      // 事象に対応するために挿入した
							        npc_hand[2] = npc_hand[N + 1];
							        npc_exchange_count++;
							        return;  // 2枚交換して終了
							    }
								else if (npc_hand[k].num < 6 && npc_hand[k].num != 1) {  // 6未満は交換
									npc_hand[k] = npc_hand[N + k];  // N + k < 9
									npc_exchange_count++;
    							}
							}
						}
						return;  //交換終了
					}
				}
			}
		}
	}
	// Two Pair
	if (npc_rate == 2) {
		for (int a = 0; a < N; a++) {
			for (int b = 0; b < N; b++) {
				if (npc_hand[a].num - npc_hand[b].num == 0 && a != b) {  // a,bでワンペア
					for (int c = 0; c < N; c++) {
						for (int d = 0; d < N; d++) {
							if (npc_hand[c].num - npc_hand[d].num == 0 && c != d && c != a && c != b) { // c,dはa,b以外のペア
								for (int i = 0; i < N; i++) {
									if (i != a && i != b && i != c && i != d) {  // a,b,c,dのどれでもないi
										npc_hand[i] = npc_hand[N];
										npc_exchange_count++;
										return;  // この一枚のみ交換
									}
								}
							}
						}
					}
				}
			}
		}
	}
	// Three in a Kind
	if (npc_rate == 3) {
		for (int a = 0; a < N; a++) {
			for (int b = 0; b < N; b++) {
				if (npc_hand[a].num - npc_hand[b].num == 0 && a != b) {
					for (int c = 0; c < N; c++) {
						if (npc_hand[a].num - npc_hand[c].num == 0 && c != a && c != b) {  // a,b,cでスリーカード
							for (int i = 0; i < N; i++) {
								for (int j = 0; j < N; j++) {
									if (i != a && i != b && i != c) {  // iがa,b,cのどれでもなく
										if (j != a && j != b && j != c && j != i) {  // jがa,b,c,iのどれでもないとき
											if (npc_hand[i].num - npc_hand[j].num > 0) {
												if (npc_hand[i].num < 7) {  // どちらも7未満なら両方交換
													npc_hand[i] = npc_hand[N];
													npc_exchange_count++;
													if (npc_hand[j].num != 1) { // Aは残す
    													npc_hand[j] = npc_hand[N + 1];
    													npc_exchange_count++;
													    return;
													}
													return;
												}
												else {
												    if (npc_hand[j].num != 1) {
    													npc_hand[j] = npc_hand[N];  // 大きいほうが7以上なら小さいほうを交換
    													npc_exchange_count++;
    													return;
												    } else {                        // Aは残す
												        npc_hand[i] = npc_hand[N];
												        npc_exchange_count++;
    													return;
												    }
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

///// sort ///// 手札の配列をカードのvalue順に並べ替える.
void sort(Card x[])
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N - 1; j++) {
            if (x[j].value < x[j + 1].value) {
                Card temp = x[j];
                x[j] = x[j + 1];
                x[j + 1] = temp;
            }
        }
    }
}

///// rate ///// 手札内で成立している役を調べる. 計算後, 引数にとるint型ポインタにrateを指示させる.
void rate(Card x[], int *y)
{
	int temp_rate = 0;

	int flash = 1;     //スートがすべて同じかどうか. ここだけスタートを 1 とする
	int royal = 0;     //A,K,Q,J,10であるかどうか　FlashでなければStraightの一種
	int straight = 0;  //手札がすべて連続する数字かどうか　A,2,3,4,5でも可
	
	/// Flash ///
	int i = 0;
	while (flash == 1) {
		flash += strcmp(x[i].suit, x[i + 1].suit);
		i++;
		if (i == N - 1) {
			break;
		}
	}
	if (flash != 1) {
		flash = 0;
	}

	/// Royal Flash and Straight(include A) ///
	if (x[0].num == 1) {
		// Royal //
		for (int j = 1; j < 5; j++) {
			if (x[j].num != 14 - j) {
				break;
			}
			if (j == 4) {
				royal = 1;
			}
		}
		// Straight(include A) //
		for (int j = 1; j < 5; j++) {
			if (x[j].num != 6 - j) {
				break;
			}
			if (j == 4) {
				straight = 1;
			}
		}
	}

	/// Straight ///
	if (straight == 0) {
		int straight_index = 1;
		int j = 0;
		while (straight_index == 1) {
			straight_index *= x[j].num - x[j + 1].num;
			if (j == N - 2 && straight_index == 1) {
				straight = 1;
				break;
			}
			j++;
		}
	}

	/// Pairs ///
	int pair_count = 0;
	for (int j = 0; j < N; j++) {
		for (int k = 0; k < N; k++) {
			if (x[j].num - x[k].num == 0) {
				pair_count++;
			}
		}
	}
	pair_count = (pair_count - N) / 2;
	
	if (royal * flash == 1) {  // 成立している強い役順にrate決定する
		temp_rate = 9;  // Royal Flash
	}
	else if (straight * flash) {
		temp_rate = 8;  // Straight Flash
	}
	else if (pair_count == 6) {
		temp_rate = 7;  // Four of a Kind
	}
	else if (pair_count == 4) {
		temp_rate = 6;  // Full House
	}
	else if (flash == 1) {
		temp_rate = 5;  // Flash
	}
	else if (royal == 1 || straight == 1) {
		temp_rate = 4;  // Straight
	}
	else if (pair_count == 3) {
		temp_rate = 3;  // Three of a Kind
	}
	else if (pair_count == 2) {
		temp_rate = 2;  // Two Pair
	}
	else if (pair_count == 1) {
		temp_rate = 1;  // One Pair
	}
	else {
		temp_rate = 0;  // High Card
	}

	*y = temp_rate;
}

///// show_down ///// npcの手札をオープンする.
void show_down(void)
{
	ui();
	printf("それでは勝負です.");
	enter();

	show_card_n = 1;
	
	ui();
	
	if (player_rate == 0) {
		printf("あなたの手札に役は成立しませんでした.\n");
	} else {
		printf("あなたの役は '%s' です.\n", hands[player_rate]);
	}
	
	if (player_rate == npc_rate) {
		if (npc_rate == 0) {
			printf("相手の手札にも役は成立しませんでした.\n");
		} else {
			printf("相手の役も '%s' です.\n", hands[npc_rate]);
		}
	} else {
		if (npc_rate == 0) {
			printf("相手の手札に役は成立しませんでした.\n");
		} else {
			printf("相手の役は '%s' です.\n", hands[npc_rate]);
		}
	}

}

///// judge ///// 役を比較して勝敗を表示する.
void judge(void)
{

    if (player_rate > npc_rate) {

		enter();

		ui();
		printf("\t勝ちました !!\n");
		printf("\n\t%d 枚のチップを獲得します.", npc_bet);
		enter();
		
		win_count++;
		
		player_chips += player_bet + npc_bet;
		
	} else if (player_rate < npc_rate) {

		enter();

		ui();
		printf("\t負けました...\n");
		printf("\n\t%d 枚のチップを失いました.", player_bet);
		enter();
		
		lose_count++;

	} else {
		
		compare();
		
	    	if (judgement == 0) {
		    	printf("しかしあなたのカードが上でした."); 
			enter();
			
			ui();
			printf("\t勝ちました !!\n");
			printf("\n\t%d 枚のチップを獲得します.", npc_bet);
			enter();
			
			win_count++;
			
			player_chips += player_bet + npc_bet;

		} else if (judgement == 1) {
			printf("しかし相手のカードが上でした.");
			enter();
			
			ui();
			printf("\t負けました...\n");
			printf("\n\t%d 枚のチップを失いました.", player_bet);
			enter();

			lose_count++;

		} else {
		    
			enter();
			
			ui();
			printf("\t引き分け\n");
			printf("\n\t賭けたチップが戻ってきます.");
			enter();
    		
			chop_count++;
    		
			player_chips += player_bet;
		}
	}
	
	player_bet = 0;
	
	npc_bet = 0;
	
	ui();
	printf("\tあなたのチップ : %d 枚\n", player_chips);
	printf("\t(%d 勝 / %d 敗 / %d 分\n", win_count, lose_count, chop_count);
	printf("\t勝負を降りた回数 : あなた  %d 回 / 相手  %d 回)", player_fold_count, npc_fold_count);
	enter();
}

///// compare ///// 役が同じ場合にさらに手札を詳細に比較する.
void compare(void)
{
    // High Card同士または Flash同士の比較
    if (player_rate == 0 || player_rate == 5) {
        for (int i = 0; i < N; i++) {
            if (player_hand[i].num != npc_hand[i].num) {
                if (player_hand[0].num == 1) {
                    judgement = 0;
                    break;
                } else if (npc_hand[0].num == 1) {
                    judgement = 1;
                    break;
                } else {
                    if (player_hand[i].num > npc_hand[i].num) {
                        judgement = 0;
                        break;
                    }
                    else if (player_hand[i].num < npc_hand[i].num) {
                        judgement = 1;
                        break;
                    }
                }
            }
        }
    }
    // One Pair同士の比較
    int player_pair_number1 = 0;
    int player_pair_number2 = 0;
    int npc_pair_number1 = 0;
    int npc_pair_number2 = 0;
    if (player_rate == 1) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (player_hand[i].num == player_hand[j].num && i != j) {
                    player_pair_number1 = player_hand[i].num;
                }
                if (npc_hand[i].num == npc_hand[j].num && i != j) {
                    npc_pair_number1 = npc_hand[i].num;
                }
            }
        }
        if (player_pair_number1 != npc_pair_number1) {
            if (player_pair_number1 == 1) {
                judgement = 0;
                return;
            } else if (npc_pair_number1 == 1) {
                judgement = 1;
                return;
            } else {
                if (player_pair_number1 > npc_pair_number1) {
                    judgement = 0;
                    return;
                } else {
                    judgement = 1;
                    return;
                } 
            }   
        } else {  // ペアの数字も同じ場合
            int j = 0;
            while (player_pair_number1 == npc_pair_number1) {
                for (int i = j; i < N; i++) {  // ペア以外で最大の数を求める
                    if (player_hand[i].num != player_pair_number1) {
                        player_pair_number1 = player_hand[i].num;
                        break;
                    }
                }
                for (int i = j; i < N; i++) {
                    if (npc_hand[i].num != npc_pair_number1) {
                        npc_pair_number1 = npc_hand[i].num;
                        break;
                    }
                }
                j++;
                if(j == N && player_pair_number1 == npc_pair_number1) { //ペア以外の数字も同じ組み合わせなら引き分け
                    judgement = 2;
                    return;
                }
            }  // ループを抜けたということはpair_numberに違いがあるのでそれを比較する
            if (player_pair_number1 == 1) {  // Aが一番強い
                judgement = 0;
                return;
            } else if (npc_pair_number1 == 1) {
                judgement = 1;
                return;
            } else {
                if (player_pair_number1 > npc_pair_number1) {
                    judgement = 0;
                    return;
                } else {  // イコールの場合は排除済み
                    judgement = 1;
                    return;
                }
            }
        }
    }
    // Two Pair同士の比較
    int player_rest;
    int npc_rest;
    if (player_rate == 2) {
        for (int a = 0; a < N; a++) {
            for (int b = 0; b < N; b++) {
                if (player_hand[a].num == player_hand[b].num && a != b && player_pair_number1 == 0) {
                    player_pair_number1 = player_hand[a].num;           // 強い方のペアの数字
                    for (int c = 0; c < N; c++) {
                        for (int d = 0; d < N; d++) {
                            if (player_hand[c].num == player_hand[d].num && c != d && player_hand[c].num != player_pair_number1) {
                                player_pair_number2 = player_hand[c].num;  // 弱い方のペアの数字
                            }
                        }
                    }
                }
            }
        }
        for (int i = 0; i < N; i++) {
            if (player_hand[i].num != player_pair_number1 && player_hand[i].num != player_pair_number2) {
                player_rest = player_hand[i].num; // ペア以外の残りの数字をとる
            }
        }
        for (int a = 0; a < N; a++) {
            for (int b = 0; b < N; b++) {
                if (npc_hand[a].num == npc_hand[b].num && a != b && npc_pair_number1 == 0) {
                    npc_pair_number1 = npc_hand[a].num;           // 強い方のペアの数字
                    for (int c = 0; c < N; c++) {
                        for (int d = 0; d < N; d++) {
                            if (npc_hand[c].num == npc_hand[d].num && c != d && npc_hand[c].num != npc_pair_number1) {
                                npc_pair_number2 = npc_hand[c].num;  // 弱い方のペアの数字
                            }
                        }
                    }
                }
            }
        }
        for (int i = 0; i < N; i++) {
            if (npc_hand[i].num != npc_pair_number1 && npc_hand[i].num != npc_pair_number2) {
                npc_rest = npc_hand[i].num; // ペア以外の残りの数字をとる
            }
        }
        if (player_pair_number1 != npc_pair_number1) {
            if (player_pair_number1 == 1) {
                judgement = 0;
                return;
            } else if (npc_pair_number1 == 1) {
                judgement = 1;
                return;
            } else {
                if (player_pair_number1 > npc_pair_number1) {
                    judgement = 0;
                    return;
                } else {
                    judgement = 1;
                    return;
                }
            }
        } else {  // 大きい方のペアが同じ数字だった場合
            if (player_pair_number2 > npc_pair_number2) {
                judgement = 0;
                return;
            } else if (player_pair_number2 < npc_pair_number2) {
                judgement = 1;
                return;
            } else {  // ペアがどちらも同じ数字の場合
                if (player_rest > npc_rest) {
                    judgement = 0;
                    return;
                } else if (player_rest < npc_rest) {
                    judgement = 1;
                    return;
                } else {
                    judgement = 2;
                    return;  // 引き分けの場合は別に書かなくてもよい
                }
            }
        }
    }
    // Three of a Kind同士または Four of a Kind同士の比較
    if (player_rate == 3 || player_rate == 7) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (player_hand[i].num == player_hand[j].num && i != j) {
                    player_pair_number1 = player_hand[i].num;
                }
                if (npc_hand[i].num == npc_hand[j].num && i != j) {
                    npc_pair_number1 = npc_hand[i].num;
                }
            }
        }
        if (player_pair_number1 == 1) {
            judgement = 0;
        } else if (npc_pair_number1 == 1) {
            judgement = 1;
        } else {
            if (player_pair_number1 > npc_pair_number1) {
                judgement = 0;
            } else {
                judgement = 1;
            }
        }
    }
    // Straight同士または Straight Flash同士の比較
    	if (player_rate == 4 || player_rate == 8) { // A,5,4,3,2 の場合 A が最弱の 1 として扱われるため基本的に2番目に強いカードで比較してゆく
        	if (player_hand[1].num == 5 && npc_hand[1].num == 5) {  //2番目が 5 の場合のみ特殊なので取り上げる
            		if (player_hand[0].num > npc_hand[0].num) {  // この場合に限って A より 6 のほうが強い
               			judgement = 0;                          //  judgement = 2 で初期化しているため引き分けの場合はわざわざ取り上げない
                		return;
            		} else if (player_hand[0].num < npc_hand[0].num) {
                		judgement = 1;
                		return;
           		 }
       		 } else if (player_hand[1].num > npc_hand[1].num) {
           		judgement = 0;
            		return;
        	} else if (player_hand[1].num < npc_hand[1].num) {
            		judgement = 1;
            		return;
        	}
    	}
    
    // Flash同士の比較は High Card同士の比較と同様なので統合済み
    
    // Full House同士の比較
	if (player_rate == 6) {
        	for (int a = 0; a < N; a++) {
            		for (int b = 0; b < N; b++) {
                		for (int c = 0; c < N; c++) {
                    			if (player_hand[a].num == player_hand[b].num && a != b) {
                        			if (player_hand[b].num == player_hand[c].num && b != c) {
                            				if (player_hand[c].num == player_hand[a].num && c != a) {
                                				player_pair_number1 = player_hand[a].num;  // 2枚組を避けて3枚組の数字をとる
                            				}
                        			}
                   			 }
                		}
            		}
        	}
        	for (int a = 0; a < N; a++) {
           		 for (int b = 0; b < N; b++) {
                		for (int c = 0; c < N; c++) {
                    			if (npc_hand[a].num == npc_hand[b].num && a != b) {
                        			if (npc_hand[b].num == npc_hand[c].num && b != c) {
                           				 if (npc_hand[c].num == npc_hand[a].num && c != a) {
                                				 npc_pair_number1 = npc_hand[a].num;
                            				}
                        			}
                    			}
               			 }
            		}
        	}
       		 if (player_pair_number1 > npc_pair_number1) {
            		judgement = 0;
           		 return;
        	} else if (player_pair_number1 < npc_pair_number1) {
           		judgement = 1;
            		return;
        	}
	}
    
    // Four of a Kind同士の比較は Three of a Kind同士の比較と同様なので統合済み
    
    // Straight Flash同士の比較は Straight同士の比較と同様なので統合済み
    
    // Royal Flash同士は引き分けとなる
}

///// bet_situation ///// 現在のベットの状況を表示する.
void bet_situation(void)
{
	printf("GAME %d\n", games + 1);
	//printf("不安度 : %d\n", npc_anxiety); // デバッグ用
	printf("あなたのチップ : %d枚\n", player_chips);
    	printf("あなたのベット : %d枚 / 相手のベット : %d枚", player_bet, npc_bet);
}

///// fold_or_not ///// npcが勝負を降りるかどうかを決定する. 引数は npc_anxiety = 0 の時何割の確率で勝負を受けるか.
int fold_or_not(int x)
{
	int fold = 0;
	fold = rand() % 10 + 1 + npc_anxiety;
	
	if (fold <= x) {
        	return 0;  // 勝負を受ける
    	} else {
        	return 1;  // 勝負を降りる
    	}
}

///// npc_decision ///// npcが下りるかどうかの最終的な判断.
void npc_decision(void)
{
	if (npc_rate >= 4) { // ストレート以上
		if (raised_chips == 2) {
			npc_fold = fold_or_not(11);
		}
		else if (raised_chips == 1) {
			npc_fold = fold_or_not(14);
		}
	}
	else if (npc_rate == 3) { // スリーカード
		if (raised_chips == 2) {
			npc_fold = fold_or_not(10); // 10は不安度 0 のとき必ず勝負するというライン.
		}
		else if (raised_chips == 1) {
			npc_fold = fold_or_not(13);
		}
	}
	else if (npc_rate == 2) { // ツーペア
		if (raised_chips == 2) {
			npc_fold = fold_or_not(9);
		}
		else if (raised_chips == 1) {
			npc_fold = fold_or_not(11);
		}
	}
	else if (npc_rate == 1) { // ワンペア
		if (raised_chips == 2) {
			npc_fold = fold_or_not(6);
		}
		else if (raised_chips == 1) {
			npc_fold = fold_or_not(9);
		}
	}
	else {  // 役のないとき
		if (raised_chips == 2 && npc_hand[0].value >= 49) {
			npc_fold = fold_or_not(1);
		}
		else if (raised_chips == 1 && npc_hand[0].value >= 49) {
			npc_fold = fold_or_not(2);
		} 
		else if (raised_chips > 0) {
		    npc_fold = 1;
		}
	} // 自分の手とプレイヤーのレイズと不安度の組み合わせで降りる確率を決める
}

///// init /////新しいゲームを始めるときに 0 であるべき変数をリセットする.
void init(void)
{
	npc_bet = 0;
	player_bet = 0;
	raised_chips = 0;
	npc_fold = 0;
	npc_anxiety = 0;
	show_card_p = 0;
	show_card_n = 0;
}

///// ui ///// 画面を更新して基本的な情報を画面の同じ場所に表示し続ける
void ui(void)
{
	system("cls");
	printf("** 用語 **\n");
	printf("ベット   : チップを賭けます.\n");
	printf("レイズ   : ベット数を増やします.\n");
	printf("コール   : 相手がレイズしたとき, それに応じてベット数を増やします.\n");
	printf("チェック : 自分と相手のベット数が同じとき, レイズせず現在のベット数を維持します.\n\n\n");
	bet_situation();
	show_card(player_hand, npc_hand);
	show_hand(player_rate, npc_rate);
}

///// enter ///// ENTERキーの入力を待つ.
void enter(void)
{
	char c; // ENTER入待ち変数.
	char buf[100];
	fgets(buf, sizeof(buf), stdin);
	sscanf(buf, "%c", &c);
}
