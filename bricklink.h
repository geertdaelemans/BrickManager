#ifndef BRICKLINK_H
#define BRICKLINK_H

#include "ctransfer.h"
#include "cref.h"
#include "datamodel.h"

#include <QtNetworkAuth>


class ItemType {
public:
    ItemType(const QString name);
    ~ItemType();

    char id() const                { return m_id; }
    QString name() const            { return m_api_name; }
    QString apiName() const         { return m_api_name; }
    QString sqlName() const         { return m_table_prefix + m_api_name; }
    Tables tableName() const        { return m_table_name; }
    static QString getTablePrefix() { return m_table_prefix; }
    static QString getSqlName(QString name);
    static QString getSqlName(int id)   { return m_table_prefix + m_api_names[id]; }
    static Tables getTableName(int id)  { return m_table_names[id]; }
    static char getPictureId(int id)    { return m_picture_ids[id]; }

//		const Category **categories ( ) const { return m_categories; }
    bool hasInventories ( ) const     { return m_has_inventories; }
    bool hasColors ( ) const          { return m_has_colors; }
    bool hasYearReleased ( ) const    { return m_has_year; }
    bool hasWeight ( ) const          { return m_has_weight; }
    char pictureId ( ) const          { return m_picture_id; }
    QSize imageSize ( ) const;

private:
    char     m_id;
    char     m_picture_id;

    bool     m_has_inventories : 1;
    bool     m_has_colors      : 1;
    bool     m_has_weight      : 1;
    bool     m_has_year        : 1;

    QString  m_api_name;
    Tables   m_table_name;

    static const QString m_table_prefix;
    static const QString m_names[8];
    static const char m_picture_ids[8];
    static const QString m_api_names[8];
    static const Tables m_table_names[8];

//		const Category **m_categories;

private:
    friend class BrickLink;
//		friend class BrickLink::TextImport;
    friend QDataStream &operator << (QDataStream &ds, const ItemType *itt);
    friend QDataStream &operator >> (QDataStream &ds, ItemType *itt);
};


class BrickLink : public QOAuth1
{
    Q_OBJECT

public:
    BrickLink(QObject *parent = nullptr);
    bool checkConnection(QObject *parent = nullptr);
    void importOrderItem(int orderID);
    void importOrders(bool filed);
    void importUserInventory();
    static QJsonArray validateBricklinkResponse(QObject* obj);

    QString getItemInformation(QString type, QString no);

    class Picture;
    class Color;
    class Item;

    Picture *picture ( const Item *item, const Color *color, bool high_priority = false );
    Picture *largePicture ( const Item *item, bool high_priority = false );

    static BrickLink *inst();

    const Item* item(const QString itemType, const char *id) const;
    const QString getApiKey() const;

signals:
    void dataBaseUpdated();
    void dataBaseUpdatedWithOrders();

private slots:
    void parseJsonOrderItem(int orderID);
    void pictureJobFinished(CTransfer::Job*);

private:
    Q_DISABLE_COPY(BrickLink)
    void parseJsonOrders();
    void parseJsonUserInventory();

    struct dummy3 {
        CTransfer* transfer;
        int update_iv;

        QList<Picture *> diskload;

        CAsciiRefCache<Picture, 503, 500> cache;
    } m_pictures;

    static BrickLink *s_inst;

public:
    class Color {
    public:
        uint id ( ) const           { return m_id; }
        const char *name ( ) const  { return m_name; }
        QColor color ( ) const      { return m_color; }

        const char *peeronName ( ) const { return m_peeron_name; }
        int ldrawId ( ) const            { return m_ldraw_id; }

        bool isTransparent ( ) const { return m_is_trans; }
        bool isGlitter ( ) const     { return m_is_glitter; }
        bool isSpeckle ( ) const     { return m_is_speckle; }
        bool isMetallic ( ) const    { return m_is_metallic; }
        bool isChrome ( ) const      { return m_is_chrome; }

        ~Color ( );

    private:
        uint   m_id;
        char * m_name;
        char * m_peeron_name;
        int    m_ldraw_id;
        QColor m_color;

