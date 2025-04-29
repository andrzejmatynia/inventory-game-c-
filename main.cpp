#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <chrono>

using namespace std;

class Item;
class Player;
enum Rarities {
    common,
    uncommon,
    magic,
    rare,
    legendary
};
enum ItemType {
    weapon,
    armor,
    potion,
    material
};


enum SortCriteria {
    sortByName,
    sortByRarity,
    sortByValue
};

string colorReset = "\033[0m";
string colorBlue = "\033[36m";
class Item {
protected:
    string itemName;
    float itemPrice;
    int itemDurability;
    Rarities itemRarity;
public:
    ItemType itemType;
    float itemDamage;
    float itemDefense;
    float itemHealAmount;
    int itemMaterialValue;
public:
    Item()
        : itemName{"Scrap"}, itemPrice{0.1f}, itemDurability{100}, itemRarity{common}, itemType{material},
        itemDamage{0.0f}, itemDefense{0.0f}, itemHealAmount{0.0f}, itemMaterialValue{1}
    {
    }
    Item(string name, float price = 20.0f, int durability = 100, Rarities rarity = common, ItemType type = material,
         float damage = 0.0f, float defense = 0.0f, float healAmount = 0.0f, int materialValue = 1)
        : itemName{name}, itemPrice{price}, itemDurability{durability}, itemRarity(rarity), itemType(type),
        itemDamage{damage}, itemDefense{defense}, itemHealAmount{healAmount}, itemMaterialValue{materialValue}
    {
    }

    virtual void display() {
        cout << "Name: " << itemName << endl;
        cout << "Price: " << itemPrice << endl;
        cout << "Durability: " << itemDurability << endl;
        cout << "Rarity: " << itemRarity << endl;
        if (itemType == weapon) cout << "Damage: " << itemDamage << endl;
        if (itemType == armor) cout << "Defense: " << itemDefense << endl;
        if (itemType == potion) cout << "Heal: " << itemHealAmount << endl;
        if (itemType == material) cout << "Material Value: " << itemMaterialValue << endl;
    }

    virtual bool use(Player* player);

    string getName() const { return itemName; }
    float getPrice() const { return itemPrice; }
    int getDurability() const { return itemDurability; }
    Rarities getRarity() const { return itemRarity; }
    ItemType getType() const { return itemType; }
    int getMaterialValue() const { return itemMaterialValue; }

    void setName(string name) { this->itemName = name; }
    void setPrice(float price) { this->itemPrice = price; }
    void setDurability(int durability) { this->itemDurability = durability; }
    void setRarity(Rarities rarity) { this->itemRarity = rarity; }
    void setType(ItemType type) { this->itemType = type; }
    void setMaterialValue(int value) { this->itemMaterialValue = value; }
};

class Weapon : public Item {
private:
    float attackSpeed;

public:
    Weapon() : Item("Rusty Sword", 5.0f, 50, common, weapon, 5.0f, 0.0f, 0.0f, 2), attackSpeed{0.8f} {
    }

    Weapon(string name,
           float price,
           int durability,
           Rarities rarity,
           float damage,
           float attackSpeed)
        : Item(name, price, durability, rarity, weapon, damage, 0.0f, 0.0f, 2),
        attackSpeed{attackSpeed} {}

    void display() override {
        Item::display();
        cout << "Attack Speed: " << attackSpeed << endl;
    }

    float getAttackSpeed() const { return attackSpeed; }
};

class Armor : public Item {
private:
public:
    Armor() : Item("Leather Chestplate", 8.0f, 80, common, armor, 0.0f, 3.0f, 0.0f, 1) {}

    Armor(string name, float price, int durability, Rarities rarity, float defense)
        : Item(name, price, durability, rarity, armor, 0.0f, defense, 0.0f, 1) {}

    void display() override {
        Item::display();
    }
};

class Potion : public Item {
public:
    Potion() : Item("Small Potion", 15.0f, 1, common, potion, 0.0f, 0.0f, 30.0f, 0) {}

    Potion(string name, float price, Rarities rarity, float healAmount)
        : Item(name, price, 1, rarity, potion, 0.0f, 0.0f, healAmount, 0) {}
};

class Inventory {
private:
    Item*** items;
    int numRows;
    int numCols;
    int itemCount;

public:
    Inventory(int rows, int cols) : numRows{rows}, numCols{cols}, itemCount{0} {
        if (numRows <= 0 || numCols <= 0) {
            cerr << "Inventory dimensions must be positive" << endl;
            this->numRows = 1;
            this->numCols = 1;
        }
        items = new Item**[this->numRows];
        for(int i = 0; i < this->numRows; i++) {
            items[i] = new Item*[this->numCols];
            for(int j = 0; j < this->numCols; j++) {
                items[i][j] = nullptr;
            }
        }
    }
    void removeItems() {
        for(int i = 0; i < numRows; i++) {
            for(int j = 0; j < numCols; j++) {
                if(items[i][j] != nullptr) {
                    delete items[i][j];
                    items[i][j] = nullptr;
                }
            }
        }
        itemCount = 0;
    }
    void clearGrid() {
        for(int i = 0; i < numRows; i++) {
            for(int j = 0; j < numCols; j++) {
                items[i][j] = nullptr;
            }
        }
        itemCount = 0;
    }


