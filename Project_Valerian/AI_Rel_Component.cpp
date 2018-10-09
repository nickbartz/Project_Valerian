#include <AI_Rel_Component.h>
#include<Service_Locator.h>
#include<Message_Array.h>
#include<Global_Constants.h>
#include<AI_Stats_Component.h>
#include<AI_Movement_Component.h>

AI_Rel_Component::AI_Rel_Component(Global_Service_Locator* sLocator, Object_Service_Locator* oLocator)
{
	service_locator = sLocator;
	object_locator = oLocator;
}

int AI_Rel_Component::Return_Object_Faction()
{
	return faction;
}

void AI_Rel_Component::Check_For_Messages()
{
	//Custom_Message* new_message;
	//
	//for (int i = 0; i < service_locator->get_MB_Pointer()->count_custom_messages; i++)
	//{
	//}
}

void AI_Rel_Component::Update()
{

}

