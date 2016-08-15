#include "Master.h"
#include "MasterCreationManager.h"
#include "SlaveControl.h"

static SimpleSystem::MasterClassDesc master_descriptor;
static SimpleSystem::NumberPBAccessor number_accessor;
IObjParam* SimpleSystem::Master::ip = nullptr;



/*************************** Class Descriptor Related **************************/

ClassDesc2* SimpleSystem::getMasterDescriptor()
{
	return &master_descriptor;
}

int SimpleSystem::MasterClassDesc::IsPublic()
{
	return true;
}


void* SimpleSystem::MasterClassDesc::Create(BOOL loading = false)
{
	return new Master();
}


const wchar_t* SimpleSystem::MasterClassDesc::ClassName()
{
	return GetString(IDS_CLASS_NAME);
}


SClass_ID SimpleSystem::MasterClassDesc::SuperClassID()
{
	return SYSTEM_CLASS_ID;
}


Class_ID SimpleSystem::MasterClassDesc::ClassID()
{
	return SIMPLE_SYSTEM_MASTER;
}


const wchar_t* SimpleSystem::MasterClassDesc::Category()
{
	return L"";//For System plugins, this is not important
}


HINSTANCE SimpleSystem::MasterClassDesc::HInstance()
{
	return hInstance;
}


int SimpleSystem::MasterClassDesc::BeginCreate(Interface* i)
{
	SuspendSetKeyMode();
	IObjCreate* iob = i->GetIObjCreate();
	master_create_mode.Begin(iob, this);
	iob->PushCommandMode(&master_create_mode);
	return true;
}


int SimpleSystem::MasterClassDesc::EndCreate(Interface* i)
{
	ResumeSetKeyMode();
	master_create_mode.End();
	i->RemoveMode(&master_create_mode);
	return true;
}





/*************************** Class Related ************************************/


SimpleSystem::Master::Master(const int num_node_slave)
	:pblock2(nullptr),
	m_nodes(nullptr)
{
	ReplaceReference(references::param_block, pblock2);
	
	IIndirectRefTargContainer* monitored_nodes = reinterpret_cast<IIndirectRefTargContainer*>(GetCOREInterface()->CreateInstance(REF_TARGET_CLASS_ID, INDIRECT_REFTARG_CONTAINER_CLASS_ID));
	ReplaceReference(references::node_container, monitored_nodes);

	if (num_node_slave > 0) { m_nodes->SetItem(num_node_slave - 1, nullptr); }
	// Autocreate paremeter blocks from paramteter block descriptor.
	// http://help.autodesk.com/view/3DSMAX/2017/ENU/?guid=__files_GUID_A232F26A_3000_471C_AD97_35C658B88A9E_htm
	getMasterDescriptor()->MakeAutoParamBlocks(this);
}


SimpleSystem::Master::~Master()
{
}


bool SimpleSystem::Master::setSlaveNode(const int index, const INode* node)
{
	if(m_nodes)
	{
		m_nodes->SetItem(index, const_cast<INode*>(node));
		return true;
	}

	return false;
}


bool SimpleSystem::Master::setNumSlaves(const TimeValue time, const int new_number, const bool notify)
{

	if (new_number < 1 || new_number == getSlaveNumber(time))
	{
		return false;
	}
	
	if(m_nodes && m_nodes->GetNumItems() > 0)
	{
		int current_node_count = m_nodes->GetNumItems();
		if(new_number < current_node_count)
		{
			//remove nodes
			for (int i = current_node_count-1; i >= new_number; --i)
			{
				ReferenceTarget* ref_targ = m_nodes->GetItem(i);
				if(ref_targ)
				{
					INode* node = static_cast<INode*>(ref_targ->GetInterface(INODE_INTERFACE));
					if(node->Selected())
					{
						pblock2->SetValue(parameters::pb_number, time, new_number + 1);
						break;
					}
					node->Delete(time, true);
				}
				m_nodes->RemoveItem(i);
			}
		}
		else
		{
			m_nodes->SetItem(new_number - 1, nullptr);
			if (m_nodes->GetItem(0))
			{
				ReferenceTarget* reference_target = m_nodes->GetItem(0);
				INode* first_node = static_cast<INode*>(reference_target->GetInterface(INODE_INTERFACE));
				Object* obj = first_node->GetObjectRef();
				INode* parent_node = first_node->GetParentNode();
				Interface* ip = GetCOREInterface();
				for (int i = current_node_count; i < new_number; ++i)
				{
					INode* new_node = ip->CreateObjectNode(obj);
					Control* slave = GetNewSlaveControl(this, i);
					new_node->SetTMController(slave);
					new_node->FlagForeground(time, false);
					parent_node->AttachChild(new_node);
					setSlaveNode(i, new_node);
				}
			}
		}
	
	}
	if(notify)
	{
		NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);
	}

	return true;
}