    float getInventoryValue() {
        float sum = 0.0f;
        for(int i = 0; i < numRows; i++) {
            for(int j = 0; j < numCols; j++) {
                if(items[i][j] != nullptr) {
                    sum += items[i][j]->getPrice();
                }
            }
        }
        return sum;
    }


    int destroyItem(int row, int col) {
        if((row >= numRows || row < 0) || (col >= numCols || col < 0)) {
            cout << "Invalid inventory coordinates" << endl;
            return 0;
        }
        if(items[row][col] == nullptr) {
            cout << "Inventory slot (" << row << "," << col << ") is already empty" << endl;
            return 0;
        }

        int materialGained = items[row][col]->getMaterialValue();
        string destroyedItemName = items[row][col]->getName();

        delete items[row][col];
        items[row][col] = nullptr;
        itemCount--;
        cout << "Destroyed " << destroyedItemName << " from (" << row << "," << col << ")" << endl;
        return materialGained;
    }

    bool moveItem(int r1, int c1, int r2, int c2) {
        if ((r1 >= numRows || r1 < 0) || (c1 >= numCols || c1 < 0) ||
            (r2 >= numRows || r2 < 0) || (c2 >= numCols || c2 < 0)) {
            cout << "Invalid source or destination coordinates" << endl;
            return false;
        }
        if (r1 == r2 && c1 == c2) {
            cout << "Source and destination are the same" << endl;
            return false;
        }
        if (items[r1][c1] == nullptr) {
            cout << "Source slot (" << r1 << "," << c1 << ") is empty" << endl;
            return false;
        }

        Item* temp = items[r2][c2];
        items[r2][c2] = items[r1][c1];
        items[r1][c1] = temp;

        cout << "Moved item from (" << r1 << "," << c1 << ") to (" << r2 << "," << c2 << ")" << endl;
        return true;
    }
    void sortItems(SortCriteria criteria) {
        vector<Item*> tempItems;
        for(int i = 0; i < numRows; i++) {
            for(int j = 0; j < numCols; j++) {
                if(items[i][j] != nullptr) {
                    tempItems.push_back(items[i][j]);
                }
            }
        }

        switch (criteria) {
        case sortByName:
            sort(tempItems.begin(), tempItems.end(), [](const Item* a, const Item* b) {
                return a->getName() < b->getName();
            });
            cout << "Inventory sorted by name" << endl;
            break;
        case sortByRarity:
            sort(tempItems.begin(), tempItems.end(), [](const Item* a, const Item* b) {
                return a->getRarity() < b->getRarity();
            });
            cout << "Inventory sorted by rarity" << endl;
            break;
        case sortByValue:
            sort(tempItems.begin(), tempItems.end(), [](const Item* a, const Item* b) {
                return a->getPrice() < b->getPrice();
            });
            cout << "Inventory sorted by value" << endl;
            break;
        }

        clearGrid();

        for(Item* item : tempItems) {
            addItem(item);
        }
    }
    ~Inventory() {
        removeItems();
        for(int i = 0; i < numRows; i++) {
            delete[] items[i];
        }
        delete[] items;
        items = nullptr;
    }
    bool addItem(Item* I) {
        if (I == nullptr) {
            cerr << "Attempted to add a null item to inventory" << endl;
            return false;
        }
        if (itemCount >= numRows * numCols) {
            cout << "Inventory is full Cannot add " << I->getName() << endl;
            return false;
        }
        for(int i = 0; i < numRows; i++) {
            for(int j = 0; j < numCols; j++) {
                if(items[i][j] == nullptr) {
                    items[i][j] = I;
                    itemCount++;
                    return true;
                }
            }
        }
        cerr << "Inventory full check failed, no empty slot found" << endl;
        return false;
    }
    void display() {
        cout << colorBlue << "--- Inventory Grid (" << itemCount << "/" << numRows * numCols << ") ---" << colorReset << endl;
        cout << "  ";
        for (int j = 0; j < numCols; ++j) {
            cout << " " << j << " ";
        }
        cout << endl;
        cout << "  ";
        for (int j = 0; j < numCols; ++j) {
            cout << colorBlue << "---" << colorReset;
        }
        cout << endl;

        for(int i = 0; i < numRows; i++) {
            cout << i << "|";
            for(int j = 0; j < numCols; j++) {
                cout << "[";
                if(items[i][j] == nullptr) {
                    cout << " ";
                }
                else {
                    cout << items[i][j]->getName()[0];
                }
                cout << "]";
            }
            cout << endl;
        }
        cout << colorBlue << "----------------------" << colorReset << endl;
    }
    Item* getItem(int row, int col) const {
        if((row >= numRows || row < 0) || (col >= numCols || col < 0)) {
            return nullptr;
        }
        return items[row][col];
    }
    Item* removeItemPointer(int row, int col) {
        if((row >= numRows || row < 0) || (col >= numCols || col < 0)) {
            return nullptr;
        }
        if(items[row][col] == nullptr) {
            return nullptr;
        }
        Item* temp = items[row][col];
        items[row][col] = nullptr;
        itemCount--;
        return temp;
    }
    bool setItemPointer(int row, int col, Item* item) {
        if((row >= numRows || row < 0) || (col >= numCols || col < 0)) {
            return false;
        }
        if (items[row][col] == nullptr && item != nullptr) {
            itemCount++;
        } else if (items[row][col] != nullptr && item == nullptr) {
            itemCount--;
        }
        items[row][col] = item;
        return true;
    }

