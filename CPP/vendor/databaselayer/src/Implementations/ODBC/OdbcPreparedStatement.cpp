#include "../include/DatabaseLayerDLLExport.h"
#include "../include/OdbcPreparedStatement.h"
#include "../include/OdbcResultSet.h"
#include "../include/OdbcDatabaseLayer.h"

// ctor
OdbcPreparedStatement::OdbcPreparedStatement(SQLHENV sqlEnvHandle, SQLHDBC sqlHDBC)
 : PreparedStatement()
{
    m_sqlEnvHandle = sqlEnvHandle;
    m_sqlHDBC = sqlHDBC;
    m_bOneTimeStatement = false;
}

OdbcPreparedStatement::OdbcPreparedStatement(SQLHENV sqlEnvHandle, SQLHDBC sqlHDBC, SQLHSTMT sqlStatementHandle)
 : PreparedStatement()
{
    m_sqlEnvHandle = sqlEnvHandle;
    m_sqlHDBC = sqlHDBC;
    m_bOneTimeStatement = false;
    m_Statements.push_back(sqlStatementHandle);
}

OdbcPreparedStatement::OdbcPreparedStatement(SQLHENV sqlEnvHandle, SQLHDBC sqlHDBC, StatementVector statements)
 : PreparedStatement()
{
    m_sqlEnvHandle = sqlEnvHandle;
    m_sqlHDBC = sqlHDBC;
    m_bOneTimeStatement = false;
    m_Statements = statements;
}

// dtor
OdbcPreparedStatement::~OdbcPreparedStatement()
{
  FreeParameters();
  Close();
}

void OdbcPreparedStatement::Close()
{
  CloseResultSets();

  StatementVector::iterator start = m_Statements.begin();
  StatementVector::iterator stop = m_Statements.end();
  while (start != stop)
  {
    if ((*start) != NULL)
    {
      SQLRETURN nRet = SQLFreeHandle(SQL_HANDLE_STMT, (SQLHSTMT)*start);
      if ( nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO )
      {
        InterpretErrorCodes(nRet, (SQLHSTMT)*start);
        ThrowDatabaseException();
        return;
      }
      (*start) = NULL;
    }
    start++;
  }
  m_Statements.Clear();
}

void OdbcPreparedStatement::FreeParameters()
{
  ArrayOfOdbcParameters::iterator start = m_Parameters.begin();
  ArrayOfOdbcParameters::iterator stop = m_Parameters.end();

  while (start != stop)
  {
    if ((*start) != NULL)
    {
      SQLHSTMT pStmt = (SQLHSTMT)(*start);
      delete(pStmt);
      (*start) = NULL;
    }
    start++;
  }
  m_Parameters.Clear();
}

void OdbcPreparedStatement::AddPreparedStatement(SQLHSTMT sqlStatementHandle)
{
  m_Statements.push_back(sqlStatementHandle);
}

// get field
void OdbcPreparedStatement::SetParamInt(int nPosition, int nValue)
{
    ResetErrorCodes();

    OdbcParameter* pParameter = new OdbcParameter(nValue);
    SetParam(nPosition, pParameter);
}

void OdbcPreparedStatement::SetParamDouble(int nPosition, double dblValue)
{
    ResetErrorCodes();

    OdbcParameter* pParameter = new OdbcParameter(dblValue);
    SetParam(nPosition, pParameter);
}

void OdbcPreparedStatement::SetParamString(int nPosition, const wxString& strValue)
{
    ResetErrorCodes();

    OdbcParameter* pParameter = new OdbcParameter(strValue);
    SetParam(nPosition, pParameter);
}

void OdbcPreparedStatement::SetParamNull(int nPosition)
{
    ResetErrorCodes();

    OdbcParameter* pParameter = new OdbcParameter();
    SetParam(nPosition, pParameter);
}

void OdbcPreparedStatement::SetParamBlob(int nPosition, const void* pData, long nDataLength)
{
    ResetErrorCodes();

    OdbcParameter* pParameter = new OdbcParameter(pData, nDataLength);
    SetParam(nPosition, pParameter);
}

void OdbcPreparedStatement::SetParamDate(int nPosition, const wxDateTime& dateValue)
{
    ResetErrorCodes();

    OdbcParameter* pParameter = new OdbcParameter(dateValue);
    SetParam(nPosition, pParameter);
}

void OdbcPreparedStatement::SetParamBool(int nPosition, bool bValue)
{
    ResetErrorCodes();

    OdbcParameter* pParameter = new OdbcParameter(bValue);
    SetParam(nPosition, pParameter);
}

int OdbcPreparedStatement::GetParameterCount()
{
    ResetErrorCodes();

    int nReturn = 0;
    StatementVector::iterator start = m_Statements.begin();
    StatementVector::iterator stop = m_Statements.end();
    while (start != stop)
    {
        SQLSMALLINT num = 0;
        SQLRETURN nRet = SQLNumParams(((SQLHSTMT)(*start)), &num);
        if ( nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO )
        {
            InterpretErrorCodes(nRet, (SQLHSTMT)(*start));
            ThrowDatabaseException();
            return -1;
        }
        nReturn += num;
        start++;
    }
    return nReturn;
}

void OdbcPreparedStatement::RunQuery()
{
    ResetErrorCodes();

    BindParameters();

    StatementVector::iterator start = m_Statements.begin();
    StatementVector::iterator stop = m_Statements.end();
    while (start != stop)
    {
        SQLRETURN nRet = 0;
        nRet = SQLFreeStmt((SQLHSTMT)(*start), SQL_CLOSE);
        if ( nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO )
        {
            InterpretErrorCodes(nRet, (SQLHSTMT)(*start));
            ThrowDatabaseException();
            return;
        }

        nRet = SQLExecute((SQLHSTMT)(*start));
        if ( nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO && nRet != SQL_NO_DATA )
        {
            InterpretErrorCodes(nRet, (SQLHSTMT)(*start));
            ThrowDatabaseException();
            return;
        }
        start++;
    }
}

