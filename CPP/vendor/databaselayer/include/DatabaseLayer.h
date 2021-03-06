#ifndef __DATABASE_LAYER_H__
#define __DATABASE_LAYER_H__

#ifdef WINDOWS
 #ifndef DLLEXPORT
  #define DLLEXPORT __declspec(dllimport)
 #endif
#endif
#ifndef WINDOWS
 #define DLLEXPORT
#endif
// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/hashset.h"
#include "wx/arrstr.h"
#include "wx/variant.h"

#include "DatabaseErrorReporter.h"
#include "DatabaseStringConverter.h"
#include "DatabaseResultSet.h"
#include "PreparedStatement.h"

WX_DECLARE_HASH_SET( DatabaseResultSet*, wxPointerHash, wxPointerEqual, DatabaseResultSetHashSet );
WX_DECLARE_HASH_SET( PreparedStatement*, wxPointerHash, wxPointerEqual, DatabaseStatementHashSet );

class DLLEXPORT DatabaseLayer : public DatabaseErrorReporter, public DatabaseStringConverter
{
public:
  /// Constructor
  DatabaseLayer();
  
  /// Destructor
  virtual ~DatabaseLayer();
  
  // Open database
  virtual bool Open(const wxString& strDatabase) = 0;
  
  /// close database  
  virtual bool Close() = 0;
 
  /// Is the connection to the database open?
  virtual bool IsOpen() = 0;

  // transaction support
  /// Begin a transaction
  virtual void BeginTransaction() = 0;
  /// Commit the current transaction
  virtual void Commit() = 0;
  /// Rollback the current transaction
  virtual void RollBack() = 0;
 
  // query database
  /// Run an insert, update, or delete query on the database
  virtual bool RunQuery(const wxString& strQuery);
  /// Run an insert, update, or delete query on the database
  virtual bool RunQuery(const wxString& strQuery, bool bParseQueries) = 0;
  /// Run a select query on the database
  virtual DatabaseResultSet* RunQueryWithResults(const wxString& strQuery) = 0;
  
  /// Close a result set returned by the database or a prepared statement previously
  virtual bool CloseResultSet(DatabaseResultSet* pResultSet);

  // PreparedStatement support
  /// Prepare a SQL statement which can be reused with different parameters
  virtual PreparedStatement* PrepareStatement(const wxString& strQuery) = 0;
  /// Close a prepared statement previously prepared by the database
  virtual bool CloseStatement(PreparedStatement* pStatement);

  // function names more consistent with JDBC and wxSQLite3
  // these just provide wrappers for existing functions
  /// See RunQuery
  bool ExecuteUpdate(const wxString& strQuery) { return RunQuery(strQuery); }
  /// See RunQueryWithResults
  DatabaseResultSet* ExecuteQuery(const wxString& strQuery) { return RunQueryWithResults(strQuery); }

  // Database schema API contributed by M. Szeftel (author of wxActiveRecordGenerator)
  /// Check for the existence of a table by name
  virtual bool TableExists(const wxString& table) = 0;
  /// Check for the existence of a view by name
  virtual bool ViewExists(const wxString& view) = 0;
  /// Retrieve all table names
  virtual wxArrayString GetTables() = 0;
  /// Retrieve all view names
  virtual wxArrayString GetViews() = 0;
  /// Retrieve all column names for a table
  virtual wxArrayString GetColumns(const wxString& table) = 0;

  /// Retrieve all primary columns for a table
  virtual int GetPrimaryKeys(const wxString& table) = 0;

  /// Retrieve the foreign key column name
  virtual wxString& GetPrimaryKeyColumn(const int index) = 0;

  /// Retrieve the foreign key column name
  virtual wxString& GetPrimaryKeySequence(const int index) = 0;

  /// Retrieve the number of foreign columns per table.
  /// Throws DATABASE_LAYER_NOT_SUPPORTED when the backend does not support foreign keys.
  /// Throws DATABASE_LAYER_NOT_IMPLEMENTED when the backend does not implement this function.
  virtual int GetForeignKeys(const wxString& table) = 0;
  
  /// Retrieve the foreign key column name
  virtual wxString& GetForeignKeyFKColumn(const int index) = 0;

  /// Retrieve the primary key column name of the current foreign key
  virtual wxString& GetForeignKeyPKColumn(const int index) = 0;

  /// Retrieve the primary key table name of the current foreign key
  virtual wxString& GetForeignKeyPKTable(const int index) = 0;
	
	virtual bool GetColumnNullable(const wxString& table, const wxString& column) = 0;

