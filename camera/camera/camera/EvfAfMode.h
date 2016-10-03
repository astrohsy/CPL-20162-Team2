/******************************************************************************
*                                                                             *
*   PROJECT : EOS Digital Software Development Kit EDSDK                      *
*      NAME : EvfAFMode.h	                                                  *
*                                                                             *
*   Description: This is the Sample code to show the usage of EDSDK.          *
*                                                                             *
*                                                                             *
*******************************************************************************
*                                                                             *
*   Written and developed by Camera Design Dept.53                            *
*   Copyright Canon Inc. 2006-2008 All Rights Reserved                        *
*                                                                             *
*******************************************************************************/

#pragma once

#include "Observer.h"
#include "ActionSource.h"
// CEvfAFMode

class CEvfAFMode : public ActionSource, public Observer
{
public:
	CEvfAFMode();
	virtual ~CEvfAFMode();

	//observer
	virtual void update(Observable* from, CameraEvent *e);

protected:
	void OnSelChange();
};
