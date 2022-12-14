#include "player.h"


extern Game game;
extern Map *map;
extern Player *player;
extern Enemy *enemy;
extern Level level;
extern int8_t moveset[MOVESET_LEN][2];
extern bool display_alert;
extern bool no_history;

uint8_t frames = 0;

void player_init() {
    player = calloc(1, sizeof (*player));
    if (!player) {
        fprintf(stderr, "\x1b[1m%s:\x1b[0m In function '%s':\033[31m %s:%i: error: calloc failed\033[0m\n",
            __FILE__, __func__, __FILE__, ((__LINE__) - 3));
        exit(2);
    }
    player->pos = (Position) {.l=0, .c=0};
    map->map_grid[player->pos.l][player->pos.c].visited = true;
    player->action = NO_ACTION;
    player->move = NO_MOVE;
    player->distance = 0;
    player->bonus_stack = 0;
    player->rewind_cnt = 6;
    player->anim_action = false;
    player->history = stack_init();
    if (no_history == false)
        stack_push(player->history, player->pos, player->action);
    
    switch (level) {
    case EASY:
        player->stamina = STAMINA_EASY;
        break;
    case MEDIUM:
        player->stamina = STAMINA_MEDIUM;
        break;
    case HARD:
        player->stamina = STAMINA_HARD;
        break;
    default: break;
    }
}

void player_inputs() {
    if (player->anim_action) {
        player->anim_action = false;
        map_render();
    }
    player->action = NO_ACTION;
    player->move = NO_MOVE;
    cbreak();
    halfdelay(7);
    switch (getch()) {
    case KEY_ESC:
        menu_pause();
        break;
    case KEY_RIGHT:
    case 'D':
    case 'd':
        player->move = RIGHT;
        break;
    case KEY_DOWN:
    case 'S':
    case 's':
        player->move = DOWN;
        break;
    case KEY_LEFT:
    case 'Q':
    case 'q':
        player->move = LEFT;
        break;
    case KEY_UP:
    case 'Z':
    case 'z':
        player->move = UP;
        break;
    case KEY_SPACE:
        player_use_bonus();
        break;
    case 'R':
    case 'r':
        player_rewind();
        break;
    // Secret key bindings, only in caps
    case 'P':
        player->anim_action = true;
        map_render_path(game.path_dist, FORMAT_COLOR_CYAN);
        break;
    case 'O':
        player->anim_action = true;
        map_render_path(game.path_stm, FORMAT_COLOR_GREEN);
        break;
    default: break;
    }
    frames++;
}

void player_update() {
    // Get the correct move coordinates
    int8_t *move = moveset[player->move];
    if (player->move == NO_MOVE || player_is_colliding(player->pos.l + move[0], player->pos.c + move[1])) {
        return;
    }
    // If there is no collision, player can move forward
    switch (player->move) {
    case RIGHT:
        player->distance += map->map_grid[player->pos.l][player->pos.c].distance[0];
        break;
    case DOWN:
        player->distance += map->map_grid[player->pos.l][player->pos.c].distance[1];
        break;
    case LEFT:
        player->distance += map->map_grid[player->pos.l][player->pos.c - 1].distance[0];
        break;
    case UP:
        player->distance += map->map_grid[player->pos.l - 1][player->pos.c].distance[1];
        break;
    default: break;
    }
    // Stamina cost for moving one cell
    player->stamina -= STAMINA_COST;
    // Update player position and history
    player->pos.l += move[0];
    player->pos.c += move[1];
    player_visited_cell_alert(player->pos.l, player->pos.c);
    map->map_grid[player->pos.l][player->pos.c].visited = true;
    if (no_history == false)
        stack_push(player->history, (Position) {.l=player->pos.l, .c=player->pos.c}, player->action);
    
    player_is_colliding_enemy();
}


