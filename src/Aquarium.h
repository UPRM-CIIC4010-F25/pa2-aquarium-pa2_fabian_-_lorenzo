#define NOMINMAX // To avoid min/max macro conflict on Windows

#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>
#include "Core.h"


enum class AquariumCreatureType {
    NPCreature,
    BiggerFish,
    FastNPCreature,   //added new fish species
    NewNemoCreature,   //added new fish species
    SharkCreature      //added final new fish species
};



// Health power up subclass
class HealthPowerUp : public PowerUp{
public:
    HealthPowerUp(float x, float y, std::shared_ptr<GameSprite> sprite);
    PowerUpType getType() {return this->m_power_upType;}
    void draw() const override;
}; 



string AquariumCreatureTypeToString(AquariumCreatureType t);

class AquariumLevelPopulationNode{
    public:
        AquariumLevelPopulationNode() = default;
        AquariumLevelPopulationNode(AquariumCreatureType creature_type, int population) {
            this->creatureType = creature_type;
            this->population = population;
            this->currentPopulation = 0;
        };
        AquariumCreatureType creatureType;
        int population;
        int currentPopulation;
};

class AquariumLevel : public GameLevel {
    public:
    // Added a powerup target score as a parameter for the class and its parametrized constructor
        AquariumLevel(int levelNumber, int targetScore, int powerUpScore)
        : GameLevel(levelNumber), m_level_score(0), m_targetScore(targetScore), m_power_up_score(powerUpScore) {};
        void ConsumePopulation(AquariumCreatureType creature, int power);
        bool isCompleted() override;
        void populationReset();
        void levelReset(){m_level_score=0;this->populationReset();}
        //Changed this function from virtual to non virtual since it will be implemented in this class
        //The classes that inherit from it will receive it directly, no needed to override
        std::vector<AquariumCreatureType> Repopulate();
        // powerup functions
        bool canSpawnPowerUp() override;
        void setPowerUpScore(int score) { m_power_up_score = score; }
    protected:
        std::vector<std::shared_ptr<AquariumLevelPopulationNode>> m_levelPopulation;
        int m_level_score;
        int m_targetScore;
        int m_power_up_score;

};


class PlayerCreature : public Creature {
public:
    double deltaTime = 0;

    PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move();
    void draw() const;
    void update();
    void changeSpeed(int speed);
    void setLives(int lives) { m_lives = lives; }
    void setDirection(float dx, float dy);
    float isXDirectionActive() { return m_dx != 0; }
    float isYDirectionActive() {return m_dy != 0; }
    float getDx() { return m_dx; }
    float getDy() { return m_dy; }

    int getScore()const { return m_score; }
    int getLives() const { return m_lives; }
    int getPower() const { return m_power; }
    
    void addToScore(int amount, int weight=1) { m_score += amount * weight; }
    void gainLive();
    void loseLife(int debounce);
    void increasePower(int value) { m_power += value; }
    void reduceDamageDebounce();
    
private:
    int m_score = 0;
    int m_lives = 3;
    int m_power = 1; // mark current power lvl
    int m_damage_debounce = 0; // frames to wait after eating
};

class NPCreature : public Creature {
public:
    NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    AquariumCreatureType GetType() {return this->m_creatureType;}
    void move() override;
    void draw() const override;
protected:
    AquariumCreatureType m_creatureType;

};

class BiggerFish : public NPCreature {
public:
    BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move() override;
    void draw() const override;
};
//New fish species that inherits from NPCreature class
class FastNPCreature : public NPCreature {
public:
    FastNPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move() override; //Since movement behaviour will be different, must overide this from parent class
};

//New fish species that inherits from NPCreature class
class NewNemoCreature : public NPCreature {
public:
    NewNemoCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move() override;  
};
// New Shark species that inherits from NPCreature class
class SharkCreature : public NPCreature {
public:
    SharkCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite);
    void move() override; //Since movement behaviour will be different, must overide this from parent class
private:
    //Declared and initialized both variables for future use in override of move() for timers
    int boostTimer = 10;   
    int restTimer = 5;
};


class AquariumSpriteManager {
    public:
        AquariumSpriteManager();
        ~AquariumSpriteManager() = default;
        std::shared_ptr<GameSprite>GetSprite(AquariumCreatureType t);
        // Added powerup sprite getter and parameters
        std::shared_ptr<GameSprite>GetSprite(PowerUpType t);
    private:
        std::shared_ptr<GameSprite> m_npc_fish;
        std::shared_ptr<GameSprite> m_big_fish;
        std::shared_ptr<GameSprite> m_fast_fish; //added new fist species for future implementation
        std::shared_ptr<GameSprite> m_nemo_fish;  //added new fist species for future implementation
        std::shared_ptr<GameSprite> m_shark_fish;  //added new fist species for future implementation
        std::shared_ptr<GameSprite> m_health_power; // health powerup sprite
};


