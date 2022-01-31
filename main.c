#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <malloc.h>
#include <math.h>
#include <windows.h>
//1) генерация карт (свои, чужие, общие)(проверить чтобы карты выпадали единожды, использовать свитч кейс)(структура: масть + достоинство "heart, diamond, club, spare")
//
//2)вывод их на экран (ascii art)
//
//3)система ставок (уравнять, вабанк, фолд, поставить сверху)(проверка на отрицательность и превышение баланса)(установка блайндов и поочерёдная смена диллера)
//
//4)генерация действий бота (просчёт вероятноси выигрыша, решение о ставке, решение о выходе)
//
//5)условие проигрыша(выход из матча, нулевой баланс)
//
//6)проверка корректности введённых данных

//typedef struct card{
//    char suit;
//    int dignity;
//};

//Настройка бота Томаса
float tomas_check_low = 0.05;

float tomas_call_low = 0.2;

float tomas_check_high = 0.4;
float tomas_call_high = 0.4;
float tomas_raise_low = 0.4;

float tomas_raise_high = 0.9;
float tomas_all_in_low = 0.9;

int blef_tomas = 10;

//Настройка бота Джона
float john_check_low = 0.2;

float john_call_low = 0.25;

float john_check_high = 0.5;
float john_call_high = 0.5;
float john_raise_low = 0.5;

float john_raise_high = 0.85;
float john_all_in_low = 0.85;

int blef_john = 5;

float check_low;

float call_low;

float check_high;
float call_high;
float raise_low;

float raise_high;
float all_in_low;

int blef;

float win_ch = 0;
float draw_chanse = 0;

void ClearScreen();

typedef struct {
    int first;
    int second;
}hand;

typedef struct {
    int first_flop;
    int second_flop;
    int third_flop;
    int turn;
    int river;
}tb;

int usage[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0 };

int copy_usage[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,
                     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };


hand user_hand;
hand bot_hand;
hand bot1_hand;
tb table;

float win_chanse(int round);

typedef struct {
    int baton; // для малых и больших блайндов, если 0 бот делает большую ставку на блайнде
    int bank;
    int bot_balance;
    int bot1_balance;
    int your_balance;
    int bot_bet;
    int bot1_bet;
    int your_bet;
    int restart;
    float user_chanse;
    float bot_chanse;
    float bot1_chanse;
    float win_chanse;
    float win_chanse1;
}Datagame;

Datagame datagame;
//генерация карты с проверкой на использование
int generation();
// Вывод карт стола. карты выводятся по диагонали, так как в строке они съезжают из-за разницы ширины символов
void print_table(int size, tb ptable);
//вывод карт на руке
void print_hand(hand ghand);
//рассчёт вероятности победы на флопе
float flop_comb(int* flop);
//аналогично для тёрна
float turn_comb(int* turn);
//для ривера
float river_comb(int* river);
//функция просто выводит старшую карту в колоде если другие комбинации не найдены
int high_card(int* dignity);
//функция ищет пару. далее функции ищут комбинацию указанную в названии
int pair(int* dignity, int size);
int two_pair(int* dignity, int size);
int set(int* dignity, int size);
int strait(int* dignity, int size);
int flush(int* suit, int* dignity, int size);
int full_house(int* dignity, int size);
int quads(int* dignity, int size);
int straight_flush(int* dignity, int* suit, int size);
int flush_royal(int* dignity, int* suit);
//рассчёт силы комбинации
int combination_power(int* dignity, int* suit, int size);
//сортировка входящего массива карт
void sort(int* arr, int lenght);

Datagame action_bot(Datagame datagame);
Datagame action(Datagame datagame);
//ставка игрока
Datagame bet_player(Datagame datagame);
void printinfo(Datagame datagame);
Datagame blind(Datagame datagame);
Datagame bot_fold(Datagame datagame);
Datagame bot_call(Datagame datagame);
Datagame bot_raise(Datagame datagame);
Datagame bot_all_in(Datagame datagame);
Datagame bot_bet(Datagame datagame);
Datagame bot_check(Datagame datagame);

float win_chanse_bot(int round);
float win_chanse_bot1(int round);

void blind_bots();
void bot_vs_bot();


