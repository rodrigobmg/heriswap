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

#include <base/PlacementHelper.h>
#include <base/TouchInputManager.h>

#include "systems/ButtonSystem.h"
#include "systems/SoundSystem.h"
#include "systems/TextRenderingSystem.h"
#include "systems/TransformationSystem.h"

#include <glm/glm.hpp>

struct HelpScene : public StateHandler<Scene::Enum> {
    HeriswapGame* game;

    // State variables
    Scene::Enum oldState;
    Entity title,text,postscriptum;

    enum State {
        HowToPlay,
        Objective
    } state;

    HelpScene(HeriswapGame* game) : StateHandler<Scene::Enum>() {
        this->game = game;
    }

    void setup() {
        const Color green("green");

        title = theEntityManager.CreateEntity("title",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("help/title"));

        // title text + bg
        text = theEntityManager.CreateEntity("text",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("help/text"));
        
        postscriptum = theEntityManager.CreateEntity("postscriptum",
            EntityType::Persistent, theEntityManager.entityTemplateLibrary.load("help/postscriptum"));
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- ENTER SECTION ----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreEnter(Scene::Enum) override {
    }

    void onEnter(Scene::Enum oldState) override {
        LOGI("'" << __PRETTY_FUNCTION__ << "'");

        state = HowToPlay;
        if (oldState == Scene::ModeMenu)
            this->oldState = Scene::BlackToSpawn;
        else
            this->oldState = oldState;

        // TODO !
        // setup how to play help page
        TEXT_RENDERING(text)->show = true;
        TEXT_RENDERING(title)->show = true;
        TEXT_RENDERING(postscriptum)->show = true;
        if (game->datas->mode == Normal) {
            TEXT_RENDERING(text)->text = game->gameThreadContext->localizeAPI->text("help_mode1_1") + "\n\n" +
                game->gameThreadContext->localizeAPI->text("help_general_1");
            TEXT_RENDERING(title)->text = game->gameThreadContext->localizeAPI->text("mode_1");
            TEXT_RENDERING(postscriptum)->text = game->gameThreadContext->localizeAPI->text("help_click_continue");
        } else if (game->datas->mode == TilesAttack) {
            TEXT_RENDERING(title)->text = game->gameThreadContext->localizeAPI->text("mode_2");
            TEXT_RENDERING(text)->text = game->gameThreadContext->localizeAPI->text("help_mode2_1");
            TEXT_RENDERING(postscriptum)->text = game->gameThreadContext->localizeAPI->text("help_click_continue");
        } else {
            TEXT_RENDERING(title)->text = game->gameThreadContext->localizeAPI->text("mode_3");
            TEXT_RENDERING(text)->text = game->gameThreadContext->localizeAPI->text("help_mode3_1") + "\n\n" +
                game->gameThreadContext->localizeAPI->text("help_general_1");
            TEXT_RENDERING(postscriptum)->text = game->gameThreadContext->localizeAPI->text("help_click_continue");
        }

        game->datas->mode2Manager[game->datas->mode]->showGameDecor(true);
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- UPDATE SECTION ---------------------------------------//
    ///----------------------------------------------------------------------------//
    Scene::Enum update(float) override {
        if (!theTouchInputManager.isTouched(0) && theTouchInputManager.wasTouched(0)) {
            if (state == HowToPlay) {
                if (game->datas->mode == Normal) {
                    TEXT_RENDERING(text)->text = game->gameThreadContext->localizeAPI->text("help_general_2") + " " +
                        game->gameThreadContext->localizeAPI->text("help_mode1_2");
                } else if (game->datas->mode == TilesAttack) {
                    TEXT_RENDERING(text)->text = game->gameThreadContext->localizeAPI->text("help_mode2_2");
                } else {
                    TEXT_RENDERING(text)->text = game->gameThreadContext->localizeAPI->text("help_general_2");
                }

                TEXT_RENDERING(postscriptum)->text = game->gameThreadContext->localizeAPI->text("help_click_play");
                state = Objective;

            } else {
                return oldState;
            }
        }
        return Scene::Help;
    }

    ///----------------------------------------------------------------------------//
    ///--------------------- EXIT SECTION -----------------------------------------//
    ///----------------------------------------------------------------------------//
    void onPreExit(Scene::Enum) override {
    }

    void onExit(Scene::Enum) override {
        LOGI("'" << __PRETTY_FUNCTION__ << "'");
        TEXT_RENDERING(text)->show = false;
        TEXT_RENDERING(title)->show = false;
        TEXT_RENDERING(postscriptum)->show = false;
        theRenderingSystem.unloadAtlas("help");
    }
};

namespace Scene {
    StateHandler<Scene::Enum>* CreateHelpSceneHandler(HeriswapGame* game) {
        return new HelpScene(game);
    }
}