bool SimpleSystem::Master::setRadius(const TimeValue time, const float radius)
{
	pblock2->SetValue(parameters::pb_radius, time, radius);
	NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);

	return true;
}


bool SimpleSystem::Master::setCycle(const TimeValue time, const float cycle)
{
	pblock2->SetValue(parameters::pb_cycle, time, cycle);
	NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);

	return true;
}


bool SimpleSystem::Master::setAmlitude(const TimeValue time, const float amplitude)
{
	pblock2->SetValue(parameters::pb_amplitude, time, amplitude);
	NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);

	return true;
}

bool SimpleSystem::Master::setPhase(const TimeValue time, const float phase)
{
	pblock2->SetValue(parameters::pb_phase, time, phase);
	NotifyDependents(FOREVER, PART_OBJ, REFMSG_CHANGE);

	return true;
}

const INode* SimpleSystem::Master::getSlaveNode(const int slavenode_number) const
{
	if (m_nodes != nullptr)
	{
		ReferenceTarget* target_node = m_nodes->GetItem(slavenode_number);
		if(target_node)
		{
			return static_cast<INode*>(target_node->GetInterface(INODE_INTERFACE));
		}
	}

	return nullptr;
}

int SimpleSystem::Master::getSlaveNumber(TimeValue time, Interval& validity) const
{

	return (m_nodes) ? m_nodes->GetNumItems() : -1;
}

float SimpleSystem::Master::getRadius(TimeValue time, Interval& validity) const
{

	return pblock2->GetFloat(parameters::pb_radius, time,validity);
}

float SimpleSystem::Master::getCycle(TimeValue time, Interval& validity) const
{

	return pblock2->GetFloat(parameters::pb_cycle, time);
}

float SimpleSystem::Master::getAmplitude(TimeValue time, Interval& validity) const
{

	return pblock2->GetFloat(parameters::pb_amplitude, time);
}

float SimpleSystem::Master::getPhase(TimeValue time,  Interval& validity) const
{

	return pblock2->GetFloat(parameters::pb_phase, time);
}


void SimpleSystem::Master::updateAnimKeyBrackets(const TimeValue time, int pb_index)
{
	//TODO: implement it later if needed [used to tune up the autokey setting]
}

void SimpleSystem::Master::DeleteThis()
{
	delete this;
}

SClass_ID SimpleSystem::Master::SuperClassID()
{
	return SYSTEM_CLASS_ID;
}

Class_ID SimpleSystem::Master::ClassID()
{
	return SIMPLE_SYSTEM_MASTER;
}


int SimpleSystem::Master::NumSubs()
{
	return 1;
}

Animatable* SimpleSystem::Master::SubAnim(int index)
{
	return pblock2;
}

MSTR SimpleSystem::Master::SubAnimName(int index)
{
	return GetString(IDS_SIMPLE_SYSTEM_PARAM);
}

void SimpleSystem::Master::GetSystemNodes(INodeTab& nodes, SysNodeContext Context)
{
	//TODO: implement it later if needed [used within saving/loading/clone proc]
}

void SimpleSystem::Master::GetClassName(MSTR& s)
{
	s = L"Master";
}


int SimpleSystem::Master::NumParamBlocks()
{
	return 1;
}

IParamBlock2* SimpleSystem::Master::GetParamBlock(int index)
{
	return pblock2;
}

IParamBlock2* SimpleSystem::Master::GetParamBlockByID(short id)
{
	return (pblock2->ID() == id) ? pblock2 : nullptr;
}

int SimpleSystem::Master::NumRefs()
{
	return references::number_of_refs;
}

RefTargetHandle SimpleSystem::Master::GetReference(int index)
{
	if(index == references::param_block)
	{
		return pblock2;
	}

	if(index == references::node_container)
	{
		return m_nodes;
	}

	return nullptr;
}

RefResult SimpleSystem::Master::NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate)
{
	switch(message)
	{
		
	case REFMSG_GET_PARAM_DIM:
	{
		GetParamDim* param_dim = reinterpret_cast<GetParamDim*>(partID);
		switch(param_dim->index)
		{
		case parameters::pb_radius:
		case parameters::pb_cycle:
		case parameters::pb_amplitude:
		case parameters::pb_phase:
			param_dim->dim = stdWorldDim;
			break;
		}
		return REF_HALT;
	}

	case REFMSG_GET_PARAM_NAME:
		GetParamName* param_name = reinterpret_cast<GetParamName*>(partID);
		switch (param_name->index)
		{
			//TODO: move all strings to the String Table
		case parameters::pb_radius:
			param_name->name = L"Radius";
			break;
		case parameters::pb_amplitude:
			param_name->name = L"Amplitude";
			break;
		case parameters::pb_cycle:
			param_name->name = L"Cycle";
			break;
		case parameters::pb_phase:
			param_name->name = L"Phase";
			break;
		}
		return REF_HALT;
	}

	return REF_SUCCEED;
}

