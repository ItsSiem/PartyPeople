//
// Created by itssiem on 3/14/24.
//

#ifndef RPS_BRACKET_H
#define RPS_BRACKET_H
#include <string_view>
#include <vector>

#include "rps_match.h"
#include "../../mini_game.h"
#include "../../client.h"

class RPSBracket final : public MiniGame {
public:
    RPSBracket(Client* host, const std::vector<Client*>& players);
    void ProcessInput(Client* client, std::string_view input) override;
    void Update(int delta_time) override;
private:
    int round_time;
    std::vector<RPSMatch> matches;
    void generate_matches();
};

#endif //RPS_BRACKET_H
