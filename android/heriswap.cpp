/*
	This file is part of Heriswap.

	@author Soupe au Caillou - Pierre-Eric Pelloux-Prayer
	@author Soupe au Caillou - Gautier Pelloux-Prayer

	Heriswap is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, version 3.

	Heriswap is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Heriswap.  If not, see <http://www.gnu.org/licenses/>.
*/
/* DO NOT EDIT THIS FILE - it is machine generated */
#include "sac/android/sacjnilib.h"
#include "../sources/HeriswapGame.h"
#include "../sources/api/android/CommunicationAPIAndroidImpl.h"
#include "../sources/api/android/StorageAPIAndroidImpl.h"

class HeriswapGameThreadJNIEnvCtx : public GameThreadJNIEnvCtx {
	public:
	CommunicationAPIAndroidImpl communication;
    StorageAPIAndroidImpl storage;
    
    void init(JNIEnv* pEnv, jobject assetMgr) {
	    communication.init(pEnv);
	    storage.init(pEnv);
	    GameThreadJNIEnvCtx::init(pEnv, assetMgr);
    }
    
    void uninit(JNIEnv* pEnv) {
		if (env == pEnv) {
		    communication.uninit();
		    storage.uninit();
		}
		GameThreadJNIEnvCtx::uninit(pEnv);
    }
};

GameHolder* GameHolder::build() {
	GameHolder* hld = new GameHolder();
	
	HeriswapGameThreadJNIEnvCtx* jniCtx = new HeriswapGameThreadJNIEnvCtx();
	hld->gameThreadJNICtx = jniCtx;
	
	hld->game = new HeriswapGame(&hld->renderThreadJNICtx.asset,
		&jniCtx->storage,
		&hld->gameThreadJNICtx->nameInput,
		&hld->gameThreadJNICtx->successAPI,
		&hld->gameThreadJNICtx->localize,
		&hld->gameThreadJNICtx->ad,
		&hld->gameThreadJNICtx->exitAPI,
		&jniCtx->communication,
        &hld->gameThreadJNICtx->vibrateAPI);
	return hld;
};