        bool   m_is_trans    : 1;
        bool   m_is_glitter  : 1;
        bool   m_is_speckle  : 1;
        bool   m_is_metallic : 1;
        bool   m_is_chrome   : 1;

    private:
        Color ( );

        friend class BrickLink;
//		friend class BrickLink::TextImport;
        friend QDataStream &operator << ( QDataStream &ds, const BrickLink::Color *col );
        friend QDataStream &operator >> ( QDataStream &ds, BrickLink::Color *col );
    };

    class Item {
    public:
        const char *id ( ) const                 { return m_id; }
        const char *name ( ) const               { return m_name; }
        const ItemType *itemType ( ) const       { return m_item_type; }
//		const Category *category ( ) const       { return m_categories [0]; }
//		const Category **allCategories ( ) const { return m_categories; }
//		bool hasCategory ( const Category *cat ) const;
        bool hasInventory ( ) const              { return ( m_last_inv_update >= 0 ); }
        QDateTime inventoryUpdated ( ) const     { QDateTime dt; if ( m_last_inv_update >= 0 ) dt.setTime_t ( m_last_inv_update ); return dt; }
//		const Color *defaultColor ( ) const      { return m_color; }
        double weight ( ) const                  { return m_weight; }
        int yearReleased ( ) const               { return m_year ? m_year + 1900 : 0; }
        Item(); // WAS PRIVATE ORIGINALLY
        ~Item ( );

//		typedef Q3ValueVector <QPair <int, const Item *> >   AppearsInMapVector ;
//		typedef QMap <const Color *, AppearsInMapVector>    AppearsInMap;

//		AppearsInMap appearsIn ( const Color *color = 0 ) const;
//		InvItemList  consistsOf ( ) const;

        uint index ( ) const { return m_index; } // only for internal use (picture/priceguide hashes)

    private:
        char *            m_id;
        char *            m_name;
        const ItemType*   m_item_type;
//		const Category ** m_categories;
        const Color *     m_color;
        time_t            m_last_inv_update;
        float             m_weight;
        quint32           m_index : 24;
        quint32           m_year  : 8;

        mutable quint32 *m_appears_in;
        mutable quint64 *m_consists_of;

    private:
//        Item ( );

//		void setAppearsIn ( const AppearsInMap &map ) const;
//		void setConsistsOf ( const InvItemList &items ) const;

        struct appears_in_record {
            quint32  m12  : 12;
            quint32  m20  : 20;
        };

        struct consists_of_record {
            quint64  m_qty      : 12;
            quint64  m_index    : 20;
            quint64  m_color    : 12;
            quint64  m_extra    : 1;
            quint64  m_isalt    : 1;
            quint64  m_altid    : 6;
            quint64  m_cpart    : 1;
            quint64  m_reserved : 11;
        };

        static Item *parse ( const BrickLink *bl, uint count, const char **strs, void *itemtype );

        static int compare ( const BrickLink::Item **a, const BrickLink::Item **b );

        friend class BrickLink;
//		friend class BrickLink::TextImport;
        friend QDataStream &operator << ( QDataStream &ds, const BrickLink::Item *item );
        friend QDataStream &operator >> ( QDataStream &ds, BrickLink::Item *item );
    };

    class Picture : public CRef {
    public:
        const Item *item ( ) const          { return m_item; }
        const Color *color ( ) const        { return m_color; }

        void update ( bool high_priority = false );
        QDateTime lastUpdate ( ) const      { return m_fetched; }

        bool valid ( ) const                { return m_valid; }
        int updateStatus ( ) const          { return m_update_status; }

        const QPixmap pixmap ( ) const;

        virtual ~Picture ( );

        const QImage image ( ) const        { return m_image; }
        const char *key ( ) const           { return m_key; }

    private:
        const Item *  m_item;
        const Color * m_color;

        QDateTime     m_fetched;

        bool          m_valid         : 1;
        int           m_update_status : 7;

        char *        m_key;
        QImage        m_image;

    private:
        Picture(const Item *item, const Color *color, const QString key);

        void load_from_disk ( );
        void save_to_disk ( );

        void parse ( const char *data, uint size );

        friend class BrickLink;
    };
};

#endif // BRICKLINK_H
