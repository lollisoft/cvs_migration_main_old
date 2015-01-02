/*...sLicence:0:*/
/*
    DMF Distributed Multiplatform Framework (the initial goal of this library)
    This file is part of lbDMF.
    Copyright (C) 2002-2015  Lothar Behrens (lothar.behrens@lollisoft.de)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


    The author of this work will be reached by e-Mail or paper mail.
    e-Mail: lothar.behrens@lollisoft.de
    p-Mail: Lothar Behrens
            Ginsterweg 4

            65760 Eschborn (germany)
*/
/*...e*/


#include <lbConfigHook.h>

/*...smisc and includes:0:*/
#ifdef __GNUG__
#pragma implementation "wxUpdateChecker.cpp"
#pragma interface "wxUpdateChecker.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

/*...swx ifdef\39\s:0:*/
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifdef LB_I_EXTENTIONS
#include <lbConfigHook.h>
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#if defined(__WXGTK__) || defined(__WXMOTIF__)
#include "mondrian.xpm"
#endif
/*...e*/

#include "wx/sstream.h"
#include "wx/protocol/http.h"

#include "wx/jsonreader.h"
#include "wx/jsonval.h"

/*...e*/
/*...sLB_DATABASE_DLL scope:0:*/
#define LB_LOGINWIZARD_DLL
#include <wxUpdateChecker-module.h>
/*...e*/

#include <timerEventInterceptor.h>
#include <wxUpdateChecker.h>

/** \brief This is the plugin module definition class for the login wizard.
 *
 */
class lbPluginModulewxUpdateChecker :
	public lb_I_PluginModule {
public:

	lbPluginModulewxUpdateChecker();
	virtual ~lbPluginModulewxUpdateChecker();

	DECLARE_LB_UNKNOWN()

	void LB_STDCALL initialize();
	void LB_STDCALL install();

	DECLARE_PLUGINS()
};

/*...sclass lbPluginModuleLoginWizard implementation:0:*/
BEGIN_IMPLEMENT_LB_UNKNOWN(lbPluginModulewxUpdateChecker)
        ADD_INTERFACE(lb_I_PluginModule)
END_IMPLEMENT_LB_UNKNOWN()

IMPLEMENT_FUNCTOR(instanceOfPluginModule, lbPluginModulewxUpdateChecker)

BEGIN_PLUGINS(lbPluginModulewxUpdateChecker)
	ADD_PLUGIN(wxUpdateChecker, GUI)
END_PLUGINS()

lbPluginModulewxUpdateChecker::lbPluginModulewxUpdateChecker() {
}

lbPluginModulewxUpdateChecker::~lbPluginModulewxUpdateChecker() {
	_CL_VERBOSE << "lbPluginModuleLoginWizard::~lbPluginModuleLoginWizard() called." LOG_
}

void LB_STDCALL lbPluginModulewxUpdateChecker::initialize() {
	enumPlugins();
}

void LB_STDCALL lbPluginModulewxUpdateChecker::install() {
	
}

lbErrCodes LB_STDCALL lbPluginModulewxUpdateChecker::setData(lb_I_Unknown* uk) {
        _CL_VERBOSE << "lbPluginModulewxUpdateChecker::setData(...) not implemented yet" LOG_

        return ERR_NOT_IMPLEMENTED;
}
/*...e*/

class UpdateCheckerHandler :
	public lb_I_Unknown,
	public lb_I_LogonHandler,
	public lb_I_EventHandler {
public:
		UpdateCheckerHandler();
		virtual ~UpdateCheckerHandler();

		DECLARE_LB_UNKNOWN()

		lbErrCodes LB_STDCALL registerEventHandler(lb_I_Dispatcher* disp);
		lb_I_Unknown* LB_STDCALL getUnknown();
		lbErrCodes LB_STDCALL RunUpdateCheck(lb_I_Unknown* uk);
};

BEGIN_IMPLEMENT_LB_UNKNOWN(UpdateCheckerHandler)
	ADD_INTERFACE(lb_I_EventHandler)
END_IMPLEMENT_LB_UNKNOWN()

lbErrCodes LB_STDCALL UpdateCheckerHandler::setData(lb_I_Unknown* uk) {
        _CL_VERBOSE << "UpdateCheckerHandler::setData(...) not implemented yet" LOG_

        return ERR_NOT_IMPLEMENTED;
}

lb_I_Unknown* LB_STDCALL UpdateCheckerHandler::getUnknown() {
	UAP(lb_I_Unknown, uk)
	queryInterface("lb_I_Unknown", (void**) &uk, __FILE__, __LINE__); 
	uk++;
	return uk.getPtr();
}

lbErrCodes LB_STDCALL UpdateCheckerHandler::registerEventHandler(lb_I_Dispatcher* disp) {
	disp->addEventHandlerFn(this, (lbEvHandler) &UpdateCheckerHandler::RunUpdateCheck, "RunUpdateCheck");

	return ERR_NONE;
}

