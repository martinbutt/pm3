#include "pm3.hh"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <filesystem>

struct gamea gamea;
struct gameb gameb;
struct gamec gamec;
struct saves saves;
struct prefs prefs;
struct gadgets gadgets;
struct gadgets_offsets gadgets_offsets;

char *gamexa = nullptr, *gamexb = nullptr, *gamexc = nullptr, *savesx = nullptr, *prefsx = nullptr, *gadgetsx = nullptr, *gadgets_offsetsx = nullptr;
FILE *fga = nullptr, *fgb = nullptr, *fgc = nullptr, *fgs = nullptr, *fgf = nullptr, *fgg = nullptr, *fgo = nullptr;

void check_consistency()
{
	int all[3932];
	for (int i = 0; i < 3932; ++i)
		all[i] = -1;

	for (int c = 0; c < CLUB_IDX_MAX; ++c) {
		fprintf(stderr, "Club[%3d]\n", c);
		for (int p = 0; p < 24; ++p) {
			if ( gameb.club[c].player_index[p] == -1 )
				continue;

			if (all[ gameb.club[c].player_index[p] ] == -1) {

				all[ gameb.club[c].player_index[p] ] = c;
				printf("Added %d %12.12s to %16.16s\n",
					gameb.club[c].player_index[p],
					gamec.player[    gameb.club[c].player_index[p] ].name,
					gameb.club[c].name);

			} else {
				printf("Duplicate. %4d %12.12s plays for\n"
					"%16.16s AND %16.16s\n",
					gameb.club[c].player_index[p],
					gamec.player[    gameb.club[c].player_index[p] ].name,
					gameb.club[ all[ gameb.club[c].player_index[p] ] ].name,
					gameb.club[c].name);
			}
		}
	}

	for (int i = 0; i < sizeof(all); ++i) {
		if ( all[i] != -1)
			continue;

		fprintf(stderr, "%4d %12.12s is without a club.\n",
			i, gamec.player[i].name);
	}
}

struct gameb::club& get_club(int idx) {
		return gameb.club[idx];
}

struct gamec::player& get_player(int16_t idx) {
	return gamec.player[idx];
}

char determine_player_type(struct gamec::player &p) {
	if (p.hn > p.tk && p.hn > p.ps && p.hn > p.sh) {
		return 'G';
	} else if (p.tk > p.hn && p.tk > p.ps && p.tk > p.sh) {
		return 'D';
	} else if (p.ps > p.hn && p.ps > p.tk && p.ps > p.sh) {
		return 'M';
	}

	return 'A';
}

uint8_t determine_player_rating(struct gamec::player &p) {
    if (p.hn > p.tk && p.hn > p.ps && p.hn > p.sh) {
        return p.hn;
    } else if (p.tk > p.hn && p.tk > p.ps && p.tk > p.sh) {
        return p.tk;
    } else if (p.ps > p.hn && p.ps > p.tk && p.ps > p.sh) {
        return p.ps;
    }

    return p.sh;
}

