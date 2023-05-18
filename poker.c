#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "poker.h"

char* suits[4] = { " クラブ  ",
		   " ダイヤ  ",
		   " ハート  ",
		   "スペード " }; // トランプのスートをポインタの配列に格納.

char* hands[10] = { "なし", "ワンペア",
		    "ツーペア", "スリーカード",
		    "ストレート", "フラッシュ", "フルハウス",
		    "フォーカード", "ストレートフラッシュ",
		    "ロイヤルストレートフラッシュ" }; // ポーカーの役も同様.

///// init_all ///// 各種カウンター、フラグ等初期化.
void init_all(Result_count* result_count, Every_game_var* every_game_var) {

	init_Result_count(result_count);
	every_game_reset(every_game_var);

}

///// init_Result_counter ///// ゲーム結果集計初期化
void init_Result_count(Result_count* result_count) {

	result_count->chips = CHIPS;
	result_count->win = 0;
	result_count->lose = 0;
	result_count->chop = 0;
	result_count->player_fold = 0;
	result_count->opponent_fold = 0;
	result_count->games = 0;

}

///// every_game_reset ///// 新しいゲームを始めるときに 0 であるべき変数をリセットする.
void every_game_reset(Every_game_var* every_game_var) {

	reset_Bet_amount(&every_game_var->bet_amount);
	reset_Opponent_thought(&every_game_var->opponent_thought);
	reset_Show_card_status(&every_game_var->show_card_status);
	reset_Hand_strength(&every_game_var->hand_strength);
	reset_Exchange_count(&every_game_var->exchange_count);
}

///// reset_Bet_amount ///// ベット額の初期化
void reset_Bet_amount(Bet_amount* bet_amount) {

	bet_amount->opponent = 0;
	bet_amount->player = 0;
	bet_amount->raised = 0;

}

///// reset_Opponent_thought ///// opponentの考えのリセット
void reset_Opponent_thought(Opponent_thought* opponent_thought) {

	opponent_thought->fold = CONTINUE;
	opponent_thought->anxiety = 0;
	opponent_thought->tendency = rand() % 2; // opponentのレイズ傾向を決定 

}

///// reset_Show_card_status ///// カード表示状態のリセット
void reset_Show_card_status(Show_card_status* show_card_status) {

	show_card_status->player = CLOSE;
	show_card_status->opponent = CLOSE;

}

///// reset_Hand_strength ///// 役のリセット
void reset_Hand_strength(Hand_strength* hand_strength) {
	
	hand_strength->player = HIGH_CARD;
	hand_strength->opponent = HIGH_CARD;
}

///// reset_Exchange_count ///// 交換枚数のリセット
void reset_Exchange_count(Exchange_count* exchange_count) {

	exchange_count->player = 0;
	exchange_count->opponent = 0;

}

///// display_title ///// タイトル画面とNOTICE.
void display_title(void) {

	printf("\n << POKER >>\n\n");
	printf(" ** NOTICE **\n");
	printf("数字や y/n の入力が求められない限り, ENTERキーで先に進みます.\n");
	printf("所持チップが 0 枚の状態で勝負が終わるとGAME OVERとなります.\n");
	printf("所持チップがなくならない限り, 勝負はいつまでも続けることができます.(相手のチップに制限はありません)\n");
	printf("勝負の途中で所持チップが 0 枚になってもその勝負は継続できます.(ただし自分も相手もレイズはできません)");
	printf("\n\n\n\n Press ENTER to Start");
	waiting_enter_pressed();

}

///// update_screen ///// 画面を更新して基本的な情報を画面の同じ場所に表示し続ける
void update_screen(const Result_count result_count,
	const Every_game_var every_game_var,
	const char* hands[],
	const Card player_card[],
	const Card opponent_card[])
{
	system("cls");
	printf("** 用語 **\n");
	printf("ベット   : チップを賭けます.\n");
	printf("レイズ   : ベット数を増やします.\n");
	printf("コール   : 相手がレイズしたとき, それに応じてベット数を増やします.\n");
	printf("チェック : 自分と相手のベット数が同じとき, レイズせず現在のベット数を維持します.\n\n\n");
	bet_situation(result_count, every_game_var.bet_amount);

	//printf("\n不安度：%d\n", every_game_var.opponent_thought.anxiety);

	show_card(every_game_var.show_card_status, player_card, opponent_card);
	show_hand(every_game_var.show_card_status, every_game_var.hand_strength, hands);
}

///// bet_situation ///// 現在のベットの状況を表示する.
void bet_situation(const Result_count result_count, const Bet_amount bet_amount)
{
	printf("GAME %d\n", result_count.games + 1);
	printf("あなたのチップ : %d枚\n", result_count.chips);
	printf("あなたのベット : %d枚 / 相手のベット : %d枚", bet_amount.player, bet_amount.opponent);
}

