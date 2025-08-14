/* Taha sohail 24i-3165
se-b
project */

//* Header Files
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include<SFML/Audio.hpp>
#include <cmath>
#include<fstream>
#include <cstring>
#include<sstream>

using namespace std;
using namespace sf;


//* Constant value for the Max buttons and window screen
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const int MAX_BUTTONS = 20;
const int MAX_FIREBALLS = 20; //* Adjustable based on performance needs
const int MAX_ITEMS = 20; //^ Inventory can hold only 20 Items in it
const int GRID_SIZE = 40; //* Matching game's grid scale
const int MAX_PATH_LENGTH = 100;


//^ Game Flow States
const int START_MENU = 0;
const int INPUT_NAME = 1;
const int POST_NAME_MENU = 2;
const int INPUT_PET_NUM = 3;
const int CHOOSE_PET = 4;
const int INPUT_PET_NAME = 5;
const int CHOOSE_ROLE = 6;
const int MAIN_MENU = 7;
const int DISPLAY_GUILD = 8;
const int TRAIN_PET = 9;
const int HEAL_PET = 10;
const int SELECT_HEAL_TARGET = 11;
const int INPUT_HEAL_AMOUNT = 12;
const int SHOP_MENU = 13;
const int INVENTORY_MENU = 14;
const int ARENA_MENU = 15;
const int BATTLE = 16;
const int TRAINING_MENU = 17;
const int CHOOSE_DIFFICULTY = 18;
const int TRAINING_MINIGAME = 19;
const int SHOW_RECORDS = 20; 
const int SELECT_PET_FOR_BATTLE = 21;
const int SELECT_PETS_FOR_2V2 = 22;

//~ Global font
Font globalFont;

//!Forward declarations
class Arena;
class Player;
class TrainingCamps;
class Pet;

//^ Renders the window with str written on it of having size x and y coordinated with calling class of Class for the text Color and Shadow Color would call constructor to take different colors and Fonts
void drawTextWithShadow(RenderWindow& window, const string& str, Font& font, int size, float x, float y, Color textColor, Color shadowColor = Color(0, 0, 0, 150), float uncertainity = 2.0f) 
{
    Text text(str, font, size);
    text.setFillColor(shadowColor);
    text.setPosition(x + uncertainity, y + uncertainity);
    window.draw(text);
    text.setFillColor(textColor);
    text.setPosition(x, y);
    window.draw(text);
}

//^Classes Starts from Here

//* Class FIre for the attack in the Arena
class Fire 
{
    private:
    //* Circle Shape Fire
        CircleShape fireShape;
        Vector2f direction;
        Vector2f position;
        float speed;
        bool active;
        float LifeTime;
        const float MAX_LIFE_TIME = 5.0f;
    
    public:
    //*Fire constructor
        Fire(float x, float y, float radius) : speed(500.0f), active(true), LifeTime(MAX_LIFE_TIME) 
        {
            fireShape.setRadius(radius);
            fireShape.setFillColor(Color::Red);
            fireShape.setOutlineColor(Color::Yellow);
            fireShape.setOutlineThickness(2.0f);
            fireShape.setPosition(x, y);
            position = Vector2f(x, y);
        }
    
        void setDirection(Vector2f target) 
        {
            direction = target - position; //*Calculate Direction Vector
            //*Formula
            float length = sqrt(direction.x * direction.x + direction.y * direction.y);

            if (length > 0) 
            {
                direction.x /= length;
                direction.y /= length;
            } else 
            {
             //* Validation if Fireball isnt printed
                cout << "Failed to set fireball direction: zero length" << endl;
            }
        }
    
        //*Update of the Fireball
        void update(float deltaTime) 
        {
            if (!active) 
            return;
    
            //* Decrease lifetime.
            LifeTime -= deltaTime;

            if (LifeTime <= 0) 
            {
                //* Deactivate fireball if lifetime expire
                active = false;
                return;
            }
    
            //* Move fireball in the specified direction.
            fireShape.move(direction * speed * deltaTime);

            //*Update Store Position
            position = fireShape.getPosition();

            //*Debugging
            cout << "Fireball at (" << position.x << ", " << position.y << "), active: " << active << endl;
        }
    
        //* Checking of Collision
        bool checkCollision(Sprite sprite) 
        {
            return (active && fireShape.getGlobalBounds().intersects(sprite.getGlobalBounds()));
        }
    
        void draw(RenderWindow& window) 
        {
            if (active) 
            {
                cout << "Drawing fireball at (" << position.x << ", " << position.y << ")" << endl;
                window.draw(fireShape); //* Draw the fireball shape.
            }
        }
        void deactivate() 
        { 
            active = false; 
        }

        bool Active() 
        {
             return active; 
        }

        Vector2f getPosition() 
        { 
            return position; 
        }
};
   
//* Class Fireball for the Training Camps
class Fireball 
{
    private:
    CircleShape shape; //* Main fireball
    CircleShape glow;  //* Glow effect
    float speed;
    bool active;
    float x, y;
    const float radius = 20.0f; //* Radius of the main circle
    const float glowRadius = 30.0f; 

    public:

    //* Fireball constructor
    Fireball(float xPos, float speed) : speed(speed), active(true), x(xPos), y(-radius * 2) 
    {
        shape.setRadius(radius);
        int randShade = rand() % 50; // *Random shade variation
        shape.setFillColor(Color(255, 69 + randShade, 0)); //* Vary red-orange hue
        shape.setOutlineColor(Color::Red);
        shape.setOutlineThickness(2.0f);
        shape.setPosition(x, y);
    }

    void update(float deltaTime) 
    {
        if (!active) 
        return;

        //* Move downward
        y += speed * deltaTime;
        shape.setPosition(x, y);
        glow.setPosition(x - (glowRadius - radius), y - (glowRadius - radius));

        //* Deactivate if off-screen
        if (y > WINDOW_HEIGHT + radius * 2) 
        {
            active = false;
        }
    }
    
    void draw(RenderWindow& window) 
    {
        if (active) 
        {
            window.draw(glow); 
            window.draw(shape); 
        }
    }
    
    bool Active() const 
    { 
    return active; 
    }
    
    //* Checking the Collision of FIreball with the Player pet
    bool checkCollision(const Sprite& target) 
    {
        if (!active) 
        return false;

        Vector2f circleCenter = shape.getPosition() + Vector2f(radius, radius); // Center of the circle
        FloatRect targetBounds = target.getGlobalBounds();

        //* Find closest point on rectangle to circle center

        float closestX = max(targetBounds.left, min(circleCenter.x, targetBounds.left + targetBounds.width));
        float closestY = max(targetBounds.top, min(circleCenter.y, targetBounds.top + targetBounds.height));

        //* Calculate distance
        float distanceX = circleCenter.x - closestX;
        float distanceY = circleCenter.y - closestY;

        float conclusion = distanceX * distanceX + distanceY * distanceY;
        float conclusion1 = radius * radius;

        return (conclusion < conclusion1);
    }

    //* Setting the Position of The Fireball
    void setPosition(float newX, float newY) 
    {
        x = newX;
        y = newY;
        shape.setPosition(x, y);
        glow.setPosition(x - (glowRadius - radius), y - (glowRadius - radius));
    }

    // *Getting the Position of Fireball
    Vector2f getPosition() const 
    {
        return shape.getPosition();
    }

    float getSpeed() const 
    {
        return speed;
    }

    void deactivate() 
    {
        active = false;
    }

};

//* Class of Button
class Button 
{
        private:
        //* Shape of Button should be Rectangle
        RectangleShape shape;
        Sprite buttonSprite;
        Texture buttonTexture;
        Text text;
        bool Hovered;
        int index;
        bool disabled = false;
        
        public:

        //* Constructor
            Button() : Hovered(false), index(-1) {}
        
        //* Set the Button
            void set(const string& label, const Font& font, float x, float y, float width, float height, int btnIndex = 0) 
            {
                if (!buttonTexture.loadFromFile("wooden_button.png")) 
                {
                    cerr << "Failed to load wooden_button.png!" << endl;
                    return;
                }
                buttonSprite.setTexture(buttonTexture);
                buttonSprite.setPosition(x, y);
                FloatRect spriteBounds = buttonSprite.getLocalBounds();
                buttonSprite.setScale(width / spriteBounds.width, height / spriteBounds.height);
                index = btnIndex;
        
                text.setFont(font);
                text.setString(label);
                text.setCharacterSize(24);
                text.setFillColor(Color::Black);
        
                FloatRect textBounds = text.getLocalBounds();
                text.setOrigin(textBounds.width / 2, textBounds.height / 2);
                text.setPosition(x + width / 2, y + height / 2);
            }

        //* DIsabling THe button
            void setDisabled(bool Disabled) 
            {
                disabled = Disabled;
                if (disabled) 
                {
                    shape.setFillColor(Color(100, 100, 100, 150));
                    text.setFillColor(Color(150, 150, 150)); // Gray text
                }
                else 
                {
                    shape.setFillColor(Color::Blue); // Restore normal color 
                    text.setFillColor(Color::White);
                }
            }
        
            bool isDisabled() const 
            {
                return disabled;
            }
        
            Vector2f getPosition() const 
            {
                return shape.getPosition(); 
            }

            //* Click check
            bool isClicked(Vector2i mousePos) 
            {
                if (disabled) return false;
                FloatRect bounds = buttonSprite.getGlobalBounds();
                return bounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
            }
        
            //* Updation the Button 
            void update(Vector2i mousePos) 
            {
                FloatRect bounds = buttonSprite.getGlobalBounds();
                Hovered = bounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                if (Hovered) 
                {
                    buttonSprite.setColor(Color(200, 200, 200)); //* darken effect on hover
                } else 
                {
                    buttonSprite.setColor(Color::White); //* Normal color
                }
            }
        
            //* Rendering of the Button
            void render(RenderWindow& window) 
            {
                window.draw(buttonSprite);
                window.draw(text);
            }
        
            bool isHovered() const 
            {
                 return Hovered; 
            }

            int getIndex() const 
            { 
                return index;
            }
};


//~Ability classes
class Ability 
{
    protected:

    string name;
    string Describe;
    int cooldown;
    int CurrentCoolDown;
    bool unlocked;
    bool OneTimeUsed;
    bool used;
//* Check if ability is locked or unlocked
    bool locked = true;

    public:

    Ability(){}

    Ability(string name, string Describe, int cooldown, int CurrentCoolDown = 0, bool unlocked = false, bool OneTimeUsed = false, bool used = false): name(name), Describe(Describe), cooldown(cooldown), CurrentCoolDown(CurrentCoolDown), unlocked(unlocked), OneTimeUsed(OneTimeUsed), used(used) {}

    virtual string ToActivateAbility() 
    {
        if (used == true) 
        return name + " is used";

        if (OneTimeUsed == true ) 
        return name + " is one time used";
    
        if (!unlocked == true) 
        return name + " is locked";

        if (CurrentCoolDown > 0)
        return name + " on cool down (" + to_string(CurrentCoolDown) + ") turns left";

        CurrentCoolDown = cooldown;

        if (OneTimeUsed == true) 
        used = true;

        return (name + " used: " + Describe);
    }

    virtual string getName() 
    { 
        return "Unknown Ability"; 
    }

    void ReductionOfCoolDown() 
    {
        if (CurrentCoolDown > 0) 
        CurrentCoolDown--;
    }


    void unlock() 
    {
        locked = false;
    }

    bool isLocked() const 
    {
        return locked;
    }

    string getName() const 
    {
        return name;
    }

    string getname()
    {
        return name; 
    }

    string GetDescribe() 
    { 
        return Describe; 
    }

    void setUnlock() 
    {
         unlocked = true; 
         locked = false;
    }

    bool getUnlocked() 
    {
        return unlocked; 
    }

    int getCurrentCoolDown() 
    {
         return CurrentCoolDown; 
    }

    virtual void ApplyEffect(Pet* target, Pet* source = nullptr) {}
};

class DamageAbility : public Ability 
{
    private:

    int damage;

    public:
//* Parametrized constructor

    DamageAbility(string name, string des, int cooldown, int dmg, int CurrentCoolDown = 0, bool unlocked = false, bool OneTimeUsed = false, bool used = false)
    : Ability(name, des, cooldown, CurrentCoolDown, unlocked, OneTimeUsed, used), damage(dmg) {}

    string getName() override 
    { 
        return "Fireball"; 
    }

    void ApplyEffect(Pet* Target, Pet* MainCharacter ) override;
};

class HealingAbility : public Ability 
{
    private:
    int Heal;

    public:
    
    HealingAbility(string name, string des, int cd, int heal, int CurrentCoolDown = 0, bool unlocked = false, bool OneTimeUsed = false, bool used = false)
        : Ability(name, des, cd, CurrentCoolDown, unlocked, OneTimeUsed, used), Heal(heal) {}

        string getName() override 
        { 
            return "Flame Heal"; 
        }

    void ApplyEffect(Pet* Target, Pet* MainCharacter ) override;
};

class RebirthAbility : public Ability 
{
    private:

    int reviveHealth;
    bool Revived;

    public:
    //* Parametrzied COnstructor
    RebirthAbility(string name, string des, int reviveHP, int CurrentCoolDown = 0, bool unlocked = false, bool OneTimeUsed = true, bool used = false)
    : Ability(name, des, 0, CurrentCoolDown, unlocked, OneTimeUsed, used), reviveHealth(reviveHP), Revived(false) {}

    string getName() override 
    { 
        return "Rebirth"; 
    }

    string ToActivateAbility() override 
    {
        if (Revived == true) 
        return name + " has already been used!";

        Revived = true;
        return name + " is ready to revive!";
    }

    void ApplyEffect(Pet* target, Pet* MainCharacter ) override;
};

class BuffAbility : public Ability 
{
    private:
    //* Which tye of bUff
    string Buffname;
    int Buffamount;
    int duration;

    public:
   
    //* Para cons
    BuffAbility(string name, string des, int cd, string BuffName, float amount, int durr, int CurrentCoolDown = 0, bool unlocked = false, bool OneTimeUsed = false, bool used = false)
    : Ability(name, des, cd, CurrentCoolDown, unlocked, OneTimeUsed, used), Buffname(BuffName), Buffamount(amount), duration(durr) {}

    string getName() override 
    { 
        return "Speed Burst"; 
    }
    void ApplyEffect(Pet* Target, Pet* MainCharacter ) override;
};

class StunAbility : public Ability 
{
    private:
    int stunDuration;

    public:
    
    StunAbility(string name, string desc, int cooldown, int duration, int CurrentCoolDown = 0, bool unlocked = false, bool OneTimeUsed = false, bool used = false)
    : Ability(name, desc, cooldown, CurrentCoolDown, unlocked, OneTimeUsed, used), stunDuration(duration) {}

    string getName() override 
    { 
        return "Roar"; 
    }

    void ApplyEffect(Pet* target, Pet* Maincharacter ) override;
};

class ShieldAbility : public Ability 
{
    public:
        ShieldAbility() : Ability("Magic Shield", "Absorbs damage for a short duration", 4, 0, false, false, false) {}

        string getName() override 
        { 
            return "Magic Shield"; 
        }
    
        void ApplyEffect(Pet* target, Pet* source) override;
};

class Pet 
{
    protected:
    Sprite sprite;                //*Visual representation of the pet.
    Texture texture;              //* Texture for the pet's sprite.
    string name;                  //* Name of the pet.
    int Health;                   //* Current health points.
    int AttackPower;              //* Base attack power.
    float Speed;                  //* Base movement speed.
    int Defense;                  //* Defense value to reduce incoming damage.
    string roles;                 //* Role of the pet (Tanker, Healer, Damage Dealer).
    bool DefenseMode;             //* Whether the pet is in defense mode.
    static const int MAXABILITIES = 3; //* Maximum number of abilities per pet.
    Ability* ability[MAXABILITIES];    //* Array of pointers to abilities.
    int abilityCount;             //* Number of assigned abilities.
    int stunTurns;                //* Number of turns the pet is stunned.
    int level;                    //* Current level of the pet.
    int xp;                       //* Current experience points.
    int xpToNextLevel;            //* XP required for the next level.
    float speedMultiplier;         //* Temporary speed multiplier (e.g., from buffs).
    float attackMultiplier;        //* Temporary attack multiplier (e.g., from buffs).
    int buffDuration;             //* Duration of active buffs.
    float stunDuration;           //* Duration of stun effect (in seconds).
    float damageReduction;         //* Damage reduction factor (e.g., from shields).
    int shieldDuration;           //* Duration of shield effect.
    bool stunned;                 //* Whether the pet is currently stunned.
    string type;                  //* Type of pet (set by derived classes).

    public:

    //* COnstructor
    Pet() : level(1), xp(0), xpToNextLevel(100), speedMultiplier(1.0f), attackMultiplier(1.0f), buffDuration(0), damageReduction(1.0f), shieldDuration(0) 
    {
        for (int i = 0; i < MAXABILITIES; i++) 
        {
            ability[i] = nullptr;
        }
    }

    //* Para cons
    Pet(string name, int Health, int AttackPower, float Speed, int Defense, string roles, bool Defensemode = false, int abilityCount = 0): name(name), Health(Health), AttackPower(AttackPower), Speed(Speed), Defense(Defense), roles(roles),
    DefenseMode(Defensemode), abilityCount(abilityCount), level(1), xp(0), xpToNextLevel(100),
    speedMultiplier(1.0f), attackMultiplier(1.0f), buffDuration(0), damageReduction(1.0f), shieldDuration(0) 
    {
        for (int i = 0; i < MAXABILITIES; i++) 
        {
            ability[i] = nullptr;
        }
    }

    string useAbilities(int index) 
    {
        if (index < 0 || index >= 3 || !ability[index]) 
        return "No ability";
        ability[index]->ApplyEffect(this, this);
        return ability[index]->getName() + " used";
    }

    //* Protoypes defined downwards 
    void gainXP(int amount, TrainingCamps* camp);
    void levelUp(TrainingCamps* camp);
    
    //& Virtual functions
    virtual string getType() const  = 0;
    virtual Pet* copy() const = 0;


    //* Setters and getters
    void setLevel(int lvl) 
    {
         level = lvl; 
    }

    void setXP(int exp) 
    {
         xp = exp; 
    }

    void setXPToNextLevel(int nextLvlXP) 
    { 
        xpToNextLevel = nextLvlXP; 
    }

    void setDefense(int def) 
    {
         Defense = def; 
    } 
   
    int getLevel() const 
    { 
        return level; 
    }

    int getXP() const
    { 
        return xp; 
    }

    int getXPToNextLevel() const 
    { 
    return xpToNextLevel; 
    }

    //* Conditions if the texture load or not
    bool loadTexture(const string& filename) 
    {
        if (!texture.loadFromFile(filename)) 
        {
            return false;
        }
        sprite.setTexture(texture);
        sprite.setScale(2.0f, 2.0f);
        return true;
    }

    Sprite& getSprite() 
    { 
        return sprite; 
    }

    void setSpritePosition(float x, float y) 
    { 
        sprite.setPosition(x, y); 
    }

    //* ++ ability count 
    void AddAbility(Ability* abilities) 
    {
        if (abilityCount < MAXABILITIES) 
        {
            ability[abilityCount++] = abilities;
        }
    }

    string getname() 
    { 
        return name; 
    }

    string getRole() { 
        return roles; 
    }

    int getHealth() { 
        return Health; 
    }

    void setHealth(int hp) { 
        Health = hp; 
    }

    void setDefensemode(bool flag) { 
        DefenseMode = flag; 
    }

    bool getDefensemode() { 
        return DefenseMode; 
    }

    float getSpeed() { 
        return Speed * speedMultiplier;
    }

    int getAttackPower() { 
        return static_cast<int>(AttackPower * attackMultiplier); 
    }

    int getDefense() { 
        return Defense; 
    }
    void setSpeed(float speed) { 
        Speed = speed; 
    }
    void setAttackPower(int At) { 
        AttackPower = At; 
    }

     Ability** getAbility() { 
        return ability; 
    }

    bool UnlockAbility(int index) 
    {
        //* Validations
        if (index < 0 || index >= abilityCount) 
        {
            cout << "Invalid Ability!" << endl;
            return false;
        }
        //* ALready unloced
        if (ability[index]->getUnlocked()) 
        {
            cout << "Ability already unlocked!" << endl;
            return false;
        }
        //* setting it to unlock
        ability[index]->setUnlock();
        cout << "Ability " << ability[index]->getname() << " unlocked!" << endl;
        return true;
    }


    //*upadting cooldown of abilities
    void UpdateAbilities() 
    {
        for (int i = 0; i < abilityCount; i++) 
        {
            ability[i]->ReductionOfCoolDown();
        }
    }

//* Assign the training and improvement of stats
    void AssignTrain(string characteristics, float points) 
    {
        if (characteristics == "Health") {
            Health += points;
        } else if (characteristics == "AttackPower") {
            AttackPower += points;
        } else if (characteristics == "Speed") {
            Speed += points;
        } else if (characteristics == "Defense") {
            Defense += points;
        }
    }

    void displayAbilities(RenderWindow& window, float x, float y) 
    {
        // Draw header
        drawTextWithShadow(window, "Abilities:", globalFont, 30, x, y, Color::White);

        // Draw abilities
        for (int i = 0; i < abilityCount; i++) 
        {
        if (!ability[i]) 
        continue;

        Color abilityColor = ability[i]->getUnlocked() ? Color::White : Color(150, 150, 150); // White for unlocked, gray for locked

        drawTextWithShadow(window, to_string(i + 1) + ". " + ability[i]->getname(), globalFont, 24, x, y + 30 + (i * 70), abilityColor);

        drawTextWithShadow(window, " - " + ability[i]->GetDescribe(), globalFont, 20, x + 20, y + 50 + (i * 70), Color::White);

        drawTextWithShadow(window, "CD: " + to_string(ability[i]->getCurrentCoolDown()), globalFont, 20, x + 20, y + 70 + (i * 70), Color::White);
        }
    }


  void DisplayStats(RenderWindow& window, float x, float y) 
  {
    // Draw pet name
    drawTextWithShadow(window, "Name: " + name, globalFont, 28, x, y, Color::White);

    // Draw level
    drawTextWithShadow(window, "Level: " + to_string(level), globalFont, 20, x, y + 30, Color::White);

    // Draw XP
    drawTextWithShadow(window, "XP: " + to_string(xp) + "/" + to_string(xpToNextLevel), globalFont, 20, x, y + 50, Color::White);

    //* Range 0-1000
    int cappedHealth = min(max(Health, 0), 1000);
    int cappedAttack = min(max(AttackPower, 0), 1000);
    int cappedDefense = min(max(Defense, 0), 1000);

    // Draw stats with white text and shadow for readability
    drawTextWithShadow(window, "Health: " + to_string(cappedHealth), globalFont, 20, x, y + 80, Color::White);
    drawTextWithShadow(window, "Attack: " + to_string(cappedAttack), globalFont, 20, x, y + 100, Color::White);
    drawTextWithShadow(window, "Speed: " + to_string(getSpeed()), globalFont, 20, x, y + 120, Color::White);
    drawTextWithShadow(window, "Defense: " + to_string(cappedDefense), globalFont, 20, x, y + 140, Color::White);

    // Draw role
    drawTextWithShadow(window, "Role: " + roles, globalFont, 20, x, y + 160, Color::White);

    //* Draw pet sprite with bounce animation
    static float bounceTime = 0.0f;

    bounceTime += 0.016f; //* Approximate deltaTime for 60 FPS

    float bounceOffset = sin(bounceTime * 2.0f) * 5.0f; //* 5px up/down bounce

    sprite.setScale(1.0f, 1.0f);

    setSpritePosition(x + 300, y + bounceOffset);

    window.draw(sprite);

    // Draw abilities
    displayAbilities(window, x + 600, y);
}


