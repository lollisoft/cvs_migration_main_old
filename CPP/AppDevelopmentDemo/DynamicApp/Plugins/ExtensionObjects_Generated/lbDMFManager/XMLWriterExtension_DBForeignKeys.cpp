
/*...sLicence:0:*/
/*
    DMF Distributed Multiplatform Framework (the initial goal of this library)
    This file is part of lbDMF.
    Copyright (C) 2002  Lothar Behrens (lothar.behrens@lollisoft.de)

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

#ifdef LBDMF_PREC
#include <lbConfigHook.h>
#endif

#include <stdio.h>
#include <string.h>
#ifndef UNIX
#include <windows.h>
#endif
#ifdef UNIX
#endif

#ifndef LBDMF_PREC
#include <lbConfigHook.h>
#endif

/*...sLB_DMFDATAMODEL_DLL scope:0:*/
#define LB_DMFDATAMODEL_DLL
#include <lbdmfdatamodel-module.h>
/*...e*/
#include <lbInterfaces-lbDMFManager.h>
#include <XMLWriterExtension_DBForeignKeys.h>

IMPLEMENT_FUNCTOR(instanceOfDBForeignKeysXMLWriterExtension, DBForeignKeysXMLWriterExtension)

BEGIN_IMPLEMENT_LB_UNKNOWN(DBForeignKeysXMLWriterExtension)
	ADD_INTERFACE(lb_I_ExtensionObject)
	ADD_INTERFACE(lb_I_VisitorExtension)
END_IMPLEMENT_LB_UNKNOWN()

DBForeignKeysXMLWriterExtension::DBForeignKeysXMLWriterExtension() {
	_CL_VERBOSE << "DBForeignKeysXMLWriterExtension::DBForeignKeysXMLWriterExtension() called." LOG_
}

DBForeignKeysXMLWriterExtension::~DBForeignKeysXMLWriterExtension() {
	_CL_VERBOSE << "DBForeignKeysXMLWriterExtension::~DBForeignKeysXMLWriterExtension() called." LOG_
}

lbErrCodes LB_STDCALL DBForeignKeysXMLWriterExtension::setData(lb_I_Unknown*) {
	_LOG << "Error: DBForeignKeysXMLWriterExtension::setData(lb_I_Unknown*) not implemented." LOG_
	return ERR_NOT_IMPLEMENTED;
}

void LB_STDCALL DBForeignKeysXMLWriterExtension::setOwningObject(lb_I_Unknown* owning) {
	// Need a lb_I_DatabaseOperation here from the aspect
	lbErrCodes err = ERR_NONE;
	
	if (owning != NULL) 
	{
		QI(owning, lb_I_DBForeignKeys, owningObject)
		if (owningObject == NULL) {
			_LOG << "Error: DBForeignKeysXMLWriterExtension::setOwningObject() owning parameter is not a lb_I_DBForeignKeys." LOG_
		}
	} else {
		_LOG << "Error: DBForeignKeysXMLWriterExtension::setOwningObject() called with a NULL pointer." LOG_
	}
}

void LB_STDCALL DBForeignKeysXMLWriterExtension::setOperator(lb_I_Aspect* operation) {
	// Need a lb_I_FileOperation here from the aspect
	lbErrCodes err = ERR_NONE;
	
	if (operation != NULL) 
	{
		UAP(lb_I_FileOperation, fileOp)
		QI(operation, lb_I_FileOperation, fileOp)
		if (fileOp == NULL) {
			_LOG << "Error: DBForeignKeysXMLWriterExtension::setOperator() operation parameter is not a lb_I_FileOperation." LOG_
		}
		UAP(lb_I_Stream, s)
		s = fileOp->getStream();
		
		QI(s, lb_I_OutputStream, oStream)
	} else {
		_LOG << "Error: DBForeignKeysXMLWriterExtension::setOperator() called with a NULL pointer." LOG_
	}
}

void LB_STDCALL DBForeignKeysXMLWriterExtension::execute() {
	*oStream << "<dbforeignkey>" << "\n";
	
	owningObject->finishIteration();
	
	while (owningObject->hasMoreElements()) {
		owningObject->setNextElement();
		*oStream << 
		"<entry ID=\"" << owningObject->get_id() << 

		"\" pkcatalog=\"" << owningObject->get_pkcatalog() <<
		"\" pkschema=\"" << owningObject->get_pkschema() <<
		"\" pktable=\"" << owningObject->get_pktable() <<
		"\" pkcolumn=\"" << owningObject->get_pkcolumn() <<
		"\" fkcatalog=\"" << owningObject->get_fkcatalog() <<
		"\" fkschema=\"" << owningObject->get_fkschema() <<
		"\" fktable=\"" << owningObject->get_fktable() <<
		"\" fkcolumn=\"" << owningObject->get_fkcolumn() <<
		"\" keysequence=\"" << owningObject->get_keysequence() <<
		"\" updaterule=\"" << owningObject->get_updaterule() <<
		"\" deleterule=\"" << owningObject->get_deleterule() <<
		"\" dbtableid=\"" << owningObject->get_dbtableid() << "\"/>" << "\n";
	}
	*oStream << "</dbforeignkey>" << "\n";
}



