

template <unsigned int segments>
using circleObject = struct {
	float verts[segments* 2];
	float center[2];
	float r;
	unsigned int segs = segments;
};
