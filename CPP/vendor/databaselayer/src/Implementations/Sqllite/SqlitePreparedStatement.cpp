#include "../include/DatabaseLayerDLLExport.h"
#include "../include/SqlitePreparedStatement.h"
#include "../include/SqliteResultSet.h"
#include "../include/SqliteDatabaseLayer.h"

// ctor
SqlitePreparedStatement::SqlitePreparedStatement(sqlite3* pDatabase)
 : PreparedStatement()
{
  m_pDatabase = pDatabase;
}

SqlitePreparedStatement::SqlitePreparedStatement(sqlite3* pDatabase, sqlite3_stmt* pStatement)
 : PreparedStatement()
{
  m_pDatabase = pDatabase;
  m_Statements.push_back(pStatement);
}

SqlitePreparedStatement::SqlitePreparedStatement(sqlite3* pDatabase, StatementVector statements)
 : PreparedStatement()
{
  m_pDatabase = pDatabase;
  m_Statements = statements;
}

// dtor
SqlitePreparedStatement::~SqlitePreparedStatement()
{
  Close();
}

void SqlitePreparedStatement::Close()
{
  CloseResultSets();

  StatementVector::iterator start = m_Statements.begin();
  StatementVector::iterator stop = m_Statements.end();
  while (start != stop)
  {
    if ((*start) != NULL)
    {
      //wxLogError(_("SqlitePreparedStatement::Close() closes a statement."));
      sqlite3_finalize((sqlite3_stmt*)(*start));
      (*start) = NULL;
      //wxDELETE(*start);
    }
    start++;
  }
  m_Statements.Clear();
}
/*
void SqlitePreparedStatement::AddPreparedStatement(CppSQLite3Statement* pStatement)
{
  m_Statements.push_back(pStatement);
}
*/
void SqlitePreparedStatement::AddPreparedStatement(sqlite3_stmt* pStatement)
{
  m_Statements.push_back(pStatement);
}

// get field
void SqlitePreparedStatement::SetParamInt(int nPosition, int nValue)
{
  ResetErrorCodes();

  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
  {
    sqlite3_reset(m_Statements[nIndex]);
    int nReturn = sqlite3_bind_int(m_Statements[nIndex], nPosition, nValue);
    if (nReturn != SQLITE_OK)
    {
      SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(nReturn));
      SetErrorMessage(ConvertFromUnicodeStream(sqlite3_errmsg(m_pDatabase)));
      ThrowDatabaseException();
    }
  }
}

void SqlitePreparedStatement::SetParamDouble(int nPosition, double dblValue)
{
  ResetErrorCodes();

  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
  {
    sqlite3_reset(m_Statements[nIndex]);
    int nReturn = sqlite3_bind_double(m_Statements[nIndex], nPosition, dblValue);
    if (nReturn != SQLITE_OK)
    {
      SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(nReturn));
      SetErrorMessage(ConvertFromUnicodeStream(sqlite3_errmsg(m_pDatabase)));
      ThrowDatabaseException();
    }
  }
}

void SqlitePreparedStatement::SetParamString(int nPosition, const wxString& strValue)
{
  ResetErrorCodes();

  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
  {
    sqlite3_reset(m_Statements[nIndex]);
    wxCharBuffer valueBuffer = ConvertToUnicodeStream(strValue);
    int nReturn = sqlite3_bind_text(m_Statements[nIndex], nPosition, valueBuffer, -1, SQLITE_TRANSIENT);
    if (nReturn != SQLITE_OK)
    {
      SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(nReturn));
      SetErrorMessage(ConvertFromUnicodeStream(sqlite3_errmsg(m_pDatabase)));
      ThrowDatabaseException();
    }
  }
}

void SqlitePreparedStatement::SetParamNull(int nPosition)
{
  ResetErrorCodes();

  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
  {
    sqlite3_reset(m_Statements[nIndex]);
    int nReturn = sqlite3_bind_null(m_Statements[nIndex], nPosition);
    if (nReturn != SQLITE_OK)
    {
      SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(nReturn));
      SetErrorMessage(ConvertFromUnicodeStream(sqlite3_errmsg(m_pDatabase)));
      ThrowDatabaseException();
    }
  }
}

void SqlitePreparedStatement::SetParamBlob(int nPosition, const void* pData, long nDataLength)
{
  ResetErrorCodes();

  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
  {
    sqlite3_reset(m_Statements[nIndex]);
    int nReturn = sqlite3_bind_blob(m_Statements[nIndex], nPosition, (const void*)pData, nDataLength, SQLITE_STATIC);
    if (nReturn != SQLITE_OK)
    {
      SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(nReturn));
      SetErrorMessage(ConvertFromUnicodeStream(sqlite3_errmsg(m_pDatabase)));
      ThrowDatabaseException();
    }
  }
}

void SqlitePreparedStatement::SetParamDate(int nPosition, const wxDateTime& dateValue)
{
  ResetErrorCodes();

  if (dateValue.IsValid())
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      sqlite3_reset(m_Statements[nIndex]);
      wxCharBuffer valueBuffer = ConvertToUnicodeStream(dateValue.Format(_("%Y-%m-%d %H:%M:%S")));
      int nReturn = sqlite3_bind_text(m_Statements[nIndex], nPosition, valueBuffer, -1, SQLITE_TRANSIENT);
      if (nReturn != SQLITE_OK)
      {
        SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(nReturn));
        SetErrorMessage(ConvertFromUnicodeStream(sqlite3_errmsg(m_pDatabase)));
        ThrowDatabaseException();
      }
    }
  }
  else
  {
    int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
    if (nIndex > -1)
    {
      sqlite3_reset(m_Statements[nIndex]);
      int nReturn = sqlite3_bind_null(m_Statements[nIndex], nPosition);
      if (nReturn != SQLITE_OK)
      {
        SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(nReturn));
        SetErrorMessage(ConvertFromUnicodeStream(sqlite3_errmsg(m_pDatabase)));
        ThrowDatabaseException();
      }
    }
  }
}

