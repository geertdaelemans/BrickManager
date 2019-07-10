#ifndef __CREF_H__
#define __CREF_H__

#include <qlist.h>
#include <QMultiHash>

class CRef;

class CAsciiRefCacheBase {
protected:
	CAsciiRefCacheBase ( uint dictsize, uint cachesize );
	virtual ~CAsciiRefCacheBase ( );

    bool put ( const QString key, CRef *ref );
    CRef *get ( const QString key );

public:
	virtual void clear ( );

private:
	void addRefFor ( const CRef *ref );
	void releaseFor ( const CRef *ref );

	friend class CRef;

private:
    QMultiHash<QString, CRef*> m_dict;
//    QMultiHash<QByteArray, CRef*> m_dict;
    QList<const CRef *>   m_no_ref;
    int                   m_cache_size;
};

template <typename T, uint DICTSIZE, uint CACHESIZE> class CAsciiRefCache : public CAsciiRefCacheBase {
public:
	CAsciiRefCache ( ) : CAsciiRefCacheBase ( DICTSIZE, CACHESIZE ) { }

    bool insert ( const QString key, T *ref )  { return put ( key, ref ); }
    T *operator [] ( const QString key )       { return (T *) get ( key ); }
};


class CRef {
public:
    CRef ( ) : m_refcnt ( 0 ), m_cache ( nullptr ) { }
	virtual ~CRef ( );

	void addRef ( ) const   { if ( m_cache ) { m_refcnt++; m_cache-> addRefFor ( this ); } }
	void release ( ) const  { if ( m_cache ) { m_refcnt--; m_cache-> releaseFor ( this ); } }

	uint refCount ( ) const { return m_refcnt; }

private:
	friend class CAsciiRefCacheBase;

	mutable uint                m_refcnt;
	mutable CAsciiRefCacheBase *m_cache;
};

#endif

