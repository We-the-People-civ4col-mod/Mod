
#include "CvGameCoreDLL.h"
#include "KmodPathFinder.h"
#include "CvGame.h"
#include "CvTeamAI.h"
#include "CvInfos.h"
#include "CvSelectionGroup.h"
#include "CvMap.h"

int KmodPathFinder::admissible_scaled_weight = 1;
int KmodPathFinder::admissible_base_weight = 1;

CvPathSettings::CvPathSettings(const CvSelectionGroup* pGroup, int iFlags, int iMaxPath, int iHW)
	: pGroup(const_cast<CvSelectionGroup*>(pGroup)), iFlags(iFlags), iMaxPath(iMaxPath), iHeuristicWeight(iHW)
// I'm really sorry about the const_cast. I can't fix the const-correctness of all the relevant functions,
// because some of them are dllexports. The original code essentially does the same thing anyway, with void* casts.
{
	// empty. (there use to be something here, but we don't need it anymore.)
}

void KmodPathFinder::InitHeuristicWeights()
{
	admissible_base_weight = GC.getMOVE_DENOMINATOR()/2;
	admissible_scaled_weight = GC.getMOVE_DENOMINATOR()/2;
	for (int r = 0; r < GC.getNumRouteInfos(); r++)
	{
		const CvRouteInfo& kInfo = GC.getRouteInfo((RouteTypes)r);
		const int iCost = kInfo.getMovementCost();
		// WTP: No tech tree yet :(
		/*
		for (int t = 0; t < GC.getNumTechInfos(); t++)
		{
			if (kInfo.getTechMovementChange(t) < 0)
				iCost += kInfo.getTechMovementChange(t);
		}
		*/
		admissible_base_weight = std::min(admissible_base_weight, iCost);
		admissible_scaled_weight = std::min(admissible_scaled_weight, kInfo.getFlatMovementCost());
	}
}

int KmodPathFinder::MinimumStepCost(int BaseMoves)
{
	return std::max(1, std::min(admissible_base_weight, BaseMoves * admissible_scaled_weight));
}

bool KmodPathFinder::OpenList_sortPred::operator()(const FAStarNode* &left, const FAStarNode* &right)
{
	return left->m_iTotalCost < right->m_iTotalCost;
}

//
KmodPathFinder::KmodPathFinder() :
	end_node(0),
	map_width(0),
	map_height(0),
	node_data(0)
{
	// Unfortunately, the pathfinder is constructed before the map width and height are determined.

	// Ideally the pathfinder would be initialised with a given CvMap, and then not refer to any global objects.
	// But that is not easy to do with the current code-base. Instead I'll just check the pathfinder settings
	// at particular times to make sure the map hasn't changed.

}

KmodPathFinder::~KmodPathFinder()
{
	free(node_data);
}

bool KmodPathFinder::ValidateNodeMap()
{
	//PROFILE_FUNC(); // advc.003o
	if (!GC.getGame().isFinalInitialized())
		return false;

	if (map_width != GC.getMap().getGridWidth() || map_height != GC.getMap().getGridHeight())
	{
		map_width = GC.getMap().getGridWidth();
		map_height = GC.getMap().getGridHeight();
		//node_data = (FAStarNode*)
		// <advc> According to cppcheck, the above is a "common realloc mistake".
		FAStarNode* new_node_data = static_cast<FAStarNode*>(
				realloc(node_data, sizeof(*node_data)*map_width*map_height));
		if(new_node_data == NULL) {
			free(node_data);
			FAssertMsg(new_node_data != NULL, "Failed to re-allocate memory");
		}
		else node_data = new_node_data; // </advc>
		end_node = NULL;
	}
	return true;
}

