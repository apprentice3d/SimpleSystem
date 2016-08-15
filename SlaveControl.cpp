#include "SlaveControl.h"

static SimpleSystem::SlaveControlClassDesc slave_descriptor;


/*************************** Class Descriptor Related **************************/

ClassDesc2* SimpleSystem::getSlaveDescriptor()
{
	return &slave_descriptor;
}

int SimpleSystem::SlaveControlClassDesc::IsPublic()
{
	return false;
}

void* SimpleSystem::SlaveControlClassDesc::Create(BOOL loading)
{
	return new SimpleSystem::SlaveControl();
}

const wchar_t* SimpleSystem::SlaveControlClassDesc::ClassName()
{
	return GetString(IDS_SLAVE_CONTROL);
}

SClass_ID SimpleSystem::SlaveControlClassDesc::SuperClassID()
{
	return CTRL_MATRIX3_CLASS_ID;
}

Class_ID SimpleSystem::SlaveControlClassDesc::ClassID()
{
	return SIMPLE_SYSTEM_SLAVE_CONTROL;
}

const wchar_t* SimpleSystem::SlaveControlClassDesc::Category()
{
	return L"";
}


/*************************** Class Related ************************************/

SimpleSystem::SlaveControl::SlaveControl(bool loading)
	: master(nullptr), id(0)
{
}

SimpleSystem::SlaveControl::SlaveControl(Master* new_master, int new_id)
	: master(nullptr), id(new_id)
{
	ReplaceReference(0, (ReferenceTarget*)new_master);
}

SimpleSystem::SlaveControl::~SlaveControl()
{
	//NOP
}

SimpleSystem::SlaveControl::SlaveControl(const SlaveControl& other_control)
{
	this->master = other_control.master;
	this->id = other_control.id;
}

SimpleSystem::SlaveControl& SimpleSystem::SlaveControl::operator=(const SlaveControl& other_control)
{
	this->master = other_control.master;
	this->id = other_control.id;
	return (*this);
}

void SimpleSystem::SlaveControl::SetID(ULONG new_id)
{
	id = new_id;
}

void SimpleSystem::SlaveControl::Copy(Control* from)
{
	//NOP
}

void SimpleSystem::SlaveControl::GetValue(TimeValue t, void* val, Interval& valid, GetSetMethod method)
{
	master->getValue(t, val, valid, method, id);
}

void SimpleSystem::SlaveControl::SetValue(TimeValue t, void* val, int commit, GetSetMethod method)
{
	//NOP
}

RefResult SimpleSystem::SlaveControl::NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate)
{
	return REF_SUCCEED;
}


BOOL SimpleSystem::SlaveControl::IsLeaf()
{
	return false;
}

BOOL SimpleSystem::SlaveControl::CanCopyAnim()
{
	return false;
}

BOOL SimpleSystem::SlaveControl::IsReplaceable()
{
	return false;
}

void* SimpleSystem::SlaveControl::GetInterface(ULONG id)
{
	return (id == I_MASTER) ? static_cast<ReferenceTarget*>(master) : Control::GetInterface(id);
}

int SimpleSystem::SlaveControl::NumSubs()
{
	return master ? master->NumSubs() : 0;
}

Animatable* SimpleSystem::SlaveControl::SubAnim(int i)
{
	return master->SubAnim(i);
}

MSTR SimpleSystem::SlaveControl::SubAnimName(int i)
{
	return master->SubAnimName(i);
}

int SimpleSystem::SlaveControl::IsKeyable()
{
	return 0;
}

void SimpleSystem::SlaveControl::GetClassName(MSTR& s)
{
	s = L"SlaveControl"; //TODO: move to the string table
}

Class_ID SimpleSystem::SlaveControl::ClassID()
{
	return Class_ID(SIMPLE_SYSTEM_SLAVE_CONTROL);
}

SClass_ID SimpleSystem::SlaveControl::SuperClassID()
{
	return CTRL_MATRIX3_CLASS_ID;
}

void SimpleSystem::SlaveControl::DeleteThis()
{
	delete this;
}

void SimpleSystem::SlaveControl::BeginEditParams(IObjParam* ip, ULONG flags, Animatable* prev)
{
	master->BeginEditParams(ip, flags, prev);
}

void SimpleSystem::SlaveControl::EndEditParams(IObjParam* ip, ULONG flags, Animatable* next)
{
	master->EndEditParams(ip, flags, next);
}

RefTargetHandle SimpleSystem::SlaveControl::Clone(RemapDir& remap)
{
	SlaveControl* new_slave_control = new SlaveControl;
	new_slave_control->id = id;
	new_slave_control->ReplaceReference(0, remap.CloneRef(master));
	BaseClone(this, new_slave_control, remap);
	return new_slave_control;
}

int SimpleSystem::SlaveControl::NumRefs()
{
	return 1;
}

RefTargetHandle SimpleSystem::SlaveControl::GetReference(int i)
{
	return master;
}

void SimpleSystem::SlaveControl::SetReference(int i, RefTargetHandle rtarg)
{
	master = (i == 0) ? static_cast<Master*>(rtarg): master;
}