    int getRows() const { return numRows; }
    int getCols() const { return numCols; }
    bool isFull() const { return itemCount >= numRows * numCols; }
};
const int equipSlotWeapon = 0;
const int equipSlotArmor = 1;
const int numEquipSlots = 4;

class Player {
private:
    string playerName;
    float playerHP;
    float playerMaxHP;
    float playerGold;
    int playerMaterials;
    Inventory* playerInventory;
    Item* playerEquipment[numEquipSlots];
public:
    Player() : playerName{"Player1"}, playerHP{100.0f}, playerMaxHP{100.0f}, playerGold{50.0f}, playerMaterials{10} {
        playerInventory = new Inventory(4, 4);
        for(int i = 0; i < numEquipSlots; i++) {
            playerEquipment[i] = nullptr;
        }
        playerInventory->addItem(new Weapon());
        playerInventory->addItem(new Armor());
        playerInventory->addItem(new Potion());
        playerInventory->addItem(new Item("Rare Gem", 100.0f, 1, rare, material, 0, 0, 0, 20));
        playerInventory->addItem(new Item("Magic Staff", 200.0f, 50, magic, weapon, 20.0f, 0, 0, 10));
    }
    Player(string name) : playerName{name}, playerHP{100.0f}, playerMaxHP{100.0f}, playerGold{50.0f}, playerMaterials{10} {
        playerInventory = new Inventory(4, 4);
        for(int i = 0; i < numEquipSlots; i++) {
            playerEquipment[i] = nullptr;
        }
        playerInventory->addItem(new Weapon());
        playerInventory->addItem(new Armor());
        playerInventory->addItem(new Potion());
        playerInventory->addItem(new Item("Rare Gem", 100.0f, 1, rare, material, 0, 0, 0, 20));
        playerInventory->addItem(new Item("Magic Staff", 200.0f, 50, magic, weapon, 20.0f, 0, 0, 10));
    }
    void showEquipment() {
        cout << colorBlue << "--- Equipment ---" << colorReset << endl;
        cout << "0: Weapon [";
        if(playerEquipment[equipSlotWeapon] != nullptr) {
            cout << playerEquipment[equipSlotWeapon]->getName();
        } else { cout << " "; }
        cout << "]" << endl;

        cout << "1: Armor  [";
        if(playerEquipment[equipSlotArmor] != nullptr) {
            cout << playerEquipment[equipSlotArmor]->getName();
        } else { cout << " "; }
        cout << "]" << endl;
        cout << colorBlue << "-----------------" << colorReset << endl;
    }

    bool equip(int row, int col) {
        if (!playerInventory) {
            cerr << "Inventory is null, cannot equip" << endl;
            return false;
        }
        Item* itemToEquip = playerInventory->getItem(row, col);
        if (!itemToEquip) {
            cout << "No item at (" << row << "," << col << ")" << endl;
            return false;
        }
        int targetSlot = -1;
        if (itemToEquip->getType() == weapon) targetSlot = equipSlotWeapon;
        else if (itemToEquip->getType() == armor) targetSlot = equipSlotArmor;
        else {
            cout << itemToEquip->getName() << " cannot be equipped" << endl;
            return false;
        }
        Item* itemFromInv = playerInventory->removeItemPointer(row, col);
        if (!itemFromInv) {
            cerr << "Failed to remove item pointer from inventory during equip" << endl;
            return false;
        }

        Item* currentlyEquipped = playerEquipment[targetSlot];

        playerEquipment[targetSlot] = itemFromInv;
        cout << "Equipped " << itemFromInv->getName() << endl;

        if (currentlyEquipped != nullptr) {
            cout << "Moving previously equipped item " << currentlyEquipped->getName() << " to inventory" << endl;
            if (!playerInventory->setItemPointer(row, col, currentlyEquipped)) {
                if (!playerInventory->addItem(currentlyEquipped)) {
                    cout << "Inventory is full Cannot store previously equipped item " << currentlyEquipped->getName() << endl;
                    cout << currentlyEquipped->getName() << " was lost" << endl;
                    delete currentlyEquipped;
                }
            }
        }
        return true;
    }

    bool unequipItem(int equipSlotIndex) {
        if (equipSlotIndex < 0 || equipSlotIndex >= numEquipSlots) {
            cout << "Invalid equipment slot" << endl;
            return false;
        }
        if (!playerInventory) {
            cerr << "Inventory is null, cannot unequip" << endl;
            return false;
        }
        Item* itemToUnequip = playerEquipment[equipSlotIndex];

        if (!itemToUnequip) {
            cout << "No item equipped in that slot" << endl;
            return false;
        }
        if (playerInventory->addItem(itemToUnequip)) {
            cout << "Unequipped " << itemToUnequip->getName() << " to inventory" << endl;
            playerEquipment[equipSlotIndex] = nullptr;
            return true;
        } else {
            cout << "Inventory is full Cannot unequip " << itemToUnequip->getName() << endl;
            return false;
        }
    }

