#ifndef SANDBOX_GRAPHICS_FLOATDATA_RENDERER_H_
#define SANDBOX_GRAPHICS_FLOATDATA_RENDERER_H_

#include "sandbox/SceneComponent.h"
#include "sandbox/data/DataView.h"
#include "OpenGL.h"

namespace sandbox {

class FloatDataRenderer : public SceneComponent {
public:
	FloatDataRenderer();
	virtual ~FloatDataRenderer() {}

	void updateModel();
	void updateSharedContext(const SceneContext& sceneContext);
	void updateContext(const SceneContext& sceneContext);
	void render(const SceneContext& sceneContext);
	void sortByVariable(int sortedIndex, bool sortDesc = false);

private:
	long updateElementVersion;
	std::vector<unsigned int> fullIndices;
	std::vector<unsigned int> sortedIndices;

	class FloatDataSharedState : public ContextState {
	public:
	    virtual ~FloatDataSharedState() {
	    	if (initialized) {
		        glDeleteBuffers(1, &vbo);
		        glDeleteBuffers(1, &elementBuffer);
	    	}
	    }

	    GLuint vbo;
	    GLuint elementBuffer;
	    long updateElementVersion;
	};

	class FloatDataState : public ContextState {
	public:
	    virtual ~FloatDataState() {
	    	if (initialized) {
	        	glDeleteVertexArrays(1, &vao);
	        }
	    }

	    GLuint vao;
	};

	FloatDataView* data;
	SceneContextHandler<FloatDataSharedState,FloatDataState> contextHandler;
};

}

#endif