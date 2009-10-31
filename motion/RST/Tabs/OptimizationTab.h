/*
 * OptimizationTab.h
 *
 *  Created on: Oct 30, 2009
 *      Author: alexgc
 */

#ifndef OPTIMIZATIONTAB_H_
#define OPTIMIZATIONTAB_H_

#include "RSTTab.h"

class OptimizationTab : public RSTTab
{
public:
	OptimizationTab(){};
    OptimizationTab(wxWindow * parent, wxWindowID id = -1,
             const wxPoint & pos = wxDefaultPosition,
             const wxSize & size = wxDefaultSize,
             long style = wxTAB_TRAVERSAL);
	virtual ~OptimizationTab(){}

	wxSizer* sizerFull;

	// Generic button handler - will swtich between button IDs
    void OnButton(wxCommandEvent &evt);

	//Button to start optimization
	//Button to set original path
	//Button to set optimized path

	void OnSlider(wxCommandEvent &evt);
	void RSTStateChange();

	DECLARE_DYNAMIC_CLASS(OptimizationTab)
	DECLARE_EVENT_TABLE()
};

#endif /* OPTIMIZATIONTAB_H_ */
