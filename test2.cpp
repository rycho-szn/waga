#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <jsoncpp/json/json.h> 
#include "HX711.h"

struct Ingredient {
    std::string name;
    float weight; 
};

struct Recipe {
    std::string name;
    std::vector<Ingredient> ingredients;
};

std::vector<Recipe> loadRecipes(const std::string& filename) {
    std::vector<Recipe> recipes;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Nie można otworzyć pliku " << filename << std::endl;
        return recipes;
    }

    Json::Value root;
    file >> root;

    for (const auto& item : root) {
        Recipe recipe;
        recipe.name = item["name"].asString();
        for (const auto& ing : item["ingredients"]) {
            Ingredient ingredient;
            ingredient.name = ing["name"].asString();
            ingredient.weight = ing["weight"].asFloat();
            recipe.ingredients.push_back(ingredient);
        }
        recipes.push_back(recipe);
    }

    file.close();
    return recipes;
}




void calibrate(HX711& scale) {
    std::cout << "Połóż produkt i podaj jego wagę (in grams): ";
    float knownWeight;
    std::cin >> knownWeight;

    float reading = scale.get_units(10);
    scale.set_scale(reading / knownWeight);

    std::cout << "Kalibracja zakończona. Wspólczynnik wagi ustawiony na " << reading / knownWeight << std::endl;
}

void tare(HX711& scale) {
    std::cout << "Tarowanie... Usuń produkty z wagi." << std::endl;
    delay(2000);
    scale.tare();
    std::cout << "Tarowanie zakończone." << std::endl;
}

void weighItems(const Recipe& recipe, HX711& scale) {
    // for (const auto& ingredient : recipe.ingredients) {
    //     std::cout << "Ważenie " << ingredient.name << ". Wymagana waga: " << ingredient.weight << " grams." << std::endl;
    //     float weight = 0;
    //     while (weight < ingredient.weight) {
    //         weight = scale.get_units(10);
    //         std::cout << "Na wadze: " << weight << " grams. Proszę dodaj produktu " << ingredient.name << "." << std::endl;
    //         delay(1000); 
    //     }
    //     std::cout << "Docelowa waga dla " << ingredient.name << " osiągnięta." << std::endl;
    // }
    for (const auto& ingredient : recipe.ingredients) {
        std::cout << "Ważenie " << ingredient.name << ". Wymagana waga: " << ingredient.weight << " grams." << std::endl;
        float weight = 0;
        while (true) {
            weight = scale.get_units(10);
            std::cout << "Na wadze: " << weight << " grams." << std::endl;
            
            float lower_bound = ingredient.weight * 0.9;
            float upper_bound = ingredient.weight * 1.1;
            
            if (weight < lower_bound) {
                std::cout << "Proszę dodaj więcej produktu " << ingredient.name << "." << std::endl;
            } else if (weight > upper_bound) {
                std::cout << "Za dużo produktu " << ingredient.name << ". Proszę odjąć trochę." << std::endl;
            } else {
                std::cout << "Docelowa waga dla " << ingredient.name << " osiągnięta." << std::endl;
                break;
            }
            
            delay(1000); 
        }
        // tareScale(scale); // Tarowanie wagi po zważeniu każdego składnika
    }
}

void weighSingleItem(HX711& scale) {
    std::cout << "Połóż produkt na wadze." << std::endl;
    float weight = scale.get_units(10);
    std::cout << "Waga produktu wynosi: " << weight << " grams." << std::endl;
}

int main() {
    HX711 scale(5, 6); // GPIO 5 for DOUT, GPIO 6 for PD_SCK

    std::vector<Recipe> recipes = loadRecipes("recipes.json");
    if (recipes.empty()) {
        std::cerr << "Nie załadowano przepisów.Wyjście" << std::endl;
        return 1;
    }

    int choice;
    while (true) {
        std::cout << "Menu:\n1. Kalibracja\n2. Tarowanie\n3. Ważenie wg przepisu\n4. Szybkie ważenie\n5. Wyjście\nWybierz opcję: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                calibrate(scale);
                break;
            case 2:
                tare(scale);
                break;
            case 3: {
                std::cout << "Wybierz przepis:" << std::endl;
                for (size_t i = 0; i < recipes.size(); ++i) {
                    std::cout << i + 1 << ". " << recipes[i].name << std::endl;
                }
                int recipe_choice;
                std::cin >> recipe_choice;
                if (recipe_choice > 0 && recipe_choice <= recipes.size()) {
                    weighItems(recipes[recipe_choice - 1], scale);
                } else {
                    std::cout << "Nieprawidłowy wybór." << std::endl;
                }
                break;
            }
            case 4:
                weighSingleItem(scale);
                break;
            case 5:
                return 0;
            default:
                std::cout << "Nieprawudłowa opcja. Spróbuj ponownie." << std::endl;
        }
    }

    return 0;
}