int main() {
    srand(time(NULL));
    datagame.your_balance = 5000;
    datagame.bot_balance = 5000;
    datagame.baton = 0;
    printf("Choose option:\n1 - You vs bot\n2 - bot Tomas vs bot John\n");
    int gamemode;
    scanf("%d", &gamemode);
    //ClearScreen();
    switch (gamemode) {
        case 1:
            printf("Choose option:\n1 - vs bot Tomas \n2 - vs bot John\n");
            int choose_bot;
            scanf("%d", &choose_bot);
            if (choose_bot == 1) {
                check_low = tomas_check_low;

                call_low = tomas_call_low;

                check_high = tomas_check_high;
                call_high = tomas_call_high;
                raise_low = tomas_raise_low;

                raise_high = tomas_raise_high;
                all_in_low = tomas_all_in_low;

                blef = blef_tomas;
            }
            if (choose_bot == 2) {
                check_low = john_check_low;

                call_low = john_call_low;

                check_high = john_check_high;
                call_high = john_call_high;
                raise_low = john_raise_low;

                raise_high = john_raise_high;
                all_in_low = john_all_in_low;

                blef = blef_john;
            }
            while (datagame.your_balance != 0 && datagame.bot_balance != 0) {
                for (int i = 0; i < 53; i++) {
                    usage[i] = 0;
                }

                printf("\nNEW GAME!\n");

                user_hand.first = generation();
                user_hand.second = generation();

                bot_hand.first = generation();
                bot_hand.second = generation();

                //раунд флоп
                //происходит генерация первых трёх карт для стола и карт для рук бота и игрока. по названиям переменных думаю ясно что есть что
                // флоп- массив для передачи карт стол+рука на проверку наличия комбинаций
                //сначала идёт поиск комбинаций у игрока, потом у бота
                //остальные раунды сделаны аналогично
                table.first_flop = generation();
                table.second_flop = generation();
                table.third_flop = generation();
                int flop[5];
                flop[0] = table.first_flop;
                flop[1] = table.second_flop;
                flop[2] = table.third_flop;
                flop[3] = user_hand.first;
                flop[4] = user_hand.second;
                //printf("gamer comb flop\n");
                print_hand(user_hand);
                printf("Table:\n");
                print_table(3, table);

                // printf("\n%.3f\n", win_chanse(1));

                datagame.user_chanse = flop_comb(flop);
                flop[0] = table.first_flop;
                flop[1] = table.second_flop;
                flop[2] = table.third_flop;
                flop[3] = bot_hand.first;
                flop[4] = bot_hand.second;
                datagame.bot_chanse = flop_comb(flop);
                datagame = blind(datagame);
                datagame.win_chanse = win_chanse(1);
                datagame = action(datagame);
                if (datagame.restart == 1) continue;
                //раунд тёрн
                int turn[6];
                table.turn = generation();
                //printf("gamer comb turn\n");
                printf("Your hand:\n");
                print_hand(user_hand);
                printf("Table:\n");
                print_table(4, table);
                turn[0] = table.first_flop;
                turn[1] = table.second_flop;
                turn[2] = table.third_flop;
                turn[3] = table.turn;
                turn[4] = user_hand.first;
                turn[5] = user_hand.second;
                datagame.user_chanse = turn_comb(turn);
                turn[0] = table.first_flop;
                turn[1] = table.second_flop;
                turn[2] = table.third_flop;
                turn[3] = table.turn;
                turn[4] = bot_hand.first;
                turn[5] = bot_hand.second;

                datagame.bot_chanse = turn_comb(turn);
                datagame.win_chanse = win_chanse(2);
                if (datagame.restart == 1) continue;
                table.river = generation();
                int river[7];
                printf("Your hand:\n");
                print_hand(user_hand);
                printf("Talbe:\n");
                print_table(5, table);
                river[0] = table.first_flop;
                river[1] = table.second_flop;
                river[2] = table.third_flop;
                river[3] = table.turn;
                river[4] = table.river;
                river[5] = user_hand.first;
                river[6] = user_hand.second;
                datagame.user_chanse = river_comb(river);


                river[0] = table.first_flop;
                river[1] = table.second_flop;
                river[2] = table.third_flop;
                river[3] = table.turn;
                river[4] = table.river;
                river[5] = bot_hand.first;
                river[6] = bot_hand.second;

                datagame.bot_chanse = river_comb(river);
                datagame.win_chanse = win_chanse(3);
                datagame = action(datagame);
                if (datagame.restart == 1) continue;
                if (datagame.user_chanse > datagame.bot_chanse) {
                    printf("Bot hand:\n");
                    print_hand(bot_hand);
                    printf("Your hand:\n");
                    print_hand(user_hand);
                    printf("Table:\n");
                    print_table(5, table);
                    printf("You win\n");
                    datagame.your_balance += datagame.bank;
                    datagame.bank = 0;
                    datagame.bot_bet = 0;
                    datagame.your_bet = 0;
                }
                if (datagame.user_chanse < datagame.bot_chanse) {
                    printf("Bot hand:\n");
                    print_hand(bot_hand);
                    printf("Your hand:\n");
                    print_hand(user_hand);
                    printf("Table:\n");
                    print_table(5, table);
                    printf("Bot win\n");
                    datagame.bot_balance += datagame.bank;
                    datagame.bank = 0;
                    datagame.bot_bet = 0;
                    datagame.your_bet = 0;
                }
                if (datagame.user_chanse == datagame.bot_chanse) {
                    printf("Bot hand:\n");
                    print_hand(bot_hand);
                    printf("Your hand:\n");
                    print_hand(user_hand);
                    printf("Table:\n");
                    print_table(5, table);
                    printf("DRAW\n");
                    datagame.your_balance += datagame.bank / 2;
                    datagame.bot_balance += datagame.bank / 2;
                    datagame.bank = 0;
                    datagame.bot_bet = 0;
                    datagame.your_bet = 0;
                }
            }
            if (datagame.bot_balance == 0) printf("You win\n");
            if (datagame.your_balance == 0) printf("Bot win\n");
            break;
        case 2:
            for (int i = 0; i < 100; ++i) {
                datagame.bot_balance = 5000;
                datagame.bot1_balance = 5000;
                while (datagame.bot_balance != 0 && datagame.bot1_balance != 0) {
                    for (int i = 0; i < 53; i++) {
                        usage[i] = 0;
                    }

                    //printf("\nNEW GAME!\n");

                    bot_hand.first = generation();
                    bot_hand.second = generation();

                    bot1_hand.first = generation();
                    bot1_hand.second = generation();

                    //раунд флоп
                    //происходит генерация первых трёх карт для стола и карт для рук бота и игрока. по названиям переменных думаю ясно что есть что
                    // флоп- массив для передачи карт стол+рука на проверку наличия комбинаций
                    //сначала идёт поиск комбинаций у игрока, потом у бота
                    //остальные раунды сделаны аналогично
                    table.first_flop = generation();
                    table.second_flop = generation();
                    table.third_flop = generation();
                    int flop[5];
                    flop[0] = table.first_flop;
                    flop[1] = table.second_flop;
                    flop[2] = table.third_flop;
                    flop[3] = bot_hand.first;
                    flop[4] = bot_hand.second;
                    //printf("gamer comb flop\n");
//                    printf("Bot Tomas hand:\n");
//                    print_hand(bot_hand);
//                    printf("Bot John hand:\n");
//                    print_hand(bot1_hand);
//                    printf("Table:\n");
//                    print_table(3, table);
                    // printf("\n%.3f\n", win_chanse(1));

                    datagame.bot_chanse = flop_comb(flop);
                    if (datagame.bot_chanse < 0) {
                        if (bot_hand.first >= bot_hand.second) {
                            datagame.bot_chanse = (bot_hand.first % 13) / 118 * 100;
                        }
                        else if (bot_hand.first < bot_hand.second) {
                            datagame.bot_chanse = (bot_hand.second % 13) / 118 * 100;
                        }
                    }
                    flop[0] = table.first_flop;
                    flop[1] = table.second_flop;
                    flop[2] = table.third_flop;
                    flop[3] = bot1_hand.first;
                    flop[4] = bot1_hand.second;


                    datagame.bot1_chanse = flop_comb(flop);
                    if (datagame.bot1_chanse < 0) {
                        if (bot1_hand.first >= bot1_hand.second) {
                            datagame.bot1_chanse = (bot1_hand.first % 13) / 118 * 100;
                        }
                        else if (bot1_hand.first < bot1_hand.second) {
                            datagame.bot1_chanse = (bot1_hand.second % 13) / 118 * 100;
                        }
                    }
                    //printf("bot comb flop\n");
                    //print_hand(bot_hand);
                    //print_table(3, table);

                    blind_bots();
                    win_chanse_bot(1);
                    //printf("\nbot Tomas - %f bot John - %f draw - %f\n", datagame.win_chanse, datagame.win_chanse1, draw_chanse);
                    //                datagame.win_chanse = win_chanse_bot(1);
                    //                datagame.win_chanse1 = win_chanse_bot1(1);
                    bot_vs_bot();
                    //                system("pause");
                    //                // ClearScreen();
                    //                system("cls");
                    if (datagame.restart == 1) continue;

                    //printf("user - %.3f\nbot - %.3f\n\n", datagame.user_chanse, datagame.bot_chanse);
                    // printf("\nwin percent = %.3f\n", win_chanse(1));

                    //раунд тёрн
                    int turn[6];
                    table.turn = generation();
                    //printf("gamer comb turn\n");
//                    printf("Bot Tomas hand:\n");
//                    print_hand(bot_hand);
//                    printf("Bot John hand:\n");
//                    print_hand(bot1_hand);
//                    printf("Table:\n");
//                    print_table(4, table);
                    turn[0] = table.first_flop;
                    turn[1] = table.second_flop;
                    turn[2] = table.third_flop;
                    turn[3] = table.turn;
                    turn[4] = bot_hand.first;
                    turn[5] = bot_hand.second;
                    datagame.bot_chanse = turn_comb(turn);
                    if (datagame.bot_chanse < 0) {
                        if (bot_hand.first >= bot_hand.second) {
                            datagame.bot_chanse = (bot_hand.first % 13) / 118 * 100;
                        }
                        else if (bot_hand.first < bot_hand.second) {
                            datagame.bot_chanse = (bot_hand.second % 13) / 118 * 100;
                        }
                    }
                    //printf("bot comb turn\n");
                    //print_hand(bot_hand);
                    //print_table(4, table);
                    // printf("\n%.3f\n", win_chanse(2));

                    turn[0] = table.first_flop;
                    turn[1] = table.second_flop;
                    turn[2] = table.third_flop;
                    turn[3] = table.turn;
                    turn[4] = bot1_hand.first;
                    turn[5] = bot1_hand.second;

                    datagame.bot1_chanse = turn_comb(turn);
                    if (datagame.bot1_chanse < 0) {
                        if (bot1_hand.first >= bot1_hand.second) {
                            datagame.bot1_chanse = (bot1_hand.first % 13) / 118 * 100;
                        }
                        else if (bot1_hand.first < bot1_hand.second) {
                            datagame.bot1_chanse = (bot1_hand.second % 13) / 118 * 100;
                        }
                    }

                    win_chanse_bot(2);
                    //printf("\nbot Tomas - %f bot John - %f draw - %f\n", datagame.win_chanse, datagame.win_chanse1, draw_chanse);
                    //                datagame.win_chanse = win_chanse_bot(2);
                    //                datagame.win_chanse1 = win_chanse_bot1(2);
                    bot_vs_bot();
                    //                system("pause");
                    //                system("cls");
                    // ClearScreen();
                    if (datagame.restart == 1) continue;

                    //printf("user - %.3f\nbot - %.3f\n\n", datagame.user_chanse, datagame.bot_chanse);
                    //  printf("\nwin percent = %.3f\n", win_chanse(2));
                    //раунд ривер
                    table.river = generation();
                    int river[7];
                    //printf("gamer comb turn\n");
//                    printf("Bot Tomas hand:\n");
//                    print_hand(bot_hand);
//                    printf("Bot John hand:\n");
//                    print_hand(bot1_hand);
//                    printf("Talbe:\n");
//                    print_table(5, table);
                    river[0] = table.first_flop;
                    river[1] = table.second_flop;
                    river[2] = table.third_flop;
                    river[3] = table.turn;
                    river[4] = table.river;
                    river[5] = bot_hand.first;
                    river[6] = bot_hand.second;
                    datagame.bot_chanse = river_comb(river);
                    if (datagame.bot_chanse < 0) {
                        if (bot_hand.first >= bot_hand.second) {
                            datagame.bot_chanse = (bot_hand.first % 13) / 118 * 100;
                        }
                        else if (bot_hand.first < bot_hand.second) {
                            datagame.bot_chanse = (bot_hand.second % 13) / 118 * 100;
                        }
                    }
                    //printf("bot comb river\n");
                    //print_hand(bot_hand);
                    //print_table(5, table);
                    // printf("\n%.3f\n", win_chanse(3));

                    river[0] = table.first_flop;
                    river[1] = table.second_flop;
                    river[2] = table.third_flop;
                    river[3] = table.turn;
                    river[4] = table.river;
                    river[5] = bot1_hand.first;
                    river[6] = bot1_hand.second;

                    datagame.bot1_chanse = river_comb(river);
                    if (datagame.bot1_chanse < 0) {
                        if (bot1_hand.first >= bot1_hand.second) {
                            datagame.bot1_chanse = (bot1_hand.first % 13) / 118 * 100;
                        }
                        else if (bot1_hand.first < bot1_hand.second) {
                            datagame.bot1_chanse = (bot1_hand.second % 13) / 118 * 100;
                        }
                    }
                    win_chanse_bot(3);
                    //printf("\nbot TOmas - %f bot John - %f draw - %f\n", datagame.win_chanse, datagame.win_chanse1, draw_chanse);
                    //                datagame.win_chanse = win_chanse_bot(3);
                    //                datagame.win_chanse1 = win_chanse_bot1(3);
                    bot_vs_bot();
                    //                system("pause");
                    //                //ClearScreen();
                    //                system("cls");
                    if (datagame.restart == 1) continue;

                    //printf("user - %.3f\nbot - %.3f\n\n", datagame.user_chanse, datagame.bot_chanse);
                    //  printf("\nwin percent = %.3f\n", win_chanse(3));
                    if (datagame.bot_chanse > datagame.bot1_chanse) {
                        //                        printf("Bot Tomas hand:\n");
                        //                        print_hand(bot_hand);
                        //                        printf("Bot John hand:\n");
                        //                        print_hand(bot1_hand);
                        //                        printf("Table:\n");
                        //                        print_table(5, table);
                        //                        printf("Bot Tomas win\n");
                        //                    system("pause");
                        //                    //ClearScreen();
                        //                    system("cls");
                        datagame.bot_balance += datagame.bank;
                        datagame.bank = 0;
                        datagame.bot_bet = 0;
                        datagame.bot1_bet = 0;
                    }
                    if (datagame.bot_chanse < datagame.bot1_chanse) {
                        //                        printf("Bot Tomas hand:\n");
                        //                        print_hand(bot_hand);
                        //                        printf("Bot John hand:\n");
                        //                        print_hand(bot1_hand);
                        //                        printf("Table:\n");
                        //                        print_table(5, table);
                        //                        printf("Bot John win\n");
                        //                    system("pause");
                        //                    system("cls");
                        //ClearScreen();
                        datagame.bot1_balance += datagame.bank;
                        datagame.bank = 0;
                        datagame.bot_bet = 0;
                        datagame.bot1_bet = 0;
                    }
                    if (datagame.bot_chanse == datagame.bot1_chanse) {
                        //                        printf("Bot Tomas hand:\n");
                        //                        print_hand(bot_hand);
                        //                        printf("Bot John hand:\n");
                        //                        print_hand(bot1_hand);
                        //                        printf("Table:\n");
                        //                        print_table(5, table);
                        //                        printf("DRAW\n");
                        //                    system("pause");
                        //                    system("cls");
                        //ClearScreen();
                        datagame.bot_balance += datagame.bank / 2;
                        datagame.bot1_balance += datagame.bank / 2;
                        datagame.bank = 0;
                        datagame.bot_bet = 0;
                        datagame.bot1_bet = 0;
                    }
                }

                //if (datagame.bot_balance != 0 && datagame.your_balance != 0) goto checkpoint;

                if (datagame.bot_balance == 0) printf("Bot John win\n");
                if (datagame.bot1_balance == 0) printf("Bot Tomas win\n");
                //break;
            }
    }
    return 0;
}

