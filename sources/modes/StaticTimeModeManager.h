#pragma once

#include "systems/ContainerSystem.h"
#include "systems/ButtonSystem.h"

#include "GameModeManager.h"
#include "DepthLayer.h"
#include "PlacementHelper.h"

class StaticTimeGameModeManager : public GameModeManager {
	public:
		StaticTimeGameModeManager();
		~StaticTimeGameModeManager();
		void Setup();
		bool Update(float dt);

		void UpdateUI(float dt);
		void HideUI(bool toHide);

		void LevelUp();
		//permet de savoir si on a change de niveau
		bool LeveledUp();

		GameMode GetMode();

		void ScoreCalc(int nb, int type);
		void Reset();

	private:
		int bonus;
};
