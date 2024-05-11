#include "NodeDataPool.h"
#include "CvGameCoreDLL.h"
#include "TBB.h"
#include "tbb/task_group.h"
#include "tbb/concurrent_queue.h"

#define BUFFERS 16

class ClearAndPushFunctor {
public:
	FAStarNode* data;
	NodeDataPool* p;

	ClearAndPushFunctor(FAStarNode* d, NodeDataPool* p_)
		: data(d), p(p_) {}

	void operator()() const {
		FAssert(p->map_width * p->map_height != 0);
		FAssert((p->map_width * p->map_height) > 50);
		p->clearNodeData(data, p->map_width * p->map_height);
		p->cleared_data_pool.push(data);
	}
};

NodeDataPool::NodeDataPool() :
	map_width(0),
	map_height(0),
	cleared_data_pool_initialized(false)
{
	cleared_data_pool.set_capacity(BUFFERS);  // Example capacity
}

NodeDataPool& NodeDataPool::getInstance() {
	static NodeDataPool instance; // Guaranteed to be destroyed and instantiated on first use
	return instance;
}

FAStarNode* NodeDataPool::getBuffer(FAStarNode* node_data)
{
	FAStarNode* new_data = NULL;

	// Lock access to the shared resource
	//tbb::mutex::scoped_lock lock(pool_mutex);

	// Ensure the pool is initialized
	if (!cleared_data_pool_initialized) {
		initializePool();
	}

	// Check if there's pre-cleared node_data in the pool
	if (!cleared_data_pool.empty()) {
		// Use the pre-cleared node_data from the pool
		cleared_data_pool.pop(new_data); // will not block due to the check above (TODO: What if we have multiple threads using the pf?)		
		// May be NULL if this is a new instance with no previous allocation
		if (node_data != NULL)
		{
			// Asynchronously clear the old node_data and push it back into the pool
			group.run(ClearAndPushFunctor(node_data, this));
		}
	}
	else {
		// May be NULL if this is a new instance with no previous allocation
		if (node_data != NULL)
		{
			// Asynchronously clear the old node_data and push it back into the pool
			group.run(ClearAndPushFunctor(node_data, this));
		}
		cleared_data_pool.pop(new_data);  // Blocks until an item is available
		// Now, swap the current node_data with the newly allocated node_data
	}

	FAssert(new_data);
	return new_data;
}

void NodeDataPool::releaseBuffer(FAStarNode* node_data)
{
	if (node_data != NULL) 
	{
		// Asynchronously clear the old node_data and push it back into the pool
		group.run(ClearAndPushFunctor(node_data, this));
	}
}

void NodeDataPool::clearNodeData(FAStarNode* data, int totalSize) {
	FAssert(totalSize > 50);
	FAssert(data != NULL);
	std::memset(data, 0, sizeof(FAStarNode) * totalSize);
}

void NodeDataPool::initializePool()
{
	//tbb::mutex::scoped_lock lock(pool_mutex);
	for (int i = 0; i < BUFFERS; ++i)
	{
		map_width = GC.getMap().getGridWidth();
		map_height = GC.getMap().getGridHeight();
		FAssert(map_width * map_height != 0);
		FAStarNode* newData = new FAStarNode[map_width * map_height];
		clearNodeData(newData, map_width * map_height);
		cleared_data_pool.push(newData);
	}
	cleared_data_pool_initialized = true;
}

NodeDataPool::~NodeDataPool()
{
	group.wait(); // Ensure all async operations are completed
	FAStarNode* data;
	while (cleared_data_pool.try_pop(data)) 
	{
		delete[] data;
	}
}
