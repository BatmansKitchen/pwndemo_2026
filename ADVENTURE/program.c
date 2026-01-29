#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "mem_alloc.h"

#define PLAYER_FULL_HEALTH 20
#define SKELETON_FULL_HEALTH 10

struct Skeleton {
    int health;
    int damage;
};

struct Weapon {
    int damage;
    int durability;
    char* name;
};

struct Player {
    char* name;
    int health;
    int has_revive;
    struct Weapon* weapon;
    int gold;
};

void do_castle(struct Player* player) {
    struct Skeleton* boss = mem_alloc(sizeof(struct Skeleton));

    boss->damage = 7;
    boss->health = 1000;

    printf("You entered the castle and found the boss! >:)\n");

    while(1) {

        printf("  press enter to attack\n");
        while(getchar() != '\n');

        player->health -= boss->damage;
        printf("  Big Bad Boss attacked you for %d damage! your health: %d/%d\n", boss->damage, player->health, PLAYER_FULL_HEALTH);

        if (player->health <= 0) {
            printf("  You died :skull:\n");
            exit(0);
        }

        int damage_mult = (rand() % 3) + 1;
        int damage = player->weapon->damage * damage_mult;

        player->weapon->durability--;

        if (player->weapon->durability == 0) {
            player->weapon->damage = 1;
        }
        
        boss->health -= damage;

        printf("  You dealt %d damage to the Big Bad Boss using your %s! Boss remaining health: %d\n", damage, player->weapon->name, boss->health);
        
        if (boss->health <= 0) {
            printf("  You killed the Big Bad Boss and beat the game or something. yippee!!\n");
            break;
        }
    }  

    mem_free(boss);
}

void do_forest(struct Player* player) {

    struct Skeleton* skeleton = mem_alloc(sizeof(struct Skeleton));

    skeleton->health = SKELETON_FULL_HEALTH;
    skeleton->damage = 3;

    printf("You're ambushed by a skeleton!\n");

    while(1) {

        printf("  press enter to attack\n");
        while(getchar() != '\n');
        
        int damage_mult = (rand() % 3) + 1;
        int damage = skeleton->damage * damage_mult;

        player->health -= damage;
        printf("  Skeleton attacked you for %d damage! your health: %d/%d\n", damage, player->health, PLAYER_FULL_HEALTH);

        if (player->health <= 0) {
            printf("  You died :skull:\n");
            if (player->has_revive) {
                player->has_revive = 0;
                player->health = PLAYER_FULL_HEALTH;
                printf("but you have a revive! so you flee'd\n");
                mem_free(skeleton);
                break;
            } else {
                exit(0);
            }
        }

        damage_mult = (rand() % 3) + 1;
        damage = player->weapon->damage * damage_mult;

        player->weapon->durability--;

        if (player->weapon->durability == 0) {
            player->weapon->damage = 1;
        }
        
        skeleton->health -= damage;

        printf("  You dealt %d damage to the skeleton using your %s! Skeleton remaining health: %d\n", damage, player->weapon->name, skeleton->health);
        
        if (skeleton->health <= 0) {
            printf("  You killed the skeleton and got 5 moneys!! :pogchamp:\n");
            player->gold += 5;
            break;
        }
    }  
    mem_free(skeleton); 
}

void do_shop(struct Player* player) {
    printf("You can buy something, what do you want? You have [%d] money\n", player->gold);
    printf(" 1. milk [1g] (full hp heal)\n");
    printf(" 2. revive [10g] (next time you die, don't ... except if you base the boss)\n");
    printf(" 3. sword of poopoo and peepee [50g] (100 attack, 1 durability)\n");
    printf(" 4. quit\n > ");

    int option = 0;
    scanf("%d", &option); // get user input
    getchar(); // consume newline

    // heal pls
    if (option == 1) {
        player->gold -= 10;
        player->health = PLAYER_FULL_HEALTH;
        printf("got the heal! your money: %d | your health: %d\n", player->gold, player->health);
    
    // u have revive
    } if (option == 2) {
        player->gold -= 10;
        player->has_revive = 1;
        printf("got the revive! your money: %d | your health: %d\n", player->gold, player->health);
    
    // get da sword
    } else if (option == 3) {
        player->gold -= 50;
        struct Weapon* sword = mem_alloc(sizeof(struct Weapon));
        sword->damage = 100;
        sword->durability = 1;
        sword->name = "sword of peepee and poopoo";
        player->weapon = sword;
        printf("got the sword of poopoo and peepee! your money: %d | your health: %d\n", player->gold, player->health);
    }
}

int main() {

    // init stuff

    // allocate player
    struct Player* player = mem_alloc(sizeof(struct Player));
    player->gold = 5;
    player->health = 20;
    player->has_revive = 0;

    // allocate base weapon
    struct Weapon* stick = mem_alloc(sizeof(struct Weapon));
    stick->name = "stick";
    stick->damage = 4;
    stick->durability = -1; // infinite durability

    player->weapon = stick;

    // game loop
    while (1) {
        printf("\nYou're on quest. do something.\n");
        printf(" 1. Enter the castle\n");
        printf(" 2. Enter the forest\n");
        printf(" 3. Enter the shop\n");
        printf(" 4. Quit\n > ");

        int option = 0;
        scanf("%d", &option); // get option from user
        getchar(); // consume newline
        printf("\n");

        if (option == 1) {
            do_castle(player);
            return 0;
        } else if (option == 2) {
            do_forest(player);
        } else if (option == 3) {
            do_shop(player);
        } else if (option == 4) {
            return 0;
        }
    }    
}