    void setStunned(int turns) 
    {
        stunTurns = turns;
    }

    bool isStunned() 
    {
        return (stunTurns > 0);
    }

    //* applying of buff ability
    void applyBuff(float attackMult, float speedMult, int duration) 
    {
        attackMultiplier = attackMult;
        speedMultiplier = speedMult;
        buffDuration = duration;
    }

    //* applying of shield ability
    void applyShield(float reduction, int duration) 
    {
        damageReduction = reduction;
        shieldDuration = duration;
    }

    //* updationg battle effects acording to game mechanics
    void updateBattleEffects(float deltaTime) 
    {
        //* stun
        if (stunned == true) 
        {
            stunDuration -= deltaTime;

            if (stunDuration <= 0) 
            {
                stunned = false;
                stunDuration = 0;
                cout << name << " is no longer stunned." << endl;
            }
        }

        //* buff
        if (buffDuration > 0) 
        {
            buffDuration -= deltaTime;
            
            if (buffDuration <= 0) 
            {
                speedMultiplier = 1.0f;
                attackMultiplier = 1.0f;
                buffDuration = 0;
                cout << name << "'s buffs have expired." << endl;
            }
        }
        //* sheild
        if (shieldDuration > 0) 
        {
            shieldDuration -= deltaTime;
            if (shieldDuration <= 0) 
            {
                damageReduction = 1.0f;
                shieldDuration = 0;
                cout << name << "'s shield has expired." << endl;
            }
        }
    }

    void setStunned(bool stun)
    {
        stunned = stun;
    }

    bool getStunned()
    {
        return stunned;
    }

    void setStunDuration(float duration)
    {
        stunDuration = duration;
    }

    float getStunDuration()
    {
        return stunDuration;
    }

    //* operator overloading 
bool operator==(const Pet& other) ;
bool operator!=(const Pet& other) ;
bool operator<(const Pet& other);
bool operator>(const Pet& other) ;
bool operator<=(const Pet& other);
bool operator>=(const Pet& other) ;
    //* destructor
    virtual ~Pet() 
    {
        for (int i = 0; i < abilityCount; i++) 
        {
            delete ability[i];
        }
    }
};

//^ Operator overloading
bool Pet::operator==(const Pet& other) 
{
    int thisStats = Health + AttackPower + Defense;
    int otherStats = other.Health + other.AttackPower + other.Defense;
    return thisStats == otherStats && level == other.level;
}

bool Pet::operator!=(const Pet& other) 
{
    return !(*this == other);
}

bool Pet::operator<(const Pet& other) 
{
    int thisStats = Health + AttackPower + Defense;
    int otherStats = other.Health + other.AttackPower + other.Defense;
    return thisStats < otherStats || (thisStats == otherStats && level < other.level);
}

bool Pet::operator>(const Pet& other) 
{
    return !(*this <= other);
}

bool Pet::operator<=(const Pet& other) 
{
    return *this < other || *this == other;
}

bool Pet::operator>=(const Pet& other) 
{
    return !(*this < other);
}


void DamageAbility :: ApplyEffect(Pet* Target, Pet* MainCharacter ) 
{
    int damageDone = damage - (Target->getDefense() / 10);
    if (damageDone <= 0) 
    damageDone = 0;

    Target->setHealth(Target->getHealth() - damageDone);

    cout << Target->getname() << " took " << damageDone << " damage from " <<  MainCharacter->getname()  << endl;
}

void HealingAbility :: ApplyEffect(Pet* Target, Pet* MainCharacter ) 
    {
        Target->setHealth(Target->getHealth() + Heal);
        cout << Target->getname() << " healed for " << Heal << " by " <<  MainCharacter->getname() << endl;
    }

void ShieldAbility :: ApplyEffect(Pet* target, Pet* source)  
{ 
    if (CurrentCoolDown == 0 && !used) 
    {
        target->applyShield(0.0f, 2.0f); // Absorbs all damage for 2 seconds
        CurrentCoolDown = cooldown;
        cout << target->getname() << " activated Magic Shield!" << endl;
    }
 } 

void RebirthAbility :: ApplyEffect(Pet* target, Pet* MainCharacter = nullptr) 
{
    if (target->getHealth() <= 0 && !Revived) 
    {
        target->setHealth(reviveHealth);
        Revived = true;
        cout << target->getname() << " has been reborn with " << reviveHealth << " HP!" << endl;
    }
}

void BuffAbility :: ApplyEffect(Pet* Target, Pet* MainCharacter = nullptr) 
{
    if (Buffname == "AttackPower") 
    {
        Target->setAttackPower(Target->getAttackPower() + Buffamount);
        cout << Target->getname() << " attack increased by " << Buffamount << endl;
    } else if (Buffname == "Defense") 
    {
        Target->setDefensemode(Target->getDefense() + Buffamount);
        cout << Target->getname() << " defense increased by " << Buffamount << endl;
    } else if (Buffname == "Speed") 
    {
        Target->setSpeed(Target->getSpeed() + Buffamount);
        cout << Target->getname() << " speed increased by " << Buffamount << endl;
    }
}

void StunAbility:: ApplyEffect(Pet* target, Pet* Maincharacter = nullptr) 
{
    if (target) 
    {
        target->setStunned(true);
        target->setStunDuration(3.0f); // Set stun duration to 3 seconds
        cout << "Roar applied: AI pet stunned for " << target->getStunDuration() << " seconds." << endl;
    }
}

//& Derived pet classes
class Dragon : public Pet 
{
    public:
    //^ Constructor

    Dragon(string name, string roles) : Pet(name, 1500, 600, 70.25, 300, roles) 
    {
        Ability* fireball = new DamageAbility("Fireball", "Launches a fireball", 3, 200 , 0 , false , false);
        Ability* roar = new StunAbility("Roar", "Stuns enemies", 5, 2 , false , false , false);
        AddAbility(fireball);
        AddAbility(roar);
        abilityCount = 2;
    }
    string getType() const override 
    { 
        return "Dragon"; 
    }

    Pet* copy() const override 
    { 
        return new Dragon(*this); 
    }
};

class Phoenix : public Pet 
{
public:
//* constructor
    Phoenix(string name, string roles) : Pet(name, 1000, 400, 40.21, 600, roles) 
    {
        Ability* healing = new HealingAbility("Flame Heal", "Heals allies", 4, 150 , false , false , false);
        Ability* rebirth = new RebirthAbility("Rebirth", "Revives when dead", 500 , false , false , false);
        AddAbility(healing);
        AddAbility(rebirth);
        abilityCount = 2;
    }
   
    string getType() const override 
    { 
        return "Phoenix"; 
    }

    Pet* copy() const override 
    { 
        return new Phoenix(*this); 
    }
    
};

class Griffin : public Pet 
{
    public:
    //& constrcutor
    Griffin(string name, string roles) : Pet(name, 1200, 500, 60.31, 300, roles) 
    {
        Ability* speedBurst = new BuffAbility("Speed Burst", "Increases speed", 3, "Speed", 20.0, 3 , false , false , false);
        Ability* shield = new BuffAbility("Shield", "Boosts defense", 4, "Defense", 100, 2 , false , false , false);
        AddAbility(speedBurst);
        AddAbility(shield);
        abilityCount = 2;
    }
   
    string getType() const override 
    { 
        return "Griffin"; 
    }

    Pet* copy() const override 
    { 
        return new Griffin(*this); 
    }
};

class Unicorn : public Pet 
{
    public:
    //* Para cons
    Unicorn(string name, string roles) : Pet(name, 800, 350, 90.00, 200, roles) 
    {
        Ability* speedBurst = new BuffAbility("Speed Burst", "Increases speed", 3, "Speed", 20.0, 3, 0, false, false, false);
        Ability* magicShield = new BuffAbility("Magic Shield", "Absorbs damage", 4, "Defense", 100, 2, 0, false, false, false);
        AddAbility(speedBurst);
        AddAbility(magicShield);
        abilityCount = 2;
    }
    string getType() const override 
    { 
        return "Unicorn"; 
    }

    Pet* copy() const override 
    { 
        return new Unicorn(*this); 
    }
};


//~ Item classes

class Item 
{
    protected:
    string name;
    int price;
    string description;

    public:

    //Para cons
    Item(string name, int price, string description) : name(name), price(price), description(description) {}


    // Setters and getters
    string getName()  
    {
        return name; 
    }

    int getPrice() 
    { 
        return price; 
    }

    string getDescription()  
    { 
        return description; 
    }

    //* Virtual Funcrions (Abstraction)
    virtual void useItem(Pet* pet) = 0;
    virtual string getType() const = 0;
    virtual Item* clone() const = 0;
};

class HealingPotion : public Item 
{
    public:
    //* Cons
    HealingPotion() : Item("Healing Potion", 50, "Restores 50 Health") {}

    //* overrinding
    void useItem(Pet* pet) override 
    {
        if (pet) 
        {
            pet->setHealth(pet->getHealth() + 50);
            //* Debugging
            cout << pet->getname() << "'s Health restored by 50!" << endl;
        }
    }

    Item* clone() const override 
    { 
        return new HealingPotion(*this); 
    }

    string getType() const override 
    { 
        return "HealingPotion"; 
    }
};

class ManaPotion : public Item 
{
    public:
    ManaPotion() : Item("Mana Potion", 40, "Restores 2 ability points") {}

    void useItem(Pet* pet) override 
    {
        if(pet) 
        {
            cout << pet->getname() << " gained 2 ability points!" << endl;
        }
    }
    //* overriding
    Item* clone() const override 
    { 
        return new ManaPotion(*this); 
    }
    string getType() const override 
    { 
        return "ManaPotion"; 
    }
};

class BuffItem : public Item 
{
    public:
    BuffItem() : Item("Buff Item", 60, "Boosts Speed and Attack by 20% for 3 turns") {}

    void useItem(Pet* pet) override 
    {
        if (pet) 
        {
            pet->applyBuff(1.2f, 1.2f, 3);
            cout << pet->getname() << "'s Speed and Attack boosted by 20% for 3 turns!" << endl;
        }
    }
    //* overriding
    Item* clone() const override 
    { 
        return new BuffItem(*this); 
    }

    string getType() const override 
    { 
        return "BuffItem"; 
    }
};

class ShieldItem : public Item 
{
    public:
    ShieldItem() : Item("Shield Item", 45, "Reduces damage by 50% for 2 turns") {}

    void useItem(Pet* pet) override {

        if (pet) 
        {
            pet->applyShield(0.5f, 2);
            cout << pet->getname() << " gained a shield reducing damage by 50% for 2 turns!" << endl;
        }
    }

    //* overriding
    Item* clone() const override 
    { 
        return new ShieldItem(*this); 
    }

    string getType() const override 
    {
         return "ShieldItem"; 
    }
};

//& Inventory class
class Inventory 
{
    private:
    //* max items 10 can hold inventory
        Item* items[MAX_ITEMS];
        int itemCount;
    
    public:
    //* cons
        Inventory() : itemCount(0) 
        {
            for (int i = 0; i < MAX_ITEMS; i++) 
            {
                items[i] = nullptr;
            }
        }
    
    //Deallocation
        ~Inventory() 
        {
            for (int i = 0; i < itemCount; i++) 
            {
                delete items[i];
            }
        }
    
    //adding itemcount++
        bool addItem(Item* item) 
        {
            if (itemCount < MAX_ITEMS) 
            {
                items[itemCount] = item;
                itemCount++;
                return true;
            }
            return false;
        }
    
        bool removeItem(int index) 
        {
            if (index < 0 || index >= itemCount) //* Validate index.
                return false;
    
            delete items[index];    //* Delete the item object.
            items[index] = nullptr; 
    
            //* Shift items to fill the gap.
            for (int i = index; i < itemCount - 1; i++) 
            {
                items[i] = items[i + 1]; //* Move next item to current slot.
                items[i + 1] = nullptr;  //* Clear the next slot.
            }
            itemCount--; //* Decrease item count.
            return true; 
        }

        //* returns use item in arena
        bool useItem(int index, Pet* pet) 
        {
            if (index < 0 || index >= itemCount || !pet) 
            return false;

            items[index]->useItem(pet);
            return removeItem(index);
        }
    
        Item** getItems() 
        { 
            return items; 
        }

        int getItemCount() const 
        { 
            return itemCount; 
        }
    
        void displayInventory(RenderWindow& window) 
        {
            //* Draw a semi-transparent panel 
            RectangleShape panel(Vector2f(500, 400));
            panel.setPosition(WINDOW_WIDTH / 2 - 250, WINDOW_HEIGHT / 2 - 200);
            panel.setFillColor(Color(30, 30, 30, 220));
            panel.setOutlineColor(Color::White);
            panel.setOutlineThickness(2);
            window.draw(panel);
    
            //* Draw inventory title
            drawTextWithShadow(window, "Inventory", globalFont, 40, WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 180, Color(255, 215, 0));
    
            //* Count quantities of each item type
            int healingCount = 0, manaCount = 0, buffCount = 0, shieldCount = 0;

            for (int i = 0; i < itemCount; i++)
             {
                if (items[i]->getName() == "Healing Potion") healingCount++;
                else if (items[i]->getName() == "Mana Potion") manaCount++;
                else if (items[i]->getName() == "Buff Item") buffCount++;
                else if (items[i]->getName() == "Shield Item") shieldCount++;
            }
    
            // Display items with quantities
            int yOffset = 0;
            //* Vertical offset for stacking item names.

            if (itemCount == 0) 
            {
                drawTextWithShadow(window, "No items in inventory", globalFont, 28, WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 100, Color::Red);
            } else 
            {
                if (healingCount > 0) {
                    drawTextWithShadow(window, "Healing Potion: " + to_string(healingCount), globalFont, 28, WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 - 100 + yOffset, Color::Green);
                    yOffset += 40;
                }
                if (manaCount > 0) {
                    drawTextWithShadow(window, "Mana Potion: " + to_string(manaCount), globalFont, 28, WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 - 100 + yOffset, Color::Blue);
                    yOffset += 40;
                }
                if (buffCount > 0) {
                    drawTextWithShadow(window, "Buff Item: " + to_string(buffCount), globalFont, 28, WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 - 100 + yOffset, Color::Yellow);
                    yOffset += 40;
                }
                if (shieldCount > 0) {
                    drawTextWithShadow(window, "Shield Item: " + to_string(shieldCount), globalFont, 28, WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 - 100 + yOffset, Color::Cyan);
                    yOffset += 40;
                }
            }
    
            //* Draw below header
            drawTextWithShadow(window, "Press ESC to return", globalFont, 24, WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 + 150, Color::Cyan);
        }

        //* deallocationg memory
        void clear() 
        {
            for (int i = 0; i < itemCount; i++) 
            {
                delete items[i];
                items[i] = nullptr;
            }
            itemCount = 0;
        }
    };

//& Guild class
class Guild 
{
    private:
    Pet** pets;
    int petCount;
    int capacity;

    public:
    //* random capacity
    Guild() : petCount(0), capacity(2) 
    {
        pets = new Pet*[capacity];
        for (int i = 0; i < capacity; i++) 
        {
            pets[i] = nullptr;
        }
    }

    // Setter and gettees
    Pet** getPets() 
    { 
    return pets; 
    }

    int getPetCount() 
    { 
        return petCount; 
    }

    // Need to be modified
    void DisplayGuild(RenderWindow& window, float deltaTime, int& selectedPetIndex, Vector2i mousePos, float& scrollOffset, int clickedIndex, int doubleClickedIndex, int& gameState, Pet*& selectedPet, bool levelUpTriggered, TrainingCamps* camp) 
    {

    // Animated title, centered
    static float titleAlpha = 0.0f;
    titleAlpha += 0.01f;

    if (titleAlpha > 1.0f) 
    titleAlpha = 0.0f;

    Color titleColor(255, 215, 0, static_cast<Uint8>(sin(titleAlpha * 3.14) * 55 + 200));

    drawTextWithShadow(window, "Guild Members\nMAGICAL PETS KINGDOM", globalFont, 50, WINDOW_WIDTH / 2 - 250, 30, titleColor);

    //* Handle empty guild
    if (petCount == 0) 
    {
        drawTextWithShadow(window, "No pets in guild!", globalFont, 36, WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 50, Color::Red);
        return;
    }

    //* Calculate total height for pet cards
    // Calculate layout for pet cards.
    const float cardHeight = 280.0f;
    const float spacing = 40.0f;
    float totalHeight = petCount * cardHeight + (petCount - 1) * spacing;
    float startY = (WINDOW_HEIGHT - totalHeight) / 2 + scrollOffset;

    //* scroll offset
    float maxScroll = 0.0f;
    float minScroll = -max(0.0f, totalHeight - (WINDOW_HEIGHT - 200));
    scrollOffset = clamp(scrollOffset, minScroll, maxScroll);

    //* Display each pet with a card-like background
    for (int i = 0; i < petCount; i++) 
    {
        float cardY = startY + i * (cardHeight + spacing); //* Calculate y-position for this card.

        if (cardY + cardHeight < 100 || cardY > WINDOW_HEIGHT) //& Skip cards outside the window.
        continue;

        //* Create a background rectangle for the pet card.
        RectangleShape petBackground(Vector2f(1100, cardHeight));

        petBackground.setPosition(WINDOW_WIDTH / 2 - 550, cardY);

        FloatRect cardBounds(WINDOW_WIDTH / 2 - 550, cardY, 1100, cardHeight);

        bool isHovered = cardBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

      //* Apply scaling effect when hovered.
      float scale = isHovered ? 1.05f : 1.0f; //* Slightly enlarge on hover.
      petBackground.setScale(scale, scale);   //* Apply scale to background.
      petBackground.setPosition(
          WINDOW_WIDTH / 2 - 550 * scale,    //* Adjust x-position for scaling.
          cardY + (cardHeight * (1.0f - scale)) / 2 //* Adjust y-position for scaling.
      );

      //* Apply pulsing background effect for visual appeal.
        static float pulseTime = 0.0f;

        pulseTime += deltaTime; //* Update pulse timer.

        float pulse = sin(pulseTime * 2 + i) * 20 + 100;
        petBackground.setFillColor(Color(40, 40, 40, static_cast<Uint8>(pulse))); //* Dark background with pulse.

        // Glowing outline for selected pet
        if (selectedPetIndex == i) 
        {
            float glowAlpha = sin(pulseTime * 3.0f) * 50 + 205;
            petBackground.setOutlineColor(Color(255, 255, 0, static_cast<Uint8>(glowAlpha)));
            petBackground.setOutlineThickness(5);
        } else 
        {
            petBackground.setOutlineColor(Color(255, 215, 0, 200));
            petBackground.setOutlineThickness(3);
        }
        window.draw(petBackground);

        // Handle interactions (passed from main)
        if (clickedIndex == i) 
        {
            if (selectedPetIndex == i) 
            {
                selectedPetIndex = -1; // Deselect
            } else 
            {
                selectedPetIndex = i; // Select
            }
        }

        if (doubleClickedIndex == i) 
        {
            selectedPet = pets[i];
            gameState = TRAINING_MENU;
            scrollOffset = 0.0f;
        }

        pets[i]->DisplayStats(window, WINDOW_WIDTH / 2 - 530 * scale, cardY + 20);
    }
}

//*Prototype
    void Train(Player* player, TrainingCamps* Trainingcamp, Pet* pet, RenderWindow& window);

    void Addpet(Pet* pet) {
        if (!pet) {
            cout << "Cannot add null pet" << endl;
            return;
        }
        if (petCount == capacity) {
            int newCapacity = capacity * 2;
            Pet** newPets = new Pet*[newCapacity];
            for (int i = 0; i < newCapacity; i++) {
                newPets[i] = nullptr; // Initialize to nullptr
            }
            for (int i = 0; i < petCount; i++) {
                newPets[i] = pets[i];
                pets[i] = nullptr; // Prevent double deletion
            }
            delete[] pets;
            pets = newPets;
            capacity = newCapacity;
        }
        pets[petCount++] = pet;
    }

    ~Guild() {
        for (int i = 0; i < petCount; i++) {
            delete pets[i];
            pets[i] = nullptr;
        }
        delete[] pets;
        pets = nullptr;
        petCount = 0;
        capacity = 0;
    }

    void AddpetCopy(const Pet& pet) {
        Addpet(pet.copy());
    }

    void reset() {
        for (int i = 0; i < petCount; i++) {
            delete pets[i];
            pets[i] = nullptr;
        }
        petCount = 0;
        delete[] pets;
        capacity = 2;
        pets = new Pet*[capacity];
        for (int i = 0; i < capacity; i++) {
            pets[i] = nullptr;
        }
    }

};

//& Player class
class Player 
{
    private:
    string name;
    Guild guild;
    Inventory inventory;
    int gold;

    public:
    Player(string name) : name(name), gold(100) {}

    //* Setter getters and use item by player
    string getName() 
    { 
        return name; 
    }

    Guild& getGuild() 
    { 
        return guild; 
    }

    int getGold()  
    { 
        return gold; 
    }

    void addGold(int amount) 
    { 
        gold += amount; 
    }

    void spendGold(int amount) 
    {
        if (amount <= gold) 
        {
            gold -= amount;
        }
    }
    Inventory& getInventory() 
    {
        return inventory; 
    }

    bool useItem(int index, Pet* pet) 
    {
        return inventory.useItem(index, pet);
    }

    ~Player() {}
};

//& Shop class

class Shop 
{
    private:
        Item* items[4];
        string message;
        Texture flaskTextures[4]; // Textures for the flask sprites
        Sprite flaskSprites[4];   // Sprites for the flask items
        Texture shopBackgroundTexture; //  background texture
        Sprite shopBackgroundSprite;   // background sprite
        int prices[4] = {50, 40, 60, 45}; //* 4 flasks with their prices
    
    public:

        Shop() 
        {
            items[0] = new HealingPotion();
            items[1] = new ManaPotion();
            items[2] = new BuffItem();
            items[3] = new ShieldItem();
            message = "";
        }
    
        //* Destructor
        ~Shop() 
        {
            for (int i = 0; i < 4; i++) 
            {
                delete items[i];
            }
        }
    
        void loadShopResources(const string backgroundFile, const string flaskFilenames[4]) 
        {
            //* Load shop background
            if (!shopBackgroundTexture.loadFromFile(backgroundFile)) 
            {
                cerr << "Failed to load " << backgroundFile << "! Using fallback color." << endl;
                shopBackgroundSprite.setColor(Color(30, 30, 30, 220));
                shopBackgroundSprite.setPosition(0, 0);
                shopBackgroundSprite.setScale(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT));

            } else 
            {
                shopBackgroundSprite.setTexture(shopBackgroundTexture);
                shopBackgroundSprite.setScale(
                    static_cast<float>(WINDOW_WIDTH) / shopBackgroundSprite.getLocalBounds().width,
                    static_cast<float>(WINDOW_HEIGHT) / shopBackgroundSprite.getLocalBounds().height
                );
            }
    