    ~Player() {
        delete playerInventory;
        playerInventory = nullptr;

        for(int i = 0; i < numEquipSlots; i++) {
            if (playerEquipment[i] != nullptr) {
                delete playerEquipment[i];
                playerEquipment[i] = nullptr;
            }
        }
    }
    void showStats() {
        cout << colorBlue << "====== Player Stats ======" << colorReset << endl;
        cout << "Name: " << playerName << endl;
        cout << "HP: " << playerHP << "/" << playerMaxHP << endl;
        cout << "Gold: " << playerGold << endl;
        cout << "Materials: " << playerMaterials << endl;
        cout << colorBlue << "--------------------------" << colorReset << endl;
        showEquipment();
        cout << colorBlue << "--------------------------" << colorReset << endl;
        if (playerInventory) {
            playerInventory->display();
        } else {
            cout << "Inventory: [NULL]" << endl;
        }
        cout << colorBlue << "==========================" << colorReset << endl;
    }
    void manageInventory() {
        int option;
        int row, col;
        int r1, c1, r2, c2;
        int equipSlotIndex;
        int sortChoice;
        while(true) {
            system("clear");
            cout << colorBlue << "--- Manage Inventory ---" << colorReset << endl;
            if (playerInventory) {
                playerInventory->display();
            } else {
                cout << "Inventory is null" << endl;
                return;
            }
            showEquipment();

            cout << "\n0: Back" << endl;
            cout << "1: View Item Details" << endl;
            cout << "2: Equip item" << endl;
            cout << "3: Unequip item" << endl;
            cout << "4: Destroy Item" << endl;
            cout << "5: Use Item" << endl;
            cout << "6: Move Item" << endl;
            cout << "7: Sort Inventory" << endl;
            cout << colorBlue << "Enter option: " << colorReset;
            cin >> option;
            if (cin.fail()) {
                cout << "Invalid input. Please enter a number" << endl;
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }
            cin.ignore(10000, '\n');

            switch(option) {
            case 1:
                cout << colorBlue << "Enter item coordinates (row col): " << colorReset;
                cin >> row >> col;
                if (cin.fail()) { cout << "Invalid coordinates\n"; cin.clear(); cin.ignore(10000, '\n'); break;}
                cin.ignore(10000, '\n');
                {
                    Item* item = playerInventory->getItem(row, col);
                    if (item) {
                        cout << colorBlue << "\n--- Item Details ---" << colorReset << endl;
                        item->display();
                        cout << colorBlue << "--------------------" << colorReset << endl;
                    } else {
                        cout << "No item at that slot" << endl;
                    }
                }
                break;
            case 2:
                cout << colorBlue << "Enter inventory coordinates of item to equip (row col): " << colorReset;
                cin >> row >> col;
                if (cin.fail()) { cout << "Invalid coordinates\n"; cin.clear(); cin.ignore(10000, '\n'); break;}
                cin.ignore(10000, '\n');
                equip(row, col);
                break;
            case 3:
                cout << colorBlue << "Enter equipment slot index to unequip (0-1): " << colorReset;
                cin >> equipSlotIndex;
                if (cin.fail()) { cout << "Invalid slot index\n"; cin.clear(); cin.ignore(10000, '\n'); break;}
                cin.ignore(10000, '\n');
                unequipItem(equipSlotIndex);
                break;
            case 4:
                cout << colorBlue << "Enter inventory coordinates of item to destroy (row col): " << colorReset;
                cin >> row >> col;
                if (cin.fail()) { cout << "Invalid coordinates\n"; cin.clear(); cin.ignore(10000, '\n'); break;}
                cin.ignore(10000, '\n');
                {
                    int materialsGained = playerInventory->destroyItem(row, col);
                    if (materialsGained > 0) {
                        addMaterials(materialsGained);
                        cout << "Gained " << materialsGained << " materials" << endl;
                    }
                }
                break;
            case 5:
                cout << colorBlue << "Enter inventory coordinates of item to use (row col): " << colorReset;
                cin >> row >> col;
                if (cin.fail()) { cout << "Invalid coordinates\n"; cin.clear(); cin.ignore(10000, '\n'); break;}
                cin.ignore(10000, '\n');
                useInventoryItem(row, col);
                break;
            case 6:
                cout << colorBlue << "Enter source coordinates (row col): " << colorReset;
                cin >> r1 >> c1;
                if (cin.fail()) { cout << "Invalid source coordinates\n"; cin.clear(); cin.ignore(10000, '\n'); break;}
                cout << colorBlue << "Enter destination coordinates (row col): " << colorReset;
                cin >> r2 >> c2;
                if (cin.fail()) { cout << "Invalid destination coordinates\n"; cin.clear(); cin.ignore(10000, '\n'); break;}
                cin.ignore(10000, '\n');
                playerInventory->moveItem(r1, c1, r2, c2);
                break;
            case 7:
                if (playerInventory) {
                    cout << colorBlue << "\nSort by:" << colorReset << endl;
                    cout << "1: Name" << endl;
                    cout << "2: Rarity" << endl;
                    cout << "3: Value" << endl;
                    cout << colorBlue << "Enter sort option: " << colorReset;
                    cin >> sortChoice;
                    if (cin.fail()) { cout << "Invalid input\n"; cin.clear(); cin.ignore(10000, '\n'); break;}
                    cin.ignore(10000, '\n');

                    if (sortChoice == 1) {
                        playerInventory->sortItems(sortByName);
                    } else if (sortChoice == 2) {
                        playerInventory->sortItems(sortByRarity);
                    } else if (sortChoice == 3) {
                        playerInventory->sortItems(sortByValue);
                    } else {
                        cout << "Invalid sort option" << endl;
                    }
                } else {
                    cout << "Inventory is null, cannot sort" << endl;
                }
                break;
            case 0:
                cout << "Exiting inventory management" << endl;
                return;
            default:
                cout << "Invalid option" << endl;
                break;
            }
            if (option != 0) {
                cout << colorBlue << "\nPress Enter to continue" << colorReset;
                cin.get();
            }
        }
    }
    string getName() const { return playerName; }
    float getHP() const { return playerHP; }
    float getMaxHP() const { return playerMaxHP; }
    float getGold() const { return playerGold; }
    int getMaterials() const { return playerMaterials; }
    Inventory* getInventory() const { return playerInventory; }
    Item* getEquippedItem(int slotIndex) const {
        if (slotIndex >= 0 && slotIndex < numEquipSlots) return playerEquipment[slotIndex];
        return nullptr;
    }
    void setGold(float gold) { this->playerGold = gold; }
    void setName(string name) { this->playerName = name; }
    void setHP(float HP) { this->playerHP = HP; }
    void setMaxHP(float maxHP) { this->playerMaxHP = maxHP; }
    void setMaterials(int materials) { this->playerMaterials = materials; }
    void addGold(float amount) { playerGold += amount; if (playerGold < 0) playerGold = 0; }
    void addMaterials(int amount) { playerMaterials += amount; if (playerMaterials < 0) playerMaterials = 0; }
    void takeDamage(float amount) { playerHP -= amount; if (playerHP < 0) playerHP = 0; }
    void heal(float amount) { playerHP += amount; if (playerHP > playerMaxHP) playerHP = playerMaxHP; }
    void useInventoryItem(int row, int col) {
        if (!playerInventory) return;
        Item* item = playerInventory->getItem(row, col);
        if (!item) {
            cout << "No item found at (" << row << "," << col << ")" << endl;
            return;
        }

        cout << "Attempting to use " << item->getName() << " from inventory" << endl;

        if(item->use(this)) {
            if (item->getType() == potion) {
                cout << item->getName() << " was consumed" << endl;
                playerInventory->destroyItem(row, col);
            }
        } else {
            cout << item->getName() << " could not be used" << endl;
        }
    }
};


