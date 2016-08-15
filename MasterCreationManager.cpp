#include "MasterCreationManager.h"
#include "SlaveControl.h"


/*************************** Creation Process Related **************************/

SimpleSystem::MasterCreationManager::MasterCreationManager()
	:ignore_selection_change(false)
{
	//TODO: consider removing this constructor
}


void SimpleSystem::MasterCreationManager::Begin(IObjCreate* ioc, ClassDesc* desc)
{
	create_interface = ioc;
	class_desc = desc;
	create_callback = nullptr;
	main_node = nullptr;
	the_master = nullptr;
	attached_to_node = false;
	createNewMaster();
}


void SimpleSystem::MasterCreationManager::End()
{
	if(the_master)
	{
		bool destroy_it = true;
		the_master->EndEditParams(static_cast<IObjParam*>(create_interface), destroy_it, nullptr);
		if(!attached_to_node)
		{
			the_master->DeleteAllRefsFromMe();
			the_master->DeleteAllRefsToMe();
			the_master->DeleteThis();
			
		}
		else if(main_node)
		{
			DeleteAllRefsFromMe();
		}
		the_master = nullptr;
	}
}


void SimpleSystem::MasterCreationManager::createNewMaster()
{
	the_master = new Master();
	the_master->BeginEditParams(static_cast<IObjParam*>(create_interface), BEGIN_EDIT_CREATE, nullptr);
}


int SimpleSystem::MasterCreationManager::proc(HWND hwnd, int msg, int point, int flags, IPoint2 m)
{
	ViewExp& vpx = create_interface->GetViewExp(hwnd);
	bool result = true;

	switch(msg)
	{
	case MOUSE_POINT:
	{
		if(point == 0)
		{
			if (create_interface->SetActiveViewport(hwnd) 
				|| create_interface->IsCPEdgeOnInView()) 
			{ return false; }

			if(attached_to_node)
			{
				the_master->EndEditParams(static_cast<IObjParam*>(create_interface), 0, nullptr);
				DeleteAllRefsFromMe();
				createNewMaster();
			}

			// TODO: Modify the creation construct here
			DummyObject* dummy_object = static_cast<DummyObject*>(create_interface->CreateInstance(HELPER_CLASS_ID, Class_ID(DUMMY_CLASS_ID, 0)));

			INode* dummy_node = create_interface->CreateObjectNode(dummy_object);
			dummy_object->SetBox(Box3(Point3(-20.0f, -20.0f, -20.0f), Point3(20.0f, 20.0f, 20.0f)));

			GenBoxObject *box;
			//[1] Uncomment this and comment [2] to have "one prameter to rule them all"
//			box = static_cast<GenBoxObject*>(create_interface->CreateInstance(GEOMOBJECT_CLASS_ID, Class_ID(BOXOBJ_CLASS_ID, 0)));
//			box->SetParams(20.0f, 20.0f, 20.0f, 1, 1, 1, false);

			// Make a bunch of nodes, hook the box object to and a
			// slave controller of the master control to each
			INode* new_node;
			for(int i = 0; i != the_master->getSlaveNumber(create_interface->GetTime()); ++i)
			{
				//[2] Uncomment this and comment [1] to have unique parameters for each object
				box = static_cast<GenBoxObject*>(create_interface->CreateInstance(GEOMOBJECT_CLASS_ID, Class_ID(BOXOBJ_CLASS_ID, 0)));
				box->SetParams(20.0f, 20.0f, 20.0f, 1, 1, 1, false);
				
				new_node = create_interface->CreateObjectNode(box);
				SlaveControl* slave = new SlaveControl(the_master, i);
				new_node->SetTMController(slave);
				dummy_node->AttachChild(new_node);
				the_master->setSlaveNode(i, new_node);
			}

			// select the dummy node
			attached_to_node = true;

			// Reference the node, to get all notifications
			ReplaceReference(0, const_cast<INode*>(the_master->getSlaveNode(0)));
			
			the_master->setRadius(TimeValue(0), 0.0f);
			
			first_click = m;
			trans_matrix.IdentityMatrix();
			center = vpx.SnapPoint(m, m, nullptr, SNAP_IN_PLANE);
			trans_matrix.SetTrans(center);
			create_interface->SetNodeTMRelConstPlane(dummy_node, trans_matrix);

			result = true;

		}
		else
		{
			ignore_selection_change = true;
			create_interface->SelectNode(const_cast<INode*>(the_master->getSlaveNode(0)));
			ignore_selection_change = false;
			result = false;

		}
		create_interface->RedrawViews(create_interface->GetTime(), REDRAW_NORMAL, the_master);
		break;
	}
	
	case MOUSE_MOVE:
	{
		if(main_node)
		{
			float radius = fabs(vpx.SnapLength(vpx.GetCPDisp(center, Point3(0, 1, 0), first_click, m)));
			the_master->setRadius(0, radius);
			create_interface->RedrawViews(create_interface->GetTime(), REDRAW_NORMAL, the_master);
		}
		result = true;
		break;
	}

	case MOUSE_FREEMOVE:
		SetCursor(LoadMouseCursor(UI::MouseCursors::Crosshair));
		break;

	case MOUSE_PROPCLICK:
		create_interface->RemoveMode(nullptr);
		break;

	case MOUSE_ABORT:
		the_master->EndEditParams(static_cast<IObjParam*>(create_interface), 0, nullptr);
		DeleteAllRefsFromMe();
		createNewMaster();
		create_interface->RedrawViews(create_interface->GetTime(), REDRAW_END, the_master);
		attached_to_node = false;
		result = false;
		break;
	}

	return result;
}