bool player_is_colliding(uint8_t line, uint8_t col) {
    werase(game.alert_win);
    // Check for out of bound values
    if (IS_NOT_OUT_OF_MAP(line, col)) {         
        // If player tries to cross an obstacle, loses more stamina
        if (IS_OBSTACLE_CELL(line, col)) {
            player->anim_action = true;
            player->stamina -= STAMINA_COST_OBS;
            player->action = HIT_OBSTACLE;
            player_obstacle_alert(line, col);
            if (no_history == false)
                stack_push(player->history, (Position) {.l=player->pos.l, .c=player->pos.c}, player->action);
        } else {
            // If player is on a bonus cell, he gains stamina or stack a bonus
            if (IS_BONUS_CELL(line, col))  {
                player_stack_bonus(line,col);
            }
        }
        return (player->anim_action == true) ? true : false;
    }
    return true;
}

void player_is_colliding_enemy() {
    for (uint8_t i = 0; i < ENEMY_NB; i++) {
        if (enemy[i].alive == false) {
            continue;
        }
        // Check for player to destroy ennemy house
        if (player->pos.l == enemy[i].house.l && player->pos.c == enemy[i].house.c) {
            system("aplay -q assets/sfx/caillou.wav &");
            player_alert_render("You have destroyed %s enemy's house ! You gained %d STM !",
                (ENEMY_NB > 1) ? "an" : "the", (player->stamina + STAMINA_GAIN_ENM_DEFEAT > 100) ?
                    STAMINA_GAIN_ENM_DEFEAT - (player->stamina + STAMINA_GAIN_ENM_DEFEAT - 100) : STAMINA_GAIN_ENM_DEFEAT);
            player->stamina += STAMINA_GAIN_ENM_DEFEAT;
            enemy[i].alive = false;
        }
        // Check for player to encounter an enemy
        if (enemy[i].current.l == player->pos.l && enemy[i].current.c == player->pos.c) {
            player_alert_render("%s enemy \u2620 killed you !", (ENEMY_NB > 1) ? "An" : "The");
            enemy[i].alive = false;
            game.gameover = true;
            return;
        }
    }
}

void player_obstacle_alert(uint8_t line, uint8_t col) {
    system("aplay -q assets/sfx/caillou.wav &");
    player_alert_render("You have hit an obstacle ! You lost %d STM !", STAMINA_COST_OBS);
    // Render the obstacle to be disctincted
    wattron(game.game_win, A_BOLD);
    wattron(game.game_win, COLOR_PAIR(FORMAT_COLOR_WHITE_BG_RED));
    mvwaddstr(game.game_win, line, col, OBSTACLE_CHAR);
    wattroff(game.game_win, COLOR_PAIR(FORMAT_COLOR_WHITE_BG_RED));
    wattron(game.game_win, A_BOLD);
    wrefresh(game.game_win);
}

void player_visited_cell_alert(uint8_t line, uint8_t col) {
    if (map->map_grid[line][col].visited == true) {
        player_alert_render("You have already visited this cell !");
        int16_t index = stack_get_index(player->history, (Position) {.l=line, .c=col});
        // If the cell that will be visit has already been visited, render all cells in history between the two same cells 
        if (index > 0) {
            player->anim_action = true;
            SNode *tmp = player->history->head;
            for (int16_t i = 0; i < index; i++) {
                switch (map->map_grid[tmp->pos.l][tmp->pos.c].cell_type) {
                case ROAD:
                    mvwaddch(game.game_win, tmp->pos.l, tmp->pos.c, PATH_VISITED_CHAR | COLOR_PAIR(FORMAT_COLOR_MAGENTA));
                    break;
                case BONUS:
                    wattron(game.game_win, COLOR_PAIR(FORMAT_COLOR_MAGENTA));
                    mvwaddstr(game.game_win, tmp->pos.l, tmp->pos.c, BONUS_CHAR);
                    wattroff(game.game_win, COLOR_PAIR(FORMAT_COLOR_MAGENTA));
                    break;
                case NO_BONUS:
                    mvwaddch(game.game_win, tmp->pos.l, tmp->pos.c, ',' | COLOR_PAIR(FORMAT_COLOR_MAGENTA));
                    break;
                default: break; 
                }
                tmp = tmp->next;
            }
        }
    }
}

