#ifndef SANDBOX_GRAPHICS_FLOATDATA_RENDERER_H_
#define SANDBOX_GRAPHICS_FLOATDATA_RENDERER_H_

#include "sandbox/SceneComponent.h"
#include "sandbox/data/FloatDataSet.h"
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

private:
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

	FloatDataSet* data;
	SceneContextHandler<FloatDataSharedState,FloatDataState> contextHandler;
};

}

#endif