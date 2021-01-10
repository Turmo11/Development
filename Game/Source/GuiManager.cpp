/*#include "GuiManager.h"

GuiElements* GuiManager::CreateElement(GuiType type)
{

	GuiElements* element = nullptr;

	switch (type)
	{
		case GuiType::BUTTON: element = new Button(12, button);
		default: break;
	}

	if (element != nullptr)
	{
		elements.add(element);
	}
	return element;
}

bool GuiManager::Update(float dt)
{
	accTime += dt;
	if (accTime >= updateMs)
	{
		doLogic = true;
	}

	if (doLogic)
	{
		accTime = 0.0f;
		doLogic != doLogic;
	}

	return true;
}*/