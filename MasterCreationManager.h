#pragma once
#include "Master.h"


namespace SimpleSystem
{

	/*************************** Creation Process Related **************************/
//[Command Modes and Mouse Procedures](http://help.autodesk.com/view/3DSMAX/2017/ENU/?guid=__files_GUID_006DE220_0F0A_4BB9_BD8B_28E6A24D5239_htm)
	class MasterCreationManager: public MouseCallBack, public ReferenceMaker
	{

	public:
		MasterCreationManager();

		void Begin(IObjCreate*, ClassDesc*);
		void End();
		void createNewMaster();

		
		// From MouseCallBack
		int proc(HWND hwnd, int msg, int point, int flags, IPoint2 m) override;


		// From ReferenceMaker
		void GetClassName(MSTR& s) override;
		int NumRefs() override;
		RefTargetHandle GetReference(int i) override;
	protected:
		void SetReference(int i, RefTargetHandle rtarg) override;
		RefResult NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate) override;

	private:
		CreateMouseCallBack* create_callback;
		INode* main_node;
		Master* the_master;
		IObjCreate* create_interface;
		ClassDesc* class_desc;
		Matrix3 trans_matrix;
		IPoint2 first_click;
		Point3 center;
		bool attached_to_node;
//		int last_put_count;
		bool ignore_selection_change;
	};


	/***************************** Creation Mode Related ***************************/
	class MasterCreateMode: public CommandMode
	{
	public:
		void Begin(IObjCreate*, ClassDesc*);
		void End();
		bool isSticky();

		// From CommandMode
		int Class() override;
		int ID() override;
		MouseCallBack* MouseProc(int* numPoints) override;
		ChangeForegroundCallback* ChangeFGProc() override;
		BOOL ChangeFG(CommandMode* oldMode) override;
		void EnterMode() override;
		void ExitMode() override;

	private:
		MasterCreationManager proc;
	};



	static MasterCreateMode master_create_mode;


}