            //* Load flask textures and set up sprites
            for (int i = 0; i < 4; i++) 
            {
                if (!flaskTextures[i].loadFromFile(flaskFilenames[i])) 
                {
                    cerr << "Failed to load " << flaskFilenames[i] << "!" << endl;
                } else 
                {
                    flaskSprites[i].setTexture(flaskTextures[i]);
                    flaskSprites[i].setScale(0.5f, 0.5f); // Adjust scale as needed

                    // Arrange horizontally with spacing

                    float spacing = 150.0f; // Space between potions

                    float totalWidth = (4 * 100 + 3 * spacing); // 100 is approximate sprite width after scaling

                    float startX = (WINDOW_WIDTH - totalWidth) / 2; // Center the potions

                    flaskSprites[i].setPosition(startX + i * (100 + spacing), WINDOW_HEIGHT / 2 - 50); // Horizontal arrangement
                }
            }
        }
    
        Item** getItems() 
        { 
            return items; 
        }
    
        //* function for player to buy the item from the shop
        bool buyItem(int index, Player* player) 
        {
            if (index < 0 || index >= 4) 
            return false;

            if (player->getGold() >= prices[index]) 
            {
                player->spendGold(prices[index]);
                player->getInventory().addItem(items[index]->clone());
                message = "Purchased " + items[index]->getName() + " for " + to_string(prices[index]) + " gold!";
                return true;
            } else 
            {
                message = "Not enough gold! Need " + to_string(prices[index]) + " gold.";
                return false;
            }
        }
    
        string getMessage() const 
        { 
            return message; 
        
        }
        void clearMessage() 
        { 
            message = ""; 
        }

//* Deletion of Message after some interval
        void clearMessageAfterDelay(float deltaTime)
         {
            static float timer = 0.0f;

            timer += deltaTime;
            if (timer >= 3.0f && message == "")
            {
                message = "";
                timer = 0.0f;
            }
        }
    
        //* getting the sprite of flask
        Sprite& getFlaskSprite(int index) 
        {
            if (index >= 0 && index < 4)
            return flaskSprites[index];

            static Sprite dummy;

            return dummy;
        }
    
        void displayShopWindow(RenderWindow& window, int playerGold) 
        {
            window.draw(shopBackgroundSprite);
        
            //* Draw error message at the top center
            if (!message.empty()) 
            {
                drawTextWithShadow(window, message, globalFont, 24, WINDOW_WIDTH / 2 - 200, 20, Color::Red);
            }
        
            //* Draw player's gold below the error message
            drawTextWithShadow(window, "Your Gold: " + to_string(playerGold), globalFont, 28, WINDOW_WIDTH / 2 - 100, 60, Color::Yellow);
        
            //* Draw flask sprites and names
            string potionNames[4] = {"Fire Essence (50)", "Moon Dew (40)", "Thunder Elixir (60)", "Star Shield (25)"};

            for (int i = 0; i < 4; i++) 
            {
                window.draw(flaskSprites[i]);
        
                FloatRect spriteBounds = flaskSprites[i].getGlobalBounds();
                drawTextWithShadow(window, potionNames[i], globalFont, 20, spriteBounds.left, spriteBounds.top + spriteBounds.height + 30, Color::Yellow); // Increased offset from +10 to +30
            }
        
            //* Draw instruction at the bottom
            drawTextWithShadow(window, "Click a potion to buy (ESC to return)", globalFont, 20, WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT - 50, Color::Cyan); // Changed from WINDOW_HEIGHT - 20 to WINDOW_HEIGHT - 50
        }
    };
    
//& TrainingCamps class
class TrainingCamps 
{
    private:

    int TrainingPoints;                    // Accumulated points used for unlocking abilities or boosting stats.
    bool showingAbilityMenu;               // Whether the ability unlock menu is currently displayed.
    Pet* selectedPet;                     // Pointer to the pet currently selected for training.
    Pet* currentPet;                      // Pointer to the pet currently being processed (e.g., for ability unlocking).
    string trainingMessage;                // Message to display training outcomes (e.g., points gained, stats improved).
    int difficultyLevel;                   // Selected difficulty (1=Easy, 2=Medium, 3=Hard, 4=Extreme).
    Fireball* fireballs[MAX_FIREBALLS];    // Array of pointers to fireballs in the mini-game.
    int fireballCount;                     // Number of active fireballs.
    float survivalTime;                    // Time the pet has survived in the mini-game (in seconds).
    float fireballSpawnTimer;              // Timer to track when to spawn the next fireball.
    float fireballSpawnInterval;           // Interval between fireball spawns (based on difficulty).
    float fireballSpeed;                   // Speed of fireballs (based on difficulty).
    bool miniGameActive;                   // Whether the fireball-dodging mini-game is running.
    int trainingType;                     // Current training type (1=ObstacleCourse, 2=TargetPractice, 3=WeightTraining).
    static const int ObstacleCourse = 1;   // Training type constant for improving speed.
    static const int TargetPractice = 2;   // Training type constant for improving attack power.
    static const int WeightTraining = 3;   // Training type constant for improving defense.
    SoundBuffer fireballBuffer;            // Buffer for the fireball sound effect.
    Sound fireballSound;                   // Sound object for playing fireball spawn sound.
    float soundCooldown;                   // Cooldown timer to prevent rapid sound playback.
    static constexpr float SOUND_COOLDOWN_DURATION = 0.2f; // Minimum time between sound plays (in seconds).
    
    public:
//* paramterized construcotr
    TrainingCamps() : miniGameActive(false), trainingType(0), difficultyLevel(0), fireballCount(0),
    fireballSpawnTimer(0.0f), fireballSpawnInterval(0.0f), fireballSpeed(0.0f),
    survivalTime(0.0f), soundCooldown(0.0f), selectedPet(nullptr) 
    {

    for (int i = 0; i < MAX_FIREBALLS; i++) 
    {
    fireballs[i] = nullptr;
    }
        if (!fireballBuffer.loadFromFile("fireball.wav")) {
        cerr << "Failed to load fireball.wav" << endl;
    }

    fireballSound.setBuffer(fireballBuffer);
    }

    //* setters and getters
    void setSelectedPet(Pet* pet) 
    {
        selectedPet = pet;
    }

    string getTrainingMessage() const
    {
        return trainingMessage;
    }

    void clearTrainingMessage() 
    {
        trainingMessage.clear();
    }

    //* deallocation
    ~TrainingCamps() 
    { 
    for (int i = 0; i < MAX_FIREBALLS; i++) 
        {
        delete fireballs[i];
        fireballs[i] = nullptr;
        } 
    }

    int getFireballCount() const 
    { 
        return fireballCount; 
    }
    
    //* increment of training points
    void addTrainingPoints(int points) 
    {
        TrainingPoints += points;
    }

    //* setting of training points
    void setTrainingPoints(int points) 
    { 
        TrainingPoints = points; 
    } 

    //* pressing of unlocing the ability
    void handleAbilityUnlockInput(Event& event) 
    {
        if (showingAbilityMenu && currentPet) 
        {
            if (event.type == Event::KeyPressed) 
            {
                if (event.key.code >= Keyboard::Num1 && event.key.code <= Keyboard::Num3) 
                {
                    int choice = event.key.code - Keyboard::Num1;

                    if (TrainingPoints >= 10 && currentPet->UnlockAbility(choice)) 
                    {
                        TrainingPoints -= 10;
                        trainingMessage = "Unlocked " + currentPet->getAbility()[choice]->getname() + "!";
                    } else 
                    {
                        trainingMessage = "Not enough training points! Need 10.";
                    }
                    showingAbilityMenu = false;

                } else if (event.key.code == Keyboard::Escape) 
                {
                    showingAbilityMenu = false;
                }
            }
        }
    }


    //* constructions of emnu
    void drawAbilityMenu(RenderWindow& window) 
    {
        if (showingAbilityMenu && currentPet) 
        {
            RectangleShape background(Vector2f(500, 450));
            background.setPosition(350, 200);
            background.setFillColor(Color(70, 70, 90, 220));
            background.setOutlineThickness(2);
            background.setOutlineColor(Color::White);
            window.draw(background);

            Text title("Unlock Abilities", globalFont, 36);
            title.setPosition(400, 220);
            title.setFillColor(Color::White);
            window.draw(title);

            Text points("Training Points: " + to_string(TrainingPoints), globalFont, 32);
            points.setPosition(400, 260);
            points.setFillColor(Color::Cyan);
            window.draw(points);

            currentPet->displayAbilities(window, 400, 300);

            Text instructions("Press 1-3 to unlock (10 points)\nESC to cancel", globalFont, 32);
            instructions.setPosition(400, 550);
            instructions.setFillColor(Color::Yellow);
            window.draw(instructions);
        }
    }

    //* show ability menu
    void showAbilityMenu(Pet* pet) 
    {
        currentPet = pet;
        showingAbilityMenu = true;
    }

    //* when becomes it will display
    bool ShowAbilityMenu() 
    {
        return showingAbilityMenu;
    }

    //* start the training
    void startTraining(int type, int difficulty , Pet * pet) 
    {
        trainingType = type;
        difficultyLevel = difficulty;
        miniGameActive = true;
        survivalTime = 0.0f;
        fireballSpawnTimer = 0.0f;
        fireballCount = 0;
        selectedPet = pet;

        //* Set difficulty parameters
        switch (difficultyLevel) 
        {
            case 1: // Easy
                fireballSpawnInterval = 2.0f;  // Slower spawn rate
                fireballSpeed = 80.0f;         // Slower movement
                break;
            case 2: // Medium
                fireballSpawnInterval = 1.2f;
                fireballSpeed = 120.0f;
                break;
            case 3: // Hard
                fireballSpawnInterval = 0.8f;
                fireballSpeed = 160.0f;
                break;
            case 4: // Extreme
                fireballSpawnInterval = 0.4f;  // Faster spawn rate
                fireballSpeed = 200.0f;        // Faster movement
                break;
        }
    
        // Clear existing fireballs
        for (int i = 0; i < MAX_FIREBALLS; i++) 
        {
            delete fireballs[i];
            fireballs[i] = nullptr;
        }
        fireballCount = 0;
    
        fireballSpawnTimer = fireballSpawnInterval; //* Force spawn on next update
    }

   //*Need to be modified
    void updateMiniGame(float deltaTime, Sprite& petSprite) 
    {
        if (!miniGameActive) 
        return;
    
        soundCooldown -= deltaTime;
        survivalTime += deltaTime;
        fireballSpawnTimer += deltaTime;
    
        //*Spawn new fireballs based on difficulty

        if (fireballSpawnTimer >= fireballSpawnInterval && fireballCount < MAX_FIREBALLS) 
        {
            //* fireball diametere
            float scaledWidth = 40.0f; 
            //* position of fireball

            float xPos = static_cast<float>(rand() % static_cast<int>(WINDOW_WIDTH - scaledWidth));
            //* creating and incremenritng
            fireballs[fireballCount] = new Fireball(xPos, fireballSpeed);
            fireballCount++;
            fireballSound.play();
            soundCooldown = SOUND_COOLDOWN_DURATION;
            fireballSpawnTimer = 0.0f;
            //* debugging
            cout << "Spawned fireball at (" << xPos << ", " << fireballs[fireballCount - 1]->getPosition().y << "), count: " << fireballCount << endl;
        }
    
        //* Update fireballs and check collisions
        bool collisionDetected = false;

        for (int i = 0; i < fireballCount; i++) 
        {
            if (fireballs[i]) // Check if fireball exists.
            {
                fireballs[i]->update(deltaTime);

                cout << "Fireball " << i << " position: (" << fireballs[i]->getPosition().x << ", " << fireballs[i]->getPosition().y << ")" << endl; //* Debug position
    
                //* collison with pet sprite
                if (fireballs[i]->checkCollision(petSprite)) 
                {
                    collisionDetected = true;
                    break;
                }
                //* off screen
                if (!fireballs[i]->Active()) 
                {
                    delete fireballs[i];
                    fireballs[i] = nullptr;

                    for (int j = i; j < fireballCount - 1; j++) 
                    {
                        fireballs[j] = fireballs[j + 1];
                    }
                    fireballs[fireballCount - 1] = nullptr;

                    fireballCount--;

                    i--; //* Re-check the current index after shift
                }
            }
        }
    
        if (collisionDetected) 
        {
            miniGameActive = false;
            awardTrainingPoints();
        }
    }


    void drawMiniGame(RenderWindow& window, Sprite& petSprite, Sprite& miniGameBackgroundSprite) 
    {
        if (!miniGameActive) 
        return;
    
        //* Draw mini-game background
        window.draw(miniGameBackgroundSprite);
    
        //* Draw fireballs
        for (int i = 0; i < fireballCount; i++) 
        {
            if (fireballs[i] && fireballs[i]->Active()) 
            {
                fireballs[i]->draw(window);
            }
        }
    
        //& Draw pet sprite
        window.draw(petSprite);
    
        //! Draw UI text on top
        Text survivalText("Time: " + to_string(static_cast<int>(survivalTime)) + "s", globalFont, 28);
        survivalText.setPosition(10, 10);
        survivalText.setFillColor(Color::Yellow);
        window.draw(survivalText);
    
        Text difficultyText("Difficulty: Level " + to_string(difficultyLevel), globalFont, 28);
        difficultyText.setPosition(10, 50);
        difficultyText.setFillColor(Color::White);
        window.draw(difficultyText);
    
        Text trainingText("Training: " + getTrainingTypeName(), globalFont, 28);
        trainingText.setPosition(10, 90);
        trainingText.setFillColor(Color::Cyan);
        window.draw(trainingText);
    }


    void awardTrainingPoints() 
    {
        //* Base points per second survived, scaled by difficulty
        float pointsPerSecond;
        int maxPoints;
        float statPerSecond;
        float maxStat;
    
        switch (difficultyLevel) 
        {
            case 1: // Easy
                pointsPerSecond = 1.0f;
                maxPoints = 5;
                statPerSecond = 0.5f;
                maxStat = 2.0f;
                break;
            case 2: // Medium
                pointsPerSecond = 2.0f;
                maxPoints = 10;
                statPerSecond = 1.0f;
                maxStat = 4.0f;
                break;
            case 3: // Hard
                pointsPerSecond = 3.0f;
                maxPoints = 15;
                statPerSecond = 1.5f;
                maxStat = 6.0f;
                break;
            case 4: // Extreme
                pointsPerSecond = 4.0f;
                maxPoints = 20;
                statPerSecond = 2.0f;
                maxStat = 8.0f;
                break;
        }
    
        //* Calculate points and stat improvements
        int pointsEarned = min(static_cast<int>(survivalTime * pointsPerSecond), maxPoints);
        float statImprovement = min(survivalTime * statPerSecond, maxStat);
    
        //* Add training points
        TrainingPoints += pointsEarned;
    
        //* Apply stat improvements based on training type
        if (selectedPet) 
        {
            string statImproved;
             
            if(trainingType == ObstacleCourse)
            {
                    selectedPet->setSpeed(selectedPet->getSpeed() + statImprovement);
                    statImproved = "Speed";
            }else if(trainingType == TargetPractice)
            {
                    selectedPet->setAttackPower(selectedPet->getAttackPower() + static_cast<int>(statImprovement));
                    statImproved = "AttackPower";
            }else if(trainingType == WeightTraining)
            {
                    selectedPet->setDefense(selectedPet->getDefense() + static_cast<int>(statImprovement));
                    statImproved = "Defense";
            }
          //* after losing training
            trainingMessage = selectedPet->getname() + " gained " + to_string(pointsEarned) + " training points and improved " + statImproved + " by " + to_string(statImprovement) + "!";
        } else 
        {
            trainingMessage = "No pet selected, but gained " + to_string(pointsEarned) + " training points.";
        }
    }

    bool MiniGameActive() const 
    { 
        return miniGameActive; 
    }

    //* ending the mini game and nulling every pointer
    void endMiniGame() 
    {
       miniGameActive = false;
       selectedPet = nullptr;

        for (int i = 0; i < MAX_FIREBALLS; i++) 
        {
            delete fireballs[i];
            fireballs[i] = nullptr;
        }
        fireballCount = 0;
    }

    //* Taking name
    string getTrainingTypeName() const 
    {
        if (trainingType == ObstacleCourse) 
        {
            return "Obstacle Course";
        } else if (trainingType == TargetPractice) 
        {
            return "Target Practice";
        } else if (trainingType == WeightTraining) 
        {
            return "Weight Training";
        } else 
        {
            cerr << "Error: Invalid trainingType value " << trainingType << endl;
            return "Unknown";
        }
    }

    //! Need to be used in training in rubircs and stats improvement
    bool PointsUsed(Pet* pet, int choice, float points) 
    {
        if (choice == 1 && TrainingPoints >= 5) 
        {
            pet->AssignTrain("Health", points);
            TrainingPoints -= 5;
            trainingMessage = pet->getname() + " gained " + to_string(points) + " Health.";
            return true;
        } else if (choice == 2 && TrainingPoints >= 5) 
        {
            pet->AssignTrain("AttackPower", points);
            TrainingPoints -= 5;
            trainingMessage = pet->getname() + " gained " + to_string(points) + " Attack Power.";
            return true;
        } else if (choice == 3 && TrainingPoints >= 5) 
        {
            pet->AssignTrain("Speed", points);
            TrainingPoints -= 5;
            trainingMessage = pet->getname() + " gained " + to_string(points) + " Speed.";
            return true;
        } else if (choice == 4 && TrainingPoints >= 5) 
        {
            pet->AssignTrain("Defense", points);
            TrainingPoints -= 5;
            trainingMessage = pet->getname() + " gained " + to_string(points) + " Defense.";
            return true;
        }
        trainingMessage = "Not enough training points!";
        return false;
    }

    int getTrainingPoints() 
    {
        return TrainingPoints;
    }

    void Training(Player* player, Pet* pet, RenderWindow& window) 
    {
        currentPet = pet;
        trainingMessage = "";
    }

    Pet* getCurrentPet() const 
    { 
        return currentPet; 
    }

};

void Pet :: gainXP(int amount, TrainingCamps* camp) 
{
    //* gaining xp of pet by gold need training camp as paramter
    xp += amount;
    while (xp >= xpToNextLevel) 
    {
        levelUp(camp);
    }
}

void Pet:: levelUp(TrainingCamps* camp) 
{
    // Increease of level
    level++;
    xp -= xpToNextLevel;
    xpToNextLevel = static_cast<int>(xpToNextLevel * 1.5);
    camp->addTrainingPoints(5);
    //debugging statement
    cout << name << " leveled up to level " << level << "! Gained 5 training points." << endl;
}

//* forward Declaration
class Battle;

//& class ARena
class Arena 
{
    private:

    Player* player;             // Pointer to the player object owning the pets.
    Pet* playerPets[2];         // Array of up to two player pets in the battle (1v1 or 2v2).
    Pet* aiPets[2];             // Array of up to two AI-controlled pets in the battle.
    int playerPetCount;         // Number of player pets currently in the battle (0-2).
    int aiPetCount;             // Number of AI pets currently in the battle (0-2).
    Pet* selectedPet;           // Pointer to the first selected player pet (for 1v1 or 2v2).
    Pet* selectedPet2;          // Pointer to the second selected player pet (for 2v2).
    bool BattleActive;          // Whether a battle is currently active.
    float BattleTimer;          // Current time elapsed in the battle (in seconds).
    float BattleTime;           // Maximum battle duration (default: 60 seconds).
    bool PlayerWon;             // Whether the player won the battle.
    int selectedPetIndex;       // Index of the currently selected player pet.
    
    public:
    //* initilizing
        Arena(Player* p) : player(p), selectedPet(nullptr), selectedPet2(nullptr), BattleActive(false),BattleTimer(0.0f), BattleTime(60.0f), PlayerWon(false), playerPetCount(0), aiPetCount(0) 
        {
            for (int i = 0; i < 2; i++) 
            {
                playerPets[i] = nullptr;
                aiPets[i] = nullptr;
            }
        }
    
    //* deallocation
        ~Arena() 
        {
            for (int i = 0; i < 2; i++) 
            { 
                if (playerPets[i] && playerPets[i] != nullptr) 
                {
                    delete playerPets[i];
                    playerPets[i] = nullptr;
                }
                if (aiPets[i] && aiPets[i] != nullptr) 
                {
                    delete aiPets[i];
                    aiPets[i] = nullptr;
                }
            }
            playerPetCount = 0;
            aiPetCount = 0;
        } 
    //* get selected pet
        int getSelectedPetIndex() const 
        { 
            return selectedPetIndex; 
        }
    
        void start1v1Battle(int& gameState) 
        {
            if (!selectedPet) 
            {
                gameState = ARENA_MENU;
                return;
            }

            BattleActive = true;
            BattleTimer = 0.0f;
            PlayerWon = false;
    
            // Clean up existing pets
            for (int i = 0; i < playerPetCount; i++) 
            {
                delete playerPets[i];
                playerPets[i] = nullptr;
            }
            for (int i = 0; i < aiPetCount; i++) 
            {
                delete aiPets[i];
                aiPets[i] = nullptr;
            }
            playerPetCount = 0;
            aiPetCount = 0; 

        if (!selectedPet) 
        {
            gameState = SELECT_PET_FOR_BATTLE;
            BattleActive = false;
            return;
        }
    
            // Set up player pet
            playerPets[0] = selectedPet->copy();
            playerPets[0]->getSprite().setPosition(100, WINDOW_HEIGHT - 200);
            playerPetCount = 1;
    
            // Set up AI pet
            int aiPetType = rand() % 4 + 1;
            string aiPetName = "AI Pet " + to_string(aiPetType);
            string aiRole = (rand() % 3 == 0) ? "Tanker" : (rand() % 2 == 0) ? "Healer" : "DamageDealer";
            string aiTexturePath;

            switch (aiPetType) 
            {
                case 1: aiPets[0] = new Dragon(aiPetName, aiRole); aiTexturePath = "Dragon.png"; break;
                case 2: aiPets[0] = new Phoenix(aiPetName, aiRole); aiTexturePath = "Phoenix.png"; break;
                case 3: aiPets[0] = new Unicorn(aiPetName, aiRole); aiTexturePath = "Unicorn.png"; break;
                case 4: aiPets[0] = new Griffin(aiPetName, aiRole); aiTexturePath = "Griffin.png"; break;
            }

            if (aiPets[0] && !aiPets[0]->loadTexture(aiTexturePath)) 
            {
                cerr << "Failed to load AI pet texture: " << aiTexturePath << endl;
            }

            aiPets[0]->setHealth(500);
            aiPets[0]->getSprite().setPosition(WINDOW_WIDTH - 200, WINDOW_HEIGHT - 200);
            aiPetCount = 1;
    
            gameState = BATTLE;
        }
    
        void start2v2Battle(int& gameState, Battle* battle);
    
        void handleClick(Vector2i mousePos, Battle* battle);
    
        void startGuildBattle(int& gameState) 
        {
            BattleActive = true;
            BattleTimer = 0.0f;
            PlayerWon = false;
    
            // *Clean up existing pets
            for (int i = 0; i < playerPetCount; i++) 
            {
                delete playerPets[i];
                playerPets[i] = nullptr;
            }
            for (int i = 0; i < aiPetCount; i++) 
            {
                delete aiPets[i];
                aiPets[i] = nullptr;
            }
            playerPetCount = 0;
            aiPetCount = 0;
    
            int petCount = player->getGuild().getPetCount();

            if (petCount == 0) 
            {
                gameState = ARENA_MENU;
                BattleActive = false;
                return;
            }
    
           //* Add all player pets (up to 2 due to array size)

        for (int i = 0; i < petCount && i < 2; i++) 
        {
            playerPets[i] = player->getGuild().getPets()[i]->copy();
            playerPets[i]->getSprite().setPosition(100 + i * 100, WINDOW_HEIGHT - 200);
            playerPetCount++;
        }

        //* Set up one strong AI pet
        aiPets[0] = new Griffin("AI Griffin", "DamageDealer");

        // * texture loading
        if (!aiPets[0]->loadTexture("Griffin.png")) 
        {
            cerr << "Failed to load AI Griffin texture" << endl;
        }

        aiPets[0]->setHealth(500 * petCount); 
        aiPets[0]->getSprite().setPosition(WINDOW_WIDTH - 200, WINDOW_HEIGHT - 200);
        aiPetCount = 1;
    
            gameState = BATTLE;
        }
    