///// show_card ///// 手札を画面に表示する.
void show_card(const Show_card_status show_card_status, const Card player_card[], const Card opponent_card[])
{
	printf("\n\n\n\t<<あなたの手札>>         <<相手の手札>>\n");

	if (show_card_status.player == CLOSE && show_card_status.opponent == CLOSE) {

		for (int i = 0; i < CARD_NUM; i++) {

			printf("\t[***********]:%d          [***********]\n", i + 1);

		}
	}
	else if (show_card_status.player == OPEN && show_card_status.opponent == CLOSE) {

		for (int i = 0; i < CARD_NUM; i++) {

			if (player_card[i].num == 1) {
				printf("\t[%s A]:%d          [***********]\n", player_card[i].suit, i + 1);
			}
			else if (player_card[i].num == 13) {
				printf("\t[%s K]:%d          [***********]\n", player_card[i].suit, i + 1);
			}
			else if (player_card[i].num == 12) {
				printf("\t[%s Q]:%d          [***********]\n", player_card[i].suit, i + 1);
			}
			else if (player_card[i].num == 11) {
				printf("\t[%s J]:%d          [***********]\n", player_card[i].suit, i + 1);
			}
			else {
				printf("\t[%s%2d]:%d          [***********]\n", player_card[i].suit, player_card[i].num, i + 1);
			}
		}
	}
	else if (show_card_status.player == OPEN && show_card_status.opponent == OPEN) {

		for (int i = 0; i < CARD_NUM; i++) {

			if (player_card[i].num == 1) {

				printf("\t[%s A]:%d          ", player_card[i].suit, i + 1);

				if (opponent_card[i].num == 1) {
					printf("[%s A]\n", opponent_card[i].suit);
				}
				else if (opponent_card[i].num == 13) {
					printf("[%s K]\n", opponent_card[i].suit);
				}
				else if (opponent_card[i].num == 12) {
					printf("[%s Q]\n", opponent_card[i].suit);
				}
				else if (opponent_card[i].num == 11) {
					printf("[%s J]\n", opponent_card[i].suit);
				}
				else {
					printf("[%s%2d]\n", opponent_card[i].suit, opponent_card[i].num);
				}
			}
			else if (player_card[i].num == 13) {
				printf("\t[%s K]:%d          ", player_card[i].suit, i + 1);
				if (opponent_card[i].num == 1) {
					printf("[%s A]\n", opponent_card[i].suit);
				}
				else if (opponent_card[i].num == 13) {
					printf("[%s K]\n", opponent_card[i].suit);
				}
				else if (opponent_card[i].num == 12) {
					printf("[%s Q]\n", opponent_card[i].suit);
				}
				else if (opponent_card[i].num == 11) {
					printf("[%s J]\n", opponent_card[i].suit);
				}
				else {
					printf("[%s%2d]\n", opponent_card[i].suit, opponent_card[i].num);
				}
			}
			else if (player_card[i].num == 12) {
				printf("\t[%s Q]:%d          ", player_card[i].suit, i + 1);
				if (opponent_card[i].num == 1) {
					printf("[%s A]\n", opponent_card[i].suit);
				}
				else if (opponent_card[i].num == 13) {
					printf("[%s K]\n", opponent_card[i].suit);
				}
				else if (opponent_card[i].num == 12) {
					printf("[%s Q]\n", opponent_card[i].suit);
				}
				else if (opponent_card[i].num == 11) {
					printf("[%s J]\n", opponent_card[i].suit);
				}
				else {
					printf("[%s%2d]\n", opponent_card[i].suit, opponent_card[i].num);
				}
			}
			else if (player_card[i].num == 11) {
				printf("\t[%s J]:%d          ", player_card[i].suit, i + 1);
				if (opponent_card[i].num == 1) {
					printf("[%s A]\n", opponent_card[i].suit);
				}
				else if (opponent_card[i].num == 13) {
					printf("[%s K]\n", opponent_card[i].suit);
				}
				else if (opponent_card[i].num == 12) {
					printf("[%s Q]\n", opponent_card[i].suit);
				}
				else if (opponent_card[i].num == 11) {
					printf("[%s J]\n", opponent_card[i].suit);
				}
				else {
					printf("[%s%2d]\n", opponent_card[i].suit, opponent_card[i].num);
				}
			}
			else {
				printf("\t[%s%2d]:%d          ", player_card[i].suit, player_card[i].num, i + 1);
				if (opponent_card[i].num == 1) {
					printf("[%s A]\n", opponent_card[i].suit);
				}
				else if (opponent_card[i].num == 13) {
					printf("[%s K]\n", opponent_card[i].suit);
				}
				else if (opponent_card[i].num == 12) {
					printf("[%s Q]\n", opponent_card[i].suit);
				}
				else if (opponent_card[i].num == 11) {
					printf("[%s J]\n", opponent_card[i].suit);
				}
				else {
					printf("[%s%2d]\n", opponent_card[i].suit, opponent_card[i].num);
				}
			}
		}
	}
}

///// show_hand ///// 手札内で成立している役名を表示する. 引数にとるintはrate関数で算出した値.
void show_hand(const Show_card_status show_card_status, const Hand_strength hand_strength, const char* hands[])
{
	if (show_card_status.player == CLOSE) {

		printf("\t(役 : ??)\n\n\n");
	}
	else if (show_card_status.player == OPEN && show_card_status.opponent == CLOSE) {

		printf("\t(役 : %s)\n\n\n", hands[hand_strength.player]);

	}
	else if (show_card_status.player == OPEN && show_card_status.opponent == OPEN) {

		printf("\t(役 : %s)", hands[hand_strength.player]);

		int length = strlen(hands[hand_strength.player]);

		for (int i = 0; i < 17 - length; i++) {

			printf(" ");

		}

		printf(" (役 : %s)\n\n\n", hands[hand_strength.opponent]);
	}
}

///// show_down ///// opponentの手札をオープンする.
void show_down(const Result_count result_count,
	Every_game_var* every_game_var,
	const Hand_strength hand_strength,
	const char* hands[],
	const Card player_card[],
	const Card opponent_card[])
{
	update_screen(result_count, *every_game_var, hands, player_card, opponent_card);

	printf("それでは勝負です.");
	waiting_enter_pressed();

	every_game_var->show_card_status.opponent = OPEN;

	update_screen(result_count, *every_game_var, hands, player_card, opponent_card);

	if (hand_strength.player == HIGH_CARD) {

		printf("あなたの手札に役は成立しませんでした.\n");

	}
	else {

		printf("あなたの役は '%s' です.\n", hands[hand_strength.player]);

	}
	if (hand_strength.player == hand_strength.opponent) {

		if (hand_strength.opponent == HIGH_CARD) {

			printf("相手の手札にも役は成立しませんでした.\n");

		}
		else {

			printf("相手の役も '%s' です.\n", hands[hand_strength.opponent]);

		}
	}
	else {

		if (hand_strength.opponent == HIGH_CARD) {

			printf("相手の手札に役は成立しませんでした.\n");

		}
		else {

			printf("相手の役は '%s' です.\n", hands[hand_strength.opponent]);

		}
	}

}

///// shuffle_and_deal /////
void shuffle_and_deal(int pre_deck[],
	const char* suits[],
	Hand_strength* hand_strength,
	Card deck[],
	Card player_card[],
	Card opponent_card[])
{
	shuffle(pre_deck);
	convert(pre_deck, suits, deck);
	deal(deck, player_card, opponent_card);
	sort(player_card);
	hand_strength->player = rate(player_card);
	sort(opponent_card);
	hand_strength->opponent = rate(opponent_card);
}

