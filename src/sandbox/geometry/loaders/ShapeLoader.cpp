#include "sandbox/geometry/loaders/ShapeLoader.h"
#include<iostream>

namespace sandbox {

ShapeLoader::ShapeLoader(Shape shape, int resolution) : shape(shape), resolution(resolution) {
	addType<ShapeLoader>();
}

void ShapeLoader::load(Mesh* mesh) {
	//readOnly = true;
	std::cout << "Update mesh" << std::endl;

	std::vector<unsigned int> indices;
    std::vector<glm::vec3> nodes;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> coords;

	if (shape == SHAPE_QUAD) {
		nodes.push_back(glm::vec3(-0.5, -0.5, 0));
		nodes.push_back(glm::vec3(-0.5, 0.5, 0));
		nodes.push_back(glm::vec3(0.5, 0.5, 0));
		nodes.push_back(glm::vec3(0.5, -0.5, 0));

		normals.push_back(glm::vec3(0, 0, 1));
		normals.push_back(glm::vec3(0, 0, 1));
		normals.push_back(glm::vec3(0, 0, 1));
		normals.push_back(glm::vec3(0, 0, 1));

		coords.push_back(glm::vec2(0, 1));
		coords.push_back(glm::vec2(0, 0));
		coords.push_back(glm::vec2(1, 0));
		coords.push_back(glm::vec2(1, 1));

		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(0);
		indices.push_back(3);
		indices.push_back(2);
	}
	else if (shape == SHAPE_CYLINDAR) {
		int numSegments = resolution > 3 ? resolution : 3;
		for (int f = 0; f < numSegments+1; f++) {
			float x = std::cos(-2.0f*3.14159*f/(numSegments));
			float y = std::sin(-2.0f*3.14159*f/(numSegments));
			float bottomRadius = 0.5;
			float topRadius = 0.5;
			nodes.push_back(glm::vec3(bottomRadius*x, -0.5, bottomRadius*y));
			nodes.push_back(glm::vec3(topRadius*x, 0.5, topRadius*y));
			normals.push_back(glm::vec3(x, 0, y));
			normals.push_back(glm::vec3(x, 0, y));
			coords.push_back(glm::vec2(1.0f*f/numSegments, 0));
			coords.push_back(glm::vec2(1.0f*f/numSegments, 1));

			if (f > 0) {
				indices.push_back((f-1)*2+0);
				indices.push_back((f-1)*2+2);
				indices.push_back((f-1)*2+1);
				indices.push_back((f-1)*2+2);
				indices.push_back((f-1)*2+3);
				indices.push_back((f-1)*2+1);
			}
		}
	}

	mesh->setIndices(indices);
	mesh->setNodes(nodes);
	mesh->setNormals(normals);
	mesh->setCoords(coords);
}

}