bool Item::use(Player* player) {
    if (itemType == potion) {
        cout << "You use the " << itemName << endl;
        player->heal(itemHealAmount);
        return true;
    }
    cout << itemName << " cannot be used directly" << endl;
    return false;
}
class Shop {
private:
    vector<Item*> shopStock;

    void generateStock() {
        for (Item* item : shopStock) {
            delete item;
        }
        shopStock.clear();

        for (int i = 0; i < 5; ++i) {
            int itemTypeRand = rand() % 4;
            Rarities rarityRand = static_cast<Rarities>(rand() % 4);

            string itemName = "Item";
            float itemPrice = 10.0f;
            float itemDamage = 0.0f;
            float itemDefense = 0.0f;
            float itemHeal = 0.0f;
            ItemType currentItemType = material;
            int currentMatValue = 1;

            if (itemTypeRand == 0) { itemName = "Sword"; itemPrice = 20.0f; itemDamage = 10.0f + rand()%5; currentItemType = weapon; currentMatValue = 3; }
            else if (itemTypeRand == 1) { itemName = "Shield"; itemPrice = 15.0f; itemDefense = 8.0f + rand()%4; currentItemType = armor; currentMatValue = 2; }
            else if (itemTypeRand == 2) { itemName = "Healing Potion"; itemPrice = 25.0f; itemHeal = 40.0f + rand()%10; currentItemType = potion; currentMatValue = 0; }
            else { itemName = "Material"; itemPrice = 5.0f; currentItemType = material; currentMatValue = 5; }

            if (rarityRand == uncommon) { itemPrice *= 1.5f; itemDamage *= 1.2f; itemDefense *= 1.2f; itemHeal *= 1.2f; }
            if (rarityRand == magic) { itemPrice *= 2.0f; itemDamage *= 1.5f; itemDefense *= 1.5f; itemHeal *= 1.5f; }
            if (rarityRand == rare) { itemPrice *= 3.0f; itemDamage *= 2.0f; itemDefense *= 2.0f; itemHeal *= 2.0f; }
            shopStock.push_back(new Item(itemName, itemPrice, 100, rarityRand, currentItemType, itemDamage, itemDefense, itemHeal, currentMatValue));
        }
        shopStock.push_back(new Item("Scrap Metal", 2.0f, 1, common, material, 0.0f, 0.0f, 0.0f, 2));
    }

public:
    Shop() {
        generateStock();
    }

