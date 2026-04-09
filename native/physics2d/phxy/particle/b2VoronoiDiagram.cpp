#include "b2VoronoiDiagram.h"
#include "b2ParticleSetting.h"
#include "b2StackQueue.h"

using namespace phxy;

b2VoronoiDiagram::b2VoronoiDiagram(sqstd::StackTempArenaAllocator *allocator, int generatorCapacity)
{
	m_allocator = allocator;
	m_generatorBuffer = (Generator *) allocator->allocateChunk(sizeof(Generator) * generatorCapacity);
	m_generatorCapacity = generatorCapacity;
	m_generatorCount = 0;
	m_countX = 0;
	m_countY = 0;
	m_diagram = NULL;
}

b2VoronoiDiagram::~b2VoronoiDiagram()
{
	m_allocator->freeChunk(m_generatorBuffer);
	if (m_diagram)
	{
		m_allocator->freeChunk(m_diagram);
	}
}

void b2VoronoiDiagram::AddGenerator(const SqVec2 &center, int tag, bool necessary)
{
	SQ_ASSERT(m_generatorCount < m_generatorCapacity);
	Generator &g = m_generatorBuffer[m_generatorCount++];
	g.center = center;
	g.tag = tag;
	g.necessary = necessary;
}

void b2VoronoiDiagram::Generate(float radius, float margin)
{

	
	SqVec2 lower(+sq_maxFloat,+sq_maxFloat);
	SqVec2 upper(-sq_maxFloat,-sq_maxFloat);

	//计算整个voroni的AABB大小
	for (int k = 0; k < m_generatorCount; k++)
	{
		Generator &g = m_generatorBuffer[k];
		if (g.necessary)
		{
			lower = SqVec2::Min(lower, g.center);
			upper = SqVec2::Max(upper, g.center);
		}
	}
	lower.x -= margin;
	lower.y -= margin;
	upper.x += margin;
	upper.y += margin;
	
	//长度/radius = 等于有多少个格子
	//根据AABB构建一个二维表格
	float inverseRadius = 1 / radius;
	m_countX = 1 + (int)(inverseRadius * (upper.x - lower.x));
	m_countY = 1 + (int)(inverseRadius * (upper.y - lower.y));
	m_diagram = (Generator **)m_allocator->allocateChunk(sizeof(Generator *) * m_countX * m_countY);
	for (int i = 0; i < m_countX * m_countY; i++)
	{
		m_diagram[i] = NULL;
	}
	
	// (4 * m_countX * m_countY) is the queue capacity that is experimentally
	// known to be necessary and sufficient for general particle distributions.
	b2StackQueue<b2VoronoiDiagramTask> queue(m_allocator, 4 * m_countX * m_countY);
	for (int k = 0; k < m_generatorCount; k++)
	{
		Generator &g = m_generatorBuffer[k];
		g.center = inverseRadius * (g.center - lower);
		int x = (int)g.center.x;
		int y = (int)g.center.y;
		if (x >= 0 && y >= 0 && x < m_countX && y < m_countY)
		{
			queue.Push(b2VoronoiDiagramTask(x, y, x + y * m_countX, &g));
		}
	}

	//使用Flood Fill Algorithm
	//让整个二维表格网格都填满粒子，如果本来没有粒子的地方重复填充一样的粒子
	while (!queue.Empty())
	{
		int x = queue.Front().m_x;
		int y = queue.Front().m_y;
		int i = queue.Front().m_i;
		Generator *g = queue.Front().m_generator;
		queue.Pop();

		if (!m_diagram[i])
		{
			m_diagram[i] = g;
			if (x > 0)
			{
				queue.Push(b2VoronoiDiagramTask(x - 1, y, i - 1, g));
			}
			if (y > 0)
			{
				queue.Push(b2VoronoiDiagramTask(x, y - 1, i - m_countX, g));
			}
			if (x < m_countX - 1)
			{
				queue.Push(b2VoronoiDiagramTask(x + 1, y, i + 1, g));
			}
			if (y < m_countY - 1)
			{
				queue.Push(b2VoronoiDiagramTask(x, y + 1, i + m_countX, g));
			}
		}
	}


	//找出边界的格子，即隔壁是不一样的粒子的一对格子
	//这样就可以得到每个粒子与谁相邻了
	for (int y = 0; y < m_countY; y++)
	{
		for (int x = 0; x < m_countX - 1; x++)
		{
			int i = x + y * m_countX;
			Generator *a = m_diagram[i];
			Generator *b = m_diagram[i + 1];
			if (a != b)
			{
				queue.Push(b2VoronoiDiagramTask(x, y, i, b)); //这里的意思是x,y点处相邻的是b
				queue.Push(b2VoronoiDiagramTask(x + 1, y, i + 1, a));//x+1,y处相邻的是a，从而将a和b建立相邻关系
			}
		}
	}

	for (int y = 0; y < m_countY - 1; y++)
	{
		for (int x = 0; x < m_countX; x++)
		{
			int i = x + y * m_countX;
			Generator *a = m_diagram[i];
			Generator *b = m_diagram[i + m_countX];
			if (a != b)
			{
				queue.Push(b2VoronoiDiagramTask(x, y, i, b));
				queue.Push(b2VoronoiDiagramTask(x, y + 1, i + m_countX, a));
			}
		}
	}


	while (!queue.Empty())
	{
		const b2VoronoiDiagramTask &task = queue.Front();
		int x = task.m_x;
		int y = task.m_y;
		int i = task.m_i;

		//通过上面的步骤，这里的k为格子x,y相邻的粒子
		Generator *k = task.m_generator; 
		queue.Pop();

		//当前的粒子，所以a为当前粒子，b为当前a粒子的相邻粒子
		Generator *a = m_diagram[i];
		Generator *b = k;
		if (a != b)
		{
			//计算当前a粒子和b粒子当前所处的位置到x,y的距离
			float ax = a->center.x - x;
			float ay = a->center.y - y;
			float bx = b->center.x - x;
			float by = b->center.y - y;
			float a2 = ax * ax + ay * ay;
			float b2 = bx * bx + by * by;

			//如果a距离b还是很远的话，继续将b进行Flood Fill，往a继续靠近
			if (a2 > b2)
			{
				m_diagram[i] = b;
				if (x > 0)
				{
					queue.Push(b2VoronoiDiagramTask(x - 1, y, i - 1, b));
				}
				if (y > 0)
				{
					queue.Push(b2VoronoiDiagramTask(x, y - 1, i - m_countX, b));
				}
				if (x < m_countX - 1)
				{
					queue.Push(b2VoronoiDiagramTask(x + 1, y, i + 1, b));
				}
				if (y < m_countY - 1)
				{
					queue.Push(b2VoronoiDiagramTask(x, y + 1, i + m_countX, b));
				}
			}
		}
	}

}

void b2VoronoiDiagram::GetNodes(NodeCallback &callback) const
{
	
	for (int y = 0; y < m_countY - 1; y++)
	{
		for (int x = 0; x < m_countX - 1; x++)
		{
			int i = x + y * m_countX;
			const Generator *a = m_diagram[i];
			const Generator *b = m_diagram[i + 1]; //右边
			const Generator *c = m_diagram[i + m_countX];//下面
			const Generator *d = m_diagram[i + 1 + m_countX];//右下
			if (b != c)
			{
				if (a != b && a != c && (a->necessary || b->necessary || c->necessary))
				{
					callback(a->tag, b->tag, c->tag);
				}

				if (d != b && d != c && (b->necessary || d->necessary || c->necessary))
				{
					callback(b->tag, d->tag, c->tag);
				}
			}
		}
	}
}
