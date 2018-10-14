#pragma once

class Global_Service_Locator;
class Object_Service_Locator;

class AI_Rel_Component
{
public:
	AI_Rel_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator);
	
	int Return_Object_Faction();
	void Check_For_Messages();
	void Update();

private:
	Global_Service_Locator * service_locator;
	Object_Service_Locator* object_locator;

	int faction = 0;
};