  // Database single result retrieval API contributed by Guru Kathiresan
  /// With the GetSingleResultX API, two additional exception types are thrown:
  ///   DATABASE_LAYER_NO_ROWS_FOUND - No database rows were returned
  ///   DATABASE_LAYER_NON_UNIQUE_RESULTSET - More than one database row was returned

  /// Retrieve a single integer value from a query
  virtual int GetSingleResultInt(const wxString& strSQL, int nField);
  virtual int GetSingleResultInt(const wxString& strSQL, const wxString& strField);
  
  /// Retrieve a single string value from a query
  virtual wxString GetSingleResultString(const wxString& strSQL, int nField);
  virtual wxString GetSingleResultString(const wxString& strSQL, const wxString& strField);
  
  /// Retrieve a single long value from a query
  virtual long GetSingleResultLong(const wxString& strSQL, int nField);
  virtual long GetSingleResultLong(const wxString& strSQL, const wxString& strField);
  
  /// Retrieve a single bool value from a query
  virtual bool GetSingleResultBool(const wxString& strSQL, int nField);
  virtual bool GetSingleResultBool(const wxString& strSQL, const wxString& strField);
  
  /// Retrieve a single date/time value from a query
  virtual wxDateTime GetSingleResultDate(const wxString& strSQL, int nField);
  virtual wxDateTime GetSingleResultDate(const wxString& strSQL, const wxString& strField);
  
  /// Retrieve a single Blob value from a query
  virtual void* GetSingleResultBlob(const wxString& strSQL, int nField, wxMemoryBuffer& Buffer);
  virtual void* GetSingleResultBlob(const wxString& strSQL, const wxString& strField, wxMemoryBuffer& Buffer);
  
  /// Retrieve a single double value from a query
  virtual double GetSingleResultDouble(const wxString& strSQL, int nField);
  virtual double GetSingleResultDouble(const wxString& strSQL, const wxString& strField);

  /// Retrieve all the values of one field in a result set
  virtual wxArrayInt GetResultsArrayInt(const wxString& strSQL, int nField);
  virtual wxArrayInt GetResultsArrayInt(const wxString& strSQL, const wxString& Field);
  
  virtual wxArrayString GetResultsArrayString(const wxString& strSQL, int nField);
  virtual wxArrayString GetResultsArrayString(const wxString& strSQL, const wxString& Field);
  
  virtual wxArrayLong GetResultsArrayLong(const wxString& strSQL, int nField);
  virtual wxArrayLong GetResultsArrayLong(const wxString& strSQL, const wxString& Field);
#if wxCHECK_VERSION(2, 7, 0)
  virtual wxArrayDouble GetResultsArrayDouble(const wxString& strSQL, int nField);
  virtual wxArrayDouble GetResultsArrayDouble(const wxString& strSQL, const wxString& Field);
#endif
  
protected:
  /// Close all result set objects that have been generated but not yet closed
  void CloseResultSets();
  /// Close all prepared statement objects that have been generated but not yet closed
  void CloseStatements();
  
  /// Add result set object pointer to the list for "garbage collection"
	void LogResultSetForCleanup(DatabaseResultSet* pResultSet);
  /// Add prepared statement object pointer to the list for "garbage collection"
	void LogStatementForCleanup(PreparedStatement* pStatement);
    
private:
  int GetSingleResultInt(const wxString& strSQL, const wxVariant* field);
  wxString GetSingleResultString(const wxString& strSQL, const wxVariant* field);
  long GetSingleResultLong(const wxString& strSQL, const wxVariant* field);
  bool GetSingleResultBool(const wxString& strSQL, const wxVariant* field);
  wxDateTime GetSingleResultDate(const wxString& strSQL, const wxVariant* field);
  void* GetSingleResultBlob(const wxString& strSQL, const wxVariant* field, wxMemoryBuffer& Buffer);
  double GetSingleResultDouble(const wxString& strSQL, const wxVariant* field);
  wxArrayInt GetResultsArrayInt(const wxString& strSQL, const wxVariant* field);
  wxArrayString GetResultsArrayString(const wxString& strSQL, const wxVariant* field);
  wxArrayLong GetResultsArrayLong(const wxString& strSQL, const wxVariant* field);
#if wxCHECK_VERSION(2, 7, 0)
  wxArrayDouble GetResultsArrayDouble(const wxString& strSQL, const wxVariant* field);
#endif

  DatabaseResultSetHashSet m_ResultSets;
  DatabaseStatementHashSet m_Statements;
};

#endif // __DATABASE_LAYER_H__

