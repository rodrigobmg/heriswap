/*
This file is part of RecursiveRunner.

@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
@author Soupe au Caillou - Gautier Pelloux-Prayer

RecursiveRunner is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, version 3.

RecursiveRunner is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RecursiveRunner.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "base/StateMachine.h"

#include "Scenes.h"

#include "Game_Private.h"
#include "HeriswapGame.h"

#include "DepthLayer.h"

#include "base/PlacementHelper.h"

#include "systems/AnimationSystem.h"
#include "systems/TextSystem.h"
#include "systems/TransformationSystem.h"

#include <sstream>

struct CountDownScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    // State variables
    Entity counter;
    Entity vorhang;
    float timeRemaining;

    CountDownScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
        this->game = game;
    }

    void setup() {
        counter = theEntityManager.CreateEntity("counter",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("counter"));

        vorhang = theEntityManager.CreateEntity("vorhang",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("vorhang"));
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
    }

    void onEnter(Scene::Enum) override {
        if (game->datas->mode != Normal) {
            TEXT(counter)->show = true;
            RENDERING(vorhang)->show = true;

            if (game->datas->mode == TilesAttack || game->datas->mode == Go100Seconds) {
                Entity herisson = game->datas->mode2Manager[game->datas->mode]->herisson;
                ANIMATION(herisson)->playbackSpeed = 0;
            }
        }
        timeRemaining = 3.f;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float dt) override {
        return Scene::Spawn;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum) override {
    }

    bool updatePreExit(Scene::Enum , float dt) {
        if (game->datas->mode == Normal)
            return true;

        timeRemaining -= dt;

        std::stringstream a;
        a << (int)timeRemaining+1;
        TEXT(counter)->text = a.str();

        return (timeRemaining <= 0.f);
    }

    void onExit(Scene::Enum) override {
        TEXT(counter)->show = false;
        RENDERING(vorhang)->show = false;
        if (game->datas->mode == TilesAttack || game->datas->mode == Go100Seconds) {
            Entity herisson = game->datas->mode2Manager[game->datas->mode]->herisson;
            ANIMATION(herisson)->playbackSpeed = 4.1;
        }
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateCountDownSceneHandler(HeriswapGame* game) {
        return new CountDownScene(game);
    }
}