void SimpleSystem::MasterCreationManager::GetClassName(MSTR& s)
{
	s = L"Master Creation Manager";
}


int SimpleSystem::MasterCreationManager::NumRefs()
{
	return 1;
}


RefTargetHandle SimpleSystem::MasterCreationManager::GetReference(int i)
{
	return (i == 0) ? static_cast<RefTargetHandle>(main_node) : nullptr;
}


void SimpleSystem::MasterCreationManager::SetReference(int i, RefTargetHandle rtarg)
{
	if (i == 0) { main_node = static_cast<INode*>(rtarg); }
}


RefResult SimpleSystem::MasterCreationManager::NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate)
{
	switch(message)
	{
	case REFMSG_TARGET_SELECTIONCHANGE:
	{
		if (ignore_selection_change) { break; }
		if (!the_master) { return REF_SUCCEED; }
		DeleteAllRefsFromMe();
	}
	// fall through
	case REFMSG_TARGET_DELETED:
		if(the_master)
		{
			bool delete_it = false;
			the_master->EndEditParams(static_cast<IObjParam*>(create_interface), delete_it, nullptr);
			the_master = nullptr;
			main_node = nullptr;
			createNewMaster();
			attached_to_node = false;
		}
		if (hTarget == main_node) { main_node = nullptr; }
		break;
	}
	return REF_SUCCEED;
}






/***************************** Creation Mode Related ***************************/


void SimpleSystem::MasterCreateMode::Begin(IObjCreate* ioc, ClassDesc* desc)
{
	proc.Begin(ioc, desc);
}

void SimpleSystem::MasterCreateMode::End()
{
	proc.End();
}

bool SimpleSystem::MasterCreateMode::isSticky()
{
	return false;
}

int SimpleSystem::MasterCreateMode::Class()
{
	return CREATE_COMMAND;
}

int SimpleSystem::MasterCreateMode::ID()
{
	return CID_USER + 32; // This value should be the constant CID_USER plus some random value chosen by the developer.
}

MouseCallBack* SimpleSystem::MasterCreateMode::MouseProc(int* numPoints)
{
	*numPoints = 100000;
	return &proc;
}

ChangeForegroundCallback* SimpleSystem::MasterCreateMode::ChangeFGProc()
{
	return CHANGE_FG_SELECTED;
}

BOOL SimpleSystem::MasterCreateMode::ChangeFG(CommandMode* oldMode)
{
	return (oldMode->ChangeFGProc() != CHANGE_FG_SELECTED);
}

void SimpleSystem::MasterCreateMode::EnterMode()
{
	//TIP: add MouseCursors.lib to dependencies, to be able to use this method
	SetCursor(LoadMouseCursor(UI::MouseCursors::Crosshair));
}

void SimpleSystem::MasterCreateMode::ExitMode()
{
	//TIP: add MouseCursors.lib to dependencies, to be able to use this method
	SetCursor(LoadCursor(nullptr, IDC_ARROW));
}
