#include "GridSystem.h"


INSTANCE_IMPL(GridSystem);

GridSystem::GridSystem() : ComponentSystemImpl<GridComponent>("Grid") {
	GridSize=8;
	nbmin=3;
}

void GridSystem::print() {
	for(int j=GridSize-1; j>=0; j--) {
		for(int i=0; i<GridSize; i++) {
			Entity e = GetOnPos(i, j);
			if (e) {
				char t = GRID(e)->type;
				std::cerr << t << " ";
			} else
				std::cerr << "_ ";
		}
		std::cerr << std::endl;
	}
}

void GridSystem::HideAll(bool activate) {
	for(ComponentIt it=components.begin(); it!=components.end(); ++it) {
		Entity e = (*it).first;
		RENDERING(e)->hide = activate;
	}
}

void GridSystem::DeleteAll() {
	for(ComponentIt it=components.begin(); it!=components.end(); ++it) {
		Entity e = (*it).first;
		theEntityManager.DeleteEntity(e);
	}
}

Entity GridSystem::GetOnPos(int i, int j) {
	for(ComponentIt it=components.begin(); it!=components.end(); ++it) {
		Entity a = (*it).first;
		GridComponent* bc = (*it).second;
		if (bc->i == i && bc->j == j)
			return a;
	}

	//std::cout << "Aucun element en position (" << i << ","<<j<<")\n";
	return 0;
}

void GridSystem::ResetTest() {
	for(ComponentIt it=components.begin(); it!=components.end(); ++it) {
		Entity a = (*it).first;
		GridComponent* bc = (*it).second;
		bc->checkedH = false;
		bc->checkedV = false;
	}
}

bool GridSystem::Intersec(std::vector<Vector2> v1, std::vector<Vector2> v2){
	for ( size_t i = 0; i < v1.size(); ++i ) {
		for ( size_t j = 0; j < v2.size(); ++j ) {
			if (v1[i] == v2[j])
				return true;
		}
	}
	return false;
}

bool GridSystem::InVect(std::vector<Vector2> v1, Vector2 v2){
	for ( size_t i = 0; i < v1.size(); ++i ) {
		if (v1[i] == v2)
			return true;
	}
	return false;
}

Combinais GridSystem::MergeVectors(Combinais c1, Combinais c2) {
	Combinais merged;
	merged = c1;
	for (size_t i=0; i<c2.points.size();i++) {
		if (!InVect(c1.points,c2.points[i]))
			merged.points.push_back(c2.points[i]);
	}
	return merged;
}

std::vector<Combinais> GridSystem::MergeCombination(std::vector<Combinais> combinaisons) {
	std::vector<Combinais> combinmerged;

	for ( size_t i = 0; i < combinaisons.size(); ++i ) {
		int match = -1;
		for ( size_t j = i+1; j < combinaisons.size(); ++j ) {
			if (combinaisons[i].type != combinaisons[j].type || !Intersec(combinaisons[i].points, combinaisons[j].points)) {
				continue;
			} else {
				match = j;
				combinaisons[j] = MergeVectors(combinaisons[i],combinaisons[j]);
			}
		}
		if (match==-1)
			combinmerged.push_back(combinaisons[i]);
	}
	return combinmerged;
}

std::vector<Combinais> GridSystem::LookForCombination(bool markAsChecked, bool useChecked) {
	std::vector<Combinais> combinaisons;

	for(ComponentIt it=components.begin(); it!=components.end(); ++it) {
		Entity a = (*it).first;
		GridComponent* gc = (*it).second;
		int i=gc->i;
		int j=gc->j;
		Combinais potential;
		potential.type = gc->type;

		/*Check on j*/
		if (!useChecked || !gc->checkedV) {
			Combinais potential;
			potential.type = gc->type;
			/*Looking for twins on the bottom of the point*/
			int k=j;
			while (k>-1){
				Entity next = GetOnPos(i,k);

				if (!next || GRID(next)->type != gc->type) {
					k=-2;
				} else {
					/*Useless to check them later : we already did it now*/
					potential.points.push_back(Vector2(i,k));
					if (markAsChecked) GRID(next)->checkedV = true;
					k--;
				}
			}

			/* Then on the top*/
			k = j+1;
			while (k<GridSize){
				Entity next = GetOnPos(i,k);



				if (!next || GRID(next)->type != gc->type){
					k=GridSize;
				} else {
					if (markAsChecked) GRID(next)->checkedV = true;
					potential.points.push_back(Vector2(i,k));
					k++;
				}
			}


			/*If there is at least 1 more cell
			 * We add it to the solutions*/

			if (potential.points.size()>=nbmin){
				combinaisons.push_back(potential);
			}

			if (markAsChecked) gc->checkedV = true;
		}

		/*Check on i*/
		if (!useChecked || !gc->checkedH) {
			Combinais potential;
			potential.type = gc->type;

			/*Looking for twins on the left of the point*/
			int k=i;
			while (k>-1){
				Entity next = GetOnPos(k,j);

				if (!next || GRID(next)->type != gc->type) {
					k=-2;
				} else {
					/*Useless to check them later : we already did it now*/
					if (markAsChecked) GRID(next)->checkedH = true;
					potential.points.push_back(Vector2(k,j));
					k--;
				}
			}

			/* Then on the right*/
			k = i+1;
			while (k<GridSize){
				Entity next = GetOnPos(k,j);

				if (!next || GRID(next)->type != gc->type) {
					k=(GridSize+1);
				} else {
					if (markAsChecked) GRID(next)->checkedH = true;
					potential.points.push_back(Vector2(k,j));
					k++;
				}
			}


			/*If there is at least 1 more cell
			 * We add it to the solutions
			 * longueurCombi < 0 <-> Horizontale */

			if (potential.points.size()>=nbmin){
				combinaisons.push_back(potential);
			}

			if (markAsChecked) gc->checkedH = true;
		}


	}

	return MergeCombination(combinaisons);
}

