#ifndef POKER_H
#define POKER_H


#define CARD_NUM	5		//手札の枚数.
#define CHIPS		20		// 初期のチップ枚数
#define MIN_BET		1
#define MAX_BET		5
#define SMALL_RAISE	1
#define BIG_RAISE	2

extern char* suits[];
extern char* hands[];

typedef enum {
	HIGH_CARD, ONE_PAIR, TWO_PAIR, THREE_OF_A_KIND, STRAIGHT, FLASH, FULL_HOUSE,
	FOUR_OF_A_KIND, STRAIGHT_FLASH, ROYAL_FLASH
}Strength; // 役の強さ順
typedef enum {
	BET_PLAYER_FIRST_BEFORE_EXCHANGE, BET_OPPONENT_FIRST_BEFORE_EXCHANGE,
	EXCHANGE_PLAYER_FIRST, EXCHANGE_OPPONENT_FIRST, 
	BET_PLAYER_FIRST_AFTER_EXCHANGE, BET_OPPONENT_FIRST_AFTER_EXCHANGE,
	PLAYER_FOLD, OPPONENT_FOLD,
	SHOW_DOWN, END_OF_GAME} Game_fase;
typedef enum { WIN, LOSE, CHOP } Judgement;
enum { JACK = 41, QUEEN = 45, KING = 49, ACE = 53 }; // 絵札ごとのvalueの最低値.
enum { CLOSE, OPEN };
enum { FOLD, EXIT = 0, CONTINUE };
enum { MODEST, AGGRESSIVE };
enum { PLAYER, OPPONENT };

typedef struct {

	int num;
	int value;
	char* suit;

}Card; // カードを番号(num)、価値(value)、スート(suit)の3つから構成される構造体とする.
// numが同じであってもvalueに差を持たせるのは手札のソートのため.

typedef struct {

	int player;// = 0; // プレイヤーのベット数.
	int opponent;// = 0;  // opponentのベット数.
	int raised;// = 0; // プレイヤーとopponentのベット数の差.

} Bet_amount;

typedef struct {

	int anxiety; // opponentの不安度. 大きくなるほど勝負を降りやすくなる.
	int tendency; // opponentのレイズ数の傾向. MODEST or AGGRESSIVE. AGGRESSIVEなら多め. ゲームごとにランダム.
	int fold; // opponentが勝負を降りるかのフラグ.

} Opponent_thought;

typedef struct {

	int player;
	int opponent;

} Show_card_status; // カードの内容を表示するかどうか.

typedef struct {

	Strength player; // プレイヤーの手札でできている役を評価.
	Strength opponent; // opponentの手札でできている役を評価.

} Hand_strength;

typedef struct {

	int player; // プレイヤーが交換したカードの枚数.
	int opponent; // opponentが交換したカードの枚数.

} Exchange_count;

typedef struct {

	int chips; // プレイヤーのチップ数.
	int win; // 勝利カウンター.
	int lose; // 敗北カウンター.
	int chop; // 引き分けカウンター.
	int player_fold; // プレイヤーが勝負を降りた回数.
	int opponent_fold; // opponentが勝負を降りた回数.
	int games; // 終了したゲーム数.

} Result_count;

typedef struct {

	Show_card_status show_card_status;
	Bet_amount bet_amount;
	Opponent_thought opponent_thought;
	Hand_strength hand_strength;
	Exchange_count exchange_count;

}Every_game_var;

// プロトタイプ宣言
	// 変数初期化
void init_all(Result_count* result_count, Every_game_var* every_game_var);
void init_Result_count(Result_count* result_count);
void every_game_reset(Every_game_var* every_game_var);
void reset_Bet_amount(Bet_amount* bet_amount);
void reset_Opponent_thought(Opponent_thought* opponent_thought);
void reset_Show_card_status(Show_card_status* show_card_status);
void reset_Hand_strength(Hand_strength* hand_strength);
void reset_Exchange_count(Exchange_count* exchange_count);
	// 画面表示
void display_title(void);
void update_screen(const Result_count result_count,
	const Every_game_var every_game_var,
	const char* hands[],
	const Card player_card[],
	const Card opponent_card[]);
void bet_situation(const Result_count result_count, const Bet_amount bet_amount);
void show_card(const Show_card_status show_card_status, const Card player_card[], const Card opponent_card[]);
void show_hand(const Show_card_status show_card_status, const Hand_strength hand_strength, const char* hands[]);
void show_down(const Result_count result_count,
	Every_game_var* every_game_var,
	const Hand_strength hand_strength,
	const char* hands[],
	const Card player_card[],
	const Card opponent_card[]);
	// カードの生成,分配
void shuffle_and_deal(int pre_deck[],
	const char* suits[],
	Hand_strength* hand_strength,
	Card deck[],
	Card player_card[],
	Card opponent_card[]);
void shuffle(int x[]);
void convert(const int pre_deck[], const char* suits[], Card deck[]);
void deal(const Card deck[], Card player[], Card opponent[]);
void sort(Card x[]);
	// 役と勝敗の評価
Strength rate(Card x[]);
void judge(Result_count* result_count,
	Every_game_var* every_game_var,
	const char* hands[],
	const Card player_card[],
	const Card opponent_card[]);
Judgement compare(const Strength strength, const Card player_card[], const Card opponent_card[]);
	// AI
int opponent_exchange(const Strength hand_strength_opponent, Card opponent_card[]);
int change_anxiety_by_player_exchange(const int exchange_count_player);
void opponent_fold_decision(const int bet_amount_raised,
	Opponent_thought* opponent_thought,
	const Strength hand_strength_opponent,
	const Card opponent_card[]);
int fold_or_not(int x, int opponent_thought_anxiety);
void avoid_being_bluffed(const Result_count result_count, int *opponent_thought_anxiety);
void opponent_bet(Game_fase* game_fase,
	Result_count* result_count,
	Every_game_var* every_game_var,
	const char* hands[],
	const Card player_card[],
	const Card opponent_card[]);
	// プレイヤー操作
void waiting_enter_pressed(void);
void input_number_of_bet(int min, int max, const int result_count_chips, int* bet_amount_raised);
int player_exchange(const Result_count result_count,
	const Every_game_var every_game_var,
	const char* hands[],
	Card player_card[],
	const Card opponent_card[]);
void player_bet(Game_fase* game_fase,
	Result_count* result_count,
	Every_game_var* every_game_var,
	const char* hands[],
	const Card player_card[],
	const Card opponent_card[]);
void player_call(Game_fase* game_fase,
	Result_count* result_count,
	Every_game_var* every_game_var,
	const char* hands[],
	const Card player_card[],
	const Card opponent_card[]);

#endif
