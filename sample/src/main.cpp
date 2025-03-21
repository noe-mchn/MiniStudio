#include "SceneManager.h"
#include "SFML/Graphics.hpp"
#include "Game.h"
#include <GameObject.h>

//TODO
int main(int argc, char** argv) {
    SceneManager a(argv[0], 1920, 1080, "CyberpunkShooter"/*,sf::Style::Fullscreen*/);
    a.AddScene(new Game(a.getWindow(),240,a.geTextureCash()));
    a.Exe();
}