void SimpleSystem::Master::SetReference(int index, RefTargetHandle rtarg)
{
	switch (index)
	{
		case references::param_block:
		{
			IParamBlock2* possible_block = static_cast<IParamBlock2*>(rtarg);
			pblock2 = (possible_block) ? possible_block : pblock2;
			break;
		}
		case references::node_container:
		{
			IIndirectRefTargContainer* possible_container = static_cast<IIndirectRefTargContainer*>(rtarg);
			m_nodes = (possible_container) ? possible_container : m_nodes;
			break;
		}
	}
}

Control* SimpleSystem::GetNewSlaveControl(Master* master, int index)
{
	return new SlaveControl(master, index);
}



/************************* Main Logic ***********************************/
void SimpleSystem::Master::getValue(TimeValue time, void* value, Interval& validity, GetSetMethod method, int id)
{
#pragma message(TODO("'Get value' implementation incomplete."))
	float radius = getRadius(time, validity);
	float amplitude = getAmplitude(time, validity);
	float cycle = getCycle(time, validity);
	float phase = getPhase(time, validity);
	float number = getSlaveNumber(time, validity);
	float angle = id * 2 * M_PI / number;
	Matrix3 transf_matrix;
	transf_matrix.IdentityMatrix();
	Matrix3* given_matrix = static_cast<Matrix3*>(value);

	transf_matrix.Translate(Point3(radius, 0.0f, static_cast<float>(amplitude*cos(cycle*angle + 2 * M_PI*phase))));
	transf_matrix.RotateZ(angle);

	(*given_matrix) = (method == CTRL_RELATIVE) ? transf_matrix*(*given_matrix) : transf_matrix;
}


/************************* UI Related ***********************************/

void SimpleSystem::Master::BeginEditParams(IObjParam* ip, ULONG flags, Animatable* prev)
{
	this->ip = ip;
	getMasterDescriptor()->BeginEditParams(ip, this, flags, prev);
}

void SimpleSystem::Master::EndEditParams(IObjParam* ip, ULONG flags, Animatable* next)
{

	getMasterDescriptor()->EndEditParams(ip, this, flags, next);
	this->ip = nullptr;
}

// ParameterBlock related. For more information see (http://help.autodesk.com/view/3DSMAX/2017/ENU/?guid=__files_GUID_CC09846C_8115_46CF_8DC1_9C4F1A06991A_htm)
static ParamBlockDesc2 simplesystem_param_blk(SimpleSystem::Master::ui_parts::generic_part, _T("params"), 0, SimpleSystem::getMasterDescriptor(),
	P_AUTO_CONSTRUCT + P_AUTO_UI, SimpleSystem::Master::references::param_block,
	//rollout
	IDD_PANEL, IDS_PARAMS, 0, 0, NULL,
	// params
	SimpleSystem::Master::parameters::pb_radius, _T("radius"), TYPE_FLOAT, P_ANIMATABLE, IDS_RADIUS,
	p_default, 100.0f,
	p_range, 0.0f, 500.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_RADIUS_EDIT, IDC_RADIUS_SPIN, 0.01f,
	p_end,

	SimpleSystem::Master::parameters::pb_amplitude, _T("amplitude"), TYPE_FLOAT, P_ANIMATABLE, IDS_AMPLITUDE,
	p_default, 20.0f,
	p_range, 0.0f, 500.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_AMPLITUDE_EDIT, IDC_AMPLITUDE_SPIN, 0.01f,
	p_end,

	SimpleSystem::Master::parameters::pb_cycle, _T("cycles"), TYPE_FLOAT, P_ANIMATABLE, IDS_CYCLE,
	p_default, 10.0f,
	p_range, 0.0f, 10.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_CYCLES_EDIT, IDC_CYCLES_SPIN, 1.0f,
	p_end,

	SimpleSystem::Master::parameters::pb_phase, _T("phase"), TYPE_FLOAT, P_ANIMATABLE, IDS_PHASE,
	p_default, 1.0f,
	p_range, -1000.0f, 1000.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_PHASE_EDIT, IDC_PHASE_SPIN, 0.01f,
	p_end,

	SimpleSystem::Master::parameters::pb_number, _T("number"), TYPE_FLOAT, P_ANIMATABLE, IDS_NUMBER,
	p_default, 4.0f,
	p_range, 1.0f, 2000.0f,
	p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_NUMBER_EDIT, IDC_NUMBER_SPIN, 1.0f,
	p_accessor, &number_accessor,
	p_end,

	p_end
);


void SimpleSystem::NumberPBAccessor::Get(PB2Value& value, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t, Interval& valid)
{
	Master* this_owner = static_cast<Master*>(owner);
	this_owner->setNumSlaves(t, value.f);
}