        //* setters and getters

        void setSelectedPet(Pet* pet) 
        { 
            selectedPet = pet; 
        }
    
        void setSelectedPet2(Pet* pet) 
        { 
            selectedPet2 = pet; 
        }
    
        Pet* getSelectedPet() const 
        { 
            return selectedPet; 
        }
    
        Pet* getPlayerPet(int index) 
        { 
            return (index < playerPetCount) ? playerPets[index] : nullptr; 
        }
    
        Pet* getAIPet(int index) 
        { 
            return (index < aiPetCount) ? aiPets[index] : nullptr; 
        }
    
        int getPlayerPetCount() const 
        { 
            return playerPetCount; 
        }
    
        int getAIPetCount() const 
        { 
            return aiPetCount; 
        }
    
        bool isBattleActive() 
        { 
            return BattleActive; 
        }
    
        float getBattleTimer() 
        { 
            return BattleTimer; 
        }
    
        float getBattleTimeLimit() 
        { 
            return BattleTime; 
        }
    
        bool getPlayerCondition() 
        { 
            return PlayerWon; 
        }

        void updateArena(float deltaTime, int& gameState, string& errorMessage, Battle* battle);
    
        void endBattle(int& gameState, string& errorMessage, Battle* battle);
          
};

class AI 
{
    private:

    Pet* botpet;                    // Pointer to the AI-controlled pet (bot) involved in the battle.
    float MoveSpeed;                // Speed at which the bot pet moves (pixels per second).
    float AttackCoolDown;           // Current cooldown timer for the bot pet's attack (in seconds).
    float abilityCooldown;          // Current cooldown timer for the bot pet's ability (in seconds).
    const float ATTACK_COOLDOWN = 1.0f;  // Constant defining the attack cooldown duration (1 second).
    const float SAME_ALIGNMENT = 50.0f;  // Constant defining alignment bonus or penalty (increased from 20.0f to 50.0f, possibly for damage or behavior).
    const float ATTACK_RANGE = 600.0f;   // Constant defining the maximum range for the bot pet's attack (increased from 300.0f to 600.0f, in pixels).
    const float ABILITY_COOLDOWN = 2.0f; // Constant defining the ability cooldown duration (2 seconds).
    
    public:

    //* Paramterized constructor
    AI(Pet* pet, float speed = 300.0f) : botpet(pet), MoveSpeed(speed), AttackCoolDown(0.0f), abilityCooldown(0.0f) 
    {
        if (!botpet) 
        {
            cout << "AI initialized with no pet!" << endl;
        }
    }
    
    //* Fireball for the Ai
        bool ShouldFireFireball(Pet* TargetPet) 
        {
            if (!botpet || !TargetPet || botpet->getHealth() <= 0) 
            return false;
    
            Vector2f BotPosition = botpet->getSprite().getPosition();
            Vector2f PlayerPosition = TargetPet->getSprite().getPosition();
    
            float dx = PlayerPosition.x - BotPosition.x;
            float dy = PlayerPosition.y - BotPosition.y;
    
            float distance = sqrt(dx * dx + dy * dy);
    
            //* alignment
            bool HorizontalLine = abs(PlayerPosition.y - BotPosition.y) < SAME_ALIGNMENT;
            bool VerticalLine = abs(PlayerPosition.x - BotPosition.x) < SAME_ALIGNMENT;
    
            if ((HorizontalLine || VerticalLine) && distance <= ATTACK_RANGE) 
            {
                AttackCoolDown -= 0.016f; //* Approximate deltaTime for 60 FPS
                if (AttackCoolDown <= 0) 
                {
                    AttackCoolDown = ATTACK_COOLDOWN;
                    return true;
                }
            }
            return false;
        }
    

        void UpdateAttack(float DeltaTime, Pet* TargetPet) 
        {
            // Exit if AI pet, target pet, or AI pet’s health is invalid.
            if (!botpet || !TargetPet || botpet->getHealth() <= 0) 
                return;
        
            // Check if AI pet’s sprite has a valid texture
            if (!botpet->getSprite().getTexture()) 
            {
                cerr << "Warning: AI pet " << botpet->getname() << " has no texture" << endl;
                return;
            }
        
            // Get current positions of AI and target pets.
            Vector2f BotPosition = botpet->getSprite().getPosition();
            Vector2f PlayerPosition = TargetPet->getSprite().getPosition();
        
            // Calculate distance vector between pets.
            float dx = PlayerPosition.x - BotPosition.x;
            float dy = PlayerPosition.y - BotPosition.y;
        
            // Compute Euclidean distance.
            float distance = sqrt(dx * dx + dy * dy);
        
            // Move AI pet toward target if distance is positive.
            if (distance > 0) 
            {
                //direction and scale by movement speed and time.
                float moveX = (dx / distance) * MoveSpeed * DeltaTime;
                float moveY = (dy / distance) * MoveSpeed * DeltaTime;
                botpet->getSprite().move(moveX, moveY); // Apply movement.
        
                // Clamp position to keep AI pet within window bounds.
                Vector2f NewPosition = botpet->getSprite().getPosition();
                FloatRect bounds = botpet->getSprite().getGlobalBounds();
        
                // Prevent moving off the left or right edges.
                if (NewPosition.x < 0) 
                    NewPosition.x = 0;
                if (NewPosition.x + bounds.width > WINDOW_WIDTH) 
                    NewPosition.x = WINDOW_WIDTH - bounds.width;
        
                // Prevent moving off the top or bottom edges.
                if (NewPosition.y < 0) 
                    NewPosition.y = 0;
                if (NewPosition.y + bounds.height > WINDOW_HEIGHT) 
                    NewPosition.y = WINDOW_HEIGHT - bounds.height;
        
                // Update AI pet’s position.
                botpet->getSprite().setPosition(NewPosition);
        
                // Log movement for debugging.
                cout << "AI pet " << botpet->getname() << " moved to (" << NewPosition.x << ", " << NewPosition.y  << "), move delta: (" << moveX << ", " << moveY << ")" << endl;
            }
        
            // Update ability cooldown.
            abilityCooldown -= DeltaTime;

            if (abilityCooldown <= 0) // Check if ability is ready.
            {
                // Select ability based on AI pet’s health.
                int abilityIndex = -1;
                if (botpet->getHealth() < 300) 
                {
                    abilityIndex = 1; // Use secondary ability if health is low.
                } 
                else 
                {
                    abilityIndex = 0; // Use primary ability otherwise.
                }
        
                // Attempt to use the selected ability.
                string abilityResult = botpet->useAbilities(abilityIndex);
        
                // Check if ability was successfully used.
                if (abilityResult.find("used") != string::npos) 
                {
                    // Apply ability effect to target pet.

                    Ability* ability = botpet->getAbility()[abilityIndex];

                    ability->ApplyEffect(TargetPet, botpet);
                    // Log ability usage.
                    cout << botpet->getname() << ": " << abilityResult << endl;
                }
                abilityCooldown = ABILITY_COOLDOWN; // Reset cooldown.
            }
        }
    
        Pet* GetAiPet() 
        {
            return botpet;
        }
};

class Battle 
{
private:
    Arena* arena;              
    AI* aiControllers[2];     
    Fire* fires[MAX_FIREBALLS]; 
    int fireballCount;         
    float attackCooldown;      
    const float ATTACK_COOLDOWN = 1.0f; 
    int selectedPairIndex; 
    string lastAbilityMessage; // Message for last ability used
float abilityMessageTimer; // Timer for message display
static constexpr float MESSAGE_DISPLAY_DURATION = 3.0f; // Display for 3 seconds

public:

    Battle(Arena* a) : arena(a), fireballCount(0), attackCooldown(0.0f) , lastAbilityMessage("") , abilityMessageTimer(0.0f)
    {
        for (int i = 0; i < 2; i++) 
        {
            aiControllers[i] = nullptr;
        }
        for (int i = 0; i < MAX_FIREBALLS; i++) 
        {
            fires[i] = nullptr;
        }
    }
//* initlize
    void InitilizeAi() 
    {
        for (int i = 0; i < arena->getAIPetCount(); i++) 
        {
            if (arena->getAIPet(i)) 
            {
                aiControllers[i] = new AI(arena->getAIPet(i), 100.0f);
            }
        }
    }

    void setAbilityMessage(const string& message) {

        lastAbilityMessage = message;
        abilityMessageTimer = MESSAGE_DISPLAY_DURATION;
        cout << "Ability message set: " << message << endl; // Debug
    }
    
    string getAbilityMessage() const 
    {
        return lastAbilityMessage;
    }
    
    float getAbilityMessageTimer() const 
    {
        return abilityMessageTimer;
    }
    
    void updateAbilityMessage(float deltaTime) 
    {
        if (abilityMessageTimer > 0) 
        {
            abilityMessageTimer -= deltaTime;
            if (abilityMessageTimer <= 0) 
            {
                lastAbilityMessage = "";
            }
        }
    }

    void setSelectedPair(int index) 
    {
        if (index < 0 || index >= arena->getPlayerPetCount() || !arena->getPlayerPet(index) || !arena->getAIPet(index)) 
        {
            cout << "Invalid pair index " << index << " or pets not available. Selection unchanged." << endl;
            selectedPairIndex = -1; // Reset to no selection if invalid
            return;
        }
        if (arena->getPlayerPet(index)->getHealth() <= 0 || arena->getAIPet(index)->getHealth() <= 0) 
        {
            cout << "Selected pair at index " << index << " has a defeated pet. Selection unchanged." << endl;

            selectedPairIndex = -1; // Reset to no selection if a pet is defeated
            return;
        }
        selectedPairIndex = index;
        cout << "Selected pair set to index " << index << ": Player Pet '" << arena->getPlayerPet(index)->getname()  << "' vs AI Pet '" << arena->getAIPet(index)->getname() << "'" << endl;
    }

    //* deallocation
    void ResetAi() 
    {
        for (int i = 0; i < arena->getAIPetCount(); i++) 
        {
            delete aiControllers[i];
            aiControllers[i] = nullptr;
        }
    }


    void update(float deltaTime) 
    {
        if (!arena->isBattleActive()) 
        return;

        updateAbilityMessage(deltaTime);

        for (int i = 0; i < arena->getAIPetCount(); i++) 
        {
            if (aiControllers[i] && arena->getAIPet(i) && arena->getAIPet(i)->getHealth() > 0) 
            {
                // Find a target player pet that's still alive
                Pet* targetPet = nullptr;

                for (int j = 0; j < arena->getPlayerPetCount(); j++) 
                {
                    if (arena->getPlayerPet(j) && arena->getPlayerPet(j)->getHealth() > 0) 
                    {
                        targetPet = arena->getPlayerPet(j);
                        break;
                    }
                }
                if (targetPet) 
                {
                    aiControllers[i]->UpdateAttack(deltaTime, targetPet);
                }
            }
        }

      
        for (int i = 0; i < arena->getAIPetCount(); i++) 
        {
            if (aiControllers[i] && aiControllers[i]->ShouldFireFireball(arena->getPlayerPet(0))) 
            {
                if (fireballCount < MAX_FIREBALLS) 
                {
                    FloatRect aiPetBounds = arena->getAIPet(i)->getSprite().getGlobalBounds();
                    float adjustedX = arena->getAIPet(i)->getSprite().getPosition().x + aiPetBounds.width / 2;
                    float adjustedY = arena->getAIPet(i)->getSprite().getPosition().y + aiPetBounds.height / 2;
                    
                    fires[fireballCount] = new Fire(adjustedX, adjustedY, 25.0f);
                    fires[fireballCount]->setDirection(arena->getPlayerPet(0)->getSprite().getPosition());
                    fireballCount++;

                    cout << arena->getAIPet(i)->getname() << " fired a flame at position (" << adjustedX << ", " << adjustedY << ")!" << endl;
                }
            }
        }
        for (int i = 0; i < fireballCount; i++) 
        {
            if (fires[i]) 
            {
                fires[i]->update(deltaTime);
                // Check for collision with player pets
                for (int j = 0; j < arena->getPlayerPetCount(); j++) 
                {
                    if (arena->getPlayerPet(j) && fires[i]->Active() && fires[i]->checkCollision(arena->getPlayerPet(j)->getSprite())) 
                    {
                        arena->getPlayerPet(j)->setHealth(arena->getPlayerPet(j)->getHealth() - 50); // Deal damage
                        fires[i]->deactivate();
                        //* debugging
                        cout << "Player pet " << arena->getPlayerPet(j)->getname() << " hit by fireball, health: " << arena->getPlayerPet(j)->getHealth() << endl;
                    }
                }
                //* Remove inactive fireballs
                if (!fires[i]->Active()) 
                {
                    delete fires[i];
                    fires[i] = nullptr;

                    for (int k = i; k < fireballCount - 1; k++) 
                    {
                        fires[k] = fires[k + 1];
                    }
                    fires[fireballCount - 1] = nullptr;
                    fireballCount--;
                    i--;
                }
            }
        }

        for (int i = 0; i < arena->getPlayerPetCount(); i++) 
        {
            arena->getPlayerPet(i)->updateBattleEffects(deltaTime);
        }
        for (int i = 0; i < arena->getAIPetCount(); i++) 
        {
            arena->getAIPet(i)->updateBattleEffects(deltaTime);
        }

        if (attackCooldown > 0) 
        {
            attackCooldown -= deltaTime;
        }
    }

    void handlePlayerInput(Event& event, float deltaTime, Inventory& inventory, int& gameState) 
    {
        if (!arena->isBattleActive()) 
        return;

        //* Inputs of the movement
        for (int i = 0; i < arena->getPlayerPetCount(); i++) 
        {
            Pet* playerPet = arena->getPlayerPet(i);

            if (playerPet->getHealth() <= 0) 
            continue;

            Sprite& petSprite = playerPet->getSprite();
            FloatRect bounds = petSprite.getGlobalBounds();
            float petX = petSprite.getPosition().x;
            float petY = petSprite.getPosition().y;

            if (Keyboard::isKeyPressed(Keyboard::Left) && petX > 0) 
            {
                petSprite.move(-300.0f * deltaTime, 0);
            }
            if (Keyboard::isKeyPressed(Keyboard::Right) && petX + bounds.width < WINDOW_WIDTH) 
            {
                petSprite.move(300.0f * deltaTime, 0);
            }
            if (Keyboard::isKeyPressed(Keyboard::Up) && petY > 0) 
            {
                petSprite.move(0, -300.0f * deltaTime);
            }
            if (Keyboard::isKeyPressed(Keyboard::Down) && petY + bounds.height < WINDOW_HEIGHT) 
            {
                petSprite.move(0, 300.0f * deltaTime);
            }

            if (event.type == Event::KeyPressed) 
            {
                if (event.key.code == Keyboard::Space && attackCooldown <= 0 && !playerPet->isStunned()) 
                {
                    if (fireballCount < MAX_FIREBALLS) 
                    {
                        FloatRect petBounds = petSprite.getGlobalBounds();
                        float adjustedX = petSprite.getPosition().x + petBounds.width / 2;
                        float adjustedY = petSprite.getPosition().y + petBounds.height / 2;
                        fires[fireballCount] = new Fire(adjustedX, adjustedY, 25.0f);
                        bool targetFound = false;

                        for (int j = 0; j < arena->getAIPetCount(); j++) 
                        {
                            if (arena->getAIPet(j)->getHealth() > 0) 
                            {
                                fires[fireballCount]->setDirection(arena->getAIPet(j)->getSprite().getPosition());
                                break;
                            }
                        }

                        if (targetFound) 
                        {
                            fireballCount++;
                            cout << playerPet->getname() << " fired a flame at position (" << adjustedX << ", " << adjustedY << ")!" << endl;
                            attackCooldown = ATTACK_COOLDOWN;
                        } else 
                        {
                            //* Log if no target found
                            cout << "Failed to fire: no valid AI pet target" << endl;
                            delete fires[fireballCount];
                            fires[fireballCount] = nullptr;
                        }
                    } else 
                    {
                            //* Log if max fireballs reached
                            cout << "Failed to fire: MAX_FIREBALLS exceeded" << endl;
                    }
                }

                if (event.key.code >= Keyboard::Num1 && event.key.code <= Keyboard::Num3 && !playerPet->isStunned()) 
                {
                    int abilityIndex = event.key.code - Keyboard::Num1;

                    string result = playerPet->useAbilities(abilityIndex);

                    if (result.find("used") != string::npos) 
                    {
                        Ability* ability = playerPet->getAbility()[abilityIndex];

                        for (int j = 0; j < arena->getAIPetCount(); j++) 
                        {
                            if (arena->getAIPet(j)->getHealth() > 0) 
                            {
                                ability->ApplyEffect(arena->getAIPet(j), playerPet);
                            }
                        }

                        if (dynamic_cast<HealingAbility*>(ability) || dynamic_cast<BuffAbility*>(ability)) 
                        {
                            for (int j = 0; j < arena->getPlayerPetCount(); j++) {

                                if (arena->getPlayerPet(j)->getHealth() > 0) 
                                {
                                    ability->ApplyEffect(arena->getPlayerPet(j), playerPet);
                                }
                            }
                        }
                        cout << playerPet->getname() << ": " << result << endl;
                    } else 
                    {
                        cout << playerPet->getname() << ": " << result << endl;
                    }
                }

                //* choosing the Item 
                if (event.key.code == Keyboard::I) 
                {
                    int itemIndex = 0;
                    if (inventory.getItemCount() > 0) 
                    {
                        inventory.useItem(itemIndex, playerPet);
                    }
                }
            }
        }
    }


 //* Handles the action associated with a button press in the battle scene.

 void handleButtonAction(int buttonIndex, Pet* playerPet) 
 {
     if (!arena->isBattleActive() || !playerPet || playerPet->getHealth() <= 0 || playerPet->isStunned()) 
     {
         cout << "Cannot perform action: ";
         if (!arena->isBattleActive())
             cout << "Battle not active";
         else if (!playerPet)
             cout << "No player pet";
         else if (playerPet->getHealth() <= 0)
             cout << "Player pet dead";
         else if (playerPet->isStunned())
             cout << "Player pet stunned";
         cout << endl;
         return; 
     }
 
     try 
     {
         if (buttonIndex == 0) 
         { 
             if (fireballCount < MAX_FIREBALLS && attackCooldown <= 0) 
             {
                 Sprite& petSprite = playerPet->getSprite();
                 if (!petSprite.getTexture()) 
                 {
                     cerr << "Warning: Player pet " << playerPet->getname() << " has no texture!" << endl;
                     return;
                 }
 
                 FloatRect petBounds = petSprite.getGlobalBounds();
                 float adjustedX = petSprite.getPosition().x + petBounds.width / 2;
                 float adjustedY = petSprite.getPosition().y + petBounds.height / 2;
 
                 fires[fireballCount] = new Fire(adjustedX, adjustedY, 25.0f);
                 
                 bool targetFound = false;
                 for (int j = 0; j < arena->getAIPetCount(); j++) 
                 {
                     if (arena->getAIPet(j) && arena->getAIPet(j)->getHealth() > 0) 
                     {
                         fires[fireballCount]->setDirection(arena->getAIPet(j)->getSprite().getPosition());
                         targetFound = true;
                         break;
                     }
                 }
 
                 if (targetFound) 
                 {
                     fireballCount++;
                     cout << playerPet->getname() << " fired a flame!" << endl;
                     setAbilityMessage(playerPet->getname() + " used Attack!"); // Add message for attack
                     attackCooldown = ATTACK_COOLDOWN;
                 } 
                 else 
                 {
                     cout << "Failed to fire: no valid AI pet target" << endl;
                     delete fires[fireballCount];
                     fires[fireballCount] = nullptr;
                 }
             }
         } 
         else if (buttonIndex == 1 || buttonIndex == 2) 
         { 
             int abilityIndex = buttonIndex - 1;
             string result = playerPet->useAbilities(abilityIndex);
             
             if (result.find("used") != string::npos) 
             {
                 Ability* ability = playerPet->getAbility()[abilityIndex];
                 // Set ability message for successful ability use
                 setAbilityMessage(playerPet->getname() + " used " + ability->getName() + "!");
                 
                 for (int j = 0; j < arena->getAIPetCount(); j++) 
                 {
                     if (arena->getAIPet(j) && arena->getAIPet(j)->getHealth() > 0) 
                     {
                         cout << playerPet->getname() << " applying ability " << abilityIndex 
                              << " to AI pet " << arena->getAIPet(j)->getname() << endl;
                         ability->ApplyEffect(arena->getAIPet(j), playerPet);
                     }
                 }
                 
                 if (dynamic_cast<HealingAbility*>(ability) || dynamic_cast<BuffAbility*>(ability)) 
                 {
                     for (int j = 0; j < arena->getPlayerPetCount(); j++) 
                     {
                         if (arena->getPlayerPet(j) && arena->getPlayerPet(j)->getHealth() > 0) 
                         {
                             cout << playerPet->getname() << " applying healing/buff ability "  << abilityIndex << " to player pet " << arena->getPlayerPet(j)->getname() << endl;
                             ability->ApplyEffect(arena->getPlayerPet(j), playerPet);
                         }
                     }
                 }
                 cout << playerPet->getname() << ": " << result << endl;
             } 
             else 
             {
                 cout << playerPet->getname() << ": " << result << endl;
             }
         }
     }
     catch (const std::bad_alloc& e) 
     {
         cout << "bad_alloc in handleButtonAction: " << e.what() << endl;
     }
 }

//* Draws the battle scene, including pets, abilities, projectiles, and UI elements.
void draw(RenderWindow& window) 
{
    //* Temporary rectangle shape for rendering health bars.
    RectangleShape healthBar;
    
    // * xit early if the battle is not in a valid state.
    if (!arena->isBattleActive()) 
    return;

    //* Draw all alive player pets.
    for (int i = 0; i < arena->getPlayerPetCount(); i++) 
    {
        if (arena->getPlayerPet(i)->getHealth() > 0) 
        {
            window.draw(arena->getPlayerPet(i)->getSprite());
        }
    }
    //* Draw all alive AI pets.
    for (int i = 0; i < arena->getAIPetCount(); i++) 
    {
        if (arena->getAIPet(i)->getHealth() > 0) 
        {
            window.draw(arena->getAIPet(i)->getSprite());
        }
    }

    //*Draw all active fireballs (flames) fired during the battle.
    for (int i = 0; i < fireballCount; i++) 
    {
        if (fires[i]) 
        {
            cout << "Drawing flame " << i << " at (" << fires[i]->getPosition().x 
                 << ", " << fires[i]->getPosition().y << ")" << endl;
            fires[i]->draw(window);
        }
    }

    
    drawTextWithShadow(window, "Time: " + to_string(static_cast<int>(arena->getBattleTimeLimit() - arena->getBattleTimer())) + "s",globalFont, 28, 10, 10, Color::Yellow);

    // Render health bars and labels for each player pet.
    for (int i = 0; i < arena->getPlayerPetCount(); i++) 
    {
        Pet* pet = arena->getPlayerPet(i);

        // Compute the ratio of current health to maximum (assumed to be 1000 here).

        float healthRatio = static_cast<float>(pet->getHealth()) / 1000.0f;

        // Create a health bar proportional to the pet's health.
        RectangleShape healthBar(Vector2f(200 * healthRatio, 20));

        healthBar.setPosition(pet->getSprite().getPosition().x, pet->getSprite().getPosition().y - 30);

        healthBar.setFillColor(Color::Green);

        window.draw(healthBar);
        // Draw the pet's name and current health above the bar.
        drawTextWithShadow(window, pet->getname() + ": " + to_string(pet->getHealth()) + " HP", globalFont, 20, pet->getSprite().getPosition().x, pet->getSprite().getPosition().y - 50, Color::White);
    }

    // Render health bars and labels for each AI pet.
    for (int i = 0; i < arena->getAIPetCount(); i++) 
    {
        Pet* pet = arena->getAIPet(i);
        float healthRatio = static_cast<float>(pet->getHealth()) / 1000.0f;
        // Reuse the healthBar variable with updated size and position.
        healthBar.setSize(Vector2f(200 * healthRatio, 20));
        healthBar.setPosition(pet->getSprite().getPosition().x, pet->getSprite().getPosition().y - 30);
        healthBar.setFillColor(Color::Red);
        window.draw(healthBar);
        // Draw the AI pet's name and health.
        drawTextWithShadow(window, pet->getname() + ": " + to_string(pet->getHealth()) + " HP", globalFont, 20, pet->getSprite().getPosition().x, pet->getSprite().getPosition().y - 50, Color::White);
    }

    //* If there is at least one player pet, display its abilities on the UI.
    if (arena->getPlayerPetCount() > 0) 
    {
        arena->getPlayerPet(0)->displayAbilities(window, WINDOW_WIDTH - 300, 50);
    }
}

//* Destructor for the Battle class that cleans up dynamically allocated memory.
~Battle() 
{
    // Delete all created fireball objects.
    for (int i = 0; i < fireballCount; i++) 
    {
        delete fires[i];
        fires[i] = nullptr;
    }
    fireballCount = 0;
    
    // Delete AI controller objects for each AI pet.
    for (int i = 0; i < arena->getAIPetCount(); i++) 
    {
        delete aiControllers[i];
        aiControllers[i] = nullptr;
    }
    
    // Reset any AI-related state or data.
   ResetAi();
}

};