void change_club(int16_t new_club_idx, int player) {
	struct gamea::manager &manager = gamea.manager[player];
	int old_club_idx = manager.club_idx;
	manager.club_idx = new_club_idx;

	switch (manager.club_idx) {
		case 0 ... 21:
			manager.division = 0;
			manager.stadium.ground_facilities.level = 3;
			manager.stadium.supporters_club.level = 3;
			manager.stadium.flood_lights.level = 2;
			manager.stadium.scoreboard.level = 3;
			manager.stadium.undersoil_heating.level = 1;
			manager.stadium.changing_rooms.level = 2;
			manager.stadium.gymnasium.level = 3;
			manager.stadium.car_park.level = 2;

			for (int i = 0; i < sizeof (manager.stadium.safety_rating); ++i) {
				manager.stadium.safety_rating[i] = 4;
			}

			for (int i = 0; i < 4; ++i) {
				manager.stadium.capacity[i].seating = 10000;
				manager.stadium.capacity[i].terraces = 0;
				manager.stadium.conversion[i].level= 2;
				manager.stadium.area_covering[i].level= 3;
			}

			manager.price.league_match_seating = 15;
			manager.price.league_match_terrace = 13;
			manager.price.cup_match_seating = 18;
			manager.price.cup_match_terrace = 15;

			break;
		case 22 ... 45:
			manager.division = 1;
			manager.stadium.ground_facilities.level = 2;
			manager.stadium.supporters_club.level = 2;
			manager.stadium.flood_lights.level = 2;
			manager.stadium.scoreboard.level = 2;
			manager.stadium.undersoil_heating.level = 1;
			manager.stadium.changing_rooms.level = 2;
			manager.stadium.gymnasium.level = 2;
			manager.stadium.car_park.level = 2;

			for (int i = 0; i < sizeof (manager.stadium.safety_rating); ++i) {
				manager.stadium.safety_rating[i] = 3;
			}

			for (int i = 0; i < 4; ++i) {
				manager.stadium.capacity[i].seating = 5000;
				manager.stadium.capacity[i].terraces = 0;
				manager.stadium.conversion[i].level = 2;
				manager.stadium.area_covering[i].level = 2;
			}

			manager.price.league_match_seating = 13;
			manager.price.league_match_terrace = 11;
			manager.price.cup_match_seating = 16;
			manager.price.cup_match_terrace = 13;

			break;
		case 46 ... 69:
			manager.division = 2;
			manager.stadium.ground_facilities.level = 2;
			manager.stadium.supporters_club.level = 2;
			manager.stadium.flood_lights.level = 1;
			manager.stadium.scoreboard.level = 2;
			manager.stadium.undersoil_heating.level = 0;
			manager.stadium.changing_rooms.level = 1;
			manager.stadium.gymnasium.level = 2;
			manager.stadium.car_park.level = 1;

			for (int i = 0; i < sizeof (manager.stadium.safety_rating); ++i) {
				manager.stadium.safety_rating[i] = 2;
			}

			for (int i = 0; i < 4; ++i) {
				manager.stadium.capacity[i].seating = 2500;
				manager.stadium.capacity[i].terraces = 0;
				manager.stadium.conversion[i].level = 1;
				manager.stadium.area_covering[i].level = 1;
			}

			manager.price.league_match_seating = 11;
			manager.price.league_match_terrace = 9;
			manager.price.cup_match_seating = 14;
			manager.price.cup_match_terrace = 11;

			break;
		case 70 ... 91:
			manager.division = 3;
			manager.stadium.ground_facilities.level = 1;
			manager.stadium.supporters_club.level = 1;
			manager.stadium.flood_lights.level = 1;
			manager.stadium.scoreboard.level = 1;
			manager.stadium.undersoil_heating.level = 0;
			manager.stadium.changing_rooms.level = 1;
			manager.stadium.gymnasium.level = 1;
			manager.stadium.car_park.level = 1;

			for (int i = 0; i < sizeof (manager.stadium.safety_rating); ++i) {
				manager.stadium.safety_rating[i] = 1;
			}

			for (int i = 0; i < 4; ++i) {
				manager.stadium.capacity[i].seating = 1000;
				manager.stadium.capacity[i].terraces = 1;
				manager.stadium.conversion[i].level = 0;
				manager.stadium.area_covering[i].level = 0;
			}

			manager.price.league_match_seating = 9;
			manager.price.league_match_terrace = 7;
			manager.price.cup_match_seating = 12;
			manager.price.cup_match_terrace = 9;

			break;
		case 92 ... 113:
			manager.division = 4;
			manager.stadium.ground_facilities.level = 1;
			manager.stadium.supporters_club.level = 1;
			manager.stadium.flood_lights.level = 0;
			manager.stadium.scoreboard.level = 1;
			manager.stadium.undersoil_heating.level = 0;
			manager.stadium.changing_rooms.level = 0;
			manager.stadium.gymnasium.level = 1;
			manager.stadium.car_park.level = 0;

			for (int i = 0; i < sizeof (manager.stadium.safety_rating); ++i) {
				manager.stadium.safety_rating[i] = 0;
			}

			for (int i = 0; i < 4; ++i) {
				manager.stadium.capacity[i].seating = 500;
				manager.stadium.capacity[i].terraces = 1;
				manager.stadium.conversion[i].level = 0;
				manager.stadium.area_covering[i].level = 0;
			}

			manager.price.league_match_seating = 7;
			manager.price.league_match_terrace = 5;
			manager.price.cup_match_seating = 10;
			manager.price.cup_match_terrace = 7;

			break;
		default:
			fprintf(stderr, "Invalid Club index (%i)\n", new_club_idx);
			exit(EXIT_FAILURE);
	}


    gameb.club[new_club_idx].player_image = gameb.club[old_club_idx].player_image;

	strncpy(gameb.club[new_club_idx].manager, gameb.club[old_club_idx].manager, 16);
	strncpy(gameb.club[old_club_idx].manager, DEFAULT_MANAGER_NAME, 16);
}