int generation() {
    int card = rand() % 52 + 1;
    while (1 > 0) {
        if (usage[card] != 0) {
            card = rand() % 52 + 1;
        }
        if (usage[card] == 0) {
            usage[card] += 1;
            return card;
        }
    }
}

float flop_comb(int* flop) {
    //поскольку максимально возможная сила комбинации 118 то силу(strong) я рассчитываю просто как отношение существующей силы к максимальной
    //здесь массив сортируется и путём деления получаются масти и достоинство карт, после чего сформированный массив отправляется на проверку
    //остальные функции раундов действуют аналогично
    float strong = 0;
    int high_hand;
    if ((flop[3] % 13 + 1) > (flop[4] % 13 + 1)) {
        high_hand = flop[3] % 13 + 2;
    }
    else {
        high_hand = flop[4] % 13 + 2;
    }
    int suit[5];
    int dignity[5];
    sort(flop, 5);
    for (int i = 0; i < 5; ++i) {
        if (flop[i] != 52) {
            suit[i] = flop[i] / 13;
        }
        else { suit[i] = 0; }
        dignity[i] = flop[i] % 13 + 2;
    }
    strong = combination_power(dignity, suit, 5);
    if (strong == -1) {
        strong = high_hand;
    }
    //printf("\n\n");
    return strong / 118 * 100;
}
float turn_comb(int* turn) {
    float strong = 0;
    int suit[6];
    int dignity[6];
    int high_hand;
    if ((turn[4] % 13 + 1) > (turn[5] % 13 + 1)) {
        high_hand = turn[4] % 13 + 2;
    }
    else {
        high_hand = turn[5] % 13 + 2;
    }
    sort(turn, 6);
    for (int i = 0; i < 6; ++i) {
        if (turn[i] != 52) {
            suit[i] = turn[i] / 13;
        }
        else { suit[i] = 0; }
        dignity[i] = turn[i] % 13 + 2;
    }
    strong = combination_power(dignity, suit, 6);
    if (strong == -1) {
        strong = high_hand;
    }
    //printf("\n\n");
    return strong / 118 * 100;
}
float river_comb(int* river) {
    float strong = 0;
    int suit[7];
    int dignity[7];
    int high_hand;
    if ((river[5] % 13 + 1) > (river[6] % 13 + 1)) {
        high_hand = river[5] % 13 + 2;
    }
    else {
        high_hand = river[6] % 13 + 2;
    }
    sort(river, 7);
    for (int i = 0; i < 7; ++i) {
        if (river[i] != 52) {
            suit[i] = river[i] / 13;
        }
        else { suit[i] = 0; }
        dignity[i] = river[i] % 13 + 2;
    }
    strong = combination_power(dignity, suit, 7);
    //printf("\n\n");
    if (strong == -1) {
        strong = high_hand;
    }
    return strong / 118 * 100;
}

void sort(int* arr, int lenght) {
    //обычная сортировка шейкером, параметры сортировки описаны в статье
    int right = lenght - 1;
    int left = 0;
    int share = 1;
    int n = 0;
    while (left < right && share>0) {
        share = 0;
        for (long long j = left; j < right; ++j) {
            if (arr[j] < arr[j + 1]) {
                n = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = n;
                share = 1;
            }
        }
        right--;
        for (long long i = right; i > left; --i) {
            if (arr[i - 1] < arr[i]) {
                n = arr[i];
                arr[i] = arr[i - 1];
                arr[i - 1] = n;
                share = 1;
            }
        }
        left++;
    }
    right = lenght - 1;
    left = 0;
    share = 1;
    n = 0;
    while (left < right && share>0) {
        share = 0;
        for (long long j = left; j < right; ++j) {
            if (arr[j] % 13 < arr[j + 1] % 13) {
                n = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = n;
                share = 1;
            }
        }
        right--;
        for (long long i = right; i > left; --i) {
            if (arr[i - 1] % 13 < arr[i] % 13) {
                n = arr[i];
                arr[i] = arr[i - 1];
                arr[i - 1] = n;
                share = 1;
            }
        }
        left++;
    }
    //    for (int i = 0; i < lenght; ++i) {
    //        printf("%d ", arr[i]);
    //    }
    //    printf("\n");

}

void print_table(int size, tb ptable) {
    int suit[5];
    int dignity[5];
    suit[0] = ptable.first_flop / 13;
    suit[1] = ptable.second_flop / 13;
    suit[2] = ptable.third_flop / 13;
    suit[3] = ptable.turn / 13;
    suit[4] = ptable.river / 13;
    for (int i = 0; i < 5; ++i) {
        if(suit[i]==4){
            suit[i]=0;
        }
    }
    dignity[0] = ptable.first_flop % 13 + 2;
    dignity[1] = ptable.second_flop % 13 + 2;
    dignity[2] = ptable.third_flop % 13 + 2;
    dignity[3] = ptable.turn % 13 + 2;
    dignity[4] = ptable.river % 13 + 2;
    for (int i = 0; i < size; ++i) {
        if (i > 0) {

            for (int j = 0; j < i; ++j) {
                printf("\t");
            }
        }
        printf(" -----\n");
        if (i > 0) {

            for (int j = 0; j < i; ++j) {
                printf("\t");
            }
        }
        printf("|%c    |\n", suit[i] + 3);
        if (i > 0) {

            for (int j = 0; j < i; ++j) {
                printf("\t");
            }
        }
        if (dignity[i] < 10) {
            printf("|  %d  |\n", dignity[i]);
        }
        else if (dignity[i] == 10) {
            printf("| %d  |\n", dignity[i]);
        }
        else if (dignity[i] == 11) {
            printf("|  J  |\n");
        }
        else if (dignity[i] == 12) {
            printf("|  Q  |\n");
        }
        else if (dignity[i] == 13) {
            printf("|  K  |\n");
        }
        else if (dignity[i] == 14) {
            printf("|  T  |\n");
        }
        if (i > 0) {

            for (int j = 0; j < i; ++j) {
                printf("\t");
            }
        }
        printf("|    %c|\n", suit[i] + 3);
        if (i > 0) {

            for (int j = 0; j < i; ++j) {
                printf("\t");
            }
        }
        printf(" -----\n");
    }
}
void print_hand(hand ghand) {

    int suit[2];
    int dignity[2];
    if (ghand.first!=52){
        suit[0] = ghand.first / 13;
    }else{suit[0]=0;}
    if (ghand.second!=52){
        suit[1] = ghand.second / 13;
    }else{suit[1]=0;}
    dignity[0] = ghand.first % 13 + 2;
    dignity[1] = ghand.second % 13 + 2;
    for (int i = 0; i < 2; ++i) {
        printf(" -----");
        printf("   ");

    }
    printf("\n");
    for (int i = 0; i < 2; ++i) {
        printf("|%c    |", suit[i] + 3);
        printf("  ");
    }
    printf("\n");
    for (int i = 0; i < 2; ++i) {
        if (dignity[i] < 10) {
            printf("|  %d  |", dignity[i]);
        }
        else if (dignity[i] == 10) {
            printf("| %d  |", dignity[i]);
        }
        else if (dignity[i] == 11) {
            printf("|  J  |");
        }
        else if (dignity[i] == 12) {
            printf("|  Q  |");
        }
        else if (dignity[i] == 13) {
            printf("|  K  |");
        }
        else if (dignity[i] == 14) {
            printf("|  T  |");
        }

        printf("  ");
    }
    printf("\n");
    for (int i = 0; i < 2; ++i) {
        printf("|    %c|", suit[i] + 3);
        printf("  ");
    }
    printf("\n");
    for (int i = 0; i < 2; ++i) {
        printf(" -----");
        printf("   ");
    }
    printf("\n");
}