//* updating the arena
void Arena :: updateArena(float deltaTime, int& gameState, string& errorMessage, Battle* battle) 
{
    if (!BattleActive) 
    return;

    BattleTimer += deltaTime;

    bool allPlayerPetsDefeated = true;
    bool allAIPetsDefeated = true;

    for (int i = 0; i < playerPetCount; i++) 
    {
        if (playerPets[i]->getHealth() > 0) 
        {
            allPlayerPetsDefeated = false;
            break;
        }
    }
    for (int i = 0; i < aiPetCount; i++) 
    {
        if (aiPets[i]->getHealth() > 0) 
        {
            allAIPetsDefeated = false;
            break;
        }
    }

    if (BattleTimer >= BattleTime || allPlayerPetsDefeated || allAIPetsDefeated) 
    {
        endBattle(gameState, errorMessage, battle);
    }
}

void Arena :: endBattle(int& gameState, string& errorMessage, Battle* battle) 
{
    BattleActive = false;

    int totalPlayerHealth = 0;
    int totalAIHealth = 0;

    for (int i = 0; i < playerPetCount; i++) 
    {
        totalPlayerHealth += playerPets[i] ? playerPets[i]->getHealth() : 0;
    }
    for (int i = 0; i < aiPetCount; i++) 
    {
        totalAIHealth += aiPets[i] ? aiPets[i]->getHealth() : 0;
    }

    if (totalPlayerHealth > 0 && totalAIHealth <= 0) 
    {
        PlayerWon = true;
    } else if (totalPlayerHealth <= 0 && totalAIHealth > 0) 
    {
        PlayerWon = false;
    } else if (totalPlayerHealth > totalAIHealth) 
    {
        PlayerWon = true;
    } else 
    {
        PlayerWon = false;
    }

    try {
        if (PlayerWon) 
        {
            errorMessage = "Player wins! Gained 100 Gold";
            if (errorMessage.length() > 100) errorMessage = "Player wins!";
            player->addGold(100);
            if (selectedPet && playerPetCount == 1) 
            {
                selectedPet->setHealth(playerPets[0]->getHealth());
            }
        } else {
            errorMessage = "AI Won";
            if (errorMessage.length() > 100) errorMessage = "AI wins!";
        }
    }
    catch (const std::length_error& e) 
    {
        cout << "length_error in endBattle: " << e.what() << endl;
        errorMessage = "Error: Invalid string length!";
    }

    for (int i = 0; i < playerPetCount; i++) 
    {
        delete playerPets[i];
        playerPets[i] = nullptr;
    }
    for (int i = 0; i < aiPetCount; i++) 
    {
        delete aiPets[i];
        aiPets[i] = nullptr;
    }

    playerPetCount = 0;
    aiPetCount = 0;
    selectedPet = nullptr;
    selectedPet2 = nullptr;

    battle->ResetAi();
    cout << "Battle ended: PlayerWon=" << PlayerWon << ", errorMessage='" << errorMessage << "'" << endl;
    gameState = ARENA_MENU;
}

void Arena :: handleClick(Vector2i mousePos, Battle* battle) 
{
    if (!BattleActive) 
    return;

    for (int i = 0; i < playerPetCount; i++) 
    {
        if (playerPets[i] && playerPets[i]->getSprite().getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) 
        {
            selectedPetIndex = i;
            cout << "Selected player pet at index " << i << endl;
            // Trigger movement for both player and AI pet at this index
            battle->setSelectedPair(i);
            break;
        }
    }
}

void Arena :: start2v2Battle(int& gameState, Battle* battle) 
{

    BattleActive = true;
    BattleTimer = 0.0f;
    PlayerWon = false;
    selectedPetIndex = -1; //* Reset selection

    //* Clean up existing pets
    for (int i = 0; i < playerPetCount; i++) 
    {
        delete playerPets[i];
        playerPets[i] = nullptr;
    }
    for (int i = 0; i < aiPetCount; i++) 
    {
        delete aiPets[i];
        aiPets[i] = nullptr;
    }
    playerPetCount = 0;
    aiPetCount = 0; 

    //* pet count to check pets for 2 v 2 
    int petCount = player->getGuild().getPetCount();
    if (petCount < 2) 
    {
        gameState = ARENA_MENU;
        BattleActive = false;
        return;
    }

    if (!selectedPet || !selectedPet2) 
    {
        gameState = SELECT_PETS_FOR_2V2;
        BattleActive = false;
        return;
    }

    //* Set up player pets
    playerPets[0] = selectedPet->copy();
    playerPets[0]->getSprite().setPosition(100, WINDOW_HEIGHT - 200);
    playerPets[1] = selectedPet2->copy();
    playerPets[1]->getSprite().setPosition(200, WINDOW_HEIGHT - 200);
    playerPetCount = 2;

    //* Set up AI pets
    aiPets[0] = new Dragon("AI Dragon 1", "Tanker");

    if (!aiPets[0]->loadTexture("Dragon.png")) 
    {
        cerr << "Failed to load AI Dragon texture" << endl;
    }
    aiPets[0]->setHealth(500);
    aiPets[0]->getSprite().setPosition(WINDOW_WIDTH - 200, WINDOW_HEIGHT - 200);

    aiPets[1] = new Phoenix("AI Phoenix 1", "Healer");
    if (!aiPets[1]->loadTexture("Phoenix.png")) 
    {
        cerr << "Failed to load AI Phoenix texture" << endl;
    }
    aiPets[1]->setHealth(500);
    aiPets[1]->getSprite().setPosition(WINDOW_WIDTH - 300, WINDOW_HEIGHT - 200);
    aiPetCount = 2;

    // Initialize AI controllers
    battle->ResetAi();
    battle->InitilizeAi();

    gameState = BATTLE;
}

//* filing class
class GameManager 
{
    private:
        Player* player;
        TrainingCamps* camp;
        string saveFilePath = "Game.txt";
    
    public:

        GameManager(Player* p, TrainingCamps* c) : player(p), camp(c) {}
    
        Player* getPlayer() 
        {
            return player;
        }
    
        //* saving the game stats
        void saveGame() 
        {
            ofstream outFile(saveFilePath);
            if (!outFile) 
            {
                cerr << "Error: Could not open save file for writing!" << endl;
                return;
            }
    
            //* Save Player Data
            outFile << "PlayerName: " << player->getName() << endl;
            outFile << "Gold: " << player->getGold() << endl;
    
            //* Save Guild Data
            int petCount = player->getGuild().getPetCount();
            outFile << "PetCount: " << petCount << endl;
    
            //* Save Pet Data
            for (int i = 0; i < petCount; i++) 
            {
                Pet* pet = player->getGuild().getPets()[i];
                string petType = pet->getType();
    
                outFile << "Pet " << i << endl;
                outFile << "Type: " << petType << endl;
                outFile << "Name: " << pet->getname() << endl;
                outFile << "Role: " << pet->getRole() << endl;
                outFile << "Health: " << pet->getHealth() << endl;
                outFile << "AttackPower: " << pet->getAttackPower() << endl;
                outFile << "Speed: " << pet->getSpeed() << endl;
                outFile << "Defense: " << pet->getDefense() << endl;
                outFile << "Level: " << pet->getLevel() << endl;
                outFile << "XP: " << pet->getXP() << endl;
                outFile << "XPToNextLevel: " << pet->getXPToNextLevel() << endl;
            }
    
                // Save Inventory Data
    Inventory& inventory = player->getInventory();
    int itemCount = inventory.getItemCount();
    outFile << "ItemCount: " << itemCount << endl;

    for (int i = 0; i < itemCount; i++) 
    {
        Item* item = inventory.getItems()[i];
        outFile << "Item " << i << endl;
        outFile << "Type: " << item->getType() << endl;
        outFile << "Name: " << item->getName() << endl;
        outFile << "Price: " << item->getPrice() << endl;
        outFile << "Description: " << item->getDescription() << endl;
    }

    // Save Training Progress
    outFile << "TrainingPoints: " << camp->getTrainingPoints() << endl;

    outFile.close();
}

bool loadGame() 
{
    ifstream inFile(saveFilePath);
    if (!inFile) 
    {
        cerr << "Error: Could not open save file for reading!" << endl;
        return false;
    }

    string line, playerName;
    int gold = 0, petCount = 0, trainingPoints = 0;

    auto safeStoi = [](const string& str, int& val, const string& field = "") 
    {
        try 
        {
            val = stoi(str);
            return true;
        } 
        catch (...) 
        {
            if (!field.empty()) cerr << "Invalid " << field << " value in save file.\n";
            return false;
        }
    };

    auto safeStof = [](const string& str, float& val, const string& field = "") 
    {
        try 
        {
            val = stof(str);
            return true;
        } 
        catch (...) 
        {
            if (!field.empty()) cerr << "Invalid " << field << " value in save file.\n";
            return false;
        }
    };

    // Read player name
    if (!getline(inFile, line) || line.find("PlayerName:") != 0) 
    {
        cerr << "Error: Failed to read PlayerName!\n";
        inFile.close();
        return false;
    }
    playerName = line.substr(12);

    // Read gold
    if (!getline(inFile, line) || line.find("Gold:") != 0) 
    {
        cerr << "Error: Failed to read Gold!\n";
        inFile.close();
        return false;
    }
    if (!safeStoi(line.substr(6), gold, "gold")) gold = 0;

    // Read pet count
    if (!getline(inFile, line) || line.find("PetCount:") != 0) 
    {
        cerr << "Error: Failed to read PetCount! " << endl;
        inFile.close();
        return false;
    }
    if (!safeStoi(line.substr(10), petCount, "pet count")) petCount = 0;

    // Setup new player
    delete player;
    player = new Player(playerName);
    player->addGold(gold);
    setPlayer(player);

    Guild& guild = player->getGuild();
    guild.reset();

    for (int i = 0; i < petCount; ++i) 
    {
        // Read pet section header
        if (!getline(inFile, line) || line != "Pet " + to_string(i)) 
        {
            cerr << "Error: Expected 'Pet " << i << "' but got '" << line << "'\n";
            inFile.close();
            return false;
        }

        string type, name, role;
        int health = 0, attack = 0, defense = 0, level = 1, xp = 0, xpNext = 100;
        float speed = 0.0f;

        // Read pet attributes until next section
        bool Section = false;
        while (getline(inFile, line)) 
        {
            // Check for next section (Pet or ItemCount)
            if (line.find("Pet ") == 0 || line.find("ItemCount:") == 0) 
            {
                Section = true;
                break; // Leave line for next iteration or inventory
            }
            if (line.empty()) continue; // Skip empty lines, if any

            string value = line.substr(line.find(":") + 2);
            if (line.find("Type:") == 0) type = value;
            else if (line.find("Name:") == 0) name = value;
            else if (line.find("Role:") == 0) role = value;
            else if (line.find("Health:") == 0) safeStoi(value, health, "health");
            else if (line.find("AttackPower:") == 0) safeStoi(value, attack, "attack power");
            else if (line.find("Speed:") == 0) safeStof(value, speed, "speed");
            else if (line.find("Defense:") == 0) safeStoi(value, defense, "defense");
            else if (line.find("Level:") == 0) safeStoi(value, level, "level");
            else if (line.find("XP:") == 0) safeStoi(value, xp, "XP");
            else if (line.find("XPToNextLevel:") == 0) safeStoi(value, xpNext, "XPToNextLevel");
        }

        if (type.empty() || name.empty() || role.empty()) 
        {
            cerr << "Error: Missing required pet attributes for pet " << i << "!\n";
            continue;
        }

        Pet* pet = nullptr;
        if (type == "Dragon") pet = new Dragon(name, role);
        else if (type == "Phoenix") pet = new Phoenix(name, role);
        else if (type == "Griffin") pet = new Griffin(name, role);
        else if (type == "Unicorn") pet = new Unicorn(name, role);

        if (!pet) 
        {
            cerr << "Unknown pet type: " << type << endl;
            continue;
        }

        pet->setHealth(health);
        pet->setAttackPower(attack);
        pet->setSpeed(speed);
        pet->setDefense(defense);
        pet->setLevel(level);
        pet->setXP(xp);
        pet->setXPToNextLevel(xpNext);
        pet->loadTexture(type + ".png");

        guild.Addpet(pet);

        //* If we hit a section boundary, push back the line for the next iteration
        if (Section && i + 1 < petCount) 
        {
            //* The line is already read (Pet i+1), so we need to ensure it's processed in the next iteration
            inFile.seekg(-static_cast<std::streamoff>(line.length() + 1), ios::cur); 
        }
    }

    //* Read Inventory Data
    int itemCount = 0;

    if (line.find("ItemCount:") != 0) 
    {
        if (!getline(inFile, line) || line.find("ItemCount:") != 0) 
        {
            cerr << "Error: Failed to read ItemCount! " << endl;
            inFile.close();
            return false;
        }
    }

    if (!safeStoi(line.substr(11), itemCount, "item count")) 
    itemCount = 0;

    Inventory& inventory = player->getInventory();
    inventory.clear();

    for (int i = 0; i < itemCount; ++i) 
    {
        //* Read item header
        if (!getline(inFile, line) || line != "Item " + to_string(i)) 
        {
            cerr << "Error: Expected 'Item " << i << "' but got '" << line << "'\n";
            inFile.close();
            return false;
        }

        string itemType, itemName, description;
        int price = 0;

        // Read next 4 lines for item details
        for (int j = 0; j < 4; ++j) 
        {
            if (!getline(inFile, line)) 
            {
                cerr << "Error: Unexpected end of file while reading item " << i << "\n";
                inFile.close();
                return false;
            }

            string value = line.substr(line.find(":") + 2);
            if (line.find("Type:") == 0) itemType = value;
            else if (line.find("Name:") == 0) itemName = value;
            else if (line.find("Price:") == 0) safeStoi(value, price, "price");
            else if (line.find("Description:") == 0) description = value;
        }

        //* Create item
        Item* item = nullptr;
        if (itemType == "HealingPotion") item = new HealingPotion();
        else if (itemType == "ManaPotion") item = new ManaPotion();
        else if (itemType == "BuffItem") item = new BuffItem();
        else if (itemType == "ShieldItem") item = new ShieldItem();

        if (!item) 
        {
            cerr << "Unknown item type: " << itemType << endl;
            continue;
        }

        player->spendGold(item->getPrice());
        inventory.addItem(item);
    }

    //* Read TrainingPoints
    if (line.find("TrainingPoints:") != 0) 
    {
        if (!getline(inFile, line) || line.find("TrainingPoints:") != 0) 
        {
            cerr << "Error: Failed to read TrainingPoints!\n";
            inFile.close();
            return false;
        }
    }
    if (!safeStoi(line.substr(15), trainingPoints, "training points")) trainingPoints = 0;
    camp->setTrainingPoints(trainingPoints);

    inFile.close();
    return true;
}

    bool hasSavedGame() 
    {
        ifstream inFile(saveFilePath);
        if (!inFile) 
        return false;

        string line;
        bool valid = getline(inFile, line) && line.substr(0, 11) == "PlayerName:";
        inFile.close();
        return valid;
    }

    void autoSave() 
    {
    saveGame();
    }

    void setPlayer(Player* p) 
    { 
    player = p; 
    }

    void setTrainingCamps(TrainingCamps* c) 
    { 
    camp = c; 
    }

};

class Game
{
    private:
    // Member variables (all variables from main)
    RenderWindow window;
    SoundBuffer startupSoundBuffer;
    Sound startupSound;
    Shop shop;
    Texture magicalPetKingdomTexture, villageBackgroundTexture, guildBackgroundTexture, miniGameBackgroundTexture, battleBackgroundTexture, frontImageTexture;
    Sprite magicalPetKingdomSprite, villageBackgroundSprite, guildBackgroundSprite, miniGameBackgroundSprite, battleBackgroundSprite, frontImageSprite;
    SoundBuffer buttonClickBuffer;
    Sound buttonClickSound;
    float buttonSoundCooldown;
    const float BUTTON_SOUND_COOLDOWN_DURATION = 0.1f;
    Clock frameClock, errorTimer, miniGameEndedTimer;
    float errorDuration = 5.0f;
    float errorTimeElapsed = 0.0f;
    bool showMiniGameEndedMessage = false;
    float miniGameEndedMessageDuration = 2.0f;
    int state = START_MENU;
    string  currentInput , errorMessage, playerName;
    int petNum = 0, currentPet = 0, petChoice = 0, roleChoice = 0;
    string petName;
    Pet* selectedPet = nullptr;
    Player* player = nullptr;
    TrainingCamps* camp;
    Button buttons[MAX_BUTTONS];
    Button submitButton;
    Button postMenuButtons[3];
    int buttonCount = 0;
    float petMoveSpeed = 300.0f;
    int selectedPetIndex = -1;
    float scrollOffset = 0.0f;
    int selectedDifficulty = 1;
    Text inputText;
    RectangleShape inputBox;
    Clock clickTimer;
    bool lastClickWasLeft = false;
    float doubleClickThreshold = 0.3f;
    int clickedIndex = -1;
    int doubleClickedIndex = -1;
    bool levelUpTriggered = false;
    Pet* petToHeal = nullptr;
    string healAmountInput;
    Arena* arena = nullptr;
    Battle* battle = nullptr;
    GameManager gameManager;
    int selectedTrainingType = 1;
    int selectedPet1; // Index of the first selected pet for 2v2
    int selectedPet2; // Index of the second selected pet for 2v2
    SoundBuffer transitionBuffer; 
    Sound transitionSound;      
    SoundBuffer titleStingBuffer; 
    Sound titleStingSound;       
    float transitionSoundCooldown; 
    static constexpr float SOUND_COOLDOWN_DURATION = 0.2f; // Cooldown duration
    Button battleButtons[6];

    public:

    //*Constructor
    Game(Player* initialPlayer, TrainingCamps* initialCamp)
    : window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Pet Guild Manager"),
      player(initialPlayer), camp(initialCamp),
      buttonSoundCooldown(0.0f), gameManager(initialPlayer, initialCamp) 
    {
    
    //* Window setup
    window.setFramerateLimit(60);

    // Play sound when window opens
    if (!startupSoundBuffer.loadFromFile("startup_sound.wav")) 
    {
        cerr << "Failed to load startup_sound.wav" << endl;
    }
    startupSound.setBuffer(startupSoundBuffer);
    startupSound.play();

    // Load font
    if (!globalFont.loadFromFile("arial.ttf")) 
    {
        cerr << "Failed to load font!" << endl;
        exit(-1);
    }

    // Load shop resources
    string shopBackgroundFile = "shop_background.png";
    string flaskFilenames[4] = {"FireEssence.png", "MoonDew.png", "ThunderElixir.png", "StarShield.png"};
    shop.loadShopResources(shopBackgroundFile, flaskFilenames);

    // Load textures and sprites
    if (!magicalPetKingdomTexture.loadFromFile("magical_pet_kingdom.png")) 
    {
        cerr << "Failed to load magical_pet_kingdom.png! Using fallback." << endl;
        magicalPetKingdomSprite.setColor(Color(30, 30, 40));
        magicalPetKingdomSprite.setScale(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT));
    } else 
    {
        magicalPetKingdomSprite.setTexture(magicalPetKingdomTexture);
        magicalPetKingdomSprite.setScale(
            static_cast<float>(WINDOW_WIDTH) / magicalPetKingdomSprite.getLocalBounds().width,
            static_cast<float>(WINDOW_HEIGHT) / magicalPetKingdomSprite.getLocalBounds().height
        );
    }

