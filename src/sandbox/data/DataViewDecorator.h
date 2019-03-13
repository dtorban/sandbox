#ifndef SANDBOX_DATA_VIEW_DECORATOR_H_
#define SANDBOX_DATA_VIEW_DECORATOR_H_

#include "sandbox/data/DataView.h"
#include "sandbox/SceneNode.h"

namespace sandbox {

template <typename T>
class DataViewDecorator : public DataView<T> {
public:
	DataViewDecorator(SceneNode* node) : node(node), view(nullptr) {
		SceneComponent::addType< DataViewDecorator<T> >();

	}
	virtual ~DataViewDecorator() {}

	virtual void updateModel() {
		if (!view) {
			view = node->getComponent< DataView<T> >();
		}
	}

	virtual const std::vector<std::string>& getVariables() const {
		if (view) { return view->getVariables(); } 
		else { static std::vector<std::string> v; return v; }
	}
	virtual const std::vector<T>& getArray() const {
		if (view) { return view->getArray(); } 
		else { static std::vector<T> v; return v; }
	}
	virtual T getMin(unsigned int dimension) const {
		if (view) { return view->getMin(dimension); } 
		else { return T(); }
	}
	virtual T getMax(unsigned int dimension) const {
		if (view) { return view->getMax(dimension); } 
		else { return T(); }
	}
	virtual const std::vector<unsigned int>& getPoints() const {
		if (view) { return view->getPoints(); } 
		else { static std::vector<unsigned int> v; return v; }
	}

protected:
	const DataView<T>* getView() const { return view; }

private:
	SceneNode* node;
	DataView<T>* view;
};

}

#endif