int flush_royal(int* dignity, int* suit) {
    if (dignity[0] == 14 && dignity[1] == 13 && dignity[2] == 12 && dignity[3] == 11 && dignity[4] == 10) {
        if (suit[0] == suit[1] && suit[2] == suit[3] && suit[4] == suit[1] && suit[2] == suit[4]) {
            return 118;
        }
    }
    return -1;
}
int straight_flush(int* dignity, int* suit, int size) {
    int mast[4] = { 0 };
    int straight = 1;
    for (int i = 0; i < size; ++i) {
        if (suit[i] == 0) {
            mast[0]++;
        }
        else if (suit[i] == 1) {
            mast[1]++;
        }
        else if (suit[i] == 2) {
            mast[2]++;
        }
        else if (suit[i] == 3) {
            mast[3]++;
        }
    }
    if (mast[0] >= 5 || mast[1] >= 5 || mast[2] >= 5 || mast[3] >= 5) {
        for (int i = 1; i < size; ++i) {
            if (dignity[i - 1] == dignity[i] + 1 && suit[i - 1] == suit[i]) {
                straight++;
                if (straight == 5) {
                    return 104 + dignity[0];
                }
            }
            else straight = 0;
        }
    }
    return -1;
}
int quads(int* dignity, int size) {
    for (int i = 3; i < size; ++i) {
        if (dignity[i] == dignity[i - 1]) {
            if (dignity[i - 2] == dignity[i - 3]) {
                if (dignity[i] == dignity[i - 3])
                    return 90 + dignity[i];
            }
        }
    }
    return -1;
}
int full_house(int* dignity, int size) {
    for (int i = 2; i < size; ++i) {
        if (dignity[i] == dignity[i - 1] && dignity[i - 2] == dignity[i - 1]) {
            for (int j = 1; j < size; ++j) {
                if (dignity[j - 1] == dignity[j] && dignity[j] != dignity[i]) {
                    return 78 + dignity[i];
                }
            }
        }
    }
    return -1;
}
int flush(int* suit, int* dignity, int size) {
    int fl[4] = { 0 };
    int max = 0;
    for (int i = 0; i < size; ++i) {
        fl[suit[i]]++;
    }

    if (fl[0] >= 5) {
        for (int i = 0; i < size; ++i) {
            if (suit[i] == 0 && dignity[i] > max) max = dignity[i];
        }
        return 65 + max;
    }
    else if (fl[1] >= 5) {
        for (int i = 0; i < size; ++i) {
            if (suit[i] == 1 && dignity[i] > max) max = dignity[i];
        }
        return 65 + max;
    }
    else if (fl[2] >= 5) {
        for (int i = 0; i < size; ++i) {
            if (suit[i] == 2 && dignity[i] > max) max = dignity[i];
        }
        return 65 + max;
    }
    else if (fl[3] >= 5) {
        for (int i = 0; i < size; ++i) {
            if (suit[i] == 3 && dignity[i] > max) max = dignity[i];
        }
        return 65 + max;
    }
    return -1;
}
int strait(int* dignity, int size) {
    int straight = 0;
    for (int i = 1; i < size; ++i) {
        if (dignity[i - 1] == dignity[i] + 1) {
            straight++;
            if (straight == 4) {
                return 104 + dignity[0];
            }
        }
        else straight = 0;
    }
    return -1;
}
int set(int* dignity, int size) {
    for (int i = 2; i < size; ++i) {
        if (dignity[i] == dignity[i - 1] && dignity[i - 2] == dignity[i - 1]) {
            return 39 + dignity[i];
        }
    }
    return -1;
}
int two_pair(int* dignity, int size) {
    for (int i = 1; i < size; ++i) {
        if (dignity[i] == dignity[i - 1]) {
            for (int j = 1; j < size; ++j) {
                if (dignity[j - 1] == dignity[j] && dignity[j] != dignity[i]) {
                    return 26 + dignity[i];
                }
            }
        }
    }
    return -1;
}
int pair(int* dignity, int size) {
    for (int i = 1; i < size; ++i) {
        if (dignity[i] == dignity[i - 1]) {
            return 13 + dignity[i];
        }
    }
    return -1;
}
int high_card(int* dignity) {
    //printf("high card\n");
    return -1;
}
int combination_power(int* dignity, int* suit, int size) {
    int result = flush_royal(dignity, suit);
    if (result != -1) {
        //printf("flush_royal\n");
        return result;
    }
    result = straight_flush(dignity, suit, size);
    if (result != -1) {
        //printf("straight_flush\n");
        return result;
    }
    result = quads(dignity, size);
    if (result != -1) {
        // printf("quads\n");
        return result;
    }
    result = full_house(dignity, size);
    if (result != -1) {
        // printf("full_house\n");
        return result;
    }
    result = flush(suit, dignity, size);
    if (result != -1) {
        //printf("flush\n");
        return result;
    }
    result = strait(dignity, size);
    if (result != -1) {
        //printf("strait\n");
        return result;
    }
    result = set(dignity, size);
    if (result != -1) {
        //printf("set\n");
        return result;
    }
    result = two_pair(dignity, size);
    if (result != -1) {
        // printf("two_pair\n");
        return result;
    }
    result = pair(dignity, size);
    if (result != -1) {
        //printf("pair\n");
        return result;
    }
    return high_card(dignity);
}
float win_chanse(int round) {
    hand test_hand;
    tb test_table;
    float chanse = 0;
    for (int i = 0; i < 53; ++i) {
        copy_usage[i] = usage[i];
    }
    float win_procent;
    int user_chanse[7];
    int bot_chanse[7];
    test_table.first_flop = table.first_flop;
    test_table.second_flop = table.second_flop;
    test_table.third_flop = table.third_flop;


    if (round == 1) {
        for (int i = 0; i < 10000; ++i) {
            user_chanse[0] = table.first_flop;
            user_chanse[1] = table.second_flop;
            user_chanse[2] = table.third_flop;
            user_chanse[3] = generation();
            user_chanse[4] = generation();
            user_chanse[5] = generation();
            user_chanse[6] = generation();

            bot_chanse[0] = table.first_flop;
            bot_chanse[1] = table.second_flop;
            bot_chanse[2] = table.third_flop;
            bot_chanse[3] = bot_hand.first;
            bot_chanse[4] = bot_hand.second;
            bot_chanse[5] = user_chanse[3];
            bot_chanse[6] = user_chanse[4];

            test_table.turn = user_chanse[3];
            test_table.river = user_chanse[4];

            test_hand.first = user_chanse[5];
            test_hand.second = user_chanse[6];

            //            printf("\ntest bot hand\n");
            //            print_hand(bot_hand);
            //            printf("\ntest user hand\n");
            //            print_hand(test_hand);
            //            printf("\ntest table\n");
            //            print_table(5,test_table);
            //printf("bot - %.3f \n user - %.3f\n", river_comb(bot_chanse),river_comb(user_chanse));
            if (river_comb(bot_chanse) > river_comb(user_chanse)) {
                chanse += 1;
            }
            for (int j = 0; j < 53; ++j) {
                usage[j] = copy_usage[j];
            }
        }
        return chanse / 10000;
    }
    else if (round == 2) {
        for (int i = 0; i < 10000; ++i) {
            user_chanse[0] = table.first_flop;
            user_chanse[1] = table.second_flop;
            user_chanse[2] = table.third_flop;
            user_chanse[3] = table.turn;
            user_chanse[4] = generation();
            user_chanse[5] = generation();
            user_chanse[6] = generation();

            bot_chanse[0] = table.first_flop;
            bot_chanse[1] = table.second_flop;
            bot_chanse[2] = table.third_flop;
            bot_chanse[3] = bot_hand.first;
            bot_chanse[4] = bot_hand.second;
            bot_chanse[5] = table.turn;
            bot_chanse[6] = user_chanse[4];

            test_table.turn = user_chanse[3];
            test_table.river = user_chanse[4];

            test_hand.first = user_chanse[5];
            test_hand.second = user_chanse[6];
            //printf("bot - %.3f \n user - %.3f\n", river_comb(bot_chanse),river_comb(user_chanse));
            if (river_comb(bot_chanse) > river_comb(user_chanse)) {
                chanse += 1;
            }
            for (int j = 0; j < 53; ++j) {
                usage[j] = copy_usage[j];
            }
        }
        return chanse / 10000;
    }
    else if (round == 3) {
        for (int i = 0; i < 10000; ++i) {
            user_chanse[0] = table.first_flop;
            user_chanse[1] = table.second_flop;
            user_chanse[2] = table.third_flop;
            user_chanse[3] = table.turn;
            user_chanse[4] = table.river;
            user_chanse[5] = generation();
            user_chanse[6] = generation();

            test_table.turn = user_chanse[3];
            test_table.river = user_chanse[4];

            test_hand.first = user_chanse[5];
            test_hand.second = user_chanse[6];

            bot_chanse[0] = table.first_flop;
            bot_chanse[1] = table.second_flop;
            bot_chanse[2] = table.third_flop;
            bot_chanse[3] = bot_hand.first;
            bot_chanse[4] = bot_hand.second;
            bot_chanse[5] = table.turn;
            bot_chanse[6] = table.river;
            //printf("bot - %.3f \n user - %.3f\n", river_comb(bot_chanse),river_comb(user_chanse));
            if (river_comb(bot_chanse) > river_comb(user_chanse)) {
                chanse += 1;
            }
            for (int j = 0; j < 53; ++j) {
                usage[j] = copy_usage[j];
            }
        }
        return chanse / 10000;
    }
}