void SqlitePreparedStatement::SetParamBool(int nPosition, bool bValue)
{
  ResetErrorCodes();

  int nIndex = FindStatementAndAdjustPositionIndex(&nPosition);
  if (nIndex > -1)
  {
    sqlite3_reset(m_Statements[nIndex]);
    int nReturn = sqlite3_bind_int(m_Statements[nIndex], nPosition, (bValue ? 1 : 0));
    if (nReturn != SQLITE_OK)
    {
      SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(nReturn));
      SetErrorMessage(ConvertFromUnicodeStream(sqlite3_errmsg(m_pDatabase)));
      ThrowDatabaseException();
    }
  }
}

int SqlitePreparedStatement::GetParameterCount()
{
  ResetErrorCodes();

  int nReturn = 0;
  StatementVector::iterator start = m_Statements.begin();
  StatementVector::iterator stop = m_Statements.end();
  while (start != stop)
  {
    nReturn += sqlite3_bind_parameter_count((sqlite3_stmt*)(*start));
    start++;
  }
  return nReturn;
}

void SqlitePreparedStatement::RunQuery()
{
  ResetErrorCodes();

  StatementVector::iterator start = m_Statements.begin();
  StatementVector::iterator stop = m_Statements.end();
  while (start != stop)
  {
    int nReturn = sqlite3_step((sqlite3_stmt*)(*start));
 
	  int autocommit = sqlite3_get_autocommit(m_pDatabase);
	  
	  if (autocommit == 0) {
		wxLogError(_("Warning: Database is not in autocommit mode.\n"));
	  }
	  
    if (nReturn != SQLITE_ROW)
      sqlite3_reset((sqlite3_stmt*)(*start));

    if ((nReturn != SQLITE_ROW) && (nReturn != SQLITE_DONE))
    {
      const char* err_msg = NULL;
      char* open_statements = NULL;

      sqlite3_stmt* pStmt = NULL;
      const char* zSql = NULL;
      pStmt = sqlite3_next_stmt(m_pDatabase, pStmt);


      err_msg = sqlite3_errmsg(m_pDatabase);
      SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(sqlite3_errcode(m_pDatabase)));


      if (pStmt != NULL) {
		  //m_Statements.push_back(pStmt);
          zSql = sqlite3_sql(pStmt);
          open_statements = (char*) malloc(strlen(err_msg) + strlen(zSql) + 2 + 2 + 1);
          open_statements[0] = 0;
          open_statements = strcat(open_statements, err_msg);
          open_statements = strcat(open_statements, "\n");
          open_statements = strcat(open_statements, zSql);
          open_statements = strcat(open_statements, ", \n");

        while ((pStmt = sqlite3_next_stmt(m_pDatabase, pStmt)) != NULL) {
          char* temp = NULL;
		  //m_Statements.push_back(pStmt);
  		  zSql = sqlite3_sql(pStmt);
          temp = (char*) malloc(strlen(open_statements) + strlen(zSql) + 2 + 1);
          temp[0] = 0;
          temp = strcat(temp, open_statements);
          temp = strcat(temp, zSql);
          temp = strcat(temp, ", \n");
          free(open_statements);
          open_statements = temp;
        }
        SetErrorMessage(_(open_statements));
      } else {
        SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(sqlite3_errcode(m_pDatabase)));
        SetErrorMessage(ConvertFromUnicodeStream(err_msg));
      }

      ThrowDatabaseException();

      return;
    }
    start++;
  }
}

DatabaseResultSet* SqlitePreparedStatement::RunQueryWithResults()
{
  ResetErrorCodes();

  if (m_Statements.size() > 1)
  {
    for (unsigned int i=0; i<m_Statements.size()-1; i++)
    {
      int nReturn = sqlite3_step(m_Statements[i]);
 
      if (nReturn != SQLITE_ROW)
        sqlite3_reset(m_Statements[i]);

      if ((nReturn != SQLITE_ROW) && (nReturn != SQLITE_DONE))
      {
        wxLogError(_("Error with RunQueryWithResults\n"));
        SetErrorCode(SqliteDatabaseLayer::TranslateErrorCode(nReturn));
        SetErrorMessage(ConvertFromUnicodeStream(sqlite3_errmsg(m_pDatabase)));
        ThrowDatabaseException();
        return NULL;
      }
    }
  }
  // Work off the assumption that only the last statement will return result
  
  SqliteResultSet* pResultSet = new SqliteResultSet(this);
  if (pResultSet)
    pResultSet->SetEncoding(GetEncoding());

  LogResultSetForCleanup(pResultSet);
  return pResultSet;
}

int SqlitePreparedStatement::FindStatementAndAdjustPositionIndex(int* pPosition)
{
  // Don't mess around if there's just one entry in the vector
  if (m_Statements.size() == 0)
    return 0;
    
  // Go through all the elements in the vector
  // Get the number of parameters in each statement
  // Adjust the nPosition for the the broken up statements
  for (unsigned int i=0; i<m_Statements.size(); i++)
  {
    int nParametersInThisStatement = sqlite3_bind_parameter_count(m_Statements[i]);
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