void player_stack_bonus(uint8_t line, uint8_t col) {
    if (player->stamina >= STAMINA_LIMIT_TO_STACK_BONUS && player->bonus_stack < BONUS_STACK_MAX) {
        system("aplay -q assets/sfx/fruit-pickup.wav &");
        player_alert_render("You have put a bonus in your pocket ! ");
        player->bonus_stack++;
        map->map_grid[line][col].cell_type = NO_BONUS;
        player->action = STACK_BONUS;
    } else {
        // /!\ TODO /!\ : change sfx to match with bonus
        system("aplay -q assets/sfx/eat-apple.wav &");
        player_alert_render("You have used a bonus ! You gained %d STM !",
            (player->stamina > STAMINA_MAX) ? STAMINA_GAIN - (player->stamina - STAMINA_MAX) : STAMINA_GAIN);
        player->stamina += STAMINA_GAIN;
        map->map_grid[line][col].cell_type = NO_BONUS;
        player->action = USE_BONUS;
    }
}

void player_use_bonus() {
    if (player->bonus_stack > BONUS_STACK_MIN && player->stamina < 100) {
        // /!\ TODO /!\ : change sfx to match with bonus
        system("aplay -q assets/sfx/eat-apple.wav &");
        player_alert_render("You have used a bonus from your pocket ! You gained %d STM !",
            (player->stamina + STAMINA_GAIN > STAMINA_MAX) ? STAMINA_GAIN - (player->stamina + STAMINA_GAIN - STAMINA_MAX) : STAMINA_GAIN);
        player->stamina += STAMINA_GAIN;
        player->bonus_stack--;
        player->action = USE_STACKED_BONUS;
        if (player->stamina > STAMINA_MAX) {
            player->stamina = STAMINA_MAX;
        }
        if (no_history == false)
            stack_push(player->history, (Position) {.l=player->pos.l, .c=player->pos.c}, player->action);
    } else if (player->stamina >= 100 && player->bonus_stack != 0) {
        player_alert_render("You are full of energy !");
    } else {
        player_alert_render("You don't have any bonus left in your pocket !");
    }
}

void player_rewind() {
    if (player->history->head->next != NULL && player->rewind_cnt != 0) {
        SNode *tmp;
        tmp = player->history->head;
        int cnt = 0;
        int rwd = 0;
        // Skip rewind node in player history
        while(tmp->action == REWIND) {
            tmp = tmp->next;
            cnt++;
        }
        // Go to the right node in the stack to apply a rewind on it
        while (cnt != 0 && tmp->next != NULL) {
            tmp = tmp->next;
            cnt--;
        }
        // Apply rewind
        if ( tmp->next != NULL ) {
            switch (tmp->action) {
            case USE_STACKED_BONUS:
                player->stamina -= STAMINA_GAIN;
                player->bonus_stack++;
                break;
            case USE_BONUS:
                player->stamina++;
                player->stamina -= STAMINA_GAIN;
                map->map_grid[tmp->pos.l][tmp->pos.c].cell_type = BONUS;
                map->map_grid[player->pos.l][player->pos.c].visited = false;
                player_substract_dist(tmp, tmp->next);
                player->pos.l = tmp->next->pos.l;
                player->pos.c = tmp->next->pos.c;
                break;
            case HIT_OBSTACLE:
                player->stamina++;
                player->stamina += STAMINA_COST_OBS;
                break;
            case STACK_BONUS:
                player->stamina++;
                player->bonus_stack--;
                if (no_history == false)
                    map->map_grid[player->history->head->pos.l][player->history->head->pos.c].cell_type = BONUS;

                map->map_grid[player->pos.l][player->pos.c].visited = false;
                player_substract_dist(tmp, tmp->next);
                player->pos.l = tmp->next->pos.l;
                player->pos.c = tmp->next->pos.c;
                break;
            case NO_ACTION:
                player->stamina++;
                map->map_grid[player->pos.l][player->pos.c].visited = false;
                player_substract_dist(tmp, tmp->next);
                player->pos.l = tmp->next->pos.l;
                player->pos.c = tmp->next->pos.c;
                break;
            case REWIND:
                player_alert_render("You can't rewind a rewind !");
                rwd = 1;
                break;
            default: break;
            }
            if (rwd != 0) {
                return;
            }
            if (player->rewind_cnt >= 0) {
                player->rewind_cnt--;
            }
            player->action = REWIND;
            if (no_history == false)
                stack_push(player->history, (Position) {.l=player->pos.l, .c=player->pos.c}, player->action);
            player_alert_render("You have rewind an action !");
        }
    } else {
        player_alert_render("You don't have any rewind left !");
    }
}