std::vector<club_player> find_free_players() {
    std::vector<club_player> free_players;

    for (int i = 0; i < 114; ++i) {
        struct gameb::club &club = get_club(i);
        for (int j = 0; j < 24; ++j) {
            if (club.player_index[j] == -1) {
                continue;
            }
            struct gamec::player &p = gamec.player[ club.player_index[j] ];
            if (club.league == 0 || p.contract != 0) {
                continue;
            }
            club_player free_player {club, p};

            free_players.push_back(free_player);
        }
    }
    return free_players;
}

std::vector<club_player> get_my_players(int player) {
    std::vector<club_player> my_players;

    struct gameb::club &club = get_club(gamea.manager[player].club_idx);
    for (int i = 0; i < 24; ++i) {
        if (club.player_index[i] == -1) {
            continue;
        }
        struct gamec::player &p = gamec.player[club.player_index[i]];
        club_player my_player{club, p};
        my_players.push_back(my_player);
    }
    return my_players;
}


void level_aggression() {
    for (int16_t i = 0; i < 3932; ++i) {
        struct gamec::player &player = get_player(i);
        player.aggr = 5;
    }
}

void load_binaries(int game_nr, const char *saves_path) {
    char* full_path = append_trailing_slash(saves_path);

    size_t gamexa_len = asprintf(&gamexa, "%sGAME%.1dA", full_path != nullptr ? full_path : "", game_nr);
    fga = fopen(gamexa, "r");
    if (fga == nullptr) {
        printf("Could not open file: %s\n", gamexa);
        exit(EXIT_FAILURE);
    }
    fread(&gamea, sizeof (struct gamea), 1, fga);
    fclose(fga);

    size_t gamexb_len = asprintf(&gamexb, "%sGAME%.1dB", full_path != nullptr ? full_path : "", game_nr);
    fgb = fopen(gamexb, "r");
    if (fgb == nullptr) {
        printf("Could not open file: %s\n", gamexb);
        exit(EXIT_FAILURE);
    }
    fread(&gameb, sizeof (struct gameb), 1, fgb);
    fclose(fgb);

    size_t gamexc_len = asprintf(&gamexc, "%sGAME%.1dC", full_path != nullptr ? full_path : "", game_nr);
    fgc = fopen(gamexc, "r");
    if (fgc == nullptr) {
        printf("Could not open file: %s\n", gamexc);
        exit(EXIT_FAILURE);
    }
    fread(&gamec, sizeof (struct gamec), 1, fgc);
    fclose(fgc);
}

void load_default_gamedata(const char *game_path) {
    char *full_path = append_trailing_slash(game_path);

    size_t gamexa_len = asprintf(&gamexa, "%sgamedata.dat", full_path != nullptr ? full_path : "");
    fga = fopen(gamexa, "r");
    if (fga == nullptr) {
        printf("Could not open file: %s\n", gamexa);
        exit(EXIT_FAILURE);
    }
    fread(&gamea, sizeof(struct gamea), 1, fga);
    fclose(fga);
}

void load_default_clubdata(const char *game_path) {
    char *full_path = append_trailing_slash(game_path);

    size_t gamexb_len = asprintf(&gamexb, "%sclubdata.dat", full_path != nullptr ? full_path : "");
    fgb = fopen(gamexb, "r");
    if (fgb == nullptr) {
        printf("Could not open file: %s\n", gamexb);
        exit(EXIT_FAILURE);
    }
    fread(&gameb, sizeof(struct gameb), 1, fgb);
    fclose(fgb);
}

void load_default_playdata(const char *game_path) {
    char *full_path = append_trailing_slash(game_path);

    size_t gamexc_len = asprintf(&gamexc, "%splaydata.dat", full_path != nullptr ? full_path : "");
    fgc = fopen(gamexc, "r");
    if (fgc == nullptr) {
        printf("Could not open file: %s\n", gamexc);
        exit(EXIT_FAILURE);
    }
    fread(&gamec, sizeof(struct gamec), 1, fgc);
    fclose(fgc);
}

void load_metadata(const char *saves_path) {
    char* full_path = append_trailing_slash(saves_path);

    size_t saves_len = asprintf(&savesx, "%sSAVES.DIR", full_path != nullptr ? full_path : "");
    fgs = fopen(savesx, "r");
    if (fgs == nullptr) {
        printf("Could not open file: %s\n", savesx);
        exit(EXIT_FAILURE);
    }
    fread(&saves, sizeof (struct saves), 1, fgs);
    fclose(fgs);

    size_t prefs_len = asprintf(&prefsx, "%sPREFS", full_path != nullptr ? full_path : "");
    fgf = fopen(prefsx, "r");
    if (fgf == nullptr) {
        printf("Could not open file: %s\n", prefsx);
        exit(EXIT_FAILURE);
    }
    fread(&prefs, sizeof (struct prefs), 1, fgf);
    fclose(fgf);
}

