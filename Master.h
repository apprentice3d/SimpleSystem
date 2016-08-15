#pragma once
#include "SimpleSystem.h"

#define SIMPLE_SYSTEM_MASTER Class_ID(0x5b352f44, 0x7c94590)


namespace SimpleSystem
{

	/*************************** Class Descriptor Related **************************/
	ClassDesc2* getMasterDescriptor();

// [Class Descriptors](http://help.autodesk.com/view/3DSMAX/2017/ENU/?guid=__files_GUID_9C5B4241_2FCE_4EBD_9732_6E2F52368E24_htm)
	class MasterClassDesc : public ClassDesc2
	{
	public:
		int IsPublic() override;
		void* Create(BOOL loading) override;
		const wchar_t* ClassName() override;
		SClass_ID SuperClassID() override;
		Class_ID ClassID() override;
		const wchar_t* Category() override;
		HINSTANCE	HInstance();

		int BeginCreate(Interface* i) override;
		int EndCreate(Interface* i) override;
	};


	/*************************** Class Related ************************************/
// [Reference System] (http://help.autodesk.com/view/3DSMAX/2017/ENU/?guid=__files_GUID_C11C8544_C5D5_4BA4_81A7_B00E8F508A03_htm)
	class Master : public ReferenceTarget // == A scene entity that is being owned and listened to by other scene entities. 
	{
	public:
		Master(const int num_node_slave = 4);
		~Master();
		
		Master(const Master&);
		Master& operator=(const Master&);

		enum parameters
		{
			pb_number = 0,
			pb_radius,
			pb_cycle,
			pb_amplitude,
			pb_phase
		};

		enum references // 
		{
			param_block,
			node_container,
			number_of_refs
		};

// [Multiple Parameter Blocks](http://help.autodesk.com/view/3DSMAX/2017/ENU/?guid=__files_GUID_B0DEDB29_D397_42C0_BCC9_0F75E59703D2_htm)
		enum ui_parts 
		{
			generic_part
		};


		// Setters
		bool setSlaveNode(const int index, const INode* node);
		bool setNumSlaves(const TimeValue time, const int new_number, const bool notify = true);
		bool setRadius(const TimeValue time, const float radius);
		bool setCycle(const TimeValue time, const float cycle);
		bool setAmlitude(const TimeValue time, const float amplitude);
		bool setPhase(const TimeValue time, const float phase);

		// Getters
		const INode* getSlaveNode(const int slavenode_number) const;
		int getSlaveNumber(TimeValue time, Interval& validity = Interval(0, 0)) const;
		float getRadius(TimeValue time, Interval& validity = Interval(0, 0)) const;
		float getCycle(TimeValue time, Interval& validity = Interval(0, 0)) const;
		float getAmplitude(TimeValue time, Interval& validity = Interval(0, 0)) const;
		float getPhase(TimeValue time, Interval& validity = Interval(0, 0)) const;

		void getValue(TimeValue time, void* value, Interval& validity, GetSetMethod method, int id);
		void updateAnimKeyBrackets(const TimeValue time, int pb_index);


		// From Animatable
		void DeleteThis() override;
		Class_ID ClassID() override;
		void BeginEditParams(IObjParam* ip, ULONG flags, Animatable* prev) override;
		void EndEditParams(IObjParam* ip, ULONG flags, Animatable* next) override;
		int NumSubs() override;
		Animatable* SubAnim(int i) override;
		MSTR SubAnimName(int i) override;
		void GetSystemNodes(INodeTab& nodes, SysNodeContext Context) override;
		void GetClassName(MSTR& s) override;
		SClass_ID SuperClassID() override;

		int NumParamBlocks() override;
		IParamBlock2* GetParamBlock(int i) override;
		IParamBlock2* GetParamBlockByID(short id) override;

		// From ReferenceTarget
		int NumRefs() override;
		RefTargetHandle GetReference(int i) override;


	protected:
		RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate) override;
		void SetReference(int i, RefTargetHandle rtarg) override;


	private:
		IParamBlock2* pblock2;
		IIndirectRefTargContainer* m_nodes;
		HWND hPanel;
		static IObjParam* ip;
		static INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};

	Control* GetNewSlaveControl(Master* master, int index);



	/*************************** PBAccessor ************************************/
	//[Monitoring Parameter Channges](http://help.autodesk.com/view/3DSMAX/2017/ENU/?guid=__files_GUID_1F541F96_79DD_4F13_89CC_AF7A1ECEF8E8_htm)
	class NumberPBAccessor : PBAccessor
	{
	public:
		void Get(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t, Interval& valid) override;
	};



}