void player_substract_dist(SNode *curr, SNode *prev) {
    // Rewind Right move by going back Left
    if (curr->pos.l == prev->pos.l && curr->pos.c > prev->pos.c) {
        player->distance -= map->map_grid[prev->pos.l][prev->pos.c].distance[0];
    }
    // Rewind Down move by boing back Up
    if (prev->pos.l < curr->pos.l && prev->pos.c == curr->pos.c) {
        player->distance -= map->map_grid[prev->pos.l][prev->pos.c].distance[1];
    }
    // Rewind Left move by going back Right
    if (curr->pos.l == prev->pos.l && curr->pos.c < prev->pos.c) {
        player->distance -= map->map_grid[curr->pos.l][curr->pos.c].distance[0];
    }
    // Rewind Up move by going back Down
    if (prev->pos.l > curr->pos.l && prev->pos.c == curr->pos.c) {
        player->distance -= map->map_grid[curr->pos.l][curr->pos.c].distance[1];
    }
}

void player_stats_render() {
    werase(game.stats_win);
    // Render bonus stack
    mvwprintw(game.stats_win, 0, 3, "BONUS");
    mvwprintw(game.stats_win, 0, 12, ".  .");
    for (uint8_t i = 0; i < player->bonus_stack; i++) {
        wattron(game.stats_win, A_BOLD);
        wattron(game.stats_win, COLOR_PAIR(FORMAT_COLOR_GREEN));
        mvwaddstr(game.stats_win, 0, 12 + i * 3, BONUS_CHAR);
        wattroff(game.stats_win, COLOR_PAIR(FORMAT_COLOR_GREEN));
        wattroff(game.stats_win, A_BOLD);
    }
    // Render rewind stat
    mvwprintw(game.stats_win, 2, 3, "REWIND   %u", player->rewind_cnt);
    // Render distance stat
    mvwprintw(game.stats_win, 4, 3, "DISTANCE  %u", player->distance);
    wattroff(game.stats_win, A_BOLD);
    wrefresh(game.stats_win);
    mvwprintw(game.dist_win, 13, 3, "BEST DISTANCE");
    mvwprintw(game.dist_win, 14, 8, "%u", game.path_stm_len);
}

void player_alert_render(const char *__restrict __fmt, ...) {
    if (display_alert == false)
        return;
    va_list arg;
    va_start(arg, __fmt);
    werase(game.alert_win);
    wmove(game.alert_win, 1, 2);
    vw_printw(game.alert_win, __fmt, arg);
    box(game.alert_win, ACS_VLINE, ACS_HLINE);
    wrefresh(game.alert_win);
    va_end(arg);
}