class lbPluginDBForeignKeysXMLWriterExtension : public lb_I_PluginImpl {
public:
	lbPluginDBForeignKeysXMLWriterExtension();
	
	virtual ~lbPluginDBForeignKeysXMLWriterExtension();

	bool LB_STDCALL canAutorun();
	lbErrCodes LB_STDCALL autorun();

	void LB_STDCALL initialize();
	
	bool LB_STDCALL run();

	lb_I_Unknown* LB_STDCALL peekImplementation();
	lb_I_Unknown* LB_STDCALL getImplementation();
	void LB_STDCALL releaseImplementation();

	void LB_STDCALL setNamespace(const char* _namespace) { }

	DECLARE_LB_UNKNOWN()
	
	UAP(lb_I_Unknown, ukDBForeignKeysModelExtension)
};

BEGIN_IMPLEMENT_LB_UNKNOWN(lbPluginDBForeignKeysXMLWriterExtension)
        ADD_INTERFACE(lb_I_PluginImpl)
END_IMPLEMENT_LB_UNKNOWN()

IMPLEMENT_FUNCTOR(instanceOflbPluginDBForeignKeysXMLWriterExtension, lbPluginDBForeignKeysXMLWriterExtension)

lbErrCodes LB_STDCALL lbPluginDBForeignKeysXMLWriterExtension::setData(lb_I_Unknown* uk) {
	lbErrCodes err = ERR_NONE;

	_CL_VERBOSE << "lbPluginDBForeignKeysXMLWriterExtension::setData(...) called.\n" LOG_

        return ERR_NOT_IMPLEMENTED;
}

lbPluginDBForeignKeysXMLWriterExtension::lbPluginDBForeignKeysXMLWriterExtension() {
	_CL_VERBOSE << "lbPluginDBForeignKeysXMLWriterExtension::lbPluginDBForeignKeysXMLWriterExtension() called.\n" LOG_
}

lbPluginDBForeignKeysXMLWriterExtension::~lbPluginDBForeignKeysXMLWriterExtension() {
	_CL_VERBOSE << "lbPluginDBForeignKeysXMLWriterExtension::~lbPluginDBForeignKeysXMLWriterExtension() called.\n" LOG_
}

bool LB_STDCALL lbPluginDBForeignKeysXMLWriterExtension::canAutorun() {
	return false;
}

lbErrCodes LB_STDCALL lbPluginDBForeignKeysXMLWriterExtension::autorun() {
	lbErrCodes err = ERR_NONE;
	return err;
}

void LB_STDCALL lbPluginDBForeignKeysXMLWriterExtension::initialize() {
}
	
bool LB_STDCALL lbPluginDBForeignKeysXMLWriterExtension::run() {
	return true;
}

lb_I_Unknown* LB_STDCALL lbPluginDBForeignKeysXMLWriterExtension::peekImplementation() {
	lbErrCodes err = ERR_NONE;

	if (ukDBForeignKeysModelExtension == NULL) {
		DBForeignKeysXMLWriterExtension* DBForeignKeysModelExtension = new DBForeignKeysXMLWriterExtension();
		
	
		QI(DBForeignKeysModelExtension, lb_I_Unknown, ukDBForeignKeysModelExtension)
	} else {
		_CL_VERBOSE << "lbPluginDatabasePanel::peekImplementation() Implementation already peeked.\n" LOG_
	}
	
	return ukDBForeignKeysModelExtension.getPtr();
}

lb_I_Unknown* LB_STDCALL lbPluginDBForeignKeysXMLWriterExtension::getImplementation() {
	lbErrCodes err = ERR_NONE;

	if (ukDBForeignKeysModelExtension == NULL) {

		_CL_VERBOSE << "Warning: peekImplementation() has not been used prior.\n" LOG_
	
		DBForeignKeysXMLWriterExtension* DBForeignKeysModelExtension = new DBForeignKeysXMLWriterExtension();
		
	
		QI(DBForeignKeysModelExtension, lb_I_Unknown, ukDBForeignKeysModelExtension)
	}
	
	lb_I_Unknown* r = ukDBForeignKeysModelExtension.getPtr();
	ukDBForeignKeysModelExtension.resetPtr();
	return r;
}

void LB_STDCALL lbPluginDBForeignKeysXMLWriterExtension::releaseImplementation() {
        lbErrCodes err = ERR_NONE;

        if (ukDBForeignKeysModelExtension != NULL) {
                ukDBForeignKeysModelExtension--;
                ukDBForeignKeysModelExtension.resetPtr();
        }
}
