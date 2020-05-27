typedef struct PShip{
    int health;
    int x;
    int y;
    int dx;
    int dy;
    int width;
    int height;
}PShip;

typedef struct Laser{
    int x;
    int y;
    int dx;
    int dy;
    int width;
    int height;
    int damage;
}Laser;

typedef struct EShip{
    int health;
    int x;
    int y;
    int width;
    int height;
}EShip;

void initializeGame(void);

int updateGame(u32 previousButtons, u32 currentButtons);

void shootELaser(void);

int updateELaser(void);

void updateHealth(void);