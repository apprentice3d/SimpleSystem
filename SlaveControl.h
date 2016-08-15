#pragma once
#include "Master.h"

#define SIMPLE_SYSTEM_SLAVE_CONTROL Class_ID(0x7acd61ca, 0x5f954622)

namespace SimpleSystem
{

	/*************************** Class Descriptor Related **************************/
	ClassDesc2* getSlaveDescriptor();

	class SlaveControlClassDesc : public ClassDesc2
	{
	public:
		int IsPublic() override;
		void* Create(BOOL loading) override;
		const wchar_t* ClassName() override;
		SClass_ID SuperClassID() override;
		Class_ID ClassID() override;
		const wchar_t* Category() override;
	};


	/*************************** Class Related ************************************/
//[Controller API Elements](http://help.autodesk.com/view/3DSMAX/2017/ENU/?guid=__files_GUID_5A09D1BA_9881_495B_B9A3_F7C9FAD8B536_htm)
//Also check [ISurfPosition Class ](http://help.autodesk.com/view/3DSMAX/2017/ENU/?guid=__cpp_ref_class_i_surf_position_html)	
	class SlaveControl: public Control
	{
	public:
		SlaveControl(bool loading = false);
		SlaveControl(Master*, int);
		virtual ~SlaveControl();
		
		SlaveControl(const SlaveControl&);
		SlaveControl& operator=(const SlaveControl&);

		void SetID(ULONG);

		// From Control
		void Copy(Control* from) override;
		void GetValue(TimeValue t, void* val, Interval& valid, GetSetMethod method = CTRL_ABSOLUTE) override;
		void SetValue(TimeValue t, void* val, int commit, GetSetMethod method = CTRL_ABSOLUTE) override;
		BOOL IsLeaf() override;
		BOOL CanCopyAnim() override;
		BOOL IsReplaceable() override;
		void* GetInterface(ULONG id) override;
		int NumSubs() override;
		Animatable* SubAnim(int i) override;
		MSTR SubAnimName(int i) override;
		int IsKeyable() override;


		// From Animatable
		void GetClassName(MSTR& s) override;
		Class_ID ClassID() override;
		SClass_ID SuperClassID() override;
		void DeleteThis() override;
		void BeginEditParams(IObjParam* ip, ULONG flags, Animatable* prev) override;
		void EndEditParams(IObjParam* ip, ULONG flags, Animatable* next) override;


		// From Reference Target
		RefTargetHandle Clone(RemapDir& remap) override;
		int NumRefs() override;
		RefTargetHandle GetReference(int i) override;
	protected:
		void SetReference(int i, RefTargetHandle rtarg) override;
		RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate) override;
	
	private:
		Master* master;
		ULONG id;

	};
} 
