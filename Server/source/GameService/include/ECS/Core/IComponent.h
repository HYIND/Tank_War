#pragma once

class IComponent
{
public:
	IComponent() = default;
	virtual ~IComponent() = default;
	// 组件添加时
	virtual void OnAdd() {}
	// 组件移除时
	virtual void OnRemove() {}
};