class Aquarium{
public:
    Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager);
    void addCreature(std::shared_ptr<Creature> creature);
    void addAquariumLevel(std::shared_ptr<AquariumLevel> level);
    void removeCreature(std::shared_ptr<Creature> creature);
    void clearCreatures();
    void update();
    void draw() const;
    void setBounds(int w, int h) { m_width = w; m_height = h; }
    void setMaxPopulation(int n) { m_maxPopulation = n; }
    void Repopulate();
    void SpawnCreature(AquariumCreatureType type);
    // powerup functions
    void addPowerUp(std::shared_ptr<PowerUp> power);
    void SpawnPowerUp(PowerUpType type);
    int getPowerUpCount() const { return m_power_ups.size();}
    bool getCanCollidePowerUp() { return m_canCollidePowerUp; }
    void setCanCollidePowerUp(bool canCollide) { m_canCollidePowerUp = canCollide;}
    void clearPowerUps();
    std::shared_ptr<PowerUp> getPowerUpAt(int index);
    
    std::shared_ptr<Creature> getCreatureAt(int index);
    int getCreatureCount() const { return m_creatures.size(); }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }


private:
    int m_maxPopulation = 0;
    int m_width;
    int m_height;
    int currentLevel = 0;
    std::vector<std::shared_ptr<Creature>> m_creatures;
    std::vector<std::shared_ptr<Creature>> m_next_creatures;
    std::vector<std::shared_ptr<AquariumLevel>> m_aquariumlevels;
    std::shared_ptr<AquariumSpriteManager> m_sprite_manager;
    // powerup properties
    bool m_canCollidePowerUp;
    std::vector<std::shared_ptr<PowerUp>> m_power_ups;
};

// function to determine when the player picks up a powerup
std::shared_ptr<GameEvent> DetectPowerUpCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player);


std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player);


class AquariumGameScene : public GameScene {
    public:
        AquariumGameScene(std::shared_ptr<PlayerCreature> player, std::shared_ptr<Aquarium> aquarium, string name)
        : m_player(std::move(player)) , m_aquarium(std::move(aquarium)), m_name(name){}
        std::shared_ptr<GameEvent> GetLastEvent(){return m_lastEvent;}
        void SetLastEvent(std::shared_ptr<GameEvent> event){this->m_lastEvent = event;}
        std::shared_ptr<PlayerCreature> GetPlayer(){return this->m_player;}
        std::shared_ptr<Aquarium> GetAquarium(){return this->m_aquarium;}
        string GetName()override {return this->m_name;}
        void Update() override;
        void Draw() override;
    private:
        void paintAquariumHUD();
        std::shared_ptr<PlayerCreature> m_player;
        std::shared_ptr<Aquarium> m_aquarium;
        std::shared_ptr<GameEvent> m_lastEvent;
        string m_name;
        AwaitFrames updateControl{5};
};

//Added new fish species in order for it to be rendered accordingly to the level
//  Added a powerup target score paramater for all levels
class Level_0 : public AquariumLevel  {
    public:
        Level_0(int levelNumber, int targetScore, int powerUpScore): AquariumLevel(levelNumber, targetScore, powerUpScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 8));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NewNemoCreature, 4));
        };
        // std::vector<AquariumCreatureType> Repopulate() override;

};
class Level_1 : public AquariumLevel  {
    public:
        Level_1(int levelNumber, int targetScore, int powerUpScore): AquariumLevel(levelNumber, targetScore, powerUpScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 12));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NewNemoCreature, 6));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::FastNPCreature, 6));
            
        };



};
class Level_2 : public AquariumLevel  {
    public:
        Level_2(int levelNumber, int targetScore, int powerUpScore): AquariumLevel(levelNumber, targetScore, powerUpScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::NPCreature, 30));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 2));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::FastNPCreature, 8));
            

        };


};
//New level 3 with its respective fish implemented
class Level_3 : public AquariumLevel {
    public:
    Level_3(int levelNumber, int targetScore, int powerUpScore): AquariumLevel(levelNumber, targetScore, powerUpScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 20));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::FastNPCreature, 20));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::SharkCreature, 6));
            
        };


};
//New level 3 with its respective fish implemented
class Level_4 : public AquariumLevel {
public:
    Level_4(int levelNumber, int targetScore, int powerUpScore): AquariumLevel(levelNumber, targetScore, powerUpScore){
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::BiggerFish, 5));
            this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::FastNPCreature, 5));
             this->m_levelPopulation.push_back(std::make_shared<AquariumLevelPopulationNode>(AquariumCreatureType::SharkCreature, 15));
        };


};