    if (!villageBackgroundTexture.loadFromFile("village_background.png")) 
    {
        cerr << "Failed to load village_background.png! Using fallback." << endl;
        villageBackgroundSprite.setColor(Color(50, 50, 50));
        villageBackgroundSprite.setScale(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT));
    } else 
    {
        villageBackgroundSprite.setTexture(villageBackgroundTexture);
        villageBackgroundSprite.setScale(
            static_cast<float>(WINDOW_WIDTH) / villageBackgroundSprite.getLocalBounds().width,
            static_cast<float>(WINDOW_HEIGHT) / villageBackgroundSprite.getLocalBounds().height
        );
    }

    if (!guildBackgroundTexture.loadFromFile("guild_background.png")) 
    {
        cerr << "Failed to load guild_background.png! Using fallback." << endl;
        guildBackgroundSprite.setColor(Color(40, 40, 40));
        guildBackgroundSprite.setScale(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT));
    } else 
    {
        guildBackgroundSprite.setTexture(guildBackgroundTexture);
        guildBackgroundSprite.setScale(
            static_cast<float>(WINDOW_WIDTH) / guildBackgroundSprite.getLocalBounds().width,
            static_cast<float>(WINDOW_HEIGHT) / guildBackgroundSprite.getLocalBounds().height
        );
    }

    if (!miniGameBackgroundTexture.loadFromFile("mini_game_background.png")) 
    {
        cerr << "Failed to load mini_game_background.png! Using fallback." << endl;
        miniGameBackgroundSprite.setColor(Color(20, 20, 60));
        miniGameBackgroundSprite.setScale(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT));
    } else 
    {
        miniGameBackgroundSprite.setTexture(miniGameBackgroundTexture);
        miniGameBackgroundSprite.setScale(
            static_cast<float>(WINDOW_WIDTH) / miniGameBackgroundSprite.getLocalBounds().width,
            static_cast<float>(WINDOW_HEIGHT) / miniGameBackgroundSprite.getLocalBounds().height
        );
    }

    if (!battleBackgroundTexture.loadFromFile("battle_background.png")) 
    {
        cerr << "Failed to load battle_background.png! Using fallback." << endl;
        battleBackgroundSprite.setColor(Color(20, 40, 20));
        battleBackgroundSprite.setScale(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT));
    } else 
    {
        battleBackgroundSprite.setTexture(battleBackgroundTexture);
        battleBackgroundSprite.setScale(
            static_cast<float>(WINDOW_WIDTH) / battleBackgroundSprite.getLocalBounds().width,
            static_cast<float>(WINDOW_HEIGHT) / battleBackgroundSprite.getLocalBounds().height
        );
    }

    if (!frontImageTexture.loadFromFile("Front_Image.png")) 
    {
        cerr << "Failed to load Front_Image.png! Using fallback." << endl;
        frontImageSprite.setColor(Color(30, 30, 40));
        frontImageSprite.setScale(static_cast<float>(WINDOW_WIDTH), static_cast<float>(WINDOW_HEIGHT));
    } else 
    {
        frontImageSprite.setTexture(frontImageTexture);
        frontImageSprite.setScale(
            static_cast<float>(WINDOW_WIDTH) / frontImageSprite.getLocalBounds().width,
            static_cast<float>(WINDOW_HEIGHT) / frontImageSprite.getLocalBounds().height
        );
    }

    // Load button click sound
    if (!buttonClickBuffer.loadFromFile("button_click.wav")) 
    {
        cerr << "Failed to load button_click.wav" << endl;
    }
    buttonClickSound.setBuffer(buttonClickBuffer);
    
    if (!transitionBuffer.loadFromFile("transition.wav")) 
    {
        cerr << "Failed to load transition.wav" << endl;
    }
    transitionSound.setBuffer(transitionBuffer);
    if (!titleStingBuffer.loadFromFile("title_sting.wav")) 
    {
        cerr << "Failed to load title_sting.wav" << endl;
    }
    titleStingSound.setBuffer(titleStingBuffer);



    //* Initialize text and input box
    inputText.setFont(globalFont);
    inputText.setCharacterSize(32);
    inputText.setFillColor(Color::White);

    inputBox.setSize(Vector2f(600, 50));
    inputBox.setFillColor(Color(50, 50, 50, 200));
    inputBox.setOutlineColor(Color::White);
    inputBox.setOutlineThickness(2);
    inputBox.setPosition(WINDOW_WIDTH / 2 - 300, WINDOW_HEIGHT / 2);

    //* et GameManager pointers
    gameManager.setPlayer(player);
    gameManager.setTrainingCamps(camp);

}

void playTransitionSound() 
{
    if (transitionSoundCooldown <= 0.0f) 
    {
        transitionSound.play();
        transitionSoundCooldown = SOUND_COOLDOWN_DURATION;
    }
}

void playTitleStingSound() 
{
    titleStingSound.play();
}