void ClearScreen()
{
    HANDLE                     hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD                      count;
    DWORD                      cellCount;
    COORD                      homeCoords = { 0, 0 };

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return;

    /* Get the number of cells in the current buffer */
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    /* Fill the entire buffer with spaces */
    if (!FillConsoleOutputCharacter(
            hStdOut,
            (TCHAR)' ',
            cellCount,
            homeCoords,
            &count
    )) return;

    /* Fill the entire buffer with the current colors and attributes */
    if (!FillConsoleOutputAttribute(
            hStdOut,
            csbi.wAttributes,
            cellCount,
            homeCoords,
            &count
    )) return;

    /* Move the cursor home */
    SetConsoleCursorPosition(hStdOut, homeCoords);
}

void printinfo(Datagame datagame) {
    printf("bot bet: %d Bot balance: %d\nPlayer bet: %d Player balance: %d\nBank: %d\n", datagame.bot_bet, datagame.bot_balance, datagame.your_bet, datagame.your_balance, datagame.bank);
}

void print_info_bots(Datagame datagame) {
    printf("Bot Tomas bet: %d Bot Tomas balance: %d\nBot John bet: %d Bot John balance: %d\nBank: %d\n", datagame.bot_bet, datagame.bot_balance, datagame.bot1_bet, datagame.bot1_balance, datagame.bank);
}

Datagame blind(Datagame datagame) {
    if (datagame.baton) {

        if (datagame.bot_balance < 50 || datagame.your_balance < 50) {
            datagame.bot_bet += fmin(datagame.bot_balance, datagame.your_balance);
            datagame.your_bet += fmin(datagame.bot_balance, datagame.your_balance);
            /*datagame.bot_balance = 0;
            datagame.your_balance -= datagame.bot_bet;
            datagame.bank += datagame.bot_bet * 2;*/
            __asm {
                mov datagame.bot_balance, 0
                mov eax, datagame.bot_bet
                sub datagame.your_balance, eax
                add datagame.bank, eax
                add datagame.bank, eax
            }
        }
        else if (datagame.your_balance < 100) {
            datagame.bot_bet += 50;
            datagame.your_bet += datagame.your_balance;
            datagame.your_balance = 0;
            datagame.bot_balance -= 50;
            datagame.bank += (datagame.bot_bet + datagame.your_bet);
            __asm {
                add	datagame.bot_bet, 50
                mov eax, datagame.your_balance
                add datagame.your_bet, eax
                mov datagame.your_balance, 0
                sub datagame.bot_balance, 50
                mov   eax, datagame.bot_bet
                add   eax, datagame.your_bet
                mov   datagame.bank, eax
            }

        }
        else {
            /*datagame.bot_bet += 50;
            datagame.your_bet += 100;
            datagame.your_balance -= 100;
            datagame.bot_balance -= 50;
            datagame.bank += (datagame.bot_bet + datagame.your_bet);*/
            __asm {
                add	datagame.bot_bet, 50
                add datagame.your_bet, 100
                sub datagame.your_balance, 100
                sub datagame.bot_balance, 50
                mov   eax, datagame.bot_bet
                add   eax, datagame.your_bet
                mov   datagame.bank, eax
            }

        }
        datagame.baton = 0;
    }

    else {
        if (datagame.bot_balance < 50 || datagame.your_balance < 50) {
            datagame.bot_bet += fmin(datagame.bot_balance, datagame.your_balance);
            datagame.your_bet += fmin(datagame.bot_balance, datagame.your_balance);
            /*
            datagame.your_balance = 0;
            datagame.bot_balance -= datagame.your_bet;
            datagame.bank += datagame.your_bet * 2;*/
            __asm {
                mov datagame.your_balance, 0
                mov eax, datagame.your_bet
                sub datagame.bot_balance, eax
                add datagame.bank, eax
                add datagame.bank, eax
            }

            
        }
        else if (datagame.bot_balance < 100) {
            /*datagame.your_bet += 50;
            datagame.bot_bet += datagame.bot_balance;
            datagame.bot_balance = 0;
            datagame.your_balance -= 50;
            datagame.bank += (datagame.bot_bet + datagame.your_bet);*/
            __asm {
                add	datagame.bot_bet, 50
                mov eax, datagame.bot_balance
                add datagame.your_bet, eax
                mov datagame.bot_balance, 0
                sub datagame.your_balance, 50
                mov   eax, datagame.bot_bet
                add   eax, datagame.your_bet
                mov   datagame.bank, eax
            }
        }
        else {
            /*datagame.bot_bet += 100;
            datagame.your_bet += 50;
            datagame.your_balance -= 50;
            datagame.bot_balance -= 100;
            datagame.bank += (datagame.bot_bet + datagame.your_bet);
            */
            __asm {
                add	datagame.bot_bet, 100
                add datagame.your_bet, 50
                sub datagame.your_balance, 50
                sub datagame.bot_balance, 100
                mov   eax, datagame.bot_bet
                add   eax, datagame.your_bet
                mov   datagame.bank, eax
            }
            

        }
        //datagame.baton = 1;
        __asm {
            mov   datagame.baton, 1
        }
    }
    //printinfo(datagame);

    return datagame;
}; //блайнды (начальные, обязательные ставки)
// меняются каждый раз (baton)
Datagame bet_player(Datagame datagame) {
    int bet;
    int res;
    if (datagame.your_balance != 0) {
        do {
            printf("Make a bet: ");
            res = scanf("%d", &bet);
            while (getchar() != '\n');
            if (res == 1 && bet > 0 && bet >= (datagame.bot_bet - datagame.your_bet) && bet <= datagame.your_balance) {
                //scanf("%d", &bet);
                if (bet > datagame.bot_balance + datagame.bot_bet - datagame.your_bet)
                    bet = datagame.bot_balance + datagame.bot_bet - datagame.your_bet;


            }
            else printf("ERROR\n");
        } while (res != 1 || bet <= 0 || bet < (datagame.bot_bet - datagame.your_bet) || bet > datagame.your_balance);
        datagame.bank += bet;
        datagame.your_bet += bet;
        datagame.your_balance -= bet;
    }
    else printf("ERROR! Your balance = 0!\n");
    return datagame;
};

Datagame bot_fold(Datagame datagame) {
    printf("\n!!!Bot fold!!!\n");
    datagame.restart = 1;
    datagame.your_balance += datagame.bank;
    datagame.bot_bet = 0;
    datagame.your_bet = 0;
    datagame.bank = 0;
    printinfo(datagame);
    return datagame;
}

Datagame bot_call(Datagame datagame) {
    printf("\n!!!Bot call!!!\n");

    /* datagame.bank += datagame.your_bet - datagame.bot_bet;
     datagame.bot_balance -= datagame.your_bet - datagame.bot_bet;
     datagame.bot_bet += datagame.your_bet - datagame.bot_bet;

     printinfo(datagame);
     datagame.restart = 0;
     return datagame;*/

    if ((datagame.your_bet - datagame.bot_bet) <= datagame.bot_balance) {
        datagame.bot_balance -= (datagame.your_bet - datagame.bot_bet);
        datagame.bank += (datagame.your_bet - datagame.bot_bet);
        datagame.bot_bet += (datagame.your_bet - datagame.bot_bet);
        printinfo(datagame);

    }
    else {
        datagame.your_balance += (datagame.your_bet - datagame.bot_bet);
        datagame.bank -= (datagame.your_bet - datagame.bot_bet);

        datagame.your_bet = datagame.bot_bet;
        datagame.your_bet += datagame.bot_balance;
        datagame.your_balance -= datagame.bot_balance;
        datagame.bot_bet += datagame.bot_balance;
        datagame.bank += 2 * datagame.bot_balance;
        datagame.bot_balance = 0;
        printinfo(datagame);

    }
    datagame.restart = 0;
    return datagame;



}

Datagame bot_raise(Datagame datagame) {
    printf("\n!!!Bot raise!!!\n");

    int bet;
    bet = datagame.win_chanse * datagame.bank;
    int raznica = datagame.your_bet - datagame.bot_bet;
    if (datagame.your_balance == 0)  bet = raznica;
    else {
        while (bet < raznica) bet += datagame.win_chanse * datagame.bank;
        if (bet > datagame.bot_balance) bet = datagame.bot_balance;
    }

    datagame.bank += bet;
    datagame.bot_bet += bet;
    datagame.bot_balance -= bet;

    if (datagame.bot_bet < datagame.your_bet) {
        datagame.bank -= datagame.your_bet - datagame.bot_bet;
        datagame.your_bet = datagame.bot_bet;
    }
    //printinfo(datagame);
    datagame = action(datagame);
    return datagame;
}