///// shuffle ///// 1から52の間で重複の無いように乱数を生成してpre_deck[]に格納する.
void shuffle(int pre_deck[]) {

	int m = rand() % 52 + 1;

	pre_deck[0] = m;

	int i = 1;

	while (i < CARD_NUM * 4) {

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

///// convert ///// pre_deck[]に格納した乱数をCard型構造体に変換して, deck[]に格納する.
void convert(const int pre_deck[], const char* suits[], Card deck[])
{
	for (int i = 0; i < CARD_NUM * 4; i++) {

		int mod = pre_deck[i] % 4;

		deck[i].num = pre_deck[i] % 13 + 1;

		if (deck[i].num == 1) {

			deck[i].value = 53 + mod;

		}
		else {

			deck[i].value = (deck[i].num - 1) * 4 + mod + 1;

		}

		deck[i].suit = suits[mod];

	}
}

///// deal ///// deckに格納したCard型構造体をプレイヤー用とopponent用に分かれた配列に分配する.
void deal(const Card deck[], Card player_card[], Card opponent_card[])
{
	int j = 0;

	for (int i = 0; i < CARD_NUM * 2; i++) {

		player_card[i] = deck[j];

		j++;

		opponent_card[i] = deck[j];

		j++;

	}
}

///// sort ///// 手札の配列をカードのvalue順に並べ替える.
void sort(Card x[])
{
	for (int i = 0; i < CARD_NUM; i++) {

		for (int j = 0; j < CARD_NUM - 1; j++) {

			if (x[j].value < x[j + 1].value) {

				Card temp = x[j];

				x[j] = x[j + 1];

				x[j + 1] = temp;

			}
		}
	}
}

///// rate ///// 手札内で成立している役を調べてStrengthの値を返す.
Strength rate(Card x[])
{
	int flash = 1;     //スートがすべて同じかどうか. ここだけスタートを 1 とする
	int royal = 0;     //A,K,Q,J,10であるかどうか　FlashでなければStraightの一種
	int straight = 0;  //手札がすべて連続する数字かどうか　A,2,3,4,5でも可

	/// Flash ///
	int i = 0;
	while (flash == 1) {
		flash += strcmp(x[i].suit, x[i + 1].suit);
		i++;
		if (i == CARD_NUM - 1) {
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
			if (j == CARD_NUM - 2 && straight_index == 1) {
				straight = 1;
				break;
			}
			j++;
		}
	}

	/// Pairs ///
	int pair_count = 0;
	for (int j = 0; j < CARD_NUM; j++) {
		for (int k = 0; k < CARD_NUM; k++) {
			if (x[j].num - x[k].num == 0) {
				pair_count++;
			}
		}
	}
	pair_count = (pair_count - CARD_NUM) / 2;

	if (royal * flash == 1) {  // 成立している強い役順に値を返す
		return ROYAL_FLASH;
	}
	else if (straight * flash) {
		return STRAIGHT_FLASH;
	}
	else if (pair_count == 6) {
		return FOUR_OF_A_KIND;
	}
	else if (pair_count == 4) {
		return FULL_HOUSE;
	}
	else if (flash == 1) {
		return FLASH;
	}
	else if (royal == 1 || straight == 1) {
		return STRAIGHT;
	}
	else if (pair_count == 3) {
		return THREE_OF_A_KIND;
	}
	else if (pair_count == 2) {
		return TWO_PAIR;
	}
	else if (pair_count == 1) {
		return ONE_PAIR;
	}
	else {
		return HIGH_CARD;
	}

}

///// judge ///// 役を比較して勝敗を表示する.
void judge(Result_count* result_count,
	Every_game_var* every_game_var,
	const char* hands[],
	const Card player_card[],
	const Card opponent_card[])
{
	int judgement;

	if (every_game_var->hand_strength.player > every_game_var->hand_strength.opponent) {

		waiting_enter_pressed();

		update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);

		printf("\t勝ちました !!\n");
		printf("\n\t%d 枚のチップを獲得します.", every_game_var->bet_amount.opponent);
		waiting_enter_pressed();

		result_count->win++;

		result_count->chips += every_game_var->bet_amount.player + every_game_var->bet_amount.opponent;

	}
	else if (every_game_var->hand_strength.player < every_game_var->hand_strength.opponent) {

		waiting_enter_pressed();

		update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);

		printf("\t負けました...\n");
		printf("\n\t%d 枚のチップを失いました.", every_game_var->bet_amount.player);
		waiting_enter_pressed();

		result_count->lose++;

	}
	else {

		judgement = compare(every_game_var->hand_strength.player, player_card, opponent_card);
		if (judgement == WIN) {
			printf("しかしあなたのカードが上でした.");
			waiting_enter_pressed();

			update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);

			printf("\t勝ちました !!\n");
			printf("\n\t%d 枚のチップを獲得します.", every_game_var->bet_amount.opponent);
			waiting_enter_pressed();

			result_count->win++;

			result_count->chips += every_game_var->bet_amount.player + every_game_var->bet_amount.opponent;

		}
		else if (judgement == LOSE) {
			printf("しかし相手のカードが上でした.");
			waiting_enter_pressed();

			update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);

			printf("\t負けました...\n");
			printf("\n\t%d 枚のチップを失いました.", every_game_var->bet_amount.player);
			waiting_enter_pressed();

			result_count->lose++;

		}
		else {

			waiting_enter_pressed();

			update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);

			printf("\t引き分け\n");
			printf("\n\t賭けたチップが戻ってきます.");
			waiting_enter_pressed();

			result_count->chop++;

			result_count->chips += every_game_var->bet_amount.player;

		}
	}

	every_game_var->bet_amount.player = 0;

	every_game_var->bet_amount.opponent = 0;

	update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);

	printf("\tあなたのチップ : %d 枚\n", result_count->chips);
	printf("\t(%d 勝 / %d 敗 / %d 分\n", result_count->win, result_count->lose, result_count->chop);
	printf("\t勝負を降りた回数 : あなた  %d 回 / 相手  %d 回)", result_count->player_fold, result_count->opponent_fold);
}

