
#pragma once
#include "../../../engine/core/sqstd/StackTempArenaAllocator.h"
#include "../common/math/SqVec2.h"

namespace phxy
{
	/// A field representing the nearest generator from each point.
	class b2VoronoiDiagram
	{

	public:
		b2VoronoiDiagram(sqstd::StackTempArenaAllocator *allocator, int generatorCapacity);
		~b2VoronoiDiagram();

		/// Add a generator.
		/// @param the position of the generator.
		/// @param a tag used to identify the generator in callback functions.
		/// @param whether to callback for nodes associated with the generator.
		void AddGenerator(const SqVec2 &center, int tag, bool necessary);

		/// Generate the Voronoi diagram. It is rasterized with a given interval
		/// in the same range as the necessary generators exist.
		/// @param the interval of the diagram.
		/// @param margin for which the range of the diagram is extended.
		void Generate(float radius, float margin);

		/// Callback used by GetNodes().
		class NodeCallback
		{
		public:
			virtual ~NodeCallback() {}
			/// Receive tags for generators associated with a node.
			virtual void operator()(int a, int b, int c) = 0;
		};

		/// Enumerate all nodes that contain at least one necessary generator.
		/// @param a callback function object called for each node.
		void GetNodes(NodeCallback &callback) const;

	private:
		struct Generator
		{
			SqVec2 center;
			int tag;
			bool necessary;
		};

		struct b2VoronoiDiagramTask
		{
			int m_x, m_y, m_i;
			Generator *m_generator;

			b2VoronoiDiagramTask() {}
			b2VoronoiDiagramTask(int x, int y, int i, Generator *g)
			{
				m_x = x;
				m_y = y;
				m_i = i;
				m_generator = g;
			}
		};

		sqstd::StackTempArenaAllocator *m_allocator;
		Generator *m_generatorBuffer;
		int m_generatorCapacity;
		int m_generatorCount;
		int m_countX, m_countY;
		Generator **m_diagram;
	};

}