void player_render() {
    wattron(game.stats_win, A_BOLD);
    wattron(game.game_win, COLOR_PAIR(FORMAT_COLOR_CYAN));
    mvwaddstr(game.game_win, player->pos.l, player->pos.c, PLAYER_CHAR);
    wattroff(game.game_win, COLOR_PAIR(FORMAT_COLOR_CYAN));
    wattroff(game.stats_win, A_BOLD);
    player_stats_render();
    enemy_chase_player();
}

void player_free() {
    stack_free(player->history);
    free(player);
}

void enemy_init() {
    enemy = calloc(ENEMY_NB, sizeof (*enemy));
    if (!enemy) {
        fprintf(stderr, "\x1b[1m%s:\x1b[0m In function '%s':\033[31m %s:%i: error: calloc failed\033[0m\n",
            __FILE__, __func__, __FILE__, ((__LINE__) - 3));
        exit(2);
    }
    uint8_t l = 0;
    uint8_t c = 0;
    // Place all enemies randomly on the map
    for (uint8_t i = 0; i < ENEMY_NB; i++) {
        do {
            l = rand() % MAP_LINES;
            c = rand() % MAP_COLS;
        } while (sqrt(pow(l, 2) + pow(c, 2)) < ENEMY_INIT_DIST || map->map_grid[l][c].cell_type != ROAD);
        enemy[i].house = (Position) {.l=l, .c=c};
        enemy[i].current = (Position) {.l=l, .c=c};
        enemy[i].alive = true;
    } 
}

void enemy_chase_player() {
    frames %= ENEMY_SPEED;
    if (!frames) {
        enemy_render();
        return;
    }
    Stack *chase_path = NULL;
    uint8_t l, c;
    for (uint8_t i = 0; i < ENEMY_NB; i++) {
        if (enemy[i].alive == false) {
            continue;
        }
        // Render again previous position of the enemy
        l = enemy[i].current.l;
        c = enemy[i].current.c;
        switch (map->map_grid[l][c].cell_type) {
        case ROAD:
            mvwaddch(game.game_win, l, c,
                ((map->map_grid[l][c].visited) ? PATH_VISITED_CHAR : ' ') | COLOR_PAIR(FORMAT_COLOR_CYAN));
            break;
        case BONUS:
            wattron(game.game_win, COLOR_PAIR(FORMAT_COLOR_GREEN));
            mvwaddstr(game.game_win, l, c, BONUS_CHAR);
            wattroff(game.game_win, COLOR_PAIR(FORMAT_COLOR_GREEN));
            break;
        case NO_BONUS:
            mvwaddch(game.game_win, l, c, ',' | COLOR_PAIR(FORMAT_COLOR_YELLOW));
            break;
        default: break;
        }
        // Refresh shortest path to the player
        chase_path = a_star(enemy[i].current, player->pos, false);
        if (chase_path) {
            enemy[i].current = chase_path->head->next->pos;
        }
        // Check if player and the enemy are colliding
        if (enemy[i].current.l == player->pos.l && enemy[i].current.c == player->pos.c) {
            player_alert_render("%s enemy \u2620 killed you !", (ENEMY_NB > 1) ? "An" : "The");
            stack_free(chase_path);
            game.gameover = true;
            game.victory = false;
            return;
        }
        stack_free(chase_path);
    }
    enemy_render();
}

void enemy_render() {
    wattron(game.game_win, A_BOLD);
    for (uint8_t i = 0; i < ENEMY_NB; i++) {
        if (enemy[i].alive == false) {
            mvwaddstr(game.game_win, enemy[i].house.l, enemy[i].house.c, ENEMY_BROKEN_HOUSE_CHAR);
            continue;
        }
        mvwaddstr(game.game_win, enemy[i].current.l, enemy[i].current.c, ENEMY_CHAR);
        mvwaddstr(game.game_win, enemy[i].house.l, enemy[i].house.c, ENEMY_HOUSE_CHAR);
    }
    wattroff(game.game_win, A_BOLD);
}