void load_gadgets(const char *game_path) {
    char* full_path = append_trailing_slash(game_path);

    size_t gadgets_len = asprintf(&gadgetsx, "%sgadgets.dat", full_path != nullptr ? full_path : "");
    fgg = fopen(gadgetsx, "r");
    if (fgg == nullptr) {
        printf("Could not open file: %s\n", gadgetsx);
        exit(EXIT_FAILURE);
    }
    fread(&gadgets, sizeof (struct gadgets), 1, fgg);
    fclose(fgg);

    size_t gadgetsoff_len = asprintf(&gadgets_offsetsx, "%sgadgets.off", full_path != nullptr ? full_path : "");
    fgo = fopen(gadgets_offsetsx, "r");
    if (fgo == nullptr) {
        printf("Could not open file: %s\n", gadgets_offsetsx);
        exit(EXIT_FAILURE);
    }
    fread(&gadgets_offsets, sizeof (struct gadgets_offsets), 1, fgo);
    fclose(fgo);
}

void save_binaries(int game_nr, const char *saves_path) {

    char* full_path = append_trailing_slash(saves_path);

    size_t gamexa_len = asprintf(&gamexa, "%sGAME%.1dA", full_path != nullptr ? full_path : "", game_nr);
    fga = fopen(gamexa, "w+");
    fwrite(&gamea, sizeof (struct gamea), 1, fga);
    fclose(fga);

    size_t gamexb_len = asprintf(&gamexb, "%sGAME%.1dB", full_path != nullptr ? full_path : "", game_nr);
    fgb = fopen(gamexb, "w+");
    fwrite(&gameb, sizeof (struct gameb), 1, fgb);
    fclose(fgb);

    size_t gamexc_len = asprintf(&gamexc, "%sGAME%.1dC", full_path != nullptr ? full_path : "", game_nr);
    fgc = fopen(gamexc, "w+");
    fwrite(&gamec, sizeof (struct gamec), 1, fgc);
    fclose(fgc);
}

void update_metadata(int game_nr) {
    saves.game[game_nr - 1].year = gamea.year;
    saves.game[game_nr - 1].turn = gamea.turn;
    strcpy(saves.game[game_nr - 1].manager[0].name, gamea.manager[0].name);
    strcpy(saves.game[game_nr - 1].manager[1].name, gamea.manager[1].name);
    saves.game[game_nr - 1].manager[0].club_idx = gamea.manager[0].club_idx;
    saves.game[game_nr - 1].manager[1].club_idx = gamea.manager[1].club_idx;
}

void save_metadata(const char *saves_path) {
    char* full_path = append_trailing_slash(saves_path);

    size_t saves_len = asprintf(&savesx, "%sSAVES.DIR", full_path != nullptr ? full_path : "");
    fgs = fopen(savesx, "w+");
    fwrite(&saves, sizeof (struct saves), 1, fgs);
    fclose(fgs);

    size_t prefs_len = asprintf(&prefsx, "%sPREFS", full_path != nullptr ? full_path : "");
    fgf = fopen(prefsx, "w+");
    fwrite(&prefs, sizeof (struct prefs), 1, fgf);
    fclose(fgf);
}

char* append_trailing_slash(const char* path) {
    char* appended_path = const_cast<char *>(path);

    size_t len = strlen(path);
    if (len == 0 || path[len - 1] == '/' || path[len - 1] == '\\') {
        return appended_path;
    }

    appended_path[len] = PATH_SEPARATOR;
    appended_path[len + 1] = '\0'; // Null-terminate the string
    return appended_path;
}

pm3_game_type get_pm3_game_type(const char *game_path) {
    if (std::filesystem::exists(std::filesystem::path(game_path) / EXE_STANDARD_FILENAME)) {
        return PM3_STANDARD;
    } else if (std::filesystem::exists(std::filesystem::path(game_path) / EXE_DELUXE_FILENAME)) {
        return PM3_DELUXE;
    } else {
        return PM3_UNKNOWN;
    }
}

const char* get_saves_folder(pm3_game_type game_type) {
    if (game_type == PM3_STANDARD) {
        return STANDARD_SAVES_PATH;
    } else if (game_type == PM3_DELUXE) {
        return DELUXE_SAVES_PATH;
    } else {
        fprintf(stderr, "Could not determine PM3 game type\n");
        exit(EXIT_FAILURE);
    }
}
