#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "poker.h"

int main(void)
{
	srand((unsigned int)time(NULL));
	Game_fase game_fase = rand() % 2; //  初戦の先手と後手をランダムに決定する
	char first_move = game_fase; // 先手がPLAYERかOPPONENTか
	char continue_game = CONTINUE; // ゲームを継続するか
	char yn[10]; // y/n入力待ち用文字列

	Result_count result_count;
	Every_game_var every_game_var;
	init_Result_count(&result_count);
	every_game_reset(&every_game_var);

	int pre_deck[CARD_NUM * 4];	// ランダムな値を格納するための配列. 構造体に変換後別の配列に移す.
	Card deck[CARD_NUM * 4]; // pre_deckに格納した数字をCard型構造体に変換した後に格納するための配列.
	Card player_card[CARD_NUM * 2]; // 一旦deckに格納した後でプレイヤーとopponentに割り振る.
	Card opponent_card[CARD_NUM * 2];

	display_title();

	update_screen(result_count, every_game_var, hands, player_card, opponent_card);
	printf("カードが配られました.\n");
	printf("ENTERキーで手札を見ます.");
	waiting_enter_pressed();

	shuffle_and_deal(pre_deck, suits, &every_game_var.hand_strength, deck, player_card, opponent_card);

	every_game_var.show_card_status.player = OPEN;
	update_screen(result_count, every_game_var, hands, player_card, opponent_card);

	while (continue_game) {
		switch (game_fase) {
			case	BET_PLAYER_FIRST_BEFORE_EXCHANGE:

				player_bet(&game_fase, &result_count, &every_game_var, hands, player_card, opponent_card);

				waiting_enter_pressed();
				update_screen(result_count, every_game_var, hands, player_card, opponent_card);

				opponent_bet(&game_fase, &result_count, &every_game_var, hands, player_card, opponent_card);
				if (game_fase == OPPONENT_FOLD) {
					break;
				}
				
				waiting_enter_pressed();

				if (every_game_var.bet_amount.raised > 0) {
					player_call(&game_fase, &result_count, &every_game_var, hands, player_card, opponent_card);
					if (game_fase == PLAYER_FOLD) {
						break;
					}
				}

				update_screen(result_count, every_game_var, hands, player_card, opponent_card);
				game_fase = EXCHANGE_PLAYER_FIRST;
				break;

			case	BET_OPPONENT_FIRST_BEFORE_EXCHANGE:

				opponent_bet(&game_fase, &result_count, &every_game_var, hands, player_card, opponent_card);

				waiting_enter_pressed();
				update_screen(result_count, every_game_var, hands, player_card, opponent_card);

				player_bet(&game_fase, &result_count, &every_game_var, hands, player_card, opponent_card);
				if (game_fase == PLAYER_FOLD) {
					break;
				}

				if (every_game_var.hand_strength.opponent == HIGH_CARD) {
					if (opponent_card[0].value < JACK && every_game_var.bet_amount.raised == BIG_RAISE) {
						game_fase = OPPONENT_FOLD;
						break;
					}
					if (opponent_card[0].value < QUEEN) {
						every_game_var.opponent_thought.anxiety += every_game_var.bet_amount.raised;
					}
				}

				if (every_game_var.bet_amount.raised > 0) {
					every_game_var.bet_amount.opponent += every_game_var.bet_amount.raised;
					update_screen(result_count, every_game_var, hands, player_card, opponent_card);
					printf("相手はコールしました.");
					waiting_enter_pressed();
				}

				game_fase = EXCHANGE_OPPONENT_FIRST;
				break;

			case	EXCHANGE_PLAYER_FIRST:

				every_game_var.exchange_count.player =
				player_exchange(result_count, every_game_var, hands, player_card, opponent_card);

				sort(player_card);
				every_game_var.hand_strength.player = rate(player_card);

				every_game_var.opponent_thought.anxiety +=
				change_anxiety_by_player_exchange(every_game_var.exchange_count.player);

				if (every_game_var.exchange_count.player > 0) {
					update_screen(result_count, every_game_var, hands, player_card, opponent_card);
					printf(" %d 枚交換しました.", every_game_var.exchange_count.player);
					waiting_enter_pressed();
				}

				every_game_var.exchange_count.opponent =
				opponent_exchange(every_game_var.hand_strength.opponent, opponent_card);

				sort(opponent_card);
				every_game_var.hand_strength.opponent = rate(opponent_card);

				update_screen(result_count, every_game_var, hands, player_card, opponent_card);
				printf("相手の番です.\n");
				if (every_game_var.exchange_count.opponent == 0) {
					printf("相手はカードを交換しませんでした.");
				}
				else{
					printf("カードを %d 枚交換しました.", every_game_var.exchange_count.opponent);
				}
				waiting_enter_pressed();

				game_fase = BET_PLAYER_FIRST_AFTER_EXCHANGE;
				break;

			case	EXCHANGE_OPPONENT_FIRST:

				every_game_var.exchange_count.opponent =
				opponent_exchange(every_game_var.hand_strength.opponent, opponent_card);

				sort(opponent_card);
				every_game_var.hand_strength.opponent = rate(opponent_card);

				update_screen(result_count, every_game_var, hands, player_card, opponent_card);
				printf("相手の番です.\n");
				if (every_game_var.exchange_count.opponent == 0) {
					printf("相手はカードを交換しませんでした.");
				}
				else {
					printf("カードを %d 枚交換しました.", every_game_var.exchange_count.opponent);
				}
				waiting_enter_pressed();
				update_screen(result_count, every_game_var, hands, player_card, opponent_card);

				every_game_var.exchange_count.player =
				player_exchange(result_count, every_game_var, hands, player_card, opponent_card);

				sort(player_card);
				every_game_var.hand_strength.player = rate(player_card);

				every_game_var.opponent_thought.anxiety +=
				change_anxiety_by_player_exchange(every_game_var.exchange_count.player);

				if (every_game_var.exchange_count.player > 0) {
					update_screen(result_count, every_game_var, hands, player_card, opponent_card);
					printf(" %d 枚交換しました.", every_game_var.exchange_count.player);
					waiting_enter_pressed();
				}

				game_fase = BET_OPPONENT_FIRST_AFTER_EXCHANGE;
				break;

			case	BET_PLAYER_FIRST_AFTER_EXCHANGE:

				every_game_var.bet_amount.raised = 0;
				update_screen(result_count, every_game_var, hands, player_card, opponent_card);

				player_bet(&game_fase, &result_count, &every_game_var, hands, player_card, opponent_card);

				waiting_enter_pressed();
				update_screen(result_count, every_game_var, hands, player_card, opponent_card);

				opponent_bet(&game_fase, &result_count, &every_game_var, hands, player_card, opponent_card);
				if (game_fase == OPPONENT_FOLD) {
					break;
				}
				waiting_enter_pressed();

				if (every_game_var.bet_amount.raised > 0) {
					player_call(&game_fase, &result_count, &every_game_var, hands, player_card, opponent_card);
					if (game_fase == PLAYER_FOLD) {
						break;
					}
				}

				game_fase = SHOW_DOWN;
				break;

			case	BET_OPPONENT_FIRST_AFTER_EXCHANGE:

				every_game_var.bet_amount.raised = 0;

				opponent_bet(&game_fase, &result_count, &every_game_var, hands, player_card, opponent_card);

				waiting_enter_pressed();
				update_screen(result_count, every_game_var, hands, player_card, opponent_card);

				printf("あなたの番です.\n");
				if (every_game_var.bet_amount.raised > 0) {
					player_call(&game_fase, &result_count, &every_game_var, hands, player_card, opponent_card);
					if (game_fase == PLAYER_FOLD) {
						break;
					}
				}
				else {
					player_bet(&game_fase, &result_count, &every_game_var, hands, player_card, opponent_card);
					waiting_enter_pressed();
				}

				if (every_game_var.bet_amount.raised == BIG_RAISE) {
					every_game_var.opponent_thought.anxiety++;
				}
				else if (result_count.chips == 0 && every_game_var.bet_amount.raised == SMALL_RAISE) { // SMALL_RAISEだが所持チップ全掛け
					if (every_game_var.hand_strength.opponent < TWO_PAIR) {
						every_game_var.opponent_thought.anxiety++;
					}
				}// プレイヤーの強気のレイズで不安になる.

				if (every_game_var.bet_amount.raised > 0) { // プレイヤーがレイズした場合opponentがコールするか決める
					update_screen(result_count, every_game_var, hands, player_card, opponent_card);
					printf("相手の番です.");
					waiting_enter_pressed();

					opponent_fold_decision(every_game_var.bet_amount.raised, &every_game_var.opponent_thought, every_game_var.hand_strength.opponent, opponent_card);

					if (every_game_var.opponent_thought.fold == FOLD) { // 勝負を降りる場合
						game_fase = OPPONENT_FOLD;
						break;
					}
					else {
						every_game_var.bet_amount.opponent += every_game_var.bet_amount.raised;
						update_screen(result_count, every_game_var, hands, player_card, opponent_card);
						printf("コールしました.");
						waiting_enter_pressed();
					}
				}

				game_fase = SHOW_DOWN;
				break;

			case	PLAYER_FOLD:
				;
				int lost_chips = every_game_var.bet_amount.player;
				if (lost_chips == 0) {
					lost_chips++;
					result_count.chips -= lost_chips;
				}
				reset_Bet_amount(&every_game_var.bet_amount);
				update_screen(result_count, every_game_var, hands, player_card, opponent_card);
				printf("勝負を降りました.\n");
				result_count.player_fold++;
				printf(" %d 枚のチップを失いました.", lost_chips);
				waiting_enter_pressed();
				game_fase = END_OF_GAME;
				break;

			case	OPPONENT_FOLD:
				;
				int gained_chips = every_game_var.bet_amount.opponent;
				if (gained_chips == 0) {
					gained_chips++;
				}
				result_count.opponent_fold++;
				result_count.chips += (gained_chips + every_game_var.bet_amount.player);
				reset_Bet_amount(&every_game_var.bet_amount);
				update_screen(result_count, every_game_var, hands, player_card, opponent_card);
				printf("相手は勝負を降りました.\n");
				printf("チップを %d 枚獲得します.", gained_chips);
				waiting_enter_pressed();
				game_fase = END_OF_GAME;
				break;

			case	SHOW_DOWN:

				show_down(result_count, &every_game_var, every_game_var.hand_strength, hands, player_card, opponent_card);
				judge(&result_count, &every_game_var, hands, player_card, opponent_card);
				game_fase = END_OF_GAME;
				break;

			case	END_OF_GAME:

				if (result_count.chips <= 0) {
					printf("\n\n\tチップがなくなりました...");
					printf("\n\tGAME OVER");
					waiting_enter_pressed();
					continue_game = EXIT;
				}
				else {
					printf("\n\n\t次のゲームに進みますか ? (y/n) : ");  	// ゲームを続けるか聞く
					do {
						fgets(yn, sizeof(yn), stdin);
						if (strcmp(yn, "n\n") == 0) {
							continue_game = EXIT;
						}
						else if (strcmp(yn, "y\n") == 0) {

							result_count.games++;

							every_game_reset(&every_game_var);
							update_screen(result_count, every_game_var, hands, player_card, opponent_card);

							shuffle_and_deal(pre_deck, suits, &every_game_var.hand_strength, deck, player_card, opponent_card);

							avoid_being_bluffed(result_count, &every_game_var.opponent_thought.anxiety);

							printf("新しいカードを配りました.\n");
							printf("ENTERキーで手札を見ます.");
							waiting_enter_pressed();

							first_move = first_move ? PLAYER : OPPONENT; // 先手後手を入れ替える

							every_game_var.show_card_status.player = OPEN;
							update_screen(result_count, every_game_var, hands, player_card, opponent_card);

							game_fase = first_move;
						}
						else {
							printf("y/n で入力してください.");
						}
					} while (strcmp(yn, "y\n") * strcmp(yn, "n\n") != 0);
				}
				break;

		}

	}
	every_game_reset(&every_game_var);
	update_screen(result_count, every_game_var, hands, player_card, opponent_card);
	printf("Thank you for playing !");	//  ゲームの終了
	waiting_enter_pressed();

	return 0;
}