DatabaseResultSet* OdbcPreparedStatement::RunQueryWithResults()
{
  return RunQueryWithResults(true);
}

DatabaseResultSet* OdbcPreparedStatement::RunQueryWithResults(bool bLogForCleanup)
{
    ResetErrorCodes();
    SQLSMALLINT ncol = 0;

    if (m_Statements.size() > 0)
    {
        BindParameters();

        for (unsigned int i=0; i<m_Statements.size(); i++)
        {
            SQLRETURN nRet = 0;
            nRet = SQLFreeStmt(m_Statements[i], SQL_CLOSE);
            if ( nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO )
            {
                InterpretErrorCodes(nRet, m_Statements[i]);
                ThrowDatabaseException();
                return NULL;
            }

            nRet = SQLExecute(m_Statements[i]);
            if ( nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO )
            {
                InterpretErrorCodes(nRet, m_Statements[i]);
                ThrowDatabaseException();
                return NULL;
            }

            nRet = SQLNumResultCols(m_Statements[i], &ncol);
            if ( nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO )
            {
                InterpretErrorCodes(nRet, m_Statements[i]);
                ThrowDatabaseException();
                return NULL;
            }
        }
    }

    // Work off the assumption that only the last statement will return result
    DatabaseResultSet* pResultSet = new OdbcResultSet(this, m_bOneTimeStatement, (int)ncol);
    if (bLogForCleanup)
      LogResultSetForCleanup(pResultSet);
    return pResultSet;
}

void OdbcPreparedStatement::BindParameters()
{
  // Iterate through all of the parameters and bind them to the prepared statement
  for (unsigned int i = 1; i <= m_Parameters.size(); i++)
  {
    int nPosition = i;
    OdbcParameter* pParameter = m_Parameters[i-1];
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);

    if ((nIndex > -1) && (pParameter != NULL))
    {
      SQLRETURN nRet = SQLBindParameter(m_Statements[nIndex], nPosition, SQL_PARAM_INPUT,
        pParameter->GetValueType(), pParameter->GetParameterType(),
        pParameter->GetColumnSize(), pParameter->GetDecimalDigits(), pParameter->GetDataPtr(),
        pParameter->GetDataLength(), pParameter->GetParameterLengthPtr() );

      if ( nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO )
      {
        InterpretErrorCodes(nRet, m_Statements[nIndex]);
        ThrowDatabaseException();
        return;
      }
    }
  }
}

int OdbcPreparedStatement::FindStatementAndAdjustPositionIndex(int* pPosition)
{
    // Don't mess around if there's just one entry in the vector
    if (m_Statements.size() == 0)
        return 0;
        
    // Go through all the elements in the vector
    // Get the number of parameters in each statement
    // Adjust the nPosition for the the broken up statements
    for (unsigned int i=0; i<m_Statements.size(); i++)
    {
        SQLSMALLINT num = 0;
        SQLRETURN nRet = SQLNumParams(m_Statements[i], &num);
        if ( nRet != SQL_SUCCESS && nRet != SQL_SUCCESS_WITH_INFO )
        {
            InterpretErrorCodes(nRet, m_Statements[i]);
            ThrowDatabaseException();
            return -1;
        }

        int nParametersInThisStatement = (int)num;
        if (*pPosition > nParametersInThisStatement)
        {
            *pPosition -= nParametersInThisStatement;    // Decrement the position indicator by the number of parameters in this statement
        }
        else
        {
            // We're in the correct statement, return the index
            return i;
        }
    }
    return -1;
}

void OdbcPreparedStatement::SetParam(int nPosition, OdbcParameter* pParameter)
{
  // First make sure that there are enough elements in the collection
  while (m_Parameters.size() < (unsigned int)(nPosition))
  {
    m_Parameters.push_back(NULL);//EmptyParameter);
  }
  // Free up any data that is being replaced so the allocated memory isn't lost
  if (m_Parameters[nPosition-1] != NULL)
  {
    delete (m_Parameters[nPosition-1]);
  }
  // Now set the new data
  m_Parameters[nPosition-1] = pParameter;
}

void OdbcPreparedStatement::InterpretErrorCodes( long nCode, SQLHSTMT stmth_ptr )
{
  wxLogDebug(_("OdbcPreparedStatement::InterpretErrorCodes()\n"));

  //if ((nCode != SQL_SUCCESS) ) // && (nCode != SQL_SUCCESS_WITH_INFO))
  {
    SQLINTEGER iNativeCode;
    SQLTCHAR strState[ERR_STATE_LEN];
    SQLTCHAR strBuffer[ERR_BUFFER_LEN];
    SQLSMALLINT iMsgLen;

    memset(strState, 0, ERR_STATE_LEN*sizeof(SQLTCHAR));
    memset(strBuffer, 0, ERR_BUFFER_LEN*sizeof(SQLTCHAR));

    if (stmth_ptr)
      SQLGetDiagRec(SQL_HANDLE_STMT, stmth_ptr, 1, strState, &iNativeCode, 
        strBuffer, ERR_BUFFER_LEN, &iMsgLen);  
    else
      SQLGetDiagRec(SQL_HANDLE_DBC, m_sqlHDBC, 1, strState, &iNativeCode,
        strBuffer, ERR_BUFFER_LEN, &iMsgLen);  
    
    SetErrorCode((int)iNativeCode);
    //SetErrorMessage(ConvertFromUnicodeStream((char*)strBuffer));
    SetErrorMessage(wxString((wxChar*)strBuffer));
  }
}