    ~Shop() {
        for (Item* item : shopStock) {
            delete item;
        }
        shopStock.clear();
    }
    void displayStock(Player* player) const {
        system("clear");
        cout << colorBlue << "====== Welcome to the Shop! ======" << colorReset << endl;
        if (player) {
            cout << "Your Gold: " << player->getGold() << endl;
        }
        cout << colorBlue << "----------------------------------" << colorReset << endl;
        cout << "Items for Sale:" << endl;
        if (shopStock.empty()) {
            cout << "Shop is empty right now" << endl;
        } else {
            for (size_t i = 0; i < shopStock.size(); ++i) {
                cout << i + 1 << ". ";
                if (shopStock[i]) {
                    shopStock[i]->display();
                } else {
                    cerr << "Null item found in shop stock at index " << i << endl;
                }
                cout << colorBlue << "--------------------" << colorReset << endl;
            }
        }
        cout << colorBlue << "==================================" << colorReset << endl;
    }
    void buyItem(Player* player, int itemIndex) {
        if (!player || !player->getInventory()) {
            cerr << "Invalid player or inventory in buyItem" << endl;
            return;
        }
        if (itemIndex < 1 || itemIndex > shopStock.size()) {
            cout << "Invalid item number" << endl;
            return;
        }

        Item* itemToBuy = shopStock[itemIndex - 1];
        if (!itemToBuy) {
            cerr << "Selected item in shop is null" << endl;
            return;
        }

        if (player->getGold() >= itemToBuy->getPrice()) {
            if (!player->getInventory()->isFull()) {
                Item* boughtItem = shopStock[itemIndex - 1];
                shopStock.erase(shopStock.begin() + (itemIndex - 1));

                if (player->getInventory()->addItem(boughtItem)) {
                    player->addGold(-boughtItem->getPrice());
                    cout << "You bought the " << boughtItem->getName() << endl;
                } else {
                    cout << "Failed to add item to inventory after purchase" << endl;
                    shopStock.insert(shopStock.begin() + (itemIndex - 1), boughtItem);
                }
            } else {
                cout << "Your inventory is full" << endl;
            }
        } else {
            cout << "Not enough gold" << endl;
        }
    }

    void sellItem(Player* player, int invRow, int invCol) {
        if (!player || !player->getInventory()) {
            cerr << "Invalid player or inventory in sellItem" << endl;
            return;
        }
        Item* itemToSell = player->getInventory()->removeItemPointer(invRow, invCol);
        if (!itemToSell) {
            cout << "No item at that inventory slot (" << invRow << "," << invCol <<") or failed to remove" << endl;
            return;
        }

        float sellPrice = itemToSell->getPrice() * 0.5f;

        player->addGold(sellPrice);
        cout << "You sold the " << itemToSell->getName() << " for " << sellPrice << " gold" << endl;

        delete itemToSell;
        itemToSell = nullptr;
    }

    void rerollStock(Player* player) {
        if (!player) return;
        if (player->getGold() >= 20) {
            player->addGold(-20);
            generateStock();
            cout << "Shop stock rerolled" << endl;
        } else {
            cout << "Not enough gold to reroll" << endl;
        }
    }
    void enterShop(Player* player) {
        if (!player) {
            cerr << "Cannot enter shop without a player" << endl;
            return;
        }
        int choice = -1;
        int itemNum;
        int invRow, invCol;
        while (choice != 0) {
            displayStock(player);
            player->getInventory()->display();

            cout << colorBlue << "\n--- Shop Menu ---" << colorReset << endl;
            cout << "1. Buy Item" << endl;
            cout << "2. Sell Item" << endl;
            cout << "3. Reroll Stock (20 Gold)" << endl;
            cout << "0. Leave Shop" << endl;
            cout << colorBlue << "Enter choice: " << colorReset;
            cin >> choice;
            if (cin.fail()) {
                cout << "Invalid input. Try again" << endl;
                cin.clear();
                cin.ignore(10000, '\n');
                choice = -1;
                continue;
            }
            cin.ignore(10000, '\n');

            if (choice == 1) {
                cout << colorBlue << "Enter item number to buy: " << colorReset;
                cin >> itemNum;
                if (cin.fail()) { cout << "Invalid input\n"; cin.clear(); cin.ignore(10000, '\n'); break;}
                cin.ignore(10000, '\n');
                buyItem(player, itemNum);
            } else if (choice == 2) {
                cout << colorBlue << "Enter inventory coordinates of item to sell (row col): " << colorReset;
                cin >> invRow >> invCol;
                if (cin.fail()) { cout << "Invalid coordinates\n"; cin.clear(); cin.ignore(10000, '\n'); break;}
                cin.ignore(10000, '\n');
                sellItem(player, invRow, invCol);
            } else if (choice == 3) {
                rerollStock(player);
            }
            if (choice != 0) {
                cout << colorBlue << "\nPress Enter to continue" << colorReset;
                cin.get();
            }
        }
    }
};

struct CraftingRecipe {
    string resultName;
    ItemType resultType;
    float resultPrice;
    int resultDurability;
    Rarities resultRarity;
    float resultDamage;
    float resultDefense;
    float resultHeal;
    int resultMatValue;
    int materialCost;
    float goldCost;
};
class Blacksmith {
private:
    vector<CraftingRecipe> blacksmithRecipes;

    void setupRecipes() {
        blacksmithRecipes.clear();
        blacksmithRecipes.push_back({"Iron Sword", weapon, 30.0f, 150, uncommon, 12.0f, 0.0f, 0.0f, 5, 30, 15.0f});
        blacksmithRecipes.push_back({"Iron Armor", armor, 40.0f, 200, uncommon, 0.0f, 10.0f, 0.0f, 6, 40, 20.0f});
        blacksmithRecipes.push_back({"Big Potion", potion, 35.0f, 1, uncommon, 0.0f, 0.0f, 60.0f, 0, 10, 10.0f});
        blacksmithRecipes.push_back({"Material Block", material, 10.0f, 1, common, 0.0f, 0.0f, 0.0f, 10, 10, 5.0f});
    }
public:
    Blacksmith() {
        setupRecipes();
    }