bool KmodPathFinder::GeneratePath(int x1, int y1, int x2, int y2)
{
	PROFILE_FUNC();

	FAssertBounds(0, map_width , x1);
	FAssertBounds(0, map_height, y1);
	FAssertBounds(0, map_width , x2);
	FAssertBounds(0, map_height, y2);

	end_node = NULL;

	if (!settings.pGroup)
		return false;

	if (!pathDestValid(x2, y2, &settings, 0))
		return false;

	if (x1 != start_x || y1 != start_y)
	{
		// Note: it may be possible to salvage some of the old data to get more speed.
		// eg. If the moves recorded on the node match the group,
		// just delete everything that isn't a direct descendant of the new start.
		// and then subtract the start cost & moves off all the remaining nodes.
		Reset(); // but this is easier.
	}

	bool bRecalcHeuristics = false;
	if (dest_x != x2 || dest_y != y2)
		bRecalcHeuristics = true;

	start_x = x1;
	start_y = y1;
	dest_x = x2;
	dest_y = y2;

	if (GetNode(x1, y1).m_bOnStack)
	{
		// WTP: MOVE_MAX_MOVES not supported yet
		//int iMoves = (settings.iFlags & MOVE_MAX_MOVES) ? settings.pGroup->maxMoves() : settings.pGroup->movesLeft();
		int iMoves = settings.pGroup->movesLeft();
		if (iMoves != GetNode(x1, y1).m_iData1)
		{
			Reset();
			FAssert(!GetNode(x1, y1).m_bOnStack);
		}
		// Note: This condition isn't actually enough to catch all significant changes.
		// We really need to check max moves /and/ moves left /and/ base moves.
		// but I don't feel like doing all that at the moment.
	}

	if (!GetNode(x1, y1).m_bOnStack)
	{
		AddStartNode();
		bRecalcHeuristics = true;
	}
	//else (not else. maybe start == dest)
	{
		// check if the end plot is already mapped.
		if (GetNode(x2, y2).m_bOnStack)
			end_node = &GetNode(x2, y2);
	}

	if (bRecalcHeuristics)
		RecalculateHeuristics();

	while (ProcessNode())
	{
		// nothing
	}

	if (end_node && (settings.iMaxPath < 0 || end_node->m_iData2 <= settings.iMaxPath))
		return true;

	return false;
}

bool KmodPathFinder::GeneratePath(const CvPlot* pToPlot)
{
	if (!settings.pGroup || !pToPlot)
		return false;
	return GeneratePath(settings.pGroup->plot()->getX(), settings.pGroup->plot()->getY(),
		pToPlot->getX(), pToPlot->getY());
}

int KmodPathFinder::GetPathTurns() const
{
	FAssert(end_node);
	return end_node ? end_node->m_iData2 : 0;
}

int KmodPathFinder::GetFinalMoves() const
{
	FAssert(end_node);
	return end_node ? end_node->m_iData1 : 0;
}

CvPlot* KmodPathFinder::GetPathFirstPlot() const
{
	FAssert(end_node);
	if (!end_node)
		return NULL;

	FAStarNode* node = end_node;

	if (!node->m_pParent)
		return GC.getMap().plotSoren(node->m_iX, node->m_iY);

	while (node->m_pParent->m_pParent)
	{
		node = node->m_pParent;
	}

	return GC.getMap().plotSoren(node->m_iX, node->m_iY);
}

CvPlot* KmodPathFinder::GetPathEndTurnPlot() const
{
	FAssert(end_node);

	FAStarNode* node = end_node;

	FAssert(!node || node->m_iData2 == 1 || node->m_pParent);

	while (node && node->m_iData2 > 1)
	{
		node = node->m_pParent;
	}
	FAssert(node);
	return node ? GC.getMap().plotSoren(node->m_iX, node->m_iY) : NULL;
}

void KmodPathFinder::SetSettings(const CvPathSettings& new_settings)
{
	// whenever settings are changed, check that we have the right map size.
	if (!ValidateNodeMap())
	{
		FAssertMsg(false, "Failed to validate node map for pathfinder.");
		settings.pGroup = NULL;
		return;
	}

	// some flags are not relevant to pathfinder. We should try to strip those out to avoid unnecessary resets.
	int relevant_flags = ~(MOVE_DIRECT_ATTACK | MOVE_SINGLE_ATTACK | MOVE_NO_ATTACK | MOVE_DIRECT_RAID); // any bar these

	if (settings.pGroup != new_settings.pGroup)
	{
		Reset();
	}
	else if ((settings.iFlags & relevant_flags) != (new_settings.iFlags & relevant_flags))
	{
		// there's one more chance to avoid a reset..
		// If the war flag is the only difference, and we aren't sneak attack ready anyway - then there is effectively no difference.
		relevant_flags &= ~MOVE_DECLARE_WAR;
		if ((settings.iFlags & relevant_flags) != (new_settings.iFlags & relevant_flags)
			|| GET_TEAM(settings.pGroup->getHeadTeam()).AI_isSneakAttackReady())
		{
			Reset();
		}
	}
	settings = new_settings;

	if (settings.iHeuristicWeight < 0)
	{
		if (!settings.pGroup)
		{
			settings.iHeuristicWeight = 1;
		}
		else
		{
			if (settings.pGroup->getDomainType() == DOMAIN_SEA)
			{
				// this assume there are no sea-roads, or promotions to reduce sea movement cost.
				settings.iHeuristicWeight = GC.getMOVE_DENOMINATOR();
			}
			else
			{
				settings.iHeuristicWeight = MinimumStepCost(settings.pGroup->baseMoves());
			}
		}
	}
}

void KmodPathFinder::Reset()
{
	memset(&node_data[0], 0, sizeof(*node_data)*map_width*map_height);
	open_list.clear();
	end_node = NULL;
	// settings is set separately.
}

void KmodPathFinder::AddStartNode()
{
	FAssertBounds(0, map_width , start_x);
	FAssertBounds(0, map_height, start_y);

	// add initial node.
	FAStarNode* start_node = &GetNode(start_x, start_y);
	start_node->m_iX = start_x;
	start_node->m_iY = start_y;
	pathAdd(0, start_node, ASNC_INITIALADD, &settings, 0);
	start_node->m_iKnownCost = 0;

	open_list.push_back(start_node);

	// Note: I'd like to use NO_FASTARLIST as a signal that the node is uninitialised, but unfortunately
	// the default value for m_eFAStarListType is FASTARLIST_OPEN, because FASTARLIST_OPEN == 0.
	// Hence KmodPathFinder uses m_bOnStack to mean the node is connected. m_eFAStarListType is initialised manually.

	start_node->m_eFAStarListType = FASTARLIST_OPEN; // This means nothing. But maybe one day I'll use it.
	start_node->m_bOnStack = true; // This means the node is connected and ready to be used.
}

void KmodPathFinder::RecalculateHeuristics()
{
	// recalculate heuristic cost for all open nodes.
	for (OpenList_t::iterator i = open_list.begin(); i != open_list.end(); ++i)
	{
		int h = settings.iHeuristicWeight * pathHeuristic((*i)->m_iX, (*i)->m_iY, dest_x, dest_y);
		(*i)->m_iHeuristicCost = h;
		(*i)->m_iTotalCost = h + (*i)->m_iKnownCost;
	}
}

bool KmodPathFinder::ProcessNode()
{
	OpenList_t::iterator best_it = open_list.end();
	{
		int iLowestCost = end_node ? end_node->m_iKnownCost : MAX_INT;
		for (OpenList_t::iterator it = open_list.begin(); it != open_list.end(); ++it)
		{
			if ((*it)->m_iTotalCost < iLowestCost &&
				(settings.iMaxPath < 0 || (*it)->m_iData2 <= settings.iMaxPath))
			{
				best_it = it;
				iLowestCost = (*it)->m_iTotalCost;
			}
		}
	}

	// if we didn't find a suitable node to process, then quit.
	if (best_it == open_list.end())
		return false;

	FAStarNode* parent_node = (*best_it);

	// erase the node from the open_list.
	// Note: this needs to be done before pushing new entries, otherwise the iterator will be invalid.
	open_list.erase(best_it);
	parent_node->m_eFAStarListType = FASTARLIST_CLOSED;

	FAssert(&GetNode(parent_node->m_iX, parent_node->m_iY) == parent_node);

	// open a new node for each direction coming off the chosen node.
	for (int i = 0; i < NUM_DIRECTION_TYPES; i++)
	{
		CvPlot* pAdjacentPlot = plotDirection(parent_node->m_iX, parent_node->m_iY, (DirectionTypes)i);
		if (!pAdjacentPlot)
			continue;

		const int& x = pAdjacentPlot->getX(); // convenience
		const int& y = pAdjacentPlot->getY(); //

		if (parent_node->m_pParent && parent_node->m_pParent->m_iX == x && parent_node->m_pParent->m_iY == y)
			continue; // no need to backtrack.

		//int iPlotNum = GC.getMap().plotNum(x, y);
		//FAssert(iPlotNum >= 0 && iPlotNum < GC.getMap().numPlots());

		FAStarNode* child_node = &GetNode(x, y);
		bool bNewNode = !child_node->m_bOnStack;

		if (bNewNode)
		{
			child_node->m_iX = x;
			child_node->m_iY = y;
			pathAdd(parent_node, child_node, ASNC_NEWADD, &settings, 0);

			if (pathValid_join(parent_node, child_node, settings.pGroup , settings.iFlags))
			{
				// This path to the new node is valid. So we need to fill in the data.
				child_node->m_iKnownCost = MAX_INT;
				child_node->m_iHeuristicCost = settings.iHeuristicWeight * pathHeuristic(x, y, dest_x, dest_y);
				// total cost will be set when the parent is set.

				child_node->m_bOnStack = true;

				if (pathValid_source(child_node, settings.pGroup, settings.iFlags))
				{
					open_list.push_back(child_node);
					child_node->m_eFAStarListType = FASTARLIST_OPEN;
				}
				else
				{
					child_node->m_eFAStarListType = FASTARLIST_CLOSED; // This node is a dead end.
				}
			}
			else
			{
				// can't get to the plot from here.
				child_node = NULL;
			}
		}
		else
		{
			if (!pathValid_join(parent_node, child_node, settings.pGroup, settings.iFlags))
				child_node = NULL;
		}

		if (child_node == NULL)
			continue;

		FAssert(child_node->m_iX == x && child_node->m_iY == y);

		if (x == dest_x && y == dest_y)
			end_node = child_node; // We've found our destination - but we still need to finish our calculations

		if (parent_node->m_iKnownCost < child_node->m_iKnownCost)
		{
			int iNewCost = parent_node->m_iKnownCost + pathCost(parent_node, child_node, 666, &settings, 0);
			FAssert(iNewCost > 0);

			if (iNewCost < child_node->m_iKnownCost)
			{
				int cost_delta = iNewCost - child_node->m_iKnownCost; // new minus old. Negative value.

				child_node->m_iKnownCost = iNewCost;
				child_node->m_iTotalCost = child_node->m_iKnownCost + child_node->m_iHeuristicCost;

				// remove child from the list of the previous parent.
				if (child_node->m_pParent)
				{
					FAssert(!bNewNode);
					FAStarNode* x_parent = child_node->m_pParent;
					// x_parent just lost one of its children. We have to break the news to them.
					// this would easier if we had stl instead of bog arrays.
					int temp = x_parent->m_iNumChildren;
					for (int j = 0; j < x_parent->m_iNumChildren; j++)
					{
						if (x_parent->m_apChildren[j] == child_node)
						{
							// found it.
							for (j++ ; j < x_parent->m_iNumChildren; j++)
								x_parent->m_apChildren[j-1] = x_parent->m_apChildren[j];
							x_parent->m_apChildren[j-1] = 0; // not necessary, but easy enough to keep things neat.

							x_parent->m_iNumChildren--;
						}
					}
					FAssert(x_parent->m_iNumChildren == temp - 1);
					// recalculate movement points
					pathAdd(parent_node, child_node, ASNC_PARENTADD_UP, &settings, 0);
				}

				// add child to the list of the new parent
				FAssert(parent_node->m_iNumChildren < NUM_DIRECTION_TYPES);
				parent_node->m_apChildren[parent_node->m_iNumChildren] = child_node;
				parent_node->m_iNumChildren++;
				child_node->m_pParent = parent_node;

				// update the new (reduced) costs for all the grandchildren.
				FAssert(child_node->m_iNumChildren == 0 || !bNewNode);
				ForwardPropagate(child_node, cost_delta);

				FAssert(child_node->m_iKnownCost > parent_node->m_iKnownCost);
			}
		}
		// else parent has higher cost. So there must already be a faster route to the child.
	}
	return true;
}