lbErrCodes LB_STDCALL UpdateCheckerHandler::RunUpdateCheck(lb_I_Unknown* uk) {
	wxHTTP get;
	get.SetHeader(_T("Content-type"), _T("text/html; charset=utf-8"));
	get.SetTimeout(10); // 10 seconds of timeout instead of 10 minutes ...
	
	// this will wait until the user connects to the internet. It is important in case of dialup (or ADSL) connections
	while (!get.Connect(_T("www.lollisoft.de")))  // only the server, no pages here yet ...
		wxSleep(5);
	
	wxApp::IsMainLoopRunning(); // should return true
	
	// use _T("/") for index.html, index.php, default.asp, etc.
	wxInputStream *httpStream = NULL;
	
	UAP_REQUEST(getModuleInstance(), lb_I_String, uri)
	
	*uri = "/updates/lbdmf.json?ostype=";
	*uri += getOsType();
	
	httpStream = get.GetInputStream(_T(uri->charrep()));
	
	if (get.GetError() == wxPROTO_NOERR)
	{
		wxString res;
		wxStringOutputStream out_stream(&res);
		httpStream->Read(out_stream);
		
		// construct the JSON root object
		wxJSONValue  root;
		
		// construct a JSON parser
		wxJSONReader reader;

		bool updateAvailable = false;
		UAP_REQUEST(getModuleInstance(), lb_I_String, msg)
		
		int numErrors = reader.Parse( res, &root );
		
		if (numErrors > 0) {
			wxMessageBox(_T("Unable to read update information!"));
		} else {
			//wxMessageBox(res);
			
			// now retrive the array of supported languages
			wxJSONValue releases = root["releases"];
			
			bool isArray = releases.IsArray();
			
			wxString currentRelease = "1.0.4.2-final";
			
			UAP_REQUEST(getModuleInstance(), lb_I_MetaApplication, meta)
			UAP(lb_I_Parameter, UpdateSettings)
			UpdateSettings = meta->getPropertySet("UpdateSettings");
			UAP_REQUEST(getModuleInstance(), lb_I_String, name)
			UAP_REQUEST(getModuleInstance(), lb_I_Container, KnownVersions)
			*name = "KnownVersions";
			UpdateSettings->getUAPContainer(*&name, *&KnownVersions);
			
			*msg = "There are new versions available. Go to the product page and download the following version: ";
			
			if (isArray) {
				for ( int i = 0; i < releases.Size(); i++ ) {
					UAP_REQUEST(getModuleInstance(), lb_I_String, version)
					wxString release = releases[i]["version"].AsString();
					
					*version = release.c_str();
					
					UAP(lb_I_KeyBase, key)
					UAP(lb_I_Unknown, uk)
					QI(version, lb_I_KeyBase, key)
					QI(version, lb_I_Unknown, uk)
					
					if (KnownVersions->exists(&key) == 0) {
						
						if (release <= currentRelease)
							break;

						updateAvailable = true;
						KnownVersions->insert(&uk, &key);
						
						*msg += version->charrep();
					}
				}
			}			
		}
		
		if (updateAvailable) wxMessageBox(_T(msg->charrep()));
	}
	else
	{
		wxMessageBox(_T("Unable to connect to check for updates!"));
	}
	
	wxDELETE(httpStream);
	get.Close();
	
	return ERR_NONE;
}

UpdateCheckerHandler::UpdateCheckerHandler() {
	_CL_VERBOSE << "UpdateCheckerHandler::UpdateCheckerHandler() called." LOG_
}

UpdateCheckerHandler::~UpdateCheckerHandler() {
	_CL_VERBOSE << "UpdateCheckerHandler::~UpdateCheckerHandler() called." LOG_
}


/** \brief Implements a wizard based login plugin.
 *
 */
class wxUpdateChecker :
	public lb_I_PluginImpl
{
public:

	wxUpdateChecker();
	virtual ~wxUpdateChecker();

	DECLARE_LB_UNKNOWN()

	bool LB_STDCALL canAutorun();
	lbErrCodes LB_STDCALL autorun();
	/** \brief Init the menu emtries.
	 *
	 * This connects the login feature to a menu.
	 */
	virtual void LB_STDCALL initialize();

	/** \brief Run the login manually.
	 *
	 * This let the login wizard appear manually without invoking it from
	 * the menu entry. You could use this to start the login wizard automatically.
	 */
	virtual bool LB_STDCALL run();

	/** \brief Get the underlying implementation.
	 *
	 * Not needed in this implementation. This implementation has no separate
	 * class with the implementation. This is due to the not existing problem
	 * of multible base class inheritation of lb_I_Unknown.
	 *
	 * If multible intarfaces could be queried, then each
	 */
	virtual lb_I_Unknown* LB_STDCALL getImplementation();

	virtual lb_I_Unknown* LB_STDCALL peekImplementation();
	void LB_STDCALL releaseImplementation();

	UAP(lb_I_Unknown, wxUpdateCheckerHandler)
	UAP(lb_I_DispatchInterceptor, timerEventInterceptor)
};

