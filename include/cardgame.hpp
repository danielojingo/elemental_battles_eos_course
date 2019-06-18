#include <eosio/eosio.hpp>

using namespace std;
using namespace eosio;

CONTRACT cardgame : public contract {
  public:
    using contract::contract;
    cardgame(name receiver, name code, datastream<const char*> ds):
      contract(receiver, code, ds),
      _users(receiver, receiver.value),
      _seed(receiver, receiver.value)
      {}

    ACTION login(name username);
    
    ACTION startgame(name username);
    
    ACTION playcard(name username, uint8_t player_card_hand_idx);
    
    ACTION nextround(name username);
    
    ACTION endgame(name username);
  
  private:
  
    enum game_status: int8_t {
      ONGOING = 0,
      PLAYER_WON = 1,
      PLAYER_LOST = -1
    };
    
    enum card_type: uint8_t {
      EMPTY = 0,
      FIRE = 1, 
      WOOD = 2,
      WATER = 3,
      NEUTRAL = 4,
      VOID = 5
    };
      
    struct card {
      uint8_t type;
      uint8_t attack_point;
    };
    
    const map<uint8_t, card> card_dict = {
      { 0, {EMPTY, 0} },       
      { 1, {FIRE, 1} },
      { 2, {FIRE, 1} },
      { 3, {FIRE, 2} },
      { 4, {FIRE, 2} },
      { 5, {FIRE, 3} },
      { 6, {WOOD, 1} },
      { 7, {WOOD, 1} },
      { 8, {WOOD, 2} },
      { 9, {WOOD, 2} },
      { 10, {WOOD, 3} }, 
      { 11, {WATER, 1} },
      { 12, {WATER, 1} },
      { 13, {WATER, 2} },
      { 14, {WATER, 2} },
      { 15, {WATER, 3} },
      { 16, {NEUTRAL, 3} }, 
      { 17, {VOID, 0} }
    };

    
    struct game {
      int8_t life_player = 5;
      int8_t life_ai = 5;
      vector<uint8_t> deck_player = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
      vector<uint8_t> deck_ai = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
      vector<uint8_t> hand_player = {0,0,0,0};
      vector<uint8_t> hand_ai = {0,0,0,0};
      uint8_t selected_card_player = 0;
      uint8_t selected_card_ai = 0;
      int8_t status = ONGOING;
      uint8_t life_lost_player = 0;
      uint8_t life_lost_ai = 0;
    };
  
    TABLE user_info {
      name     username;
      string   usermessage;
      uint64_t win_count = 0;
      uint64_t lost_count = 0;
      game     game_data;
      
      auto primary_key() const {
        return username.value;
      }
    };
    typedef multi_index <"users"_n, user_info> users_table;
    
    TABLE seed {
      uint64_t        key = 1;
      uint32_t        value = 1;

      auto primary_key() const { return key; }
    };
    typedef multi_index<"seed"_n, seed> seed_table;
    
    users_table _users;
    seed_table _seed;
    
    void draw_one_card(vector<uint8_t>& deck, vector<uint8_t>& hand);
    
    int ai_best_card_win_strategy(const int ai_attack_point, const int player_attack_point);

    int ai_min_loss_strategy(const int ai_attack_point, const int player_attack_point);

    int ai_points_tally_strategy(const int ai_attack_point, const int player_attack_point);

    int ai_loss_prevention_strategy(const int8_t life_ai, const int ai_attack_point, const int player_attack_point);

    int ai_choose_card(const game& game_data);
    
    void resolve_selected_cards(game& game_data);
    
    int calculate_ai_card_score(const int strategy_idx, const uint8_t life_ai, const card& ai_card, vector<uint8_t>hand_player);
    
    int calculate_attack_point(const card& card1, const card& card2);
    
    void update_game_status(user_info& user_data);
    
    int random(const int range);
    
};

EOSIO_DISPATCH(cardgame, (login)(startgame)(playcard)(nextround)(endgame))
