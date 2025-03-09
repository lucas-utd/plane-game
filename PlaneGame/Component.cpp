#include "Component.h"

namespace GUI
{
	Component::Component()
		: isSelected_(false)
		, isActive_(false)
	{
	}

	Component::~Component()
	{
	}

	bool Component::isSelected() const
	{
		return isSelected_;
	}

	void Component::select()
	{
		isSelected_ = true;
	}

	void Component::deselect()
	{
		isSelected_ = false;
	}

	bool Component::isActive() const
	{
		return isActive_;
	}

	void Component::activate()
	{
		isActive_ = true;
	}

	void Component::deactivate()
	{
		isActive_ = false;
	}
}