Datagame bot_all_in(Datagame datagame) {
    printf("\n!!!Bot all-in!!!\n");
    datagame.bank += datagame.bot_balance;
    datagame.bot_bet += datagame.bot_balance;
    datagame.bot_balance = 0;
    //printinfo(datagame);
    datagame = action(datagame);
    return datagame;

    if (datagame.bot_balance > datagame.your_balance + datagame.your_bet - datagame.bot_bet) { //???баланс игрока больше баланса бота учитывая их ставки
        datagame.bot_bet += datagame.your_balance + datagame.your_bet - datagame.bot_bet;
        datagame.bank += datagame.your_balance + datagame.your_bet - datagame.bot_bet;
        datagame.bot_balance -= datagame.your_balance + datagame.your_bet - datagame.bot_bet;
        printinfo(datagame);

        datagame = action(datagame);
    }
    else {
        datagame.bot_bet += datagame.bot_balance;
        datagame.bank += datagame.bot_balance;
        datagame.bot_balance = 0;
        printinfo(datagame);

        datagame = action(datagame);
    }
    return datagame;


}

Datagame bot_bet(Datagame datagame) {
    printf("\n!!!Bot bet!!!\n");
    ////srand(time(NULL));
    //int bet = rand() % 101 + 100;
    //if (bet > datagame.bot_balance) bet = datagame.bot_balance;

    //datagame.bank += bet;
    //datagame.bot_bet += bet;
    //datagame.bot_balance -= bet;
    ////printinfo(datagame);
    //datagame = action(datagame);
    //return datagame;

    int bet;
    bet = datagame.win_chanse * datagame.bank;
    int raznica = datagame.your_bet - datagame.bot_bet;
    if (datagame.your_balance == 0)  bet = raznica;
    else {
        while (bet < raznica) bet += datagame.win_chanse * datagame.bank;
        if (bet > datagame.bot_balance) bet = datagame.bot_balance;
    }


    if (datagame.bot_bet < datagame.your_bet) {
        datagame.bank -= datagame.your_bet - datagame.bot_bet;
        datagame.your_bet = datagame.bot_bet;
    }

    datagame.bank += bet;
    datagame.bot_bet += bet;
    datagame.bot_balance -= bet;
    printinfo(datagame);

    datagame = action(datagame);
    return datagame;
}

Datagame bot_check(Datagame datagame) {
    printf("\n!!!Bot check!!!\n");

    printinfo(datagame);
    //datagame = action (datagame);
    return datagame;
}

Datagame action_bot(Datagame datagame) {
    //Sleep(rand() % 4000 + 2000);
    //printf("\n%f\n", datagame.win_chanse);
    if (datagame.bot_balance != 0 && datagame.your_balance != 0 || datagame.bot_bet != datagame.your_bet) { //условие, чтоб партия доигрывалась не вызывались ставки, если один из игроков не может поставить
        if (datagame.bot_bet < datagame.your_bet) {
            if ((datagame.win_chanse < call_high && datagame.win_chanse > call_low || (rand() % 100) < blef)) {
                datagame = bot_call(datagame);
            }
            else if (datagame.win_chanse > raise_low && datagame.win_chanse < raise_high || (rand() % 100) < blef) {
                datagame = bot_raise(datagame);
            }
            else if (datagame.win_chanse > all_in_low || (rand() % 100) < blef) {
                datagame = bot_all_in(datagame);
            }
            else datagame = bot_fold(datagame);
        }
        else if (datagame.bot_bet == datagame.your_bet || (rand() % 100) < blef) {
            if (datagame.win_chanse > raise_low && datagame.win_chanse < raise_high) {
                datagame = bot_raise(datagame);
            }
            else if (datagame.win_chanse > all_in_low || (rand() % 100) < blef) {
                datagame = bot_all_in(datagame);
            }
            else datagame = bot_check(datagame);
        }
    }
    return datagame;
};

Datagame fold(Datagame datagame) {
    datagame.bot_balance += datagame.bank;
    datagame.bot_bet = 0;
    datagame.your_bet = 0;
    datagame.bank = 0;
    datagame.restart = 1;
    printinfo(datagame);
    return datagame;
};

Datagame call(Datagame datagame) {
    if ((datagame.bot_bet - datagame.your_bet) <= datagame.your_balance) {
        datagame.your_balance -= (datagame.bot_bet - datagame.your_bet);
        datagame.bank += (datagame.bot_bet - datagame.your_bet);
        datagame.your_bet += (datagame.bot_bet - datagame.your_bet);
        printinfo(datagame);

    }
    else {
        datagame.bot_balance += datagame.bot_bet - datagame.your_bet;
        datagame.bank -= datagame.bot_bet - datagame.your_bet;

        datagame.bot_bet = datagame.your_bet;
        datagame.bot_bet += datagame.your_balance;
        datagame.bot_balance -= datagame.your_balance;
        datagame.your_bet += datagame.your_balance;
        datagame.bank += 2 * datagame.your_balance;
        datagame.your_balance = 0;
        printinfo(datagame);

    }
    datagame.restart = 0;
    return datagame;
};

Datagame raise(Datagame datagame) {
    datagame = bet_player(datagame);
    printinfo(datagame);

    datagame = action_bot(datagame);
    return datagame;
};

Datagame all_in(Datagame datagame) {
    int raznica = datagame.your_bet;
    if (datagame.your_balance > datagame.bot_balance + datagame.bot_bet - datagame.your_bet) { //баланс игрока больше баланса бота учитывая их ставки
        datagame.bank += datagame.bot_balance + datagame.bot_bet - datagame.your_bet;
        datagame.your_bet += datagame.bot_balance + datagame.bot_bet - datagame.your_bet;

        datagame.your_balance -= datagame.bot_balance + datagame.bot_bet - raznica;
        printinfo(datagame);

        datagame = action_bot(datagame);
    }
    else {
        datagame.your_bet += datagame.your_balance;
        datagame.bank += datagame.your_balance;
        datagame.your_balance = 0;
        printinfo(datagame);

        datagame = action_bot(datagame);
    }
    return datagame;
};

Datagame check(Datagame datagame) {
    datagame.restart = 0;
    printinfo(datagame);

    if (datagame.baton) datagame = action_bot(datagame);
    return datagame;
};

Datagame action(Datagame datagame) {
    if ((datagame.bot_balance != 0 && datagame.your_balance != 0) || ((datagame.bot_balance != 0 || datagame.your_balance != 0) && datagame.bot_bet != datagame.your_bet)) {
        int choose = 0; //значение введенное пользователем (1.2.3.4)
        int res = 0; //переменная для корректного ввода (для ввода только чисел)
        //if (datagame.bot_balance != 0 && datagame.your_balance != 0 || datagame.bot_bet != datagame.your_bet) {

        printinfo(datagame);
        if (datagame.bot_bet > datagame.your_bet) {
            checkpoint:
            printf("\n1-fold\n2-call\n3-raise\n4-all-in\nChoose your action: ");
            do {
                res = scanf("%d", &choose);
                while (getchar() != '\n');
                if ((res == 1) && ((choose == 1) || (choose == 2) || (choose == 3) || (choose == 4))) {
                    switch (choose) {
                        case 1: //fold
                            datagame = fold(datagame);
                            // закончить раздачу, начать заново
                            break;
                        case 2: //call
                            datagame = call(datagame);
                            break;
                        case 3: //raise
                            datagame = raise(datagame);
                            //действие бота, продолжать игру
                            break;
                        case 4: //all-in
                            datagame = all_in(datagame);
                            break;
                    }
                }
                else {
                    printf("ERROR\n");
                    goto checkpoint;
                }
            } while (res != 1);
        }

        else
        if (datagame.bot_bet < datagame.your_bet) {
            datagame = action_bot(datagame);
        }
        else
        if (datagame.bot_bet == datagame.your_bet && datagame.baton) {
            // if (datagame.baton) {
            checkpoint1:
            printf("\n1-check\n2-bet\n3-all-in\nChoose your action: ");
            do {
                res = scanf("%d", &choose);
                while (getchar() != '\n');
                if ((res == 1) && ((choose == 1) || (choose == 2) || (choose == 3))) {
                    switch (choose) {
                        case 1: //check
                            datagame = check(datagame);
                            break;
                        case 2: //bet
                            datagame = raise(datagame);
                            //действие бота, продолжать игру
                            break;
                        case 3: //all-in
                            datagame = all_in(datagame);
                            break;
                    }
                }
                else {
                    printf("ERROR\n");
                    goto checkpoint1;
                }
            } while (res != 1);
            //}

        }
        else if (datagame.bot_bet == datagame.your_bet && datagame.baton != 1) {
            int check = datagame.bot_bet;
            datagame = action_bot(datagame);
            if (check == datagame.bot_bet) goto checkpoint1;

        }
    }
    else  printinfo(datagame);
    return datagame;
};//действие

