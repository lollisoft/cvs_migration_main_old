#ifndef __POSTGRESQL_DATABASE_LAYER_H__
#define __POSTGRESQL_DATABASE_LAYER_H__

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

#include "DatabaseLayer.h"
#include "PreparedStatement.h"

#include "libpq-fe.h"

class DLLEXPORT PostgresDatabaseLayer : public DatabaseLayer
{
public:
  // Information that can be specified for a PostgreSQL database
  //  host or hostaddr
  //  port
  //  dbname
  //  user
  //  password
  // ctor
  PostgresDatabaseLayer();
  PostgresDatabaseLayer(const wxString& strDatabase);
  PostgresDatabaseLayer(const wxString& strServer, const wxString& strDatabase);
  PostgresDatabaseLayer(const wxString& strDatabase, const wxString& strUser, const wxString& strPassword);
  PostgresDatabaseLayer(const wxString& strServer, const wxString& strDatabase, const wxString& strUser, const wxString& strPassword);
  PostgresDatabaseLayer(PGconn* pDatabase) { m_pDatabase = pDatabase; }

  // dtor
  virtual ~PostgresDatabaseLayer();

  // open database
  virtual bool Open();
  virtual bool Open(const wxString& strDatabase);
  virtual bool Open(const wxString& strServer, const wxString& strDatabase);
  virtual bool Open(const wxString& strDatabase, const wxString& strUser, const wxString& strPassword);
  virtual bool Open(const wxString& strServer, const wxString& strDatabase, const wxString& strUser, const wxString& strPassword);

  // close database
  virtual bool Close();
  
  // Is the connection to the database open?
  virtual bool IsOpen();

  // transaction support
  virtual void BeginTransaction();
  virtual void Commit();
  virtual void RollBack();
  
  // query database
  virtual bool RunQuery(const wxString& strQuery, bool bParseQuery);
  virtual DatabaseResultSet* RunQueryWithResults(const wxString& strQuery);

  // PreparedStatement support
  virtual PreparedStatement* PrepareStatement(const wxString& strQuery);

  // Database schema API contributed by M. Szeftel (author of wxActiveRecordGenerator)
  virtual bool TableExists(const wxString& table);
  virtual bool ViewExists(const wxString& view);
  virtual wxArrayString GetTables();
  virtual wxArrayString GetViews();
  virtual wxArrayString GetColumns(const wxString& table);

  static int TranslateErrorCode(int nCode);

private:
  wxString m_strServer;
  wxString m_strDatabase;
  wxString m_strUser;
  wxString m_strPassword;

  PGconn* m_pDatabase;
};

#endif // __POSTGRESQL_DATABASE_LAYER_H__

