

#ifndef YUE_BELL_SETTINGS_HPP
#define YUE_BELL_SETTINGS_HPP


#include <QSettings>

namespace yue {
namespace bell {

class Settings : public QObject
{
    Q_OBJECT

    static Settings* m_instance;


    QSettings settings;
public:
    Settings();
    ~Settings();

    static Settings* create() {
        m_instance = new Settings();
        return m_instance;
    }
    static Settings* instance() {
        return m_instance;
    }

    QList<QPair<QString, QString>> dynamicLists();
    void setDynamicLists(QList<QPair<QString, QString>> lst);

};

} // namespace bell
} // namespace yue

#endif