    void displayRecipes(Player* player) const {
        system("clear");
        cout << colorBlue << "====== Welcome to the Blacksmith! ======" << colorReset << endl;
        if(player) {
            cout << "Your Gold: " << player->getGold() << endl;
            cout << "Your Materials: " << player->getMaterials() << endl;
        }
        cout << colorBlue << "--------------------------------------" << colorReset << endl;
        cout << "Craftable Items:" << endl;
        if (blacksmithRecipes.empty()) {
            cout << "No recipes available" << endl;
        } else {
            for (size_t i = 0; i < blacksmithRecipes.size(); ++i) {
                const auto& r = blacksmithRecipes[i];
                cout << i + 1 << ". " << r.resultName << " (Type: " << r.resultType << ", Rarity: " << r.resultRarity << ")" << endl;
                cout << "   Cost: " << r.materialCost << " Materials, " << r.goldCost << " Gold" << endl;
                cout << colorBlue << "--------------------" << colorReset << endl;
            }
        }
        cout << colorBlue << "======================================" << colorReset << endl;
    }

    void craftItem(Player* player, int recipeIndex) {
        if (!player || !player->getInventory()) {
            cerr << "Invalid player or inventory in craftItem" << endl;
            return;
        }
        if (recipeIndex < 1 || recipeIndex > blacksmithRecipes.size()) {
            cout << "Invalid recipe number" << endl;
            return;
        }

        const CraftingRecipe& recipe = blacksmithRecipes[recipeIndex - 1];

        if (player->getMaterials() < recipe.materialCost) {
            cout << "Not enough materials" << endl;
            return;
        }
        if (player->getGold() < recipe.goldCost) {
            cout << "Not enough gold" << endl;
            return;
        }
        if (player->getInventory()->isFull()) {
            cout << "Your inventory is full" << endl;
            return;
        }
        Item* craftedItem = new Item(recipe.resultName, recipe.resultPrice, recipe.resultDurability,
                                     recipe.resultRarity, recipe.resultType, recipe.resultDamage,
                                     recipe.resultDefense, recipe.resultHeal, recipe.resultMatValue);

        if (player->getInventory()->addItem(craftedItem)) {
            player->addMaterials(-recipe.materialCost);
            player->addGold(-recipe.goldCost);
            cout << "Successfully crafted " << craftedItem->getName() << endl;
        } else {
            cout << "Failed to add crafted item to inventory" << endl;
            delete craftedItem;
        }
    }
    void enterBlacksmith(Player* player) {
        if (!player) {
            cerr << "Cannot enter blacksmith without player" << endl;
            return;
        }
        int choice = -1;
        int recipeNum;
        while (choice != 0) {
            displayRecipes(player);

            cout << colorBlue << "\n--- Blacksmith Menu ---" << colorReset << endl;
            cout << "1. Craft Item" << endl;
            cout << "0: Leave Blacksmith" << endl;
            cout << colorBlue << "Enter choice: " << colorReset;
            cin >> choice;

            if (cin.fail()) {
                cout << "Invalid input. Try again" << endl;
                cin.clear();
                cin.ignore(10000, '\n');
                choice = -1;
                continue;
            }
            cin.ignore(10000, '\n');

            if (choice == 1) {
                cout << colorBlue << "Enter recipe number to craft: " << colorReset;
                cin >> recipeNum;
                if (cin.fail()) { cout << "Invalid input\n"; cin.clear(); cin.ignore(10000, '\n'); break;}
                cin.ignore(10000, '\n');
                craftItem(player, recipeNum);
            }
            if (choice != 0) {
                cout << colorBlue << "\nPress Enter to continue" << colorReset;
                cin.get();
            }
        }
    }
};
class Game {
private:
    Player** gamePlayers;
    int gameSlots;
    Shop* gameShop;
    Blacksmith* gameBlacksmith;
    chrono::steady_clock::time_point lastGoldGainTime;
public:
    Game(int slots) : gameSlots{slots} {
        if (gameSlots <= 0) {
            cerr << "Game slots must be positive. Using 1 slot" << endl;
            this->gameSlots = 1;
        }
        gamePlayers = new Player*[this->gameSlots];
        for(int i = 0; i < this->gameSlots; i++) {
            gamePlayers[i] = nullptr;
        }

        gameShop = new Shop();
        gameBlacksmith = new Blacksmith();

        srand(time(0));
        lastGoldGainTime = chrono::steady_clock::now();
    }

    bool addPlayer(Player* player) {
        if (player == nullptr) {
            cerr << "Attempted to add a null player" << endl;
            return false;
        }
        for(int i = 0; i < gameSlots; i++) {
            if(gamePlayers[i] == nullptr) {
                gamePlayers[i] = player;
                cout << "Added player '" << player->getName() << "' to slot " << i << endl;
                return true;
            }
        }
        cout << "Player slots are full Cannot add " << player->getName() << endl;
        return false;
    }