BEGIN_IMPLEMENT_LB_UNKNOWN(wxUpdateChecker)
        ADD_INTERFACE(lb_I_PluginImpl)
END_IMPLEMENT_LB_UNKNOWN()

IMPLEMENT_FUNCTOR(instanceOfwxUpdateChecker, wxUpdateChecker)

lbErrCodes LB_STDCALL wxUpdateChecker::setData(lb_I_Unknown* uk) {
        _CL_VERBOSE << "wxUpdateChecker::setData(...) not implemented yet" LOG_

        return ERR_NOT_IMPLEMENTED;
}

wxUpdateChecker::wxUpdateChecker() {
}

wxUpdateChecker::~wxUpdateChecker() {
	_CL_VERBOSE << "wxUpdateChecker::~wxUpdateChecker() called." LOG_
}

bool LB_STDCALL wxUpdateChecker::canAutorun() {
	return true;
}

lbErrCodes LB_STDCALL wxUpdateChecker::autorun() {
	lbErrCodes err = ERR_NONE;

	UAP_REQUEST(getModuleInstance(), lb_I_EventManager, ev)

	int lEvent;

	ev->registerEvent("RunUpdateCheck", lEvent);

	UAP_REQUEST(getModuleInstance(), lb_I_Dispatcher, disp)

	UpdateCheckerHandler* hdl = new UpdateCheckerHandler();
	

	QI(hdl, lb_I_Unknown, wxUpdateCheckerHandler)

	hdl->registerEventHandler(*&disp);

	UAP_REQUEST(getModuleInstance(), lb_I_MetaApplication, meta)

	char* file = strdup(_trans("&File"));
	char* entry = strdup(_trans("Check for &Updates\tCtrl-U"));

	meta->addMenuEntry(file, entry, "RunUpdateCheck", "");

	free(file);
	free(entry);

	lbTimerEventInterceptor* ti = new lbTimerEventInterceptor();
	
	QI(ti, lb_I_DispatchInterceptor, timerEventInterceptor)
	
	return err;
}

/*...svoid LB_STDCALL wxUpdateChecker\58\\58\initialize\40\\41\:0:*/
void LB_STDCALL wxUpdateChecker::initialize() {
}
/*...e*/

/*...sbool LB_STDCALL wxUpdateChecker\58\\58\run\40\\41\:0:*/
bool LB_STDCALL wxUpdateChecker::run() {
	return true;
}
/*...e*/

/*...slb_I_Unknown\42\ LB_STDCALL wxUpdateChecker\58\\58\peekImplementation\40\\41\:0:*/
lb_I_Unknown* LB_STDCALL wxUpdateChecker::peekImplementation() {
	lbErrCodes err = ERR_NONE;

	return this;
}
/*...e*/
/*...slb_I_Unknown\42\ LB_STDCALL wxUpdateChecker\58\\58\getImplementation\40\\41\:0:*/
lb_I_Unknown* LB_STDCALL wxUpdateChecker::getImplementation() {
	lbErrCodes err = ERR_NONE;

	return this;
}
/*...e*/
void LB_STDCALL wxUpdateChecker::releaseImplementation() {
	_CL_VERBOSE << "wxUpdateChecker::releaseImplementation() called." LOG_
}

#ifdef WINDOWS
/*...sDllMain:0:*/
BOOL WINAPI DllMain(HINSTANCE dllHandle, DWORD reason, LPVOID situation) {
        char buf[100]="";

        switch (reason) {
                case DLL_PROCESS_ATTACH:
                	TRMemOpen();
                	TRMemSetModuleName(__FILE__);

			if (isSetTRMemTrackBreak()) TRMemSetAdrBreakPoint(getTRMemTrackBreak(), 0);

                        if (situation) {
                                _CL_VERBOSE << "DLL statically loaded." LOG_
                        }
                        else {
                                _CL_VERBOSE << "DLL dynamically loaded.\n" LOG_
                        }
                        break;
                case DLL_THREAD_ATTACH:
                        _CL_VERBOSE << "New thread starting.\n" LOG_
                        break;
                case DLL_PROCESS_DETACH:
                	_CL_LOG << "DLL_PROCESS_DETACH for " << __FILE__ LOG_
                        if (situation)
                        {
                                _CL_VERBOSE << "DLL released by system." LOG_
                        }
                        else
                        {
                                _CL_VERBOSE << "DLL released by program.\n" LOG_
                        }
                        break;
                case DLL_THREAD_DETACH:
                        _CL_VERBOSE << "Thread terminating.\n" LOG_
                default:
                        return FALSE;
        }

        return TRUE;
}
/*...e*/
#endif