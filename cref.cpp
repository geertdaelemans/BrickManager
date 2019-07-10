#include "cref.h"


// IMPORTANT: ported from Q3AsciiDict see https://doc.qt.io/archives/qt-4.8/porting4.html#qdict-section


CAsciiRefCacheBase::CAsciiRefCacheBase(uint dictsize, uint cachesize)
    : m_dict (/* dictsize */), m_cache_size ( static_cast<int>(cachesize) )
{
    //m_dict. setAutoDelete ( true );
    Q_UNUSED(dictsize);
}

CAsciiRefCacheBase::~CAsciiRefCacheBase ( )
{
	clear ( );
}

bool CAsciiRefCacheBase::put ( const QString key, CRef *ref )
{
	if ( ref-> m_cache )
		return false;

	ref-> m_cache = this;
	m_no_ref. append ( ref );
	m_dict. insert ( key, ref );
	return true;
}

CRef *CAsciiRefCacheBase::get ( const QString key )
{
    return m_dict.value(key);
}

void CAsciiRefCacheBase::clear ( )
{
    m_no_ref.clear();
    while (!m_dict.isEmpty()) {
        auto *value = *m_dict.begin();
        m_dict.erase(m_dict.begin());
        delete value;
    }
}

void CAsciiRefCacheBase::addRefFor ( const CRef *ref )
{
	//qDebug ( "addRefFor called" );

    if (ref && (ref-> m_cache == this) && (ref-> m_refcnt == 1)) {
		//qDebug ( "Moving item [%p] to in-use dict...", (void *) ref );
        m_no_ref.removeOne(ref);
	}
}

void CAsciiRefCacheBase::releaseFor ( const CRef *ref )
{
	if ( ref && ( ref-> m_refcnt == 0 ) && ( ref-> m_cache == this )) {
		//qDebug ( "Moving item [%p] to cache...", (void *) ref );
		m_no_ref. append ( ref );

		while ( m_no_ref. count ( ) > m_cache_size ) {
            const CRef *del = m_no_ref. takeAt ( 0 );

            QHashIterator<QString, CRef*> i(m_dict);
            while (i.hasNext()) {
                i.next();
                if (i.value() == del) {
                    delete m_dict.take(i.key());
                    break;
                }
            }



//			for ( Q3AsciiDictIterator<CRef> it ( m_dict ); it. current ( ); ++it ) {
//				if ( it. current ( ) == del ) {
//					//qDebug ( "Purging item \"%s\" from cache...", it. currentKey ( ));
//					m_dict. remove ( it. currentKey ( ));
//					break;
//				}
//			}
		}
	}
}

CRef::~CRef()
{
    if (m_refcnt)
        qWarning("Deleting %p, although refcnt=%d", static_cast<void*>(this), m_refcnt);
}