float win_chanse_bot(int round) {
    hand test_hand;
    tb test_table;
    float chanse = 0;
    for (int i = 0; i < 53; ++i) {
        copy_usage[i] = usage[i];
    }
    float draw = 0;
    int bot_chanse[7];
    int bot1_chanse[7];
    test_table.first_flop = table.first_flop;
    test_table.second_flop = table.second_flop;
    test_table.third_flop = table.third_flop;


    if (round == 1) {
        for (int i = 0; i < 10000; ++i) {
            bot1_chanse[0] = table.first_flop;
            bot1_chanse[1] = table.second_flop;
            bot1_chanse[2] = table.third_flop;
            bot1_chanse[3] = generation();
            bot1_chanse[4] = generation();
            bot1_chanse[5] = generation();
            bot1_chanse[6] = generation();

            bot_chanse[0] = table.first_flop;
            bot_chanse[1] = table.second_flop;
            bot_chanse[2] = table.third_flop;
            bot_chanse[3] = bot_hand.first;
            bot_chanse[4] = bot_hand.second;
            bot_chanse[5] = bot1_chanse[3];
            bot_chanse[6] = bot1_chanse[4];

            test_table.turn = bot1_chanse[3];
            test_table.river = bot1_chanse[4];

            test_hand.first = bot1_chanse[5];
            test_hand.second = bot1_chanse[6];
            if (river_comb(bot_chanse) > river_comb(bot1_chanse)) {
                chanse += 1;
            }
            else if (river_comb(bot_chanse) == river_comb(bot1_chanse)) {
                draw += 1;
            }
            for (int j = 0; j < 53; ++j) {
                usage[j] = copy_usage[j];
            }
        }
        draw_chanse = draw / 10000;
        win_ch = chanse / 10000;
        datagame.win_chanse1 = 1 - win_ch - draw_chanse;
        datagame.win_chanse = win_ch;
        return chanse / 10000;
    }
    else if (round == 2) {
        for (int i = 0; i < 10000; ++i) {
            bot1_chanse[0] = table.first_flop;
            bot1_chanse[1] = table.second_flop;
            bot1_chanse[2] = table.third_flop;
            bot1_chanse[3] = table.turn;
            bot1_chanse[4] = generation();
            bot1_chanse[5] = generation();
            bot1_chanse[6] = generation();

            bot_chanse[0] = table.first_flop;
            bot_chanse[1] = table.second_flop;
            bot_chanse[2] = table.third_flop;
            bot_chanse[3] = bot_hand.first;
            bot_chanse[4] = bot_hand.second;
            bot_chanse[5] = table.turn;
            bot_chanse[6] = bot1_chanse[4];

            test_table.turn = bot1_chanse[3];
            test_table.river = bot1_chanse[4];

            test_hand.first = bot1_chanse[5];
            test_hand.second = bot1_chanse[6];
            if (river_comb(bot_chanse) > river_comb(bot1_chanse)) {
                chanse += 1;
            }
            else if (river_comb(bot_chanse) == river_comb(bot1_chanse)) {
                draw += 1;
            }
            for (int j = 0; j < 53; ++j) {
                usage[j] = copy_usage[j];
            }
        }
        draw_chanse = draw / 10000;
        win_ch = chanse / 10000;
        datagame.win_chanse1 = 1 - win_ch - draw_chanse;
        datagame.win_chanse = win_ch;
        return chanse / 10000;
    }
    else if (round == 3) {
        for (int i = 0; i < 10000; ++i) {
            bot1_chanse[0] = table.first_flop;
            bot1_chanse[1] = table.second_flop;
            bot1_chanse[2] = table.third_flop;
            bot1_chanse[3] = table.turn;
            bot1_chanse[4] = table.river;
            bot1_chanse[5] = generation();
            bot1_chanse[6] = generation();

            test_table.turn = bot1_chanse[3];
            test_table.river = bot1_chanse[4];

            test_hand.first = bot1_chanse[5];
            test_hand.second = bot1_chanse[6];

            bot_chanse[0] = table.first_flop;
            bot_chanse[1] = table.second_flop;
            bot_chanse[2] = table.third_flop;
            bot_chanse[3] = bot_hand.first;
            bot_chanse[4] = bot_hand.second;
            bot_chanse[5] = table.turn;
            bot_chanse[6] = table.river;
            if (river_comb(bot_chanse) > river_comb(bot1_chanse)) {
                chanse += 1;
            }
            else if (river_comb(bot_chanse) == river_comb(bot1_chanse)) {
                draw += 1;
            }
            for (int j = 0; j < 53; ++j) {
                usage[j] = copy_usage[j];
            }
        }
        draw_chanse = draw / 10000;
        win_ch = chanse / 10000;
        datagame.win_chanse1 = 1 - win_ch - draw_chanse;
        datagame.win_chanse = win_ch;
        return chanse / 10000;
    }
}

float win_chanse_bot1(int round) {
    hand test_hand;
    tb test_table;
    float chanse = 0;
    for (int i = 0; i < 53; ++i) {
        copy_usage[i] = usage[i];
    }
    float win_procent;
    int bot_chanse[7];
    int bot1_chanse[7];
    test_table.first_flop = table.first_flop;
    test_table.second_flop = table.second_flop;
    test_table.third_flop = table.third_flop;


    if (round == 1) {
        for (int i = 0; i < 10000; ++i) {
            bot_chanse[0] = table.first_flop;
            bot_chanse[1] = table.second_flop;
            bot_chanse[2] = table.third_flop;
            bot_chanse[3] = generation();
            bot_chanse[4] = generation();
            bot_chanse[5] = generation();
            bot_chanse[6] = generation();

            bot1_chanse[0] = table.first_flop;
            bot1_chanse[1] = table.second_flop;
            bot1_chanse[2] = table.third_flop;
            bot1_chanse[3] = bot_hand.first;
            bot1_chanse[4] = bot_hand.second;
            bot1_chanse[5] = bot_chanse[3];
            bot1_chanse[6] = bot_chanse[4];

            test_table.turn = bot_chanse[3];
            test_table.river = bot_chanse[4];

            test_hand.first = bot_chanse[5];
            test_hand.second = bot_chanse[6];

            //            printf("\ntest bot hand\n");
            //            print_hand(bot_hand);
            //            printf("\ntest user hand\n");
            //            print_hand(test_hand);
            //            printf("\ntest table\n");
            //            print_table(5,test_table);
            //printf("bot - %.3f \n user - %.3f\n", river_comb(bot_chanse),river_comb(user_chanse));
            if (river_comb(bot1_chanse) > river_comb(bot_chanse)) {
                chanse += 1;
            }
            for (int j = 0; j < 53; ++j) {
                usage[j] = copy_usage[j];
            }
        }
        return chanse / 10000;
    }
    else if (round == 2) {
        for (int i = 0; i < 10000; ++i) {
            bot_chanse[0] = table.first_flop;
            bot_chanse[1] = table.second_flop;
            bot_chanse[2] = table.third_flop;
            bot_chanse[3] = table.turn;
            bot_chanse[4] = generation();
            bot_chanse[5] = generation();
            bot_chanse[6] = generation();

            bot1_chanse[0] = table.first_flop;
            bot1_chanse[1] = table.second_flop;
            bot1_chanse[2] = table.third_flop;
            bot1_chanse[3] = bot_hand.first;
            bot1_chanse[4] = bot_hand.second;
            bot1_chanse[5] = table.turn;
            bot1_chanse[6] = bot_chanse[4];

            test_table.turn = bot_chanse[3];
            test_table.river = bot_chanse[4];

            test_hand.first = bot_chanse[5];
            test_hand.second = bot_chanse[6];
            //printf("bot - %.3f \n user - %.3f\n", river_comb(bot_chanse),river_comb(user_chanse));
            if (river_comb(bot1_chanse) > river_comb(bot_chanse)) {
                chanse += 1;
            }
            for (int j = 0; j < 53; ++j) {
                usage[j] = copy_usage[j];
            }
        }
        return chanse / 10000;
    }
    else if (round == 3) {
        for (int i = 0; i < 10000; ++i) {
            bot_chanse[0] = table.first_flop;
            bot_chanse[1] = table.second_flop;
            bot_chanse[2] = table.third_flop;
            bot_chanse[3] = table.turn;
            bot_chanse[4] = table.river;
            bot_chanse[5] = generation();
            bot_chanse[6] = generation();

            test_table.turn = bot_chanse[3];
            test_table.river = bot_chanse[4];

            test_hand.first = bot_chanse[5];
            test_hand.second = bot_chanse[6];

            bot1_chanse[0] = table.first_flop;
            bot1_chanse[1] = table.second_flop;
            bot1_chanse[2] = table.third_flop;
            bot1_chanse[3] = bot_hand.first;
            bot1_chanse[4] = bot_hand.second;
            bot1_chanse[5] = table.turn;
            bot1_chanse[6] = table.river;
            //printf("bot - %.3f \n user - %.3f\n", river_comb(bot_chanse),river_comb(user_chanse));
            if (river_comb(bot1_chanse) > river_comb(bot_chanse)) {
                chanse += 1;
            }
            for (int j = 0; j < 53; ++j) {
                usage[j] = copy_usage[j];
            }
        }
        return chanse / 10000;
    }
}

void blind_bots() {
    if (datagame.baton) {

        if (datagame.bot_balance < 50 || datagame.bot1_balance < 50) {
            datagame.bot_bet += fmin(datagame.bot_balance, datagame.bot1_balance);
            datagame.bot1_bet += fmin(datagame.bot_balance, datagame.bot1_balance);
            datagame.bot_balance -= datagame.bot_bet;
            datagame.bot1_balance -= datagame.bot_bet;
            datagame.bank += datagame.bot_bet * 2;
        }
        else if (datagame.bot_balance < 100) {
            datagame.bot1_bet += 50;
            datagame.bot1_balance -= 50;
            datagame.bot_bet += datagame.bot_balance;
            datagame.bot_balance = 0;
            datagame.bank += (datagame.bot1_bet + datagame.bot_bet);
        }
        else {
            datagame.bot1_bet += 50;
            datagame.bot_bet += 100;
            datagame.bot_balance -= 100;
            datagame.bot1_balance -= 50;
            datagame.bank += (datagame.bot1_bet + datagame.bot_bet);

        }
        datagame.baton = 0;
    }

    else {
        if (datagame.bot1_balance < 50 || datagame.bot_balance < 50) {
            datagame.bot1_bet += fmin(datagame.bot1_balance, datagame.bot_balance);
            datagame.bot_bet += fmin(datagame.bot1_balance, datagame.bot_balance);
            datagame.bot_balance = 0;
            datagame.bot1_balance -= datagame.bot_bet;
            datagame.bank += datagame.bot_bet * 2;
        }
        else if (datagame.bot1_balance < 100) {
            datagame.bot_bet += 50;
            datagame.bot1_bet += datagame.bot1_balance;
            datagame.bot1_balance = 0;
            datagame.bot_balance -= 50;
            datagame.bank += (datagame.bot1_bet + datagame.bot_bet);
        }
        else {
            datagame.bot1_bet += 100;
            datagame.bot_bet += 50;
            datagame.bot_balance -= 50;
            datagame.bot1_balance -= 100;
            datagame.bank += (datagame.bot1_bet + datagame.bot_bet);

        }
        datagame.baton = 1;
    }
    //printinfo(datagame);
    //print_info_bots(datagame);
    //return datagame;
}; //блайнды (начальные, обязательные ставки)


