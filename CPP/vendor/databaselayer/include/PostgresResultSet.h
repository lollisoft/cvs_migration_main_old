#ifndef __POSTGRESQL_RESULT_SET_H__
#define __POSTGRESQL_RESULT_SET_H__

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

#include "DatabaseResultSet.h"
#include "libpq-fe.h"

class DLLEXPORT PostgresResultSet : public DatabaseResultSet
{
public:
  // ctor
  PostgresResultSet();
  PostgresResultSet(PGresult* pResult);

  // dtor
  virtual ~PostgresResultSet();
  
  virtual bool Next();
  virtual void Close();
  
  virtual int LookupField(const wxString& strField);

  // get field
  virtual int GetResultInt(int nField);
  virtual wxString GetResultString(int nField);
  virtual long GetResultLong(int nField);
  virtual bool GetResultBool(int nField);
  virtual wxDateTime GetResultDate(int nField);
  virtual void* GetResultBlob(int nField, wxMemoryBuffer& Buffer);
  virtual double GetResultDouble(int nField);
  virtual bool IsFieldNull(int nField);

  // get MetaData
  virtual ResultSetMetaData* GetMetaData();

private:
  
  PGresult* m_pResult;

  StringToIntMap m_FieldLookupMap;
  int m_nCurrentRow;
  int m_nTotalRows;
  bool m_bBinaryResults;
};

#endif // __POSTGRESQL_RESULT_SET_H__