    void displayMenu() {
        cout << colorBlue << "\n===== Game Menu =====" << colorReset << endl;
        cout << "1: View Players" << endl;
        cout << "2: Add Player" << endl;
        cout << "3: Pick Player" << endl;
        cout << "4: Show Player stats" << endl;
        cout << "5: Manage Inventory" << endl;
        cout << "6: Visit Shop" << endl;
        cout << "7: Visit Blacksmith" << endl;
        cout << "0: Exit Game" << endl;
        cout << colorBlue << "=====================" << colorReset << endl;
        cout << colorBlue << "Enter option: " << colorReset;
    }
    void play() {
        Player* currentPlayer = nullptr;
        int currentPlayerIndex = -1;
        int option;
        string inputName;

        if (gameSlots > 0 && gamePlayers[0] == nullptr) {
            cout << "Creating a default player" << endl;
            addPlayer(new Player("Player1"));
            if (gamePlayers[0] != nullptr) {
                currentPlayer = gamePlayers[0];
                currentPlayerIndex = 0;
                cout << "Welcome, " << currentPlayer->getName() << endl;
                cout << colorBlue << "\nPress Enter to continue" << colorReset;
                cin.get();
            } else {
                cerr << "Error creating default player" << endl;
            }
        } else if (gameSlots > 0 && gamePlayers[0] != nullptr) {
            currentPlayer = gamePlayers[0];
            currentPlayerIndex = 0;
            cout << "Welcome back, " << currentPlayer->getName() << endl;
            cout << colorBlue << "\nPress Enter to continue" << colorReset;
            cin.get();
        }


        while(true) {
            auto currentTime = chrono::steady_clock::now();
            auto elapsedSeconds = chrono::duration_cast<chrono::seconds>(currentTime - lastGoldGainTime).count();

            if (elapsedSeconds >= 10) {
                if (currentPlayer != nullptr) {
                    currentPlayer->addGold(10.0f);
                    cout << "\nGained 10 gold" << endl;
                }
                lastGoldGainTime = currentTime;
            }
            system("clear");

            if (currentPlayer) {
                cout << "Current Player: " << currentPlayer->getName() << endl;
                cout << "Gold: " << currentPlayer->getGold() << " | Materials: " << currentPlayer->getMaterials() << " | HP: " << currentPlayer->getHP() << "/" << currentPlayer->getMaxHP() << endl;
                cout << colorBlue << "-----------------------------" << colorReset << endl;
            }
            displayMenu();
            cin >> option;

            if (cin.fail()) {
                cout << "Invalid input. Please enter a number" << endl;
                cin.clear();
                cin.ignore(10000, '\n');
                cout << colorBlue << "\nPress Enter to continue" << colorReset;
                cin.get();
                continue;
            }
            cin.ignore(10000, '\n');

            switch(option) {
            case 1:
                viewPlayers();
                break;
            case 2:
                cout << colorBlue << "Enter name for new player: " << colorReset;
                getline(cin, inputName);
                if (!inputName.empty()) {
                    addPlayer(new Player(inputName));
                } else {
                    cout << "Player name cannot be empty" << endl;
                }
                break;
            case 3:
                viewPlayers();
                cout << colorBlue << "Enter index of player to select: " << colorReset;
                int indexToSelect;
                cin >> indexToSelect;
                if (cin.fail()) { cout << "Invalid index\n"; cin.clear(); cin.ignore(10000, '\n'); break;}
                cin.ignore(10000, '\n');
                if(indexToSelect >= 0 && indexToSelect < gameSlots && gamePlayers[indexToSelect] != nullptr)
                {
                    currentPlayer = gamePlayers[indexToSelect];
                    currentPlayerIndex = indexToSelect;
                    cout << "Selected player: " << currentPlayer->getName() << endl;
                }
                else {
                    cout << "Invalid player index or slot is empty" << endl;
                }
                break;
            case 4:
                if(currentPlayer == nullptr) {
                    cout << "Pick a player first" << endl;
                }
                else {
                    currentPlayer->showStats();
                }
                break;
            case 5:
                if(currentPlayer == nullptr) {
                    cout << "Pick a player first" << endl;
                }
                else {
                    cout << "Managing " << currentPlayer->getName() << "'s Inventory" << endl;
                    currentPlayer->manageInventory();
                }
                break;
            case 6:
                if (currentPlayer && gameShop) {
                    gameShop->enterShop(currentPlayer);
                } else if (!currentPlayer) {
                    cout << "No player selected" << endl;
                } else {
                    cerr << "Shop is not available" << endl;
                }
                break;
            case 7:
                if (currentPlayer && gameBlacksmith) {
                    gameBlacksmith->enterBlacksmith(currentPlayer);
                } else if (!currentPlayer) {
                    cout << "No player selected" << endl;
                } else {
                    cerr << "Blacksmith is not available" << endl;
                }
                break;
            case 0:
                cout << "Exiting game. Goodbye" << endl;
                return;
            default:
                cout << "Invalid option" << endl;
                break;
            }

            if (option != 0) {
                cout << colorBlue << "\nPress Enter to continue" << colorReset;
                cin.get();
            }
        }
    }

    void viewPlayers() {
        cout << colorBlue << "--- Player Slots ---" << colorReset << endl;
        for(int i = 0; i < gameSlots; i++) {
            cout << i << ": ";
            if(gamePlayers[i] != nullptr) {
                cout << gamePlayers[i]->getName();
            }
            else {
                cout << "[Empty]";
            }
            cout << endl;
        }
        cout << colorBlue << "--------------------" << colorReset << endl;
    }
    ~Game() {
        for(int i = 0; i < gameSlots; i++) {
            if(gamePlayers[i] != nullptr) {
                delete gamePlayers[i];
                gamePlayers[i] = nullptr;
            }
        }
        delete[] gamePlayers;
        gamePlayers = nullptr;

        delete gameShop;
        gameShop = nullptr;
        delete gameBlacksmith;
        gameBlacksmith = nullptr;

        cout << "Game object destroyed" << endl;
    }
};
int main()
{
    Game game(3);
    game.play();

    return 0;
}