void bot_vs_bot() {
    if (datagame.bot_balance != 0 && datagame.bot1_balance != 0 || datagame.bot_bet != datagame.bot1_bet) { //условие, чтоб партия доигрывалась не вызывались ставки, если один из игроков не может поставить

        if (datagame.bot1_bet == datagame.bot_bet) {

            if (datagame.baton) {//bot Tomas

                if (datagame.win_chanse > tomas_check_low && datagame.win_chanse < tomas_check_high || (rand() % 100) < blef_tomas) {
                    //                    printf("\n!!!Bot Tomas check!!!\n");
                    //                    print_info_bots(datagame);
                    datagame.restart = 0;


                }
                else if (datagame.win_chanse > tomas_raise_low && datagame.win_chanse < tomas_raise_high || (rand() % 100) < blef_tomas) {

                    //printf("\n!!!Bot Tomas raise!!!\n");
                    int bet;
                    bet = datagame.win_chanse * datagame.bank;
                    while (bet < datagame.bot1_bet - datagame.bot_bet)  bet += datagame.win_chanse * datagame.bank;
                    if (bet > fmin(datagame.bot_balance, datagame.bot1_balance)) bet = fmin(datagame.bot_balance, datagame.bot1_balance);
                    datagame.bank += bet;
                    datagame.bot_bet += bet;
                    datagame.bot_balance -= bet;
                    //print_info_bots(datagame);

                }
                else if (datagame.win_chanse > tomas_all_in_low || (rand() % 100) < blef_tomas) {
                    // printf("\n!!!Bot Tomas all-in!!!\n");
                    int bet = datagame.bot_balance;
                    if (bet > datagame.bot1_balance) bet = datagame.bot1_balance;
                    datagame.bank += bet;
                    datagame.bot_bet += bet;
                    datagame.bot_balance -= bet;
                    //print_info_bots(datagame);

                }
                else {
                    //printf("\n!!!Bot Tomas fold!!!\n");
                    datagame.restart = 1;
                    datagame.bot1_balance += datagame.bank;
                    datagame.bot_bet = 0;
                    datagame.bot1_bet = 0;
                    datagame.bank = 0;
                    //print_info_bots(datagame);
                }
            }

            else {                                          //bot John

                if (datagame.win_chanse1 > john_check_low && datagame.win_chanse1 < john_check_high || (rand() % 100) < blef_john) {
                    //printf("\n!!!Bot John check!!!\n");
                    // print_info_bots(datagame);
                    datagame.restart = 0;

                }
                else if (datagame.win_chanse1 > john_raise_low && datagame.win_chanse1 < john_raise_high || (rand() % 100) < blef_john) {

                    // printf("\n!!!Bot John raise!!!\n");
                    int bet;
                    bet = datagame.win_chanse1 * datagame.bank;
                    while (bet < datagame.bot_bet - datagame.bot1_bet)  bet += datagame.win_chanse * datagame.bank;
                    if (bet > fmin(datagame.bot_balance, datagame.bot1_balance)) bet = fmin(datagame.bot_balance, datagame.bot1_balance);

                    datagame.bank += bet;
                    datagame.bot1_bet += bet;
                    datagame.bot1_balance -= bet;
                    //print_info_bots(datagame);

                }
                else if (datagame.win_chanse1 > john_all_in_low || (rand() % 100) < blef_john) {
                    //printf("\n!!!BotJohn all-in!!!\n");
                    int bet = datagame.bot1_balance;
                    if (bet > datagame.bot_balance) bet = datagame.bot_balance;
                    datagame.bank += bet;
                    datagame.bot1_bet += bet;
                    datagame.bot1_balance -= bet;
                    //print_info_bots(datagame);

                }
                else {
                    //printf("\n!!!BotJohn fold!!!\n");
                    datagame.restart = 1;
                    datagame.bot_balance += datagame.bank;
                    datagame.bot1_bet = 0;
                    datagame.bot_bet = 0;
                    datagame.bank = 0;
                    //print_info_bots(datagame);
                }
            }
        }

        while (datagame.bot1_bet != datagame.bot_bet) {


            if (datagame.bot_bet < datagame.bot1_bet) {// bot Tomas

                if (datagame.win_chanse > tomas_call_low && datagame.win_chanse < tomas_call_high || (rand() % 100) < blef_tomas) {
                    //printf("\n!!!Bot Tomas call!!!\n");
                    datagame.bank += datagame.bot1_bet - datagame.bot_bet;
                    datagame.bot_balance -= datagame.bot1_bet - datagame.bot_bet;
                    datagame.bot_bet += datagame.bot1_bet - datagame.bot_bet;

                    //print_info_bots(datagame);
                    datagame.restart = 0;

                }
                else if (datagame.win_chanse > tomas_raise_low && datagame.win_chanse < tomas_raise_high || (rand() % 100) < blef_tomas) {

                    //printf("\n!!!Bot Tomas raise!!!\n");
                    int bet;
                    bet = datagame.win_chanse * datagame.bank;
                    while (bet < datagame.bot1_bet - datagame.bot_bet) {
                        bet += datagame.win_chanse * datagame.bank;
                        if (bet == 0) break;
                    }
                    if (bet > fmin(datagame.bot_balance, datagame.bot1_balance)) bet = fmin(datagame.bot_balance, datagame.bot1_balance);
                    datagame.bank += bet;
                    datagame.bot_bet += bet;
                    datagame.bot_balance -= bet;
                    //print_info_bots(datagame);

                }
                else if (datagame.win_chanse > tomas_all_in_low || (rand() % 100) < blef_tomas) {
                    // printf("\n!!!Bot Tomas all-in!!!\n");
                    int bet = datagame.bot_balance;
                    if (bet > datagame.bot1_balance) bet = datagame.bot1_balance;
                    datagame.bank += bet;
                    datagame.bot_bet += bet;
                    datagame.bot_balance -= bet;
                    // print_info_bots(datagame);

                }
                else {
                    //printf("\n!!!Bot Tomas fold!!!\n");
                    datagame.restart = 1;
                    datagame.bot1_balance += datagame.bank;
                    datagame.bot_bet = 0;
                    datagame.bot1_bet = 0;
                    datagame.bank = 0;
                    //print_info_bots(datagame);
                }
            }

            if (datagame.bot1_bet < datagame.bot_bet) {// bot John

                if (datagame.win_chanse1 > john_call_low && datagame.win_chanse1 < john_call_high || (rand() % 100) < blef_john) {
                    //printf("\n!!!Bot John call!!!\n");
                    datagame.bank += datagame.bot_bet - datagame.bot1_bet;
                    datagame.bot1_balance -= datagame.bot_bet - datagame.bot1_bet;
                    datagame.bot1_bet += datagame.bot_bet - datagame.bot1_bet;

                    //print_info_bots(datagame);
                    datagame.restart = 0;

                }
                else if (datagame.win_chanse1 > john_raise_low && datagame.win_chanse1 < john_raise_high || (rand() % 100) < blef_john) {

                    //printf("\n!!!Bot John raise!!!\n");
                    int bet;
                    bet = datagame.win_chanse1 * datagame.bank;
                    while (bet < datagame.bot_bet - datagame.bot1_bet)  bet += datagame.win_chanse1 * datagame.bank;
                    if (bet > fmin(datagame.bot_balance, datagame.bot1_balance)) bet = fmin(datagame.bot_balance, datagame.bot1_balance);
                    if (bet > datagame.bot_balance) bet = datagame.bot_balance;
                    datagame.bank += bet;
                    datagame.bot1_bet += bet;
                    datagame.bot1_balance -= bet;
                    //print_info_bots(datagame);

                }
                else if (datagame.win_chanse1 > john_all_in_low || (rand() % 100) < blef_john) {
                    // printf("\n!!!Bot John all-in!!!\n");
                    int bet = datagame.bot1_balance;
                    if (bet > datagame.bot_balance) bet = datagame.bot_balance;
                    datagame.bank += bet;
                    datagame.bot1_bet += bet;
                    datagame.bot1_balance -= bet;
                    // print_info_bots(datagame);

                }
                else {
                    // printf("\n!!!Bot John fold!!!\n");
                    datagame.restart = 1;
                    datagame.bot_balance += datagame.bank;
                    datagame.bot1_bet = 0;
                    datagame.bot_bet = 0;
                    datagame.bank = 0;
                    // print_info_bots(datagame);
                }
            }

        }
    }
}