///// compare ///// 役が同じ場合にさらに手札を詳細に比較する.
int compare(const Strength strength, const Card player_card[], const Card opponent_card[])
{
	// High Card同士または Flash同士の比較
	if (strength == HIGH_CARD || strength == FLASH) {
		for (int i = 0; i < CARD_NUM; i++) {
			if (player_card[i].num != opponent_card[i].num) {
				if (player_card[i].num == 1) {
					return WIN;
				}
				else if (opponent_card[i].num == 1) {
					return LOSE;
				}
				else {
					if (player_card[i].num > opponent_card[i].num) {
						return WIN;
					}
					else if (player_card[i].num < opponent_card[i].num) {
						return LOSE;
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
	if (strength == ONE_PAIR) {
		for (int i = 0; i < CARD_NUM; i++) {
			for (int j = 0; j < CARD_NUM; j++) {
				if (player_card[i].num == player_card[j].num && i != j) {
					player_pair_number1 = player_card[i].num;
				}
				if (opponent_card[i].num == opponent_card[j].num && i != j) {
					npc_pair_number1 = opponent_card[i].num;
				}
			}
		}
		if (player_pair_number1 != npc_pair_number1) {
			if (player_pair_number1 == 1) {
				return WIN;
			}
			else if (npc_pair_number1 == 1) {
				return LOSE;
			}
			else {
				if (player_pair_number1 > npc_pair_number1) {
					return WIN;
				}
				else {
					return LOSE;
				}
			}
		}
		else {  // ペアの数字も同じ場合
			int j = 0;
			while (player_pair_number1 == npc_pair_number1) {
				for (int i = j; i < CARD_NUM; i++) {  // ペア以外で最大の数を求める
					if (player_card[i].num != player_pair_number1) {
						player_pair_number1 = player_card[i].num;
						break;
					}
				}
				for (int i = j; i < CARD_NUM; i++) {
					if (opponent_card[i].num != npc_pair_number1) {
						npc_pair_number1 = opponent_card[i].num;
						break;
					}
				}
				j++;
				if (j == CARD_NUM && player_pair_number1 == npc_pair_number1) { //ペア以外の数字も同じ組み合わせなら引き分け
					return CHOP;
				}
			}  // ループを抜けたということはpair_numberに違いがあるのでそれを比較する
			if (player_pair_number1 == 1) {  // Aが一番強い
				return WIN;
			}
			else if (npc_pair_number1 == 1) {
				return LOSE;
			}
			else {
				if (player_pair_number1 > npc_pair_number1) {
					return WIN;
				}
				else {  // イコールの場合は排除済み
					return LOSE;
				}
			}
		}
	}
	// Two Pair同士の比較
	int player_rest;
	int npc_rest;
	if (strength == TWO_PAIR) {
		for (int a = 0; a < CARD_NUM; a++) {
			for (int b = 0; b < CARD_NUM; b++) {
				if (player_card[a].num == player_card[b].num && a != b && player_pair_number1 == 0) {
					player_pair_number1 = player_card[a].num;           // 強い方のペアの数字
					for (int c = 0; c < CARD_NUM; c++) {
						for (int d = 0; d < CARD_NUM; d++) {
							if (player_card[c].num == player_card[d].num && c != d && player_card[c].num != player_pair_number1) {
								player_pair_number2 = player_card[c].num;  // 弱い方のペアの数字
							}
						}
					}
				}
			}
		}
		for (int i = 0; i < CARD_NUM; i++) {
			if (player_card[i].num != player_pair_number1 && player_card[i].num != player_pair_number2) {
				player_rest = player_card[i].num; // ペア以外の残りの数字をとる
			}
		}
		for (int a = 0; a < CARD_NUM; a++) {
			for (int b = 0; b < CARD_NUM; b++) {
				if (opponent_card[a].num == opponent_card[b].num && a != b && npc_pair_number1 == 0) {
					npc_pair_number1 = opponent_card[a].num;           // 強い方のペアの数字
					for (int c = 0; c < CARD_NUM; c++) {
						for (int d = 0; d < CARD_NUM; d++) {
							if (opponent_card[c].num == opponent_card[d].num && c != d && opponent_card[c].num != npc_pair_number1) {
								npc_pair_number2 = opponent_card[c].num;  // 弱い方のペアの数字
							}
						}
					}
				}
			}
		}
		for (int i = 0; i < CARD_NUM; i++) {
			if (opponent_card[i].num != npc_pair_number1 && opponent_card[i].num != npc_pair_number2) {
				npc_rest = opponent_card[i].num; // ペア以外の残りの数字をとる
			}
		}
		if (player_pair_number1 != npc_pair_number1) {
			if (player_pair_number1 == 1) {
				return WIN;
			}
			else if (npc_pair_number1 == 1) {
				return LOSE;
			}
			else {
				if (player_pair_number1 > npc_pair_number1) {
					return WIN;
				}
				else {
					return LOSE;
				}
			}
		}
		else {  // 大きい方のペアが同じ数字だった場合
			if (player_pair_number2 > npc_pair_number2) {
				return WIN;
			}
			else if (player_pair_number2 < npc_pair_number2) {
				return LOSE;
			}
			else {  // ペアがどちらも同じ数字の場合
				if (player_rest > npc_rest) {
					return WIN;
				}
				else if (player_rest < npc_rest) {
					return LOSE;
				}
				else {
					return CHOP;
				}
			}
		}
	}
	// Three of a Kind同士または Four of a Kind同士の比較
	if (strength == THREE_OF_A_KIND || strength == FOUR_OF_A_KIND) {
		for (int i = 0; i < CARD_NUM; i++) {
			for (int j = 0; j < CARD_NUM; j++) {
				if (player_card[i].num == player_card[j].num && i != j) {
					player_pair_number1 = player_card[i].num;
				}
				if (opponent_card[i].num == opponent_card[j].num && i != j) {
					npc_pair_number1 = opponent_card[i].num;
				}
			}
		}
		if (player_pair_number1 == 1) {
			return WIN;
		}
		else if (npc_pair_number1 == 1) {
			return LOSE;
		}
		else {
			if (player_pair_number1 > npc_pair_number1) {
				return WIN;
			}
			else {
				return LOSE;
			}
		}
	}
	// Straight同士または Straight Flash同士の比較
	if (strength == STRAIGHT || strength == STRAIGHT_FLASH) { // A,5,4,3,2 の場合 A が最弱の 1 として扱われるため基本的に2番目に強いカードで比較してゆく
		if (player_card[1].num == 5 && opponent_card[1].num == 5) {  //2番目が 5 の場合のみ特殊なので取り上げる
			if (player_card[0].num > opponent_card[0].num) {  // この場合に限って A より 6 のほうが強い
				return WIN;                          //  return CHOP; が関数の末尾のため引き分けは扱わない
			}
			else if (player_card[0].num < opponent_card[0].num) {
				return LOSE;
			}
		}
		else if (player_card[1].num > opponent_card[1].num) {
			return WIN;
		}
		else if (player_card[1].num < opponent_card[1].num) {
			return LOSE;
		}
	}

	// Flash同士の比較は High Card同士の比較と同様なので統合済み

	// Full House同士の比較
	if (strength == FULL_HOUSE) {
		for (int a = 0; a < CARD_NUM; a++) {
			for (int b = 0; b < CARD_NUM; b++) {
				for (int c = 0; c < CARD_NUM; c++) {
					if (player_card[a].num == player_card[b].num && a != b) {
						if (player_card[b].num == player_card[c].num && b != c) {
							if (player_card[c].num == player_card[a].num && c != a) {
								player_pair_number1 = player_card[a].num;  // 2枚組を避けて3枚組の数字をとる
							}
						}
					}
				}
			}
		}
		for (int a = 0; a < CARD_NUM; a++) {
			for (int b = 0; b < CARD_NUM; b++) {
				for (int c = 0; c < CARD_NUM; c++) {
					if (opponent_card[a].num == opponent_card[b].num && a != b) {
						if (opponent_card[b].num == opponent_card[c].num && b != c) {
							if (opponent_card[c].num == opponent_card[a].num && c != a) {
								npc_pair_number1 = opponent_card[a].num;
							}
						}
					}
				}
			}
		}
		if (player_pair_number1 > npc_pair_number1) {
			return WIN;
		}
		else if (player_pair_number1 < npc_pair_number1) {
			return LOSE;
		}
	}

	// Four of a Kind同士の比較は Three of a Kind同士の比較と同様なので統合済み

	// Straight Flash同士の比較は Straight同士の比較と同様なので統合済み

	// Royal Flash同士は引き分けとなる
	return CHOP;
}

///// opponent_exchange ///// 手札の内容に応じてopponentにカード交換をさせる.戻り値は交換枚数.
int opponent_exchange(const Strength hand_strength_opponent, Card opponent_card[])
{
	int exchange_count = 0;

	// High Card
	if (hand_strength_opponent == HIGH_CARD) {  // あと1枚でFlashが完成する場合はFlashの完成を目指す
		for (int a = 0; a < CARD_NUM; a++) {
			for (int b = 0; b < CARD_NUM; b++) {
				for (int c = 0; c < CARD_NUM; c++) {
					for (int d = 0; d < CARD_NUM; d++) {
						if (strcmp(opponent_card[a].suit, opponent_card[b].suit) == 0 && a != b) {
							if (strcmp(opponent_card[b].suit, opponent_card[c].suit) == 0 && b != c) {
								if (strcmp(opponent_card[c].suit, opponent_card[d].suit) == 0 && c != d) {
									if (strcmp(opponent_card[d].suit, opponent_card[a].suit) == 0 && d != a) {
										if (strcmp(opponent_card[a].suit, opponent_card[c].suit) == 0 && a != c) {
											if (strcmp(opponent_card[b].suit, opponent_card[d].suit) == 0 && b != d) {
												for (int i = 0; i < CARD_NUM; i++) {
													if (i != a && i != b && i != c && i != d) {  // 1枚だけスート違いの i
														if (opponent_card[i].num != 1 && opponent_card[i].num != 13) {  // A や K を捨ててまでは目指さない
															opponent_card[i] = opponent_card[CARD_NUM];
															exchange_count++;
															return exchange_count;  // スート違いの1枚を交換して終了
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
	if (hand_strength_opponent == HIGH_CARD) { // Straightまであと1枚で, かつ一番強いカードが10未満ならStraightを目指す
		if (opponent_card[0].num < 10 && opponent_card[0].num > 1) {
			if (opponent_card[0].num - opponent_card[CARD_NUM - 2].num == 3 || opponent_card[0].num - opponent_card[CARD_NUM - 2].num == 4) { // 上から4枚があと1枚でStraightになる条件
				opponent_card[CARD_NUM - 1] = opponent_card[CARD_NUM];
				exchange_count++;
				return exchange_count;
			}
			if (opponent_card[1].num - opponent_card[CARD_NUM - 1].num == 3 || opponent_card[1].num - opponent_card[CARD_NUM - 1].num == 4) { // 下から4枚があと1枚でStraightになる条件
				opponent_card[0] = opponent_card[CARD_NUM];
				exchange_count++;
				return exchange_count;
			}
		}
	}
	if (hand_strength_opponent == HIGH_CARD) {
		for (int i = 0; i < CARD_NUM; i++) {
			if (opponent_card[i].num < 7 && opponent_card[i].num != 1) {
				opponent_card[i] = opponent_card[CARD_NUM + i];  // 7未満はすべて交換
				exchange_count++;
			}
		}
		if (exchange_count == 0) {      // High Cardで交換しないのはおかしいので少なくとも2枚は交換させる
			opponent_card[CARD_NUM - 1] = opponent_card[CARD_NUM];
			exchange_count++;
			opponent_card[CARD_NUM - 2] = opponent_card[CARD_NUM + 1];
			exchange_count++;
		}
		else if (exchange_count == 1) {
			opponent_card[CARD_NUM - 2] = opponent_card[CARD_NUM + 1];
			exchange_count++;
		}
	}
	// One Pair
	if (hand_strength_opponent == ONE_PAIR) {
		for (int i = 0; i < CARD_NUM; i++) {
			for (int j = 0; j < CARD_NUM; j++) {
				if (opponent_card[i].num == opponent_card[j].num && i != j) {
					// ペアを構成しているのは i と j なので交換するカードはそれ以外から選ぶ
					if (opponent_card[CARD_NUM - 1].num >= 6) {  // 一番弱いカードが6以上の場合は一番強いカードは残して交換
						for (int k = 0; k < CARD_NUM; k++) {
							if (i == 0) {
								if (k > 2) {
									opponent_card[k] = opponent_card[CARD_NUM + k];  // CARD_NUM + k < 9
									exchange_count++;
								}
							}
							else if (k != 0 && k != i && k != j) {
								opponent_card[k] = opponent_card[CARD_NUM + k];  // CARD_NUM + k < 9
								exchange_count++;
							}
						}
						return exchange_count;  //交換終了
					}
					else {
						for (int k = 0; k < CARD_NUM; k++) {
							if (k != i && k != j) { // kはペアを構成しているカードではない
								if (opponent_card[CARD_NUM - 1].num == opponent_card[i].num) {  // 一番弱いカードがペアの時2枚交換
									opponent_card[1] = opponent_card[CARD_NUM];                 // このブロックは2のワンペアでカードを1枚も交換しなかった
									exchange_count++;                      // 事象に対応するために挿入した
									opponent_card[2] = opponent_card[CARD_NUM + 1];
									exchange_count++;
									return exchange_count;  // 2枚交換して終了
								}
								else if (opponent_card[k].num < 6 && opponent_card[k].num != 1) {  // 6未満は交換
									opponent_card[k] = opponent_card[CARD_NUM + k];  // CARD_NUM + k < 9
									exchange_count++;
								}
							}
						}
						return exchange_count;  //交換終了
					}
				}
			}
		}
	}
	// Two Pair
	if (hand_strength_opponent == TWO_PAIR) {
		for (int a = 0; a < CARD_NUM; a++) {
			for (int b = 0; b < CARD_NUM; b++) {
				if (opponent_card[a].num - opponent_card[b].num == 0 && a != b) {  // a,bでワンペア
					for (int c = 0; c < CARD_NUM; c++) {
						for (int d = 0; d < CARD_NUM; d++) {
							if (opponent_card[c].num - opponent_card[d].num == 0 && c != d && c != a && c != b) { // c,dはa,b以外のペア
								for (int i = 0; i < CARD_NUM; i++) {
									if (i != a && i != b && i != c && i != d) {  // a,b,c,dのどれでもないi
										opponent_card[i] = opponent_card[CARD_NUM];
										exchange_count++;
										return exchange_count;  // この一枚のみ交換
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
	if (hand_strength_opponent == THREE_OF_A_KIND) {
		for (int a = 0; a < CARD_NUM; a++) {
			for (int b = 0; b < CARD_NUM; b++) {
				if (opponent_card[a].num - opponent_card[b].num == 0 && a != b) {
					for (int c = 0; c < CARD_NUM; c++) {
						if (opponent_card[a].num - opponent_card[c].num == 0 && c != a && c != b) {  // a,b,cでスリーカード
							for (int i = 0; i < CARD_NUM; i++) {
								for (int j = 0; j < CARD_NUM; j++) {
									if (i != a && i != b && i != c) {  // iがa,b,cのどれでもなく
										if (j != a && j != b && j != c && j != i) {  // jがa,b,c,iのどれでもないとき
											if (opponent_card[i].num - opponent_card[j].num > 0) {
												if (opponent_card[i].num < 7) {  // どちらも7未満なら両方交換
													opponent_card[i] = opponent_card[CARD_NUM];
													exchange_count++;
													if (opponent_card[j].num != 1) { // Aは残す
														opponent_card[j] = opponent_card[CARD_NUM + 1];
														exchange_count++;
														return exchange_count;
													}
													return exchange_count;
												}
												else {
													if (opponent_card[j].num != 1) {
														opponent_card[j] = opponent_card[CARD_NUM];  // 大きいほうが7以上なら小さいほうを交換
														exchange_count++;
														return exchange_count;
													}
													else {                        // Aは残す
														opponent_card[i] = opponent_card[CARD_NUM];
														exchange_count++;
														return exchange_count;
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
	return exchange_count;
}

///// change_anxiety_by_player_exchange ///// プレイヤーの交換枚数によって変動するanxietyの増減値を返す
int change_anxiety_by_player_exchange(const int exchange_count_player)
{
	switch (exchange_count_player) {
	case 0:
		return 3;
	case 1:
		return 1;
	case 4:
	case 5:
		return -5;
	}
	return 0;
}

///// opponent_fold_decision ///// opponentが下りるかどうかの最終的な判断.
void opponent_fold_decision(const int bet_amount_raised, Opponent_thought* opponent_thought, const Strength hand_strength_opponent, const Card opponent_card[])
{
	if (hand_strength_opponent >= FULL_HOUSE) { // フルハウス以上なら下りない
		return;
	}
	else if (hand_strength_opponent >= STRAIGHT) { // ストレート以上
		if (bet_amount_raised == 2) {
			opponent_thought->fold = fold_or_not(11, opponent_thought->anxiety);
		}
		else if (bet_amount_raised == 1) {
			opponent_thought->fold = fold_or_not(14, opponent_thought->anxiety);
		}
	}
	else if (hand_strength_opponent == THREE_OF_A_KIND) { // スリーカード
		if (bet_amount_raised == 2) {
			opponent_thought->fold = fold_or_not(10, opponent_thought->anxiety); // 10は不安度 0 のとき必ず勝負するというライン.
		}
		else if (bet_amount_raised == 1) {
			opponent_thought->fold = fold_or_not(13, opponent_thought->anxiety);
		}
	}
	else if (hand_strength_opponent == TWO_PAIR) { // ツーペア
		if (bet_amount_raised == 2) {
			opponent_thought->fold = fold_or_not(9, opponent_thought->anxiety);
		}
		else if (bet_amount_raised == 1) {
			opponent_thought->fold = fold_or_not(11, opponent_thought->anxiety);
		}
	}
	else if (hand_strength_opponent == ONE_PAIR) { // ワンペア
		if (bet_amount_raised == 2) {
			opponent_thought->fold = fold_or_not(6, opponent_thought->anxiety);
		}
		else if (bet_amount_raised == 1) {
			opponent_thought->fold = fold_or_not(9, opponent_thought->anxiety);
		}
	}
	else {  // 役のないとき
		if (bet_amount_raised == 2 && opponent_card[0].value >= KING) {
			opponent_thought->fold = fold_or_not(1, opponent_thought->anxiety);
		}
		else if (bet_amount_raised == 1 && opponent_card[0].value >= KING) {
			opponent_thought->fold = fold_or_not(2, opponent_thought->anxiety);
		}
		else if (bet_amount_raised > 0) {
			opponent_thought->fold = FOLD;
		}
	} // 自分の手とプレイヤーのレイズと不安度の組み合わせで降りる確率を決める
}

///// fold_or_not ///// opponentが勝負を降りるかどうかを決定する. 引数は Opponent_thought.anxiety = 0 の時何割の確率で勝負を受けるか.
int fold_or_not(int x, int opponent_thought_anxiety)
{
	int fold = 0;

	fold = rand() % 10 + 1 + opponent_thought_anxiety;

	if (fold <= x) {

		return CONTINUE;  // 勝負を受ける

	}
	else {

		return FOLD;  // 勝負を降りる

	}
}

///// avoid_being_bluffed ///// プレイヤーがブラフで勝ち続けるのを防ぐ
void avoid_being_bluffed(const Result_count result_count, int* opponent_thought_anxiety)
{
	if (result_count.games >= 4) {
		if (result_count.opponent_fold > 2) {
			if (result_count.player_fold == 0 || (result_count.player_fold != 0 && result_count.opponent_fold / result_count.player_fold >= 3)) {
				*opponent_thought_anxiety -= 5;
			}
		}
	}
}

///// opponent_bet ///// opponentのベット枚数を決定
void opponent_bet(Game_fase* game_fase,
	Result_count* result_count,
	Every_game_var* every_game_var,
	const char* hands[],
	const Card player_card[],
	const Card opponent_card[])
{
	switch (*game_fase) {
	case BET_PLAYER_FIRST_BEFORE_EXCHANGE:

		printf("相手の番です.");
		waiting_enter_pressed();
		if (every_game_var->bet_amount.player >= 4) {
			if (every_game_var->hand_strength.opponent == HIGH_CARD && opponent_card[0].value < JACK) {
				*game_fase = OPPONENT_FOLD;
				return;
			}
		} // プレイヤーのベットが4枚以上でopponentがハイカード J 未満ならチップを 1 枚支払って降りる
		every_game_var->bet_amount.opponent = every_game_var->bet_amount.player;
		every_game_var->bet_amount.raised = 0;
		if (every_game_var->opponent_thought.tendency == AGGRESSIVE) {
			if (every_game_var->hand_strength.opponent >= TWO_PAIR) {
				every_game_var->bet_amount.raised = BIG_RAISE;
			}
			else if (every_game_var->hand_strength.opponent >= ONE_PAIR) {
				every_game_var->bet_amount.raised = SMALL_RAISE;
			}
			else if (opponent_card[0].value >= KING) {
				every_game_var->bet_amount.raised = SMALL_RAISE;
			}
		}
		else {
			if (every_game_var->hand_strength.opponent >= THREE_OF_A_KIND) {
				every_game_var->bet_amount.raised = BIG_RAISE;
			}
			else if (every_game_var->hand_strength.opponent >= TWO_PAIR) {
				every_game_var->bet_amount.raised = SMALL_RAISE;
			}
		} // 何枚レイズするかの判断
		while (every_game_var->bet_amount.raised > result_count->chips) {
			every_game_var->bet_amount.raised--;
		} // レイズがプレイヤーのチップ数を超えないように
		every_game_var->bet_amount.opponent += every_game_var->bet_amount.raised;
		update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);
		if (every_game_var->bet_amount.raised == BIG_RAISE || every_game_var->bet_amount.raised == SMALL_RAISE) {
			printf("相手はベットを受け入れ, さらに %d 枚レイズしました.", every_game_var->bet_amount.raised);
		}
		else {
			printf("コールしました.");
		}

		break;

	case BET_OPPONENT_FIRST_BEFORE_EXCHANGE:

		printf("オープンしました.");
		waiting_enter_pressed();
		int temp = rand() % 10 + 1;
		if (every_game_var->hand_strength.opponent >= TWO_PAIR) { //  最初のベット枚数をカードの強さと乱数を用いて決定する
			if (temp >= 9) {
				every_game_var->bet_amount.raised = 5;
			}
			else if (temp >= 5 && temp <= 8) {
				every_game_var->bet_amount.raised = 4;
			}
			else if (temp >= 1 && temp <= 4) {
				every_game_var->bet_amount.raised = 3;
			}
		}
		else if (every_game_var->hand_strength.opponent == ONE_PAIR) {
			if (temp == 10) {
				every_game_var->bet_amount.raised = 5;
			}
			else if (temp == 9) {
				every_game_var->bet_amount.raised = 4;
			}
			else if (temp >= 5 && temp <= 8) {
				every_game_var->bet_amount.raised = 3;
			}
			else if (temp >= 1 && temp <= 4) {
				every_game_var->bet_amount.raised = 2;
			}
		}
		else {
			if (opponent_card[1].value >= QUEEN) { // 2番目に強いカードが Q 以上
				every_game_var->bet_amount.raised = 3;
			}
			else {
				every_game_var->bet_amount.raised = 2;
			}
		}
		while (every_game_var->bet_amount.raised > result_count->chips) {
			every_game_var->bet_amount.raised--;
		} // プレイヤーのチップ数を超えないように
		every_game_var->bet_amount.opponent += every_game_var->bet_amount.raised;
		update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);
		printf("相手の番です.\n");
		printf(" %d 枚ベットしました.", every_game_var->bet_amount.raised);

		break;

	case BET_PLAYER_FIRST_AFTER_EXCHANGE:

		printf("相手の番です.");
		waiting_enter_pressed();
		opponent_fold_decision(every_game_var->bet_amount.raised, &every_game_var->opponent_thought, every_game_var->hand_strength.opponent, opponent_card);
		if (every_game_var->opponent_thought.fold == FOLD) { // 勝負を降りる場合
			*game_fase = OPPONENT_FOLD;
			return;
		}
		else if (every_game_var->opponent_thought.fold == CONTINUE) { // 勝負を受ける場合
			every_game_var->bet_amount.opponent += every_game_var->bet_amount.raised; // いったん相手のベットをプレイヤーにそろえて
			if (every_game_var->bet_amount.raised == 0) { // プレイヤーがチェックした場合
				if (every_game_var->opponent_thought.tendency == AGGRESSIVE) { // opponent側のレイズ額を決めていく
					if (every_game_var->hand_strength.opponent >= ONE_PAIR) {
						every_game_var->bet_amount.raised = BIG_RAISE;
					}
					else {
						every_game_var->bet_amount.raised = 0;
					}
				}
				else if (every_game_var->opponent_thought.tendency == MODEST) {
					if (every_game_var->hand_strength.opponent >= TWO_PAIR) {
						every_game_var->bet_amount.raised = BIG_RAISE;
					}
					else if (every_game_var->hand_strength.opponent == ONE_PAIR) {
						every_game_var->bet_amount.raised = SMALL_RAISE;
					}
				}
				while (every_game_var->bet_amount.raised > result_count->chips) {
					every_game_var->bet_amount.raised--;
				} // プレイヤーのチップ数を超えないように
				every_game_var->bet_amount.opponent += every_game_var->bet_amount.raised;  // opponentのベット額を増やす
				update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);
				if (every_game_var->bet_amount.raised > 0) {
					printf("相手は %d 枚レイズしました.", every_game_var->bet_amount.raised);
				}
				else {
					printf("相手もチェックしました.");
				}
			}
			else if (every_game_var->bet_amount.raised > 0) { // プレイヤーがレイズした場合
				every_game_var->bet_amount.raised = 0; // プレイヤーのレイズ分をリセット
				if (every_game_var->opponent_thought.tendency == AGGRESSIVE) {
					if (every_game_var->hand_strength.opponent >= THREE_OF_A_KIND) {
						every_game_var->bet_amount.raised = BIG_RAISE;
					}
					else if (every_game_var->hand_strength.opponent == TWO_PAIR) {
						every_game_var->bet_amount.raised = SMALL_RAISE;
					}
				}
				else if (every_game_var->opponent_thought.tendency == MODEST) {
					if (every_game_var->hand_strength.opponent >= STRAIGHT) {
						every_game_var->bet_amount.raised = BIG_RAISE;
					}
					else if (every_game_var->hand_strength.opponent == THREE_OF_A_KIND) {
						every_game_var->bet_amount.raised = SMALL_RAISE;
					} // レイズの傾向と手の強さによっていくらレイズするか決める
				}
				while (every_game_var->bet_amount.raised > result_count->chips) {  // プレイヤーのチップ数を超えないように
					every_game_var->bet_amount.raised--;
				}
				every_game_var->bet_amount.opponent += every_game_var->bet_amount.raised; // opponentがレイズした分
				if (every_game_var->bet_amount.raised > 0) {
					update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);
					printf("相手はレイズを受け入れ, さらに %d 枚レイズしました.", every_game_var->bet_amount.raised);
				}
				else {
					update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);
					printf("相手はコールしました.");
				}
			}
		}

		break;

	case BET_OPPONENT_FIRST_AFTER_EXCHANGE:

		temp = rand() % 10 + 1;
		if (every_game_var->opponent_thought.tendency == AGGRESSIVE) { // 強気の場合
			if (temp <= 1) { // ブラフ
				if (every_game_var->hand_strength.opponent >= ONE_PAIR) {
					every_game_var->bet_amount.raised = BIG_RAISE;
				}
			}
			else if (temp <= 8) {
				if (every_game_var->hand_strength.opponent == TWO_PAIR) {
					if (every_game_var->opponent_thought.anxiety < 3) {
						every_game_var->bet_amount.raised = BIG_RAISE;
					}
				}
				else if (every_game_var->hand_strength.opponent >= THREE_OF_A_KIND) {
					every_game_var->bet_amount.raised = BIG_RAISE;
				}
			}
			else {
				if (every_game_var->opponent_thought.anxiety < 3) {
					if (every_game_var->hand_strength.opponent >= ONE_PAIR) {
						every_game_var->bet_amount.raised = SMALL_RAISE;
					}
				}
				else {
					if (every_game_var->hand_strength.opponent >= TWO_PAIR) {
						every_game_var->bet_amount.raised = SMALL_RAISE;
					}
				}
			}
		}
		else if (every_game_var->opponent_thought.tendency == MODEST) { // 弱気の場合
			if (temp <= 8) {
				if (every_game_var->hand_strength.opponent == STRAIGHT) {
					if (every_game_var->opponent_thought.anxiety < 3) {
						every_game_var->bet_amount.raised = BIG_RAISE;
					}
				}
				else if (every_game_var->hand_strength.opponent >= FLASH) {
					every_game_var->bet_amount.raised = BIG_RAISE;
				}
			}
			else {
				if (every_game_var->opponent_thought.anxiety < 3) {
					if (every_game_var->hand_strength.opponent >= TWO_PAIR) {
						every_game_var->bet_amount.raised = SMALL_RAISE;
					}
				}
				else {
					if (every_game_var->hand_strength.opponent >= THREE_OF_A_KIND) {
						every_game_var->bet_amount.raised = SMALL_RAISE;
					}
				}
			}
		}
		while (every_game_var->bet_amount.raised > result_count->chips) {
			every_game_var->bet_amount.raised--;
		} // プレイヤーのチップ数を超えないように
		every_game_var->bet_amount.opponent += every_game_var->bet_amount.raised;
		update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);
		printf("相手の番です.\n");
		if (every_game_var->bet_amount.raised > 0) {
			printf(" %d 枚レイズしました.", every_game_var->bet_amount.raised);
		}
		else {
			printf("チェックしました.");
		}

		break;
	}
}

///// waiting_enter_pressed ///// ENTERキーの入力を待つ.
void waiting_enter_pressed(void)
{
	char c;
	char buf[100];
	fgets(buf, sizeof(buf), stdin);
	sscanf(buf, "%c", &c);
}

///// input_number_of_bet ///// min以上max以下の枚数をベットさせる
void input_number_of_bet(int min, int max, const int result_count_chips, int* bet_amount_raised)
{
	char buf[10];
	int temp;
	do {
		fgets(buf, sizeof(buf), stdin);
		sscanf(buf, "%d", &temp);
		if (temp >= min && temp <= max && temp > result_count_chips) {
			printf("チップが足りません. 入力しなおしてください. : ");
		}
		else if (temp < min || temp > max) {
			printf(" %d ～ %d の数字を入力してください. : ", min, max);
		}
	} while (temp < min || temp > max || temp > result_count_chips);
	*bet_amount_raised = temp;
}

///// player_exchange ///// プレイヤーのカード交換を受け付ける.戻り値は交換枚数.
int player_exchange(const Result_count result_count,
	const Every_game_var every_game_var,
	const char* hands[],
	Card player_card[],
	const Card opponent_card[])
{
	char yn[10];
	int exclude_number = 0;
	int exchange_count = 0;

	printf("あなたの番です.\n");
	printf("カードを交換しますか ? (y/n) : ");

	do {

		fgets(yn, sizeof(yn), stdin);

		if (strcmp(yn, "n\n") == 0) {

			update_screen(result_count, every_game_var, hands, player_card, opponent_card);

			printf("では手札はそのままです.");
			waiting_enter_pressed();

			return exchange_count;
		}
		else if (strcmp(yn, "y\n") == 0) {

			update_screen(result_count, every_game_var, hands, player_card, opponent_card);

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

						player_card[exclude_number - 1] = player_card[CARD_NUM + exchange_count];

						exchange_count++;

					}
				}
			}
			if (exchange_count == 0) {

				printf("番号が確認できませんでした. 交換せずに進みます.");
				waiting_enter_pressed();

			}
		}
		else {

			printf("y/n で入力してください. : ");

		}

	} while (strcmp(yn, "y\n") * strcmp(yn, "n\n") != 0);

	return exchange_count;

}

///// player_bet ///// playerのベット枚数を決定
void player_bet(Game_fase* game_fase,
	Result_count* result_count,
	Every_game_var* every_game_var,
	const char* hands[],
	const Card player_card[],
	const Card opponent_card[])
{
	switch (*game_fase) {
		case BET_PLAYER_FIRST_BEFORE_EXCHANGE:

			printf("あなたの番です.\n");
			printf("初めにベットするチップの枚数を決めてください.(%d ～ %d) : ", MIN_BET, MAX_BET);
			input_number_of_bet(MIN_BET, MAX_BET, result_count->chips, &every_game_var->bet_amount.raised);
			if (every_game_var->bet_amount.raised <= 2) {
				every_game_var->opponent_thought.anxiety -= 2;
			}
			else if (every_game_var->bet_amount.raised == MAX_BET) {
				every_game_var->opponent_thought.anxiety += 1;
			} // プレイヤーのベット額によって不安度を変化させる
			result_count->chips -= every_game_var->bet_amount.raised;   // 所持枚数を減らす
			every_game_var->bet_amount.player += every_game_var->bet_amount.raised;        // プレイヤーの賭け枚数を増やす
			update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);
			printf(" %d 枚ベットしました.", every_game_var->bet_amount.raised);
			
			break;

		case BET_OPPONENT_FIRST_BEFORE_EXCHANGE:

			player_call(game_fase, result_count, every_game_var, hands, player_card, opponent_card);

			break;

		case BET_PLAYER_FIRST_AFTER_EXCHANGE:

			printf("あなたの番です.\n");

		case BET_OPPONENT_FIRST_AFTER_EXCHANGE:

			if (result_count->chips == 0) {
				printf("チップが無いのでレイズできません.");
				waiting_enter_pressed();
			}
			else {
				printf("チェックなら 0 を, レイズなら上乗せする枚数を入力してください.(0 ～ %d) : ", BIG_RAISE);
				input_number_of_bet(0, BIG_RAISE, result_count->chips, &every_game_var->bet_amount.raised);
			}
			if (every_game_var->bet_amount.raised == 0) {
				update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);
				printf("チェックしました.");
			}
			else {
				result_count->chips -= every_game_var->bet_amount.raised; // 所持枚数を減らす
				every_game_var->bet_amount.player += every_game_var->bet_amount.raised; // プレイヤーの賭け枚数を増やす
				update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);
				printf(" %d 枚レイズしました.", every_game_var->bet_amount.raised);
			}

			break;
	}
}

///// player_call ///// プレイヤーのコール
void player_call(Game_fase* game_fase,
	Result_count* result_count,
	Every_game_var* every_game_var,
	const char* hands[],
	const Card player_card[],
	const Card opponent_card[])
{
	printf("コールしますか ?");
	if (*game_fase == BET_OPPONENT_FIRST_BEFORE_EXCHANGE) {
		printf("\n('n'を選ぶとチップを 1 枚払って勝負を終えます)");
	}
	printf(" (y/n) : ");
	char yn[10];
	do {
		fgets(yn, sizeof(yn), stdin);
		if (strcmp(yn, "n\n") == 0) {
			*game_fase = PLAYER_FOLD;
			update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);
			break;
		}
		else if (strcmp(yn, "y\n") == 0) {
			result_count->chips -= every_game_var->bet_amount.raised;
			every_game_var->bet_amount.player += every_game_var->bet_amount.raised;
			update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);

			if (*game_fase == BET_OPPONENT_FIRST_BEFORE_EXCHANGE || *game_fase == BET_OPPONENT_FIRST_AFTER_EXCHANGE) {
				if (result_count->chips == 0) { // チップが足りないのでコールまで
					printf("コールしました.");
				}
				else {
					printf("さらにレイズすることもできます.\n");
					printf("このままなら 0 を, レイズなら上乗せする枚数を入力してください.(0 ～ %d) : ", BIG_RAISE);
					input_number_of_bet(0, BIG_RAISE, result_count->chips, &every_game_var->bet_amount.raised);
					if (every_game_var->bet_amount.raised == 0) {
						update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);
						printf("コールしました.");
					}
					else if (every_game_var->bet_amount.raised > 0) {
						result_count->chips -= every_game_var->bet_amount.raised;   // 所持枚数を減らす
						every_game_var->bet_amount.player += every_game_var->bet_amount.raised;        // プレイヤーの賭け枚数を増やす
						update_screen(*result_count, *every_game_var, hands, player_card, opponent_card);
						printf(" %d 枚レイズしました.", every_game_var->bet_amount.raised);
					}
				}
			}
			else {
				printf("ベットを %d 枚増やしました.", every_game_var->bet_amount.raised);
			}
			waiting_enter_pressed();
		}
		else {
			printf("y/n で入力してください.");
		}
	} while (strcmp(yn, "y\n") * strcmp(yn, "n\n") != 0);
}
