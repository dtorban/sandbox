#include "sandbox/graphics/FloatDataRenderer.h"
#include <iostream>
#include "sandbox/graphics/RenderState.h"

namespace sandbox {

FloatDataRenderer::FloatDataRenderer() : data(nullptr), updateElementVersion(0) {
	addType<FloatDataRenderer>();
}

void FloatDataRenderer::updateModel() {
	if (!data) {
		data = getSceneNode().getComponent<FloatDataSet>();

		std::cout << data->getNumPoints() << std::endl;
		for (unsigned int f = 0; f < data->getNumPoints(); f++) {
		    fullIndices.push_back(f);
		}
	}
}

void FloatDataRenderer::updateSharedContext(const SceneContext& sceneContext) {
	FloatDataSharedState& state = *contextHandler.getSharedState(sceneContext);

	if (data && !state.initialized) {
	    std::cout << "INitialize float data shared context " << std::endl;

	    glGenBuffers(1, &state.elementBuffer);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state.elementBuffer);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->getNumPoints() * sizeof(unsigned int), &fullIndices[0], GL_DYNAMIC_DRAW);

		glGenBuffers(1, &state.vbo);
	    glBindBuffer(GL_ARRAY_BUFFER, state.vbo);
	    glBufferData(GL_ARRAY_BUFFER, data->getArray().size()*sizeof(float), 0, GL_DYNAMIC_DRAW);
	    glBufferSubData(GL_ARRAY_BUFFER, 0, data->getArray().size()*sizeof(float), &data->getArray()[0]);

	    state.initialized = true;
	}
	else if (!data && state.initialized) {
		glDeleteBuffers(1, &state.vbo);
	    glDeleteBuffers(1, &state.elementBuffer);
	    state.initialized = false;
	}

	if (state.updateElementVersion != updateElementVersion) {
		std::cout << "Update Elements" << std::endl;
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state.elementBuffer);
	    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0, data->getNumPoints() * sizeof(unsigned int), &sortedIndices[0]);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	    state.updateElementVersion = updateElementVersion;
	}
}

void FloatDataRenderer::updateContext(const SceneContext& sceneContext) {
	FloatDataSharedState& sharedState = *contextHandler.getSharedState(sceneContext);
	FloatDataState& state = *contextHandler.getState(sceneContext);

	if (data && !state.initialized) {
        std::cout << "INitialize float data context" << std::endl;
	    glGenVertexArrays(1, &state.vao);
	    glBindVertexArray(state.vao);
	    glBindBuffer(GL_ARRAY_BUFFER, sharedState.vbo);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sharedState.elementBuffer);
	    glEnableVertexAttribArray(0);
	    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)*data->getVariables().size(), (char*)(0));
	    glEnableVertexAttribArray(1);
	    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float)*data->getVariables().size(), (char*)(4*sizeof(float)));
	    glEnableVertexAttribArray(2);
	    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float)*data->getVariables().size(), (char*)(2*4*sizeof(float)));
	    glEnableVertexAttribArray(3);
	    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float)*data->getVariables().size(), (char*)(3*4*sizeof(float)));
	    glBindVertexArray(0);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	    glBindBuffer(GL_ARRAY_BUFFER, 0);
	    state.initialized = true;
	}
	else if (!data && state.initialized) {
		glDeleteVertexArrays(1, &state.vao);
		state.initialized = false;
	}
}

void FloatDataRenderer::sortByVariable(int index, bool sortDesc) {
	if (index > 0) {
		std::vector<unsigned int> indices;
	    for (unsigned int f = 0; f < data->getNumPoints(); f++) {
	    	indices.push_back(f/index);
	    }
	    this->sortedIndices = indices;
	}
	else {
	    this->sortedIndices = fullIndices;
	}

	updateElementVersion++;
}

void FloatDataRenderer::render(const SceneContext& sceneContext) {
	FloatDataSharedState& sharedState = *contextHandler.getSharedState(sceneContext);
	FloatDataState& state = *contextHandler.getState(sceneContext);

	if (state.initialized) {
		//std::cout << "Render FloatData" << state.vao << " " << data->getIndices().size() << std::endl;
		//glBindBuffer(GL_ARRAY_BUFFER, sharedState.vbo);
	    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sharedState.elementBuffer);
	    glBindVertexArray(state.vao);

	    RenderState& renderState = RenderState::get(sceneContext);
	    ShaderProgram* shader = renderState.getShaderProgram().get();
	    if (shader) {
	    	shader->use(sceneContext);
	    }

	    //glDrawElements(GL_PATCHES, data.indices.size(), GL_UNSIGNED_INT, (void*)0);
	    //glDrawElements(GL_TRIANGLES, data->getIndices().size(), GL_UNSIGNED_INT, (void*)0);
		glDrawElementsInstancedBaseVertex(GL_POINTS,
				data->getNumPoints(),
				GL_UNSIGNED_INT,
				(void*)(sizeof(unsigned int) * 0),
				1, //numInstances,
				0);
	    glBindVertexArray(0);

	    if (shader) {
	    	shader->release(sceneContext);
	    }
	}
}

}