void KmodPathFinder::ForwardPropagate(FAStarNode* head, int cost_delta)
{
	//FAssert(cost_delta < 0 || head->m_iNumChildren == 0);
	// Note: there are some legitimate cases in which the cost_delta can be positive.
	// For example, suppose a shorter path is found to the parent plot, but the path involves
	// resting on less attractive plots. And suppose the addition moves saved by the shorter path
	// are then spent anyway to take the final step onto the destination...
	// In that case, although the path the parent plot has been upgraded, the path to the destination
	// is actually degraded (ie. it has a higher total cost). I can't think of a way to solve this problem.
	// (but I don't think it is very important.)

	// change the known cost of all children by cost_delta, recursively
	for (int i = 0; i < head->m_iNumChildren; i++)
	{
		FAssert(head->m_apChildren[i]->m_pParent == head);

		// recalculate movement points.
		int iOldMoves = head->m_apChildren[i]->m_iData1;
		int iOldTurns = head->m_apChildren[i]->m_iData2;
		int iNewDelta = cost_delta;
		pathAdd(head, head->m_apChildren[i], ASNC_PARENTADD_UP, &settings, 0);

		// if the moves don't match, we may need to recalculate the path cost.
		//if (iOldMoves != head->m_apChildren[i]->m_iData1)
		{
			// Strictly, the cost shouldn't depend on our path history, but it does - because I wanted to use
			// the path history for path symmetry breaking.
			// But anyway, according to the profiler, this is only going to cost us about a milisecond per turn.
			int iPathCost = pathCost(head, head->m_apChildren[i], 666, &settings, 0);
			iNewDelta = head->m_iKnownCost + iPathCost - head->m_apChildren[i]->m_iKnownCost;
			//FAssert(iNewDelta <= 0);
		}

		head->m_apChildren[i]->m_iKnownCost += iNewDelta;
		head->m_apChildren[i]->m_iTotalCost += iNewDelta;

		FAssert(head->m_apChildren[i]->m_iKnownCost > head->m_iKnownCost);

		if (iNewDelta != 0 || iOldMoves != head->m_apChildren[i]->m_iData1 || iOldTurns != head->m_apChildren[i]->m_iData2)
			ForwardPropagate(head->m_apChildren[i], iNewDelta);
	}
}