//*Destructor
    ~Game()
    {
        delete player;
        delete arena;
        delete battle;
        player = nullptr;
        arena = nullptr;
        battle = nullptr;
    }

    void run()
    {
        while(window.isOpen())
        {
        float deltaTime = frameClock.restart().asSeconds();
        Vector2i mousePos = Mouse::getPosition(window);
        clickedIndex = -1;
        doubleClickedIndex = -1;
        levelUpTriggered = false;

        // Update button sound cooldown
        buttonSoundCooldown -= deltaTime;

        if (showMiniGameEndedMessage) 
        {
            if (miniGameEndedTimer.getElapsedTime().asSeconds() >= miniGameEndedMessageDuration) 
            {
                showMiniGameEndedMessage = false;
                errorMessage.clear();
            }
        }

        handleEvents();
        update(deltaTime, mousePos);
        render();
        }
    }
       
    void handleEvents()
    {
        Event event;

        while (window.pollEvent(event)) 
        {
            if (event.type == Event::Closed) 
            {
                window.close();

            } else if (event.type == Event::TextEntered && (state == INPUT_NAME || state == INPUT_PET_NUM || state == INPUT_PET_NAME || state == INPUT_HEAL_AMOUNT)) 
            {
                if (event.text.unicode == '\b' && currentInput.length()> 0) 
                {
                    //!Need to change
                    playTransitionSound();
                    currentInput = currentInput.substr(0, currentInput.length() - 1);

                } else if (event.text.unicode == '\r' && currentInput.length() > 0)
                {
                    if (state == INPUT_NAME) 
                    {
                        playerName = string(currentInput);
                        player = new Player(playerName);
                        gameManager.setPlayer(player);
                        state = POST_NAME_MENU;
                        playTransitionSound();
                        currentInput = "";

                    } else if (state == INPUT_PET_NUM) 
                    {
                        try {
                            petNum = stoi(currentInput);
                            if (petNum > 0) 
                            {
                                state = CHOOSE_PET;
                                playTransitionSound();
                                errorMessage = "";
                            } else 
                            {
                                errorMessage = "Enter a positive number!";
                                errorTimer.restart();
                            }

                            currentInput = "";
                        } catch (...) 
                        {
                            errorMessage = "Invalid number!";
                            errorTimer.restart();
                            errorTimeElapsed = 0.0f;
                            currentInput = "";
                        }

                    } else if (state == INPUT_PET_NAME) 
                    {
                        petName = currentInput;
                        
                        state = CHOOSE_ROLE;
                        playTransitionSound();
                        currentInput = "";

                    } else if (state == INPUT_HEAL_AMOUNT) 
                    {
                        try 
                        {
                            int healAmount = stoi(currentInput);
                            if (healAmount > 0) 
                            {
                                int goldCost = healAmount / 10;
                                if (player->getGold() >= goldCost) 
                                {
                                    player->spendGold(goldCost);
                                    petToHeal->setHealth(petToHeal->getHealth() + healAmount);
                                    errorMessage = petToHeal->getname() + " healed for " + to_string(healAmount) + " HP! Cost: " + to_string(goldCost) + " gold.";
                                    gameManager.autoSave();
                                } else 
                                {
                                    errorMessage = "Not enough gold! Need " + to_string(goldCost) + " gold.";
                                }
                            } else 
                            {
                                errorMessage = "Enter a positive number!";
                            }
                        } catch (...) 
                        {
                            errorMessage = "Invalid number!";
                        }
                       
                        state = HEAL_PET;
                        playTransitionSound();
                        currentInput = "";
                        petToHeal = nullptr;
                    }

                } else if (event.text.unicode < 128 && event.text.unicode != '\r') 
                {
                    char inputChar = static_cast<char>(event.text.unicode);
                    if (state == INPUT_PET_NUM && !isdigit(inputChar)) 
                    {
                        errorMessage = "Invalid number!";
                        errorTimer.restart();
                        currentInput = "";
                    } else 
                    {
                        currentInput += inputChar;
                    }
                }
                inputText.setString(currentInput);

            } else if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) 
            {
                float timeSinceLastClick = clickTimer.getElapsedTime().asSeconds();
                bool isDoubleClick = lastClickWasLeft && timeSinceLastClick <= doubleClickThreshold;
                lastClickWasLeft = true;
                clickTimer.restart();
    
                //* Play button click sound if cooldown allows
                auto playButtonSound = [&]() 
                {
                    if (buttonSoundCooldown <= 0) 
                    {
                        buttonClickSound.play();
                        buttonSoundCooldown = BUTTON_SOUND_COOLDOWN_DURATION;
                    }
                };
    
                if (state == START_MENU) 
                {
                    for (int i = 0; i < buttonCount; i++) 
                    {
                        if (buttons[i].isClicked(Mouse::getPosition(window))) 
                        {
                            playButtonSound();
                            if (i == 0) 
                            {
                                state = INPUT_NAME;
                                playTransitionSound();
                                buttonCount = 0;
                                errorMessage.clear();

                            } else if (i == 1) 
                            {
                                if (gameManager.hasSavedGame()) 
                                {
                                    if (arena) 
                                    {
                                        delete arena;
                                        arena = nullptr;
                                    }
                                    if (battle) 
                                    {
                                        delete battle;
                                        battle = nullptr;
                                    }
                                    if (gameManager.loadGame()) 
                                    {
                                        state = MAIN_MENU;
                                        playTransitionSound();
                                        buttonCount = 0;
                                        errorMessage = "Game loaded successfully!";
                                        player = gameManager.getPlayer();
                                        arena = new Arena(player);
                                        battle = new Battle(arena);
                                    } else 
                                    {
                                        errorMessage = "Failed to load game!";
                                    }
                                } else 
                                {
                                    errorMessage = "No saved game found!";
                                }
                            } else if (i == 2) 
                            {
                                window.close();
                            }
                            break;
                        }
                    }

                } else if (state == POST_NAME_MENU && event.type == Event::MouseButtonPressed) 
                {
                    for (int i = 0; i < 3; ++i) 
                    {
                        if (postMenuButtons[i].isClicked(Mouse::getPosition(window))) 
                        {
                            playButtonSound();
                            if (i == 0) 
                            {
                                state = INPUT_PET_NUM;
                                playTransitionSound();
                            } else if (i == 1) 
                            {
                                state = MAIN_MENU;
                                playTransitionSound();
                            } else if (i == 2) 
                            {
                                window.close();
                            }
                        }
                    }

                } else if (state == INPUT_NAME || state == INPUT_PET_NUM || state == INPUT_PET_NAME || state == INPUT_HEAL_AMOUNT) 
                {
                    if (event.type == Event::TextEntered) 
                    {
                        if (event.text.unicode == '\r') //* Enter key
                        {
                            try {
                                if (state == INPUT_PET_NAME) 
                                {
                                    if (currentInput.empty()) 
                                    {
                                        errorMessage = "Pet name cannot be empty!";
                                        errorTimer.restart();
                                        playTransitionSound();
                                    }
                                    else if (currentInput.length() > 50) {
                                        errorMessage = "Pet name too long!";
                                        errorTimer.restart();
                                        playTransitionSound();
                                    }
                                    else 
                                    {
                                        petName = currentInput;
                                        state = CHOOSE_ROLE;
                                        currentInput = "";
                                        playTransitionSound();
                                        buttonCount = 3;
                                        buttons[0].set("Dragon", globalFont, WINDOW_WIDTH / 2 - 310, WINDOW_HEIGHT / 2 - 110, 200, 50, 0);
                                        buttons[1].set("Phoenix", globalFont, WINDOW_WIDTH / 2 - 310, WINDOW_HEIGHT / 2 - 10, 200, 50, 1);
                                        buttons[2].set("Cancel", globalFont, WINDOW_WIDTH / 2 - 310, WINDOW_HEIGHT / 2 + 90, 200, 50, 2);
                                    }
                                }
                                else if (state == INPUT_HEAL_AMOUNT) 
                                {
                                    if (!petToHeal) 
                                    {
                                        errorMessage = "Error: No pet selected!";
                                        state = HEAL_PET;
                                        currentInput = "";
                                        playTransitionSound();
                                        continue;
                                    }
                                    int healAmount = 0;
                                    try 
                                    {
                                        healAmount = stoi(currentInput);
                                    }
                                    catch (...) 
                                    {
                                        errorMessage = "Invalid input! Enter a number.";
                                        errorTimer.restart();
                                        playTransitionSound();
                                        currentInput = "";
                                        continue;
                                    }
                                    int goldCost = healAmount;
                                    if (healAmount <= 0) 
                                    {
                                        errorMessage = "Heal amount must be positive!";
                                        errorTimer.restart();
                                        playTransitionSound();
                                    }
                                    else if (player->getGold() < goldCost) 
                                    {
                                        errorMessage = "Not enough gold! Need " + to_string(goldCost) + " gold.";
                                        errorTimer.restart();
                                        playTransitionSound();
                                    }
                                    else if (petToHeal->getHealth() + healAmount > petToHeal->getHealth()) 
                                    {
                                        errorMessage = "Cannot heal beyond max health!";
                                        errorTimer.restart();
                                        playTransitionSound();
                                    }
                                    else 
                                    {
                                        petToHeal->setHealth(healAmount);
                                        player->spendGold(goldCost);
                                        if (errorMessage.length() > 100) errorMessage = "Heal message too long!";
                                        errorTimer.restart();
                                        playTransitionSound();
                                        state = HEAL_PET;
                                        currentInput = "";
                                        gameManager.autoSave();
                                    }
                                }
                                
                            }
                            catch (const std::length_error& e) 
                            {
                                cout << "length_error in input handling: " << e.what() << endl;
                                errorMessage = "Error: Invalid string length!";
                                currentInput = "";
                                state = MAIN_MENU;
                            }
                        }
                        else if (event.text.unicode == '\b' && !currentInput.empty()) //* Backspace
                        {
                            currentInput.pop_back();
                        }
                        else if (event.text.unicode < 128 && event.text.unicode != '\r' && currentInput.length() < 50) 
                        {
                            currentInput += static_cast<char>(event.text.unicode);
                        }
                    }
                } else if (state == DISPLAY_GUILD) 
                {
                    const float cardHeight = 280.0f;
                    const float spacing = 40.0f;
                    float totalHeight = player->getGuild().getPetCount() * cardHeight + (player->getGuild().getPetCount() - 1) * spacing;
                    float startY = (WINDOW_HEIGHT - totalHeight) / 2 + scrollOffset;
                    
                    for (int i = 0; i < player->getGuild().getPetCount(); i++) 
                    {
                        float cardY = startY + i * (cardHeight + spacing);

                        FloatRect cardBounds(WINDOW_WIDTH / 2 - 550, cardY, 1100, cardHeight);

                        if (cardBounds.contains(static_cast<float>(Mouse::getPosition(window).x), static_cast<float>(Mouse::getPosition(window).y))) 
                        {
                            clickedIndex = i;
                            if (isDoubleClick) 
                            doubleClickedIndex = i;
                            break;
                        }
                    }

                    if (buttons[0].isClicked(Mouse::getPosition(window))) 
                    {
                        playButtonSound();
                        state = MAIN_MENU;
                        playTransitionSound();
                        buttonCount = 0;
                        errorMessage = "";
                        camp->endMiniGame();
                        selectedPetIndex = -1;
                        scrollOffset = 0.0f;

                    } else if (buttons[1].isClicked(Mouse::getPosition(window))) 
                    {
                        playButtonSound();
                        if (selectedPetIndex != -1) 
                        {
                            selectedPet = player->getGuild().getPets()[selectedPetIndex];
                            state = TRAINING_MENU;
                            playTransitionSound();
                            buttonCount = 0;
                            errorMessage = "";
                            scrollOffset = 0.0f;
                        } else 
                        {
                            errorMessage = "Select a pet first!";
                        }

                    } else if (buttons[2].isClicked(Mouse::getPosition(window))) 
                    {
                        playButtonSound();
                        if (selectedPetIndex != -1) 
                        {
                            if (player->getGold() >= 50) 
                            {
                                player->spendGold(50);
                                Pet* pet = player->getGuild().getPets()[selectedPetIndex];
                                pet->levelUp(camp);
                                levelUpTriggered = true;
                                errorMessage = pet->getname() + " leveled up to " + to_string(pet->getLevel()) + "!";
                                gameManager.autoSave();
                            } else 
                            {
                                errorMessage = "Not enough gold! Need 50 gold to level up.";
                            }
                        } else 
                        {
                            errorMessage = "Select a pet first!";
                        }
                    }

                } else if (state == TRAIN_PET) 
                {
                    for (int i = 0; i < buttonCount; i++) 
                    {
                        if (buttons[i].isClicked(Mouse::getPosition(window))) 
                        {
                            playButtonSound();
                            selectedPet = player->getGuild().getPets()[i];
                            state = TRAINING_MENU;
                            playTransitionSound();
                            buttonCount = 0;
                            errorMessage.clear();
                            break;
                        }
                    }
                } else if (state == HEAL_PET) 
                {
                    for (int i = 0; i < buttonCount; i++) 
                    {
                        if (buttons[i].isClicked(Mouse::getPosition(window))) 
                        {
                            playButtonSound();
                            int healerIndex = 0;
                            for (int j = 0; j < player->getGuild().getPetCount(); j++) 
                            {
                                Pet* pet = player->getGuild().getPets()[j];
                                if (pet && pet->getname() == "Phoenix" && pet->getRole() == "Healer") 
                                {
                                    if (healerIndex == i) 
                                    {
                                        petToHeal = pet;
                                        break;
                                    }
                                    healerIndex++;
                                }
                            }
                            state = SELECT_HEAL_TARGET;
                            playTransitionSound();
                            buttonCount = 0;
                            break;
                        }
                    }

                } else if (state == SELECT_HEAL_TARGET) 
                {
                    if (buttons[0].isClicked(Mouse::getPosition(window))) 
                    {
                        playButtonSound();
                        state = INPUT_HEAL_AMOUNT;
                        playTransitionSound();
                        currentInput.clear();
                        buttonCount = 0;

                    } else if (buttons[1].isClicked(Mouse::getPosition(window))) 
                    {
                        playButtonSound();
                        state = HEAL_PET;
                        playTransitionSound();
                        petToHeal = nullptr;
                        buttonCount = 0;
                    }

                } else if (state == CHOOSE_PET) 
                {
                    for (int i = 0; i < buttonCount; i++) 
                    {
                        if (buttons[i].isClicked(Mouse::getPosition(window))) 
                        {
                            playButtonSound();
                            petChoice = i + 1;
                            state = INPUT_PET_NAME;
                            playTransitionSound();
                            currentInput.clear();
                            break;
                        }
                    }

                } else if (state == CHOOSE_ROLE) 
                {
                    for (int i = 0; i < buttonCount; i++) 
                    {
                        if (buttons[i].isClicked(Mouse::getPosition(window))) 
                        {
                            playButtonSound();
                            roleChoice = (petChoice == 2) ? i + 1 : i + 2;
                            string role = (roleChoice == 1 ? "Healer" : roleChoice == 2 ? "Tanker" : "DamageDealer");
                            Pet* pet = nullptr;
                            string texturePath;
                            switch (petChoice) 
                            {
                                case 1: pet = new Dragon(petName, role); texturePath = "Dragon.png"; break;
                                case 2: pet = new Phoenix(petName, role); texturePath = "Phoenix.png"; break;
                                case 3: pet = new Unicorn(petName, role); texturePath = "Unicorn.png"; break;
                                case 4: pet = new Griffin(petName, role); texturePath = "Griffin.png"; break;
                            }

                            if (pet && !pet->loadTexture(texturePath)) 
                            {
                                errorMessage = "Failed to load texture for " + petName;
                            }
                            if (pet) 
                            {
                                player->getGuild().Addpet(pet);
                                currentPet++;
                            }
                            state = (currentPet < petNum) ? CHOOSE_PET : MAIN_MENU;
                            buttonCount = 0;
                            gameManager.autoSave();
                            break;
                        }
                    }

                } else if (state == CHOOSE_DIFFICULTY) 
                {
                    for (int i = 0; i < buttonCount; i++) 
                    {
                        if (buttons[i].isClicked(Mouse::getPosition(window))) 
                        {
                            playButtonSound();
                            selectedDifficulty = i + 1;
                            camp->startTraining(selectedTrainingType, selectedDifficulty, selectedPet);
                            state = TRAINING_MINIGAME;
                            playTransitionSound();
                            if (selectedPet) 
                            {
                                FloatRect bounds = selectedPet->getSprite().getLocalBounds();
                                selectedPet->getSprite().setScale(1.5f, 1.5f);
                                selectedPet->getSprite().setPosition( (WINDOW_WIDTH - bounds.width * 1.5f) / 2, WINDOW_HEIGHT - bounds.height * 1.5f - 20);
                            }
                            buttonCount = 0;
                            break;
                        }
                    }

                } else if (state == TRAINING_MENU) 
                {
                    for (int i = 0; i < buttonCount; i++) 
                    {
                        if (buttons[i].isClicked(Mouse::getPosition(window))) 
                        {
                            playButtonSound();
                            if (i == 3) 
                            {
                                camp->showAbilityMenu(selectedPet);
                            } else 
                            {
                                selectedTrainingType = i + 1;
                                state = CHOOSE_DIFFICULTY;
                                playTransitionSound();
                                buttonCount = 0;
                                selectedDifficulty = 1;
                            }
                            break;
                        }
                    }
                } else if (state == TRAINING_MINIGAME) 
                {
                    if (buttons[0].isClicked(Mouse::getPosition(window))) 
                    {
                        camp->awardTrainingPoints();
                        playButtonSound();
                        camp->endMiniGame();
                        state = TRAINING_MENU;
                        playTransitionSound();
                        if (selectedPet) 
                        selectedPet->getSprite().setScale(0.5f, 0.5f);

                        errorMessage = "Mini-game ended!";
                        showMiniGameEndedMessage = true;
                        miniGameEndedTimer.restart();
                        gameManager.autoSave();
                    }
                } else if (state == SHOP_MENU) 
                {
                    for (int i = 0; i < 4; i++) 
                    {
                        if (shop.getFlaskSprite(i).getGlobalBounds().contains(static_cast<float>(Mouse::getPosition(window).x), static_cast<float>(Mouse::getPosition(window).y))) 
                        {
                            playButtonSound();
                            if (shop.buyItem(i, player)) 
                            {
                                errorMessage = "Purchased " + shop.getItems()[i]->getName() + "!";
                                gameManager.autoSave();
                            } else 
                            {
                                errorMessage = shop.getMessage();
                            }
                            break;
                        }
                    }

                } else if (state == INVENTORY_MENU) 
                {
                    player->getInventory().displayInventory(window);

                } else if (state == ARENA_MENU) 
                {
                    for (int i = 0; i < buttonCount; i++) 
                    {
                        if (buttons[i].isClicked(Mouse::getPosition(window))) 
                        {
                            playButtonSound();
                            switch (i) 
                            {
                                case 0: // 1 vs 1
                                if (player->getGuild().getPetCount() == 0) 
                                {
                                    errorMessage = "No pets available to battle!";

                                } else if (player->getGuild().getPetCount() == 1) 
                                {
                                    if (!arena) arena = new Arena(player);
                                    if (!battle) battle = new Battle(arena);
                                    arena->setSelectedPet(player->getGuild().getPets()[0]);
                                    //* Call new 1v1 battle function
                                    arena->start1v1Battle(state);
                                    if (arena->isBattleActive()) 
                                    {
                                        battle->InitilizeAi();
                                        playTransitionSound();
                                        buttonCount = 0;
                                        errorMessage = "";
                                    }
                                } else 
                                {
                                    state = SELECT_PET_FOR_BATTLE;
                                    playTransitionSound();
                                    buttonCount = 0;
                                    errorMessage = "";
                                }
                                break;

                                case 1: // 2 vs 2
                                if (player->getGuild().getPetCount() < 2) 
                                {
                                    errorMessage = "Need at least 2 pets for 2 vs 2 battle!";
                                } else 
                                {
                                    if (!arena) arena = new Arena(player);
                                    if (!battle) battle = new Battle(arena);
                                    selectedPet1 = -1;
                                    selectedPet2 = -1;
                                    state = SELECT_PETS_FOR_2V2;
                                    playTransitionSound();
                                    buttonCount = 0;
                                    errorMessage = "";
                                }
                                break;

                                case 2: // Guild Fight
                                if (player->getGuild().getPetCount() == 0) 
                                {
                                    errorMessage = "No pets available for guild fight!";
                                } else 
                                {
                                    if (!arena) arena = new Arena(player);
                                    if (!battle) battle = new Battle(arena);
                                    // Call new guild battle function
                                    arena->startGuildBattle(state);
                                    if (arena->isBattleActive()) 
                                    {
                                        battle->InitilizeAi();
                                        playTransitionSound();
                                        buttonCount = 0;
                                        errorMessage = "";
                                    }
                                }
                                break;

                                case 3: // Show Records
                                    state = SHOW_RECORDS;
                                    playTransitionSound();
                                    buttonCount = 0;
                                    errorMessage = "";
                                    break;

                                case 4: // Back
                                    state = MAIN_MENU;
                                    playTransitionSound();
                                    buttonCount = 0;
                                    errorMessage = "";
                                    if (arena) 
                                    {
                                        delete arena;
                                        arena = nullptr;
                                    }
                                    if (battle) 
                                    {
                                        delete battle;
                                        battle = nullptr;
                                    }
                                    break;
                            }
                            break;
                        }
                    }
                } else if(state == SELECT_PET_FOR_BATTLE)
                { 
                    for (int i = 0; i < player->getGuild().getPetCount(); i++) 
                    {
                        FloatRect petBounds(WINDOW_WIDTH / 2 - 150, 150 + i * 120, 300, 100);
                        if (petBounds.contains(static_cast<float>(Mouse::getPosition(window).x), static_cast<float>(Mouse::getPosition(window).y))) 
                        {
                            playButtonSound();
                            if (!arena) arena = new Arena(player);
                            if (!battle) battle = new Battle(arena);
                            arena->setSelectedPet(player->getGuild().getPets()[i]);
                            arena->start1v1Battle(state);
                            if (arena->isBattleActive()) {
                                battle->InitilizeAi();
                                playTransitionSound();
                                buttonCount = 0;
                                errorMessage = "";
                            }
                            break;
                        }
                    }
                if (buttons[0].isClicked(Mouse::getPosition(window))) 
                {
                    playButtonSound();
                    state = ARENA_MENU;
                    playTransitionSound();
                    buttonCount = 0;
                    errorMessage = "";
                }


                }else if(state == SELECT_PETS_FOR_2V2)
                {
                    static int selectedPet1 = -1;
                static int selectedPet2 = -1;

                for (int i = 0; i < player->getGuild().getPetCount(); i++) 
                {
                    FloatRect petBounds(WINDOW_WIDTH / 2 - 150, 150 + i * 120, 300, 100);
                    if (petBounds.contains(static_cast<float>(Mouse::getPosition(window).x), static_cast<float>(Mouse::getPosition(window).y))) 
                    {
                        playButtonSound();
                        if (selectedPet1 == -1) 
                        {
                            selectedPet1 = i;
                        }
                        else if (selectedPet2 == -1 && i != selectedPet1) 
                        {
                            selectedPet2 = i;
                        }
                        break;
                    }
                }

                if (buttons[0].isClicked(Mouse::getPosition(window)) && selectedPet1 != -1 && selectedPet2 != -1) 
                {
                    playButtonSound();
                    if (!arena) arena = new Arena(player);
                    if (!battle) battle = new Battle(arena);
                    arena->setSelectedPet(player->getGuild().getPets()[selectedPet1]); 
                    arena->setSelectedPet2(player->getGuild().getPets()[selectedPet2]);
                        // Call new 2v2 battle function
                    arena->start2v2Battle(state , battle);
                    if (arena->isBattleActive()) 
                    {
                        battle->InitilizeAi();
                        playTransitionSound();
                        buttonCount = 0;
                        errorMessage = "";        
                    }
                    selectedPet1 = -1;
                    selectedPet2 = -1;
                }

                if (buttons[1].isClicked(Mouse::getPosition(window))) 
                {
                    playButtonSound();
                    state = ARENA_MENU;
                    playTransitionSound();
                    buttonCount = 0;
                    errorMessage = "";
                    selectedPet1 = -1;
                    selectedPet2 = -1;
                }

             } else if (state == SHOW_RECORDS) 
                {
                    for (int i = 0; i < buttonCount; i++) 
                    {
                        if (buttons[i].isClicked(Mouse::getPosition(window))) 
                        {
                            playButtonSound();
                            if (i == 0) // Back
                            {
                                state = ARENA_MENU;
                                playTransitionSound();
                                buttonCount = 0;
                                errorMessage = "";
                            }
                            break;
                        }
                    }

                } else if (state == BATTLE && arena && battle) 
                {
                    if (!arena || !battle) {
                        cout << "Arena or battle is null!" << endl;
                        errorMessage = "Error: Battle setup failed!";
                        state = ARENA_MENU;
                        playTransitionSound();
                        return;
                    }
                
                    try {
                        if (arena->getPlayerPetCount() == 1) 
                        { // 1v1 battle
                            Pet* pet = arena->getPlayerPet(0);
                            if (!pet) 
                            {
                                cout << "Pet is null in 1v1 battle!" << endl;
                                errorMessage = "Error: No pet available!";
                                state = ARENA_MENU;
                                return;
                            }
                            for (int i = 0; i < 3; i++) 
                            {
                                if (battleButtons[i].isClicked(Mouse::getPosition(window))) 
                                {
                                    if (battleButtons[i].isDisabled()) 
                                    {
                                        errorMessage = "Ability is locked!";
                                        if (errorMessage.length() > 100) errorMessage = "Error: Invalid length!";
                                        errorTimer.restart();
                                        playButtonSound();
                                        cout << "Attempted to use locked ability button " << i << endl;
                                    }
                                    else if (i == 0) 
                                    { // Attack
                                        cout << "Battle button " << i << " clicked: Attack" << endl;
                                        playButtonSound();
                                        battle->handleButtonAction(i, pet);
                                    }
                                    else if (i == 1 && pet->getAbility() && pet->getAbility()[0] && !pet->getAbility()[0]->isLocked()) 
                                    { // Ability 1
                                        cout << "Battle button " << i << " clicked: Ability 1" << endl;
                                        playButtonSound();
                                        battle->handleButtonAction(i, pet);
                                    }
                                    else if (i == 2 && pet->getAbility() && pet->getAbility()[1] && !pet->getAbility()[1]->isLocked()) 
                                    { // Ability 2
                                        cout << "Battle button " << i << " clicked: Ability 2" << endl;
                                        playButtonSound();
                                        battle->handleButtonAction(i, pet);
                                    }
                                    else 
                                    {
                                        errorMessage = "Ability is locked!";
                                        if (errorMessage.length() > 100) errorMessage = "Error: Invalid length!";
                                        errorTimer.restart();
                                        playButtonSound();
                                        cout << "Attempted to use locked ability button " << i << endl;
                                    }
                                    break;
                                }
                            }
                        }
                        else if (arena->getPlayerPetCount() == 2) 
                        { // 2v2 battle
                            for (int i = 0; i < 6; i++) 
                            {
                                if (battleButtons[i].isClicked(Mouse::getPosition(window))) 
                                {
                                    int petIndex = i / 3; // 0 for Pet 1, 1 for Pet 2
                                    int actionIndex = i % 3; // 0 for Attack, 1 for Ability 1, 2 for Ability 2
                                    Pet* pet = arena->getPlayerPet(petIndex);
                                    if (!pet) 
                                    {
                                        cout << "Pet " << (petIndex + 1) << " is null in 2v2 battle!" << endl;
                                        errorMessage = "Error: No pet available!";
                                        state = ARENA_MENU;
                                        return;
                                    }
                                    if (battleButtons[i].isDisabled()) 
                                    {
                                        errorMessage = "Ability is locked!";
                                        if (errorMessage.length() > 100) errorMessage = "Error: Invalid length!";
                                        errorTimer.restart();
                                        playButtonSound();
                                        cout << "Attempted to use locked ability button " << i << " for Pet " << (petIndex + 1) << endl;
                                    }
                                    else if (actionIndex == 0) 
                                    { // Attack
                                        cout << "Battle button " << i << " clicked for Pet " << (petIndex + 1) << ": Attack" << endl;
                                        playButtonSound();
                                        battle->handleButtonAction(actionIndex, pet);
                                    }
                                    else if (actionIndex == 1 && pet->getAbility() && pet->getAbility()[0] && !pet->getAbility()[0]->isLocked()) 
                                    {
                                         // Ability 1
                                        cout << "Battle button " << i << " clicked for Pet " << (petIndex + 1) << ": Ability 1" << endl;
                                        playButtonSound();
                                        battle->handleButtonAction(actionIndex, pet);
                                    }
                                    else if (actionIndex == 2 && pet->getAbility() && pet->getAbility()[1] && !pet->getAbility()[1]->isLocked()) 
                                    {
                                         // Ability 2
                                        cout << "Battle button " << i << " clicked for Pet " << (petIndex + 1) << ": Ability 2" << endl;
                                        playButtonSound();
                                        battle->handleButtonAction(actionIndex, pet);
                                    }
                                    else {
                                        errorMessage = "Ability is locked!";
                                        if (errorMessage.length() > 100) errorMessage = "Error: Invalid length!";
                                        errorTimer.restart();
                                        playButtonSound();
                                        cout << "Attempted to use locked ability button " << i << " for Pet " << (petIndex + 1) << endl;
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    catch (const std::length_error& e) 
                    {
                        cout << "length_error in BATTLE handleEvents: " << e.what() << endl;
                        errorMessage = "Error: Invalid string length!";
                        state = ARENA_MENU;
                        return;
                    }
                
                    if (buttons[0].isClicked(Mouse::getPosition(window))) 
                    {
                        playButtonSound();
                        arena->endBattle(state, errorMessage, battle);
                        buttonCount = 0;
                        gameManager.autoSave();
                    }
                } else if (state == MAIN_MENU) 
                {
                    for (int i = 0; i < buttonCount; i++) 
                    {
                        if (buttons[i].isClicked(Mouse::getPosition(window))) 
                        {
                            cout << "Button " << i << " clicked" << endl;
                            playButtonSound();

                            switch (i) 
                            {
                                case 0: // Display Guild
                                    state = DISPLAY_GUILD;
                                    playTransitionSound();
                                    buttonCount = 0;
                                    errorMessage.clear();
                                    selectedPetIndex = -1;
                                    scrollOffset = 0.0f;
                                    break;

                                case 1: // Train Pet
                                    state = TRAIN_PET;
                                    playTransitionSound();
                                    buttonCount = 0;
                                    errorMessage.clear();
                                    break;

                                case 2: // Heal Pets
                                    state = HEAL_PET;
                                    playTransitionSound();
                                    buttonCount = 0;
                                    errorMessage.clear();
                                    break;

                                case 3: // Shop
                                    state = SHOP_MENU;
                                    playTransitionSound();
                                    buttonCount = 0;
                                    errorMessage.clear();
                                    break;

                                case 4: // Inventory
                                    state = INVENTORY_MENU;
                                    playTransitionSound();
                                    buttonCount = 0;
                                    errorMessage.clear();
                                    break;

                                case 5: // Arena
                                    state = ARENA_MENU;
                                    playTransitionSound();
                                    buttonCount = 0;
                                    errorMessage.clear();
                                    break;

                                case 6: // Exit
                                    window.close();
                                    break;

                                case 7: // Back
                                    state = POST_NAME_MENU;
                                    playTransitionSound();
                                    buttonCount = 0;
                                    errorMessage.clear();
                                    break;
                            }
                            break;
                        }
                    }
                }

            } else if (event.type == Event::MouseWheelScrolled && state == DISPLAY_GUILD) 
            {
                scrollOffset += event.mouseWheelScroll.delta * 30.0f;

            } else if (event.type == Event::KeyPressed) 
            {
                if (event.key.code == Keyboard::Escape) 
                {
                    if (state == DISPLAY_GUILD || state == TRAIN_PET || state == SHOP_MENU || state == INVENTORY_MENU || state == TRAINING_MENU || state == CHOOSE_DIFFICULTY || state == HEAL_PET) {
                        state = MAIN_MENU;
                        playTransitionSound();
                        buttonCount = 0;
                        errorMessage.clear();
                        camp->endMiniGame();
                        selectedPetIndex = -1;
                        scrollOffset = 0.0f;
                        petToHeal = nullptr;

                    } else if (state == TRAINING_MINIGAME) 
                    {
                        camp->endMiniGame();
                        state = TRAINING_MENU;
                        playTransitionSound();
                        if (selectedPet) 
                        selectedPet->getSprite().setScale(0.5f, 0.5f);
                        errorMessage = "Mini-game ended!";
                        showMiniGameEndedMessage = true;
                        miniGameEndedTimer.restart();
                        gameManager.autoSave();

                    } else if (state == ARENA_MENU || state == BATTLE) 
                    {
                        state = MAIN_MENU;
                        playTransitionSound();
                        buttonCount = 0;
                        if (arena) {
                            cout << "Deleting arena on escape" << endl;
                            delete arena;
                            arena = nullptr;
                        }
                        if (battle) {
                            cout << "Deleting battle on escape" << endl;
                            delete battle;
                            battle = nullptr;
                        }
                        errorMessage.clear();
                    } else if (state == SELECT_HEAL_TARGET) 
                    {
                        state = HEAL_PET;
                        playTransitionSound();
                        petToHeal = nullptr;
                        buttonCount = 0;
                    } else if (state == INPUT_HEAL_AMOUNT) 
                    {
                        state = SELECT_HEAL_TARGET;
                        playTransitionSound();
                        currentInput.clear();
                        buttonCount = 0;
                    } else if (state == BATTLE && arena && battle) 
                    {
                        arena->endBattle(state, errorMessage, battle);
                        buttonCount = 0;
                        gameManager.autoSave();
                    }
                } else if (state == DISPLAY_GUILD) 
                {
                    if (event.key.code == Keyboard::Up) scrollOffset += 30.0f;
                    if (event.key.code == Keyboard::Down) scrollOffset -= 30.0f;
                }
            }
            camp->handleAbilityUnlockInput(event);
        }
    }

   

    void update(float deltaTime, Vector2i mousePos) 
    {

        if (state == TRAINING_MINIGAME && selectedPet) 
        {
             // Update transition sound cooldown
        transitionSoundCooldown -= deltaTime;
        if (transitionSoundCooldown < 0.0f) transitionSoundCooldown = 0.0f;
        // [Existing update code]
            Sprite& petSprite = selectedPet->getSprite();
            float petX = petSprite.getPosition().x;
            FloatRect bounds = petSprite.getLocalBounds();

            if (Keyboard::isKeyPressed(Keyboard::Left) && petX > 0) 
            {
                petSprite.move(-petMoveSpeed * deltaTime, 0);
            }
            if (Keyboard::isKeyPressed(Keyboard::Right) && petX + bounds.width * petSprite.getScale().x < WINDOW_WIDTH) 
            {
                petSprite.move(petMoveSpeed * deltaTime, 0);
            }
            camp->updateMiniGame(deltaTime, petSprite);

            drawTextWithShadow(window, "Fireballs: " + to_string(camp->getFireballCount()), globalFont, 28, 10, 130, Color::White);

            if (!camp->MiniGameActive()) 
            {
                state = TRAINING_MENU;
                selectedPet->getSprite().setScale(0.5f, 0.5f);
                if (errorMessage.empty()) 
                {
                    errorMessage = camp->getTrainingMessage();
                }
                showMiniGameEndedMessage = true;
                miniGameEndedTimer.restart();
                gameManager.autoSave();
            }
        }
    
        if (state == BATTLE && arena && battle) 
        {
            Event event;
            battle->handlePlayerInput(event, deltaTime, player->getInventory(), state);
            battle->update(deltaTime);
            arena->updateArena(deltaTime, state, errorMessage, battle);
        }
    }

    void render()
    {
        window.clear(Color(30, 30, 40));
        static bool titleStingPlayed = false; // Track if title sting has played for START_MENU
        static bool postNameTitleStingPlayed = false; // Track for POST_NAME_MENU


        // Select wallpaper based on state
        if (state == START_MENU || state == MAIN_MENU) 
        {
            window.draw(frontImageSprite);

        } else if (state == DISPLAY_GUILD) 
        {
            window.draw(guildBackgroundSprite);

        } else if (state == TRAINING_MINIGAME) 
        {
            window.draw(miniGameBackgroundSprite);

        } else if (state == BATTLE) 
        {
            window.draw(battleBackgroundSprite);

        } else if (state == ARENA_MENU) 
        {
            window.draw(battleBackgroundSprite);

        } else 
        {
            window.draw(magicalPetKingdomSprite);
        }
    
        if (!errorMessage.empty() && state != DISPLAY_GUILD && state != TRAINING_MINIGAME) 
        {
            drawTextWithShadow(window, errorMessage, globalFont, 26, WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT - 50, Color::Red);
        }
    
        if (state == START_MENU) 
        {
            if (!titleStingPlayed) 
            {
                playTitleStingSound();
                titleStingPlayed = true;
            }
            static Clock clockk;
            float titleAnimationTime = clockk.getElapsedTime().asSeconds();
            float deltaTime = clockk.restart().asSeconds();
            titleAnimationTime += deltaTime;
            float floatOffset = std::sin(titleAnimationTime * 2.0f) * 10.0f;
            buttonCount = 3;
            buttons[0].set("New Game", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 60, 360, 50, 0);
            buttons[1].set("Load Game", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2, 360, 50, 1);
            buttons[2].set("Exit", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 + 60, 360, 50, 2);
    
            drawTextWithShadow(window, "Magical Pet Kingdom ", globalFont, 70, WINDOW_WIDTH / 2 - 350, WINDOW_HEIGHT / 2 - 250 + floatOffset, Color::Yellow);
    
            for (int i = 0; i < buttonCount; i++) 
            {
                buttons[i].update(Mouse::getPosition(window));
                buttons[i].render(window);
            }

        } else if (state == POST_NAME_MENU) 
        {
            if (!postNameTitleStingPlayed) 
            {
                playTitleStingSound();
                postNameTitleStingPlayed = true;
            }
            drawTextWithShadow(window, "Magical Pets Kingdom", globalFont, 50, 400, 80, Color::Yellow);
            drawTextWithShadow(window, "Choose your path", globalFont, 30, 470, 150, Color::Cyan);
    
            postMenuButtons[0].set("Create Guild", globalFont, WINDOW_WIDTH / 2 - 200, 200, 400, 60, 0);
            postMenuButtons[1].set("Next", globalFont, WINDOW_WIDTH / 2 - 200, 300, 400, 60, 1);
            postMenuButtons[2].set("Exit", globalFont, WINDOW_WIDTH / 2 - 200, 400, 400, 60, 2);
    
            for (int i = 0; i < 3; ++i) 
            {
                postMenuButtons[i].update(Mouse::getPosition(window));
                postMenuButtons[i].render(window);
            }

        } else if (state == INPUT_NAME || state == INPUT_PET_NUM || state == INPUT_PET_NAME || state == INPUT_HEAL_AMOUNT) 
        {

            string promptText = (state == INPUT_NAME) ? "Enter your name" :(state == INPUT_PET_NUM) ? "Enter number of pets" :
                               (state == INPUT_PET_NAME) ? "Enter name for pet " + to_string(currentPet + 1) : "Enter heal amount for " + petToHeal->getname() + " (1 gold/10 HP)";

            drawTextWithShadow(window, promptText, globalFont, 36, WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 80, Color::White);
            window.draw(inputBox);
            inputText.setString(currentInput);
            FloatRect textBounds = inputText.getLocalBounds();
            inputText.setPosition(WINDOW_WIDTH / 2 - textBounds.width / 2, WINDOW_HEIGHT / 2 + (50 - textBounds.height) / 2 - 5);
            window.draw(inputText);
            submitButton.set("Submit", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 + 70, 360, 50);
            submitButton.update(Mouse::getPosition(window));
            submitButton.render(window);

        } else if (state == CHOOSE_PET) 
        {
            buttonCount = 4;
            buttons[0].set("Dragon", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 120, 360, 50, 0);
            buttons[1].set("Phoenix", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 60, 360, 50, 1);
            buttons[2].set("Unicorn", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2, 360, 50, 2);
            buttons[3].set("Griffin", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 + 60, 360, 50, 3);
            drawTextWithShadow(window, "Choose pet type:", globalFont, 36, WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 180, Color::White);
            for (int i = 0; i < buttonCount; i++) 
            {
                buttons[i].update(Mouse::getPosition(window));
                buttons[i].render(window);
            }

        } else if (state == CHOOSE_ROLE) 
        {
            buttonCount = (petChoice == 2) ? 3 : 2;
            int idx = 0;
            if (petChoice == 2) buttons[idx++].set("Healer", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 80, 360, 50, 0);
            buttons[idx++].set("Tanker", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 20, 360, 50, idx - 1);
            buttons[idx].set("Damage Dealer", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 + 40, 360, 50, idx - 1);
            drawTextWithShadow(window, "Choose role for " + petName + ":", globalFont, 36, WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 140, Color::White);
            for (int i = 0; i < buttonCount; i++) 
            {
                buttons[i].update(Mouse::getPosition(window));
                buttons[i].render(window);
            }

        } else if (state == MAIN_MENU) 
        {
            buttonCount = 8;
            buttons[0].set("Display Guild", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 150, 360, 50, 0);
            buttons[1].set("Train Pet", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 100, 360, 50, 1);
            buttons[2].set("Heal Pets", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 50, 360, 50, 2);
            buttons[3].set("Shop", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2, 360, 50, 3);
            buttons[4].set("Inventory", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 + 50, 360, 50, 4);
            buttons[5].set("Arena", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 + 100, 360, 50, 5);
            buttons[6].set("Exit", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 + 150, 360, 50, 6);
            buttons[7].set("Back", globalFont, 20, WINDOW_HEIGHT - 45, 360, 35, 7);
            for (int i = 0; i < buttonCount; i++) 
            {
                buttons[i].update(Mouse::getPosition(window));
                buttons[i].render(window);
            }

            if (player) drawTextWithShadow(window, "Gold: " + to_string(player->getGold()), globalFont, 28, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 + 180, Color::Yellow);

        } else if (state == DISPLAY_GUILD) 
        {
            if (player->getGuild().getPetCount() == 0) 
            {
                drawTextWithShadow(window, "No pets in guild! Go back to create a guild.", globalFont, 36, WINDOW_WIDTH / 2 - 300, WINDOW_HEIGHT / 2 - 50, Color::Red);
                buttonCount = 1;
                buttons[0].set("Back", globalFont, WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 70, 200, 50, 0);
                buttons[0].update(Mouse::getPosition(window));
                buttons[0].render(window);
            } else 
            {
                player->getGuild().DisplayGuild(window, frameClock.restart().asSeconds(), selectedPetIndex, Mouse::getPosition(window), scrollOffset, clickedIndex, doubleClickedIndex, state, selectedPet, levelUpTriggered, camp);
                buttonCount = 3;
                buttons[0].set("Back", globalFont, WINDOW_WIDTH / 2 - 250, WINDOW_HEIGHT - 70, 200, 50, 0);
                buttons[1].set("Train Pet", globalFont, WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT - 70, 200, 50, 1);
                buttons[2].set("Level Up ", globalFont, WINDOW_WIDTH / 2 + 150, WINDOW_HEIGHT - 70, 200, 50, 2);
                for (int i = 0; i < buttonCount; i++) 
                {
                    buttons[i].update(Mouse::getPosition(window));
                    buttons[i].render(window);
                }
            }
            if (player) 
            {
                drawTextWithShadow(window, "Player: " + player->getName(), globalFont, 20, 20, 20, Color::White);
                drawTextWithShadow(window, "Gold: " + to_string(player->getGold()), globalFont, 20, 20, 60, Color::Yellow);
            }
            if (!errorMessage.empty()) 
            {
                drawTextWithShadow(window, errorMessage, globalFont, 26, WINDOW_WIDTH / 2 - 200, 20, Color::Red);
            }

        } else if (state == TRAIN_PET) 
        {
            buttonCount = min(player->getGuild().getPetCount(), MAX_BUTTONS);
            drawTextWithShadow(window, "Select a Pet to Train:", globalFont, 36, WINDOW_WIDTH / 2 - 150, 50, Color::White);

            for (int i = 0; i < buttonCount; i++) 
            {
                if (player->getGuild().getPets()[i]) 
                {
                    string petName = player->getGuild().getPets()[i]->getname();
                    if (petName.empty())
                     petName = "Unnamed Pet " + to_string(i + 1);
                    buttons[i].set(petName, globalFont, WINDOW_WIDTH / 2 - 180, 120 + i * 60, 360, 50, i);
                    buttons[i].update(Mouse::getPosition(window));
                    buttons[i].render(window);
    
                    float xp = player->getGuild().getPets()[i]->getXP();
                    float maxXP = player->getGuild().getPets()[i]->getXPToNextLevel();
                    float progress = min(1.0f, xp / maxXP);
                    RectangleShape progressBar(Vector2f(280 * progress, 20));
                    progressBar.setPosition(WINDOW_WIDTH / 2 - 130, 120 + i * 60 + 30);
                    progressBar.setFillColor(Color::Green);
                    window.draw(progressBar);
    
                    string xpText = "XP: " + to_string(static_cast<int>(xp)) + "/" + to_string(static_cast<int>(maxXP));
                    drawTextWithShadow(window, xpText, globalFont, 16, WINDOW_WIDTH / 2 - 120, 123 + i * 60 + 30, Color::White);
                }
            }
            drawTextWithShadow(window, "Training Points: " + to_string(camp->getTrainingPoints()), globalFont, 24, WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 50, Color::Cyan);

        } else if (state == HEAL_PET)
        {
            buttonCount = 0;
            bool hasHealer = false;
            for (int i = 0; i < player->getGuild().getPetCount(); i++) 
            {
                Pet* pet = player->getGuild().getPets()[i];
                if (pet && pet->getname() == "Phoenix" && pet->getRole() == "Healer") 
                {
                    hasHealer = true;
                    break;
                }
            }
    
            if (!hasHealer) 
            {
                drawTextWithShadow(window, "No healers available!", globalFont, 36, WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 50, Color::Red);
            } else 
            {
                drawTextWithShadow(window, "Select a Healer to Heal:", globalFont, 36, WINDOW_WIDTH / 2 - 150, 50, Color::White);
                int validHealerCount = 0;
                for (int i = 0; i < player->getGuild().getPetCount() && validHealerCount < MAX_BUTTONS; i++) 
                {
                    Pet* pet = player->getGuild().getPets()[i];
                    if (pet && pet->getname() == "Phoenix" && pet->getRole() == "Healer") 
                    {
                        string petName = pet->getname();
                        if (petName.empty()) petName = "Unnamed Phoenix " + to_string(i + 1);
                        buttons[validHealerCount].set(petName + " (HP: " + to_string(pet->getHealth()) + ")",
                         globalFont, WINDOW_WIDTH / 2 - 180, 120 + validHealerCount * 60, 360, 50, validHealerCount);
                        buttons[validHealerCount].update(Mouse::getPosition(window));
                        buttons[validHealerCount].render(window);
                        validHealerCount++;
                    }
                }
                buttonCount = validHealerCount;
                drawTextWithShadow(window, "Gold: " + to_string(player->getGold()), globalFont, 24, WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 50, Color::Yellow);
            }

        } else if (state == SELECT_HEAL_TARGET) 
        {
            drawTextWithShadow(window, "Heal " + petToHeal->getname() + " (HP: " + to_string(petToHeal->getHealth()) + ")?",
             globalFont, 36, WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 80, Color::White);
            buttonCount = 2;
            buttons[0].set("Confirm", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2, 160, 50, 0);
            buttons[1].set("Cancel", globalFont, WINDOW_WIDTH / 2 + 20, WINDOW_HEIGHT / 2, 160, 50, 1);
            for (int i = 0; i < buttonCount; i++) 
            {
                buttons[i].update(Mouse::getPosition(window));
                buttons[i].render(window);
            }

        } else if (state == TRAINING_MENU) 
        {
            VertexArray gradientBackground(TriangleStrip, 6);
            float bgWidth = 400.0f;
            float bgHeight = 500.0f;
            float bgX = WINDOW_WIDTH / 2 - bgWidth / 2;
            float bgY = WINDOW_HEIGHT / 2 - bgHeight / 2;
    
            gradientBackground[0] = Vertex(Vector2f(WINDOW_WIDTH / 2, bgY), Color(255, 0, 0));
            gradientBackground[1] = Vertex(Vector2f(bgX, bgY + bgHeight / 2), Color(255, 165, 0));
            gradientBackground[2] = Vertex(Vector2f(WINDOW_WIDTH / 2, bgY + bgHeight / 2), Color(255, 255, 0));
            gradientBackground[3] = Vertex(Vector2f(bgX + bgWidth, bgY + bgHeight / 2), Color(0, 255, 0));
            gradientBackground[4] = Vertex(Vector2f(bgX, bgY + bgHeight), Color(0, 0, 255));
            gradientBackground[5] = Vertex(Vector2f(bgX + bgWidth, bgY + bgHeight), Color(128, 0, 128));
    
            window.draw(gradientBackground);
    
            float titleY = bgY + bgHeight / 2;
            drawTextWithShadow(window, "Training Menu", globalFont, 40, WINDOW_WIDTH / 2, titleY, Color::White);
    
            buttonCount = 4;
            buttons[0].set("Obstacle Course", globalFont, WINDOW_WIDTH / 2 - 150, titleY + 80, 300, 60, 0);
            buttons[1].set("Target Practice", globalFont, WINDOW_WIDTH / 2 - 150, titleY + 160, 300, 60, 1);
            buttons[2].set("Weight Training", globalFont, WINDOW_WIDTH / 2 - 150, titleY + 240, 300, 60, 2);
            buttons[3].set("Unlock Abilities", globalFont, WINDOW_WIDTH / 2 - 150, titleY + 320, 300, 60, 3);
    
            for (int i = 0; i < buttonCount; i++) 
            {
                buttons[i].update(Mouse::getPosition(window));
                buttons[i].render(window);
            }
    
            static float messageTimer = 0.0f;

            if (!camp->getTrainingMessage().empty()) 
            {
                messageTimer += frameClock.restart().asSeconds();
                if (messageTimer < 5.0f) {
                    Text msg(camp->getTrainingMessage(), globalFont, 26);
                    FloatRect bounds = msg.getLocalBounds();
                    msg.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
                    msg.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT - 40);
                    msg.setFillColor(Color::Cyan);
                    window.draw(msg);
                } else 
                {
                    messageTimer = 0.0f;
                    camp->clearTrainingMessage();
                }
            }
            if (camp->ShowAbilityMenu()) camp->drawAbilityMenu(window);

        } else if (state == CHOOSE_DIFFICULTY) 
        {
            drawTextWithShadow(window, "Choose Difficulty:", globalFont, 36, WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 140, Color::White);
            buttonCount = 4;
            buttons[0].set("Easy (1-5)", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 80, 360, 50, 0);
            buttons[1].set("Medium (2-10)", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 30, 360, 50, 1);
            buttons[2].set("Hard (3-15)", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 + 20, 360, 50, 2);
            buttons[3].set("Extreme (4-20)", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 + 70, 360, 50, 3);
            for (int i = 0; i < buttonCount; i++) 
            {
                buttons[i].update(Mouse::getPosition(window));
                buttons[i].render(window);
            }

        } else if (state == TRAINING_MINIGAME && selectedPet) 
        {
            camp->drawMiniGame(window, selectedPet->getSprite(), miniGameBackgroundSprite);
            buttonCount = 1;
            buttons[0].set("End Mini-Game", globalFont, WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 70, 200, 50, 0);
            for (int i = 0; i < buttonCount; i++) 
            {
                if (buttons[i].isClicked(Mouse::getPosition(window))) 
                {
                    camp->endMiniGame();
                    state = TRAINING_MENU;
                    if (selectedPet) selectedPet->getSprite().setScale(0.5f, 0.5f);
                    errorMessage = camp->getTrainingMessage();
                    showMiniGameEndedMessage = true;
                    miniGameEndedTimer.restart();
                    gameManager.autoSave();
                    break;
                }
                buttons[i].update(Mouse::getPosition(window));
                buttons[i].render(window);
            }

        } else if (state == SHOP_MENU) 
        {
            shop.displayShopWindow(window, player->getGold());
            shop.clearMessageAfterDelay(frameClock.restart().asSeconds());

        } else if (state == INVENTORY_MENU) 
        {
            player->getInventory().displayInventory(window);

        } else if (state == ARENA_MENU) 
        {
            window.draw(battleBackgroundSprite);
            drawTextWithShadow(window, "Arena - Select Opponent or Exit", globalFont, 36, WINDOW_WIDTH / 2 - 200, 50, Color::Yellow);
            buttonCount = 5;
            buttons[0].set("1 vs 1", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 120, 360, 50, 0);
            buttons[1].set("2 vs 2", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 60, 360, 50, 1);
            buttons[2].set("Guild Fight", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2, 360, 50, 2);
            buttons[3].set("Show Records", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 + 60, 360, 50, 3);
            buttons[4].set("Back", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 + 120, 360, 50, 4);

            for (int i = 0; i < buttonCount; i++) 
            {
                buttons[i].update(Mouse::getPosition(window));
                buttons[i].render(window);
            }
            if (errorMessage != "") 
            {
             drawTextWithShadow(window, errorMessage, globalFont, 26, WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT - 50, Color::Red);
            }

        }else if(state == SELECT_PET_FOR_BATTLE)
        {
            window.draw(battleBackgroundSprite);
            drawTextWithShadow(window, "Select a Pet for 1 vs 1 Battle", globalFont, 36, WINDOW_WIDTH / 2 - 200, 50, Color::White);

            for (int i = 0; i < player->getGuild().getPetCount(); i++) 
            {
                Pet* pet = player->getGuild().getPets()[i];
                string petName = pet->getname();
                if (petName == "") 
                    petName = "Unnamed Pet " + to_string(i + 1);

                float yPos = 150 + i * 120;

                Sprite petSprite = pet->getSprite();
                petSprite.setScale(0.5f, 0.5f);
                FloatRect bounds = petSprite.getLocalBounds();
                petSprite.setPosition(WINDOW_WIDTH / 2 - 150, yPos);
                window.draw(petSprite);

                drawTextWithShadow(window, petName, globalFont, 28, WINDOW_WIDTH / 2 - 50, yPos + 20, Color::Yellow);

                RectangleShape petBox(Vector2f(300, 100));
                petBox.setPosition(WINDOW_WIDTH / 2 - 150, yPos);
                petBox.setFillColor(Color(0, 0, 0, 50));
                petBox.setOutlineColor(Color::White);
                petBox.setOutlineThickness(2);
                window.draw(petBox);
            }

            buttonCount = 1;
            buttons[0].set("Back", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT - 100, 360, 50, 0);
            for (int i = 0; i < buttonCount; i++) 
            {
                buttons[i].update(Mouse::getPosition(window));
                buttons[i].render(window);
            }

            if (errorMessage != "") 
            {
                drawTextWithShadow(window, errorMessage, globalFont, 26, WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT - 50, Color::Red);
            }
        }else if(state == SELECT_PETS_FOR_2V2)
        {
            window.draw(battleBackgroundSprite);
            drawTextWithShadow(window, "Select Two Pets for 2 vs 2 Battle", globalFont, 36, WINDOW_WIDTH / 2 - 200, 50, Color::White);

            for (int i = 0; i < player->getGuild().getPetCount(); i++) 
            {
                Pet* pet = player->getGuild().getPets()[i];
                string petName = pet->getname();
                if (petName == "") petName = "Unnamed Pet " + to_string(i + 1);

                float yPos = 150 + i * 120;

                Sprite petSprite = pet->getSprite();
                petSprite.setScale(0.5f, 0.5f);
                FloatRect bounds = petSprite.getLocalBounds();
                petSprite.setPosition(WINDOW_WIDTH / 2 - 150, yPos);
                window.draw(petSprite);

                drawTextWithShadow(window, petName, globalFont, 28, WINDOW_WIDTH / 2 - 50, yPos + 20, Color::Yellow);

                RectangleShape petBox(Vector2f(300, 100));
                petBox.setPosition(WINDOW_WIDTH / 2 - 150, yPos);
                petBox.setFillColor(Color(0, 0, 0, 50));

                if (i == selectedPet1 || i == selectedPet2) 
                    petBox.setOutlineColor(Color::Green);
                else 
                    petBox.setOutlineColor(Color::White);
                petBox.setOutlineThickness(2);
                window.draw(petBox);
            }

        buttonCount = 2;
        buttons[0].set("Start Battle", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT - 100, 360, 50, 0);
        buttons[1].set("Back", globalFont, WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT - 40, 360, 50, 0);
        for (int i = 0; i < buttonCount; i++) 
        {
            buttons[i].update(Mouse::getPosition(window));
            buttons[i].render(window);
        }

        if (errorMessage != "") 
        {
            drawTextWithShadow(window, errorMessage, globalFont, 26, WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT - 50, Color::Red);
        }

        } else if (state == BATTLE && arena && battle) 
        {
            battle->draw(window);
    
            if (!arena || !battle) {
                cout << "Arena or battle is null!" << endl;
                errorMessage = "Error: Battle setup failed!";
                state = MAIN_MENU;
                return;
            }
        
            if (arena->getPlayerPetCount() == 1) 
            { // 1v1 battle
                Pet* pet = arena->getPlayerPet(0);
                if (!pet) 
                {
                    cout << "Pet is null in 1v1 battle!" << endl;
                    errorMessage = "Error: No pet available!";
                    state = ARENA_MENU;
                    return;
                }
                try 
                {
                    battleButtons[0].set("Attack", globalFont, 50, 70, 200, 50, 0);
                    
                    string ability1Name = "Locked";
                    bool ability1Locked = true;
                    if (pet->getAbility() && pet->getAbility()[0] && !pet->getAbility()[0]->isLocked()) 
                    {
                        ability1Name = pet->getAbility()[0]->getName();
                        if (ability1Name.length() > 100) 
                        {
                            cout << "Invalid Ability 1 Name length: " << ability1Name.length() << endl;
                            ability1Name = "Locked";
                        } else {
                            ability1Locked = false;
                            cout << "Ability 1 Name: " << ability1Name << ", Length: " << ability1Name.length() << endl;
                        }
                    }
                    battleButtons[1].set(ability1Name, globalFont, 300, 70, 200, 50, 1);
                    battleButtons[1].setDisabled(ability1Locked);
                    
                    string ability2Name = "Locked";
                    bool ability2Locked = true;
                    if (pet->getAbility() && pet->getAbility()[1] && !pet->getAbility()[1]->isLocked()) 
                    {
                        ability2Name = pet->getAbility()[1]->getName();
                        if (ability2Name.length() > 100) {
                            cout << "Invalid Ability 2 Name length: " << ability2Name.length() << endl;
                            ability2Name = "Locked";
                        } else 
                        {
                            ability2Locked = false;
                            cout << "Ability 2 Name: " << ability2Name << ", Length: " << ability2Name.length() << endl;
                        }
                        if (battle && !battle->getAbilityMessage().empty() && battle->getAbilityMessageTimer() > 0) {
                            drawTextWithShadow(window, battle->getAbilityMessage(), globalFont, 26, WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT - 50, Color::Cyan);
                        }
                    }
                    battleButtons[2].set(ability2Name, globalFont, 550, 70, 200, 50, 2);
                    battleButtons[2].setDisabled(ability2Locked);
                }
                catch (const std::length_error& e) 
                {
                    cout << "length_error in 1v1 render: " << e.what() << endl;
                    errorMessage = "Error: Invalid string length!";
                    state = ARENA_MENU;
                    return;
                }
        
                for (int i = 0; i < 3; i++) 
                {
                    Vector2f pos = battleButtons[i].getPosition();
                    string buttonLabel = (i == 0 ? "Attack" : i == 1 ? "ability1Name" : "ability2Name");
                    cout << "Button " << i << " ('" << buttonLabel << " | Disabled: " << battleButtons[i].isDisabled() << "') at x=" << pos.x << ", y=" << pos.y << endl;
                    battleButtons[i].update(Mouse::getPosition(window));
                    battleButtons[i].render(window);
                }
            }
            else if (arena->getPlayerPetCount() == 2) 
            { // 2v2 battle
                try 
                {
                    // Pet 1 buttons (left side)
                    Pet* pet1 = arena->getPlayerPet(0);
                    if (!pet1) 
                    {
                        cout << "Pet 1 is null in 2v2 battle!" << endl;
                        errorMessage = "Error: No pet available!";
                        state = ARENA_MENU;
                        return;
                    }
                    drawTextWithShadow(window, pet1->getname().length() <= 100 ? pet1->getname() : "Unknown", globalFont, 24, 50, 20, Color::Yellow);
                    battleButtons[0].set("Attack", globalFont, 50, 70, 200, 50, 0);
                    
                    string ability1Name = "Locked";
                    bool ability1Locked = true;
                    if (pet1->getAbility() && pet1->getAbility()[0] && !pet1->getAbility()[0]->isLocked()) 
                    {
                        ability1Name = pet1->getAbility()[0]->getName();
                        if (ability1Name.length() > 100) {
                            cout << "Invalid Pet 1 Ability 1 Name length: " << ability1Name.length() << endl;
                            ability1Name = "Locked";
                        } else 
                        {
                            ability1Locked = false;
                            cout << "Pet 1 Ability 1 Name: " << ability1Name << ", Length: " << ability1Name.length() << endl;
                        }
                    }
                    battleButtons[1].set(ability1Name, globalFont, 50, 130, 200, 50, 1);
                    battleButtons[1].setDisabled(ability1Locked);
                    
                    string ability2Name = "Locked";
                    bool ability2Locked = true;
                    if (pet1->getAbility() && pet1->getAbility()[1] && !pet1->getAbility()[1]->isLocked()) 
                    {
                        ability2Name = pet1->getAbility()[1]->getName();
                        if (ability2Name.length() > 100) {
                            cout << "Invalid Pet 1 Ability 2 Name length: " << ability2Name.length() << endl;
                            ability2Name = "Locked";
                        } else 
                        {
                            ability2Locked = false;
                            cout << "Pet 1 Ability 2 Name: " << ability2Name << ", Length: " << ability2Name.length() << endl;
                        }
                    }
                    battleButtons[2].set(ability2Name, globalFont, 50, 190, 200, 50, 2);
                    battleButtons[2].setDisabled(ability2Locked);
        
                    // Pet 2 buttons (right side)
                    Pet* pet2 = arena->getPlayerPet(1);
                    if (!pet2) 
                    {
                        cout << "Pet 2 is null in 2v2 battle!" << endl;
                        errorMessage = "Error: No pet available!";
                        state = ARENA_MENU;
                        return;
                    }
                    drawTextWithShadow(window, pet2->getname().length() <= 100 ? pet2->getname() : "Unknown", globalFont, 24, 300, 20, Color::Yellow);
                    battleButtons[3].set("Attack", globalFont, 300, 70, 200, 50, 3);
                    
                    string ability1NamePet2 = "Locked";
                    bool ability1LockedPet2 = true;
                    if (pet2->getAbility() && pet2->getAbility()[0] && !pet2->getAbility()[0]->isLocked()) 
                    {
                        ability1NamePet2 = pet2->getAbility()[0]->getName();
                        if (ability1NamePet2.length() > 100) {
                            cout << "Invalid Pet 2 Ability 1 Name length: " << ability1NamePet2.length() << endl;
                            ability1NamePet2 = "Locked";
                        } else {
                            ability1LockedPet2 = false;
                            cout << "Pet 2 Ability 1 Name: " << ability1NamePet2 << ", Length: " << ability1NamePet2.length() << endl;
                        }
                    }
                    battleButtons[4].set(ability1NamePet2, globalFont, 300, 130, 200, 50, 4);
                    battleButtons[4].setDisabled(ability1LockedPet2);
                    
                    string ability2NamePet2 = "Locked";
                    bool ability2LockedPet2 = true;
                    if (pet2->getAbility() && pet2->getAbility()[1] && !pet2->getAbility()[1]->isLocked()) 
                    {
                        ability2NamePet2 = pet2->getAbility()[1]->getName();
                        if (ability2NamePet2.length() > 100) 
                        {
                            cout << "Invalid Pet 2 Ability 2 Name length: " << ability2NamePet2.length() << endl;
                            ability2NamePet2 = "Locked";
                        } else {
                            ability2LockedPet2 = false;
                            cout << "Pet 2 Ability 2 Name: " << ability2NamePet2 << ", Length: " << ability2NamePet2.length() << endl;
                        }
                    }
                    battleButtons[5].set(ability2NamePet2, globalFont, 300, 190, 200, 50, 5);
                    battleButtons[5].setDisabled(ability2LockedPet2);
                }
                catch (const std::length_error& e) 
                {
                    cout << "length_error in 2v2 render: " << e.what() << endl;
                    errorMessage = "Error: Invalid string length!";
                    state = ARENA_MENU;
                    return;
                }
        
                for (int i = 0; i < 6; i++) 
                {
                    Vector2f pos = battleButtons[i].getPosition();
                    string buttonLabel = (i % 3 == 0 ? "Attack" : (i % 3 == 1 ? (i < 3 ? "ability1Name" : "ability1NamePet2") : (i < 3 ? "ability2Name" : "ability2NamePet2")));
                    cout << "Button " << i << " ('" << buttonLabel << " | Disabled: " << battleButtons[i].isDisabled() << "') at x=" << pos.x << ", y=" << pos.y << endl;
                    battleButtons[i].update(Mouse::getPosition(window));
                    battleButtons[i].render(window);
                }
            }
        
            // End battle button
            buttonCount = 1;
            buttons[0].set("End Battle", globalFont, WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT - 70, 200, 50, 0);
            buttons[0].update(Mouse::getPosition(window));
            buttons[0].render(window);
    }
        // Draw the "Mini-game ended!" message last to ensure it appears on top
        if (showMiniGameEndedMessage) 
        {
            drawTextWithShadow(window, "Mini-game ended!", globalFont, 28, WINDOW_WIDTH / 2, 5, Color::Red);
        }
    
        window.display();
    }

};

int main() 
{
/*
Dragon d("FireDragon", "Tanker");
Phoenix p("FlamePhoenix", "Healer");
cout << (d > p) << endl;
Pet* fused = d + p; 
delete fused; // Clean up
*/
    srand(time(0));
    Player* player = nullptr;
    TrainingCamps camp;
    Game game(player, &camp);
    game.run();
    return 0;
}