std::vector<CellFall> GridSystem::TileFall() {
	std::vector<CellFall> result;

	for (int i=0; i<GridSize; i++) {
		for (int j=0; j<GridSize; j++) {
			/* if call is empty, find nearest non empty cell above*/
			if (!GetOnPos(i,j)){
				int k=j+1;
				while (k<GridSize){
					Entity e = GetOnPos(i, k);
					if (e) {
						int fallHeight = k - j;
						while (k < GridSize) {
							Entity e = GetOnPos(i, k);
							if (e)
								result.push_back(CellFall(e, i, k, k - fallHeight));
							else fallHeight++;
							k++;
						}
						break;
					} else {
						k++;
					}
				}
				/* only one fall possible per column */
				break;
			}
		}
	}
	return result;
}
void GridSystem::DoUpdate(float dt) {

}


bool GridSystem::NewCombiOnSwitch(Entity a, int i, int j) {
	//test right and top
	Entity e = GetOnPos(i+1,j);
	if (e) {
		GRID(e)->i--;
		GRID(a)->i++;
		std::vector<Combinais> combin = LookForCombination(false,false);
		GRID(e)->i++;
		GRID(a)->i--;
		if (combin.size()>0) return true;
	}
	e = GetOnPos(i,j+1);
	if (e) {
		GRID(e)->j--;
		GRID(a)->j++;
		std::vector<Combinais> combin = LookForCombination(false,false);
		GRID(e)->j++;
		GRID(a)->j--;
		if (combin.size()>0) return true;
	}
	return false;
}


bool GridSystem::StillCombinations() {
	std::vector<Combinais> combin = LookForCombination(false,false);
	if (combin.size()>0) return true;

	for(ComponentIt it=components.begin(); it!=components.end(); ++it) {
		if (NewCombiOnSwitch(it->first,it->second->i,it->second->j)) return true;
	}
	return false;
}

bool GridSystem::Egal(Combinais c1, Combinais c2) {
	if (c1.points.size() != c2.points.size()) return false;
	if (c1.type != c2.type) return false;

	bool match = false;
	for (std::vector<Vector2>::reverse_iterator it = c1.points.rbegin(); it != c1.points.rend(); ++it) {
		bool match = false;
		for (std::vector<Vector2>::reverse_iterator it2 = c2.points.rbegin(); !match && it2 != c2.points.rend(); ++it2) {
			if (it->X == it2->X && it->Y == it2->Y)
				match = true;
		}
		if (!match) return false;
	}
	return true;
}

bool GridSystem::EgalVec(std::vector<Combinais> v1, std::vector<Combinais> v2) {
	if (v1.size()!=v2.size()) return false;

	for (std::vector<Combinais>::iterator it = v1.begin(); it != v1.end();) {
		for (std::vector<Combinais>::iterator it2 = v2.begin(); it2 != v2.end();) {
			if (Egal(*it,*it2))
				it2 = v2.erase(it2);
			else
				return false;
			it2++;
		}
		it++;
	}
	return true;
}

std::vector<Combinais> GridSystem::Diff(std::vector<Combinais> v1, std::vector<Combinais> v2) {
	std::vector<Combinais> v;
	for (std::vector<Combinais>::reverse_iterator it = v1.rbegin(); it != v1.rend(); ++it) {
		bool single = true;
		for (std::vector<Combinais>::reverse_iterator it2 = v2.rbegin(); single && it2 != v2.rend(); ++it2) {
			if (Egal(*it,*it2)) single = false;
		}
		if (single)
			v.push_back(*it);
	}
	return v;
}

std::vector<Vector2> GridSystem::LookForCombinationsOnSwitchVertical() {
	std::vector<Combinais> combinaisonsAvailable = LookForCombination(false,false), combinaisons;
	std::vector<Vector2> combin;

	for (int i=0; i<GridSize; i++) {
		for (int j=0; j<GridSize; j++) {
			Entity a = GetOnPos(i,j);
			Entity e = GetOnPos(i,j+1);
			if (e) {
				GRID(e)->j--;
				GRID(a)->j++;
				combinaisons = LookForCombination(false,false);
				combinaisons=Diff(combinaisons,combinaisonsAvailable);


				if (combinaisons.size()>0)
					combin.push_back(Vector2(i, j));
				GRID(e)->j++;
				GRID(a)->j--;
			}

		}
	}
	return combin;
}

std::vector<Vector2> GridSystem::LookForCombinationsOnSwitchHorizontal() {
	std::vector<Combinais> combinaisonsAvailable = LookForCombination(false,false), combinaisons;
	std::vector<Vector2> combin;

	for (int i=0; i<GridSize; i++) {
		for (int j=0; j<GridSize; j++) {
			Entity a = GetOnPos(i,j);
			Entity e = GetOnPos(i+1,j);
			if (e) {
				GRID(e)->i--;
				GRID(a)->i++;
				combinaisons = LookForCombination(false,false);
				combinaisons=Diff(combinaisons,combinaisonsAvailable);
				if (combinaisons.size()>0)
					combin.push_back(Vector2(i, j));
				GRID(e)->i++;
				GRID(a)->i--;
			}
		}
	}
	return combin;
}
