#pragma once

#ifndef NODEDATAPOOL_H
#define NODEDATAPOOL_H

#include <queue>
#include "TBB.h"
#include "tbb/task_group.h"
#include "tbb/concurrent_queue.h"

class FAStarNode;

class NodeDataPool
{
	NodeDataPool();
	~NodeDataPool();
	void initializePool();

public:

	static NodeDataPool& getInstance();
	FAStarNode* getBuffer(FAStarNode* node_data);
	void releaseBuffer(FAStarNode* node_data);

	bool cleared_data_pool_initialized;
	void clearNodeData(FAStarNode* data, int totalSize);

	int map_width;
	int map_height;
	tbb::task_group group;
	tbb::mutex pool_mutex;
	tbb::concurrent_bounded_queue<FAStarNode*> cleared_data_pool;
};

#endif
