#include <cardgame.hpp>
#include "gameplay.cpp"

ACTION cardgame::login(name username) {
  require_auth(username);
  
  auto user_iterator = _users.find(username.value);
  if (user_iterator == _users.end()) {
    user_iterator = _users.emplace(username, [&](auto& new_user){
      new_user.username = username;
    });
  } 
}

ACTION cardgame::startgame(name username) {
  require_auth(username);
  
  auto& user_data = _users.get(username.value, "User doesn't exist");
  
  _users.modify(user_data, username, [&](auto& modified_user_data) { 
    game game_data;
    
    for (uint8_t cards_drawn = 0; cards_drawn < 4; cards_drawn++) {
      draw_one_card(game_data.deck_player, game_data.hand_player);
      draw_one_card(game_data.deck_ai, game_data.hand_ai);
    }
    
    modified_user_data.game_data = game_data;
  });
}

ACTION cardgame::playcard(name username, uint8_t player_card_hand_idx) {
  require_auth(username);
  
  check(player_card_hand_idx < 4, "playcard: invalid hand index");
  
  auto& user_data = _users.get(username.value, "User doesn't exist");
  
  check(user_data.game_data.status == ONGOING, "playcard: This game has already ended");
  
  check(user_data.game_data.selected_card_player == 0, "playcard: Player has already played a card this turn");
  
  _users.modify(user_data, username, [&](auto& modified_user) {
    game& game_data = modified_user.game_data;
    
    //player selects card
    game_data.selected_card_player = game_data.hand_player[player_card_hand_idx];
    game_data.hand_player[player_card_hand_idx] = 0;
    
    //AI picks a card
    int ai_card_hand_idx = ai_choose_card(game_data);
    game_data.selected_card_ai = game_data.hand_ai[ai_card_hand_idx];
    game_data.hand_ai[ai_card_hand_idx] = 0;
    
    resolve_selected_cards(game_data);
    update_game_status(modified_user);
  });
}

ACTION cardgame::nextround(name username) {
  require_auth(username);
  
  auto& user_data = _users.get(username.value, "User doesn't exist");
  
  check(user_data.game_data.status == ONGOING, "This game has ended.");
  
  check(user_data.game_data.selected_card_player != 0, "You need to select a card first");
  
  _users.modify(user_data, username, [&](auto& modified_user) {
    game& game_data = modified_user.game_data;
    
    game_data.life_lost_player = 0;
    game_data.life_lost_ai = 0;
    game_data.selected_card_player = 0;
    game_data.selected_card_ai = 0;
    
    if(game_data.deck_player.size() > 0) {
      draw_one_card(game_data.deck_player, game_data.hand_player);
      draw_one_card(game_data.deck_ai, game_data.hand_ai);
    }
  });  
}

ACTION cardgame::endgame(name username) {
  require_auth(username);
  
  auto& user_data = _users.get(username.value, "User doesn't exist");
  
  _users.modify(user_data, username, [&](auto& modified_user) {
    modified_user.game_data = game();
  });
}
