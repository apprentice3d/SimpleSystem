#pragma once

//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Includes for Plugins
// AUTHOR: 
//***************************************************************************/

#define _USE_MATH_DEFINES
#include <math.h>
#include "3dsmaxsdk_preinclude.h"
#include <3dsmaxdlport.h>
#include "resource.h"
#include <istdplug.h>
#include <iparamb2.h>
#include <Simpobj.h>
#include <dummy.h>
#include <mouseman.h>
#include <MouseCursors.h>
#include <IIndirectRefTargContainer.h>
//SIMPLE TYPE




//#include <IIndirectRefMaker.h>



extern TCHAR *GetString(int id);

extern HINSTANCE hInstance;
