
#include "yue/core/search/rules.hpp"

//-----------------------------------------------

namespace yue {
namespace core {

namespace util {

template<>
std::unique_ptr<Variant>
Variant::create<int>(int value)
{
    return std::unique_ptr<Variant>(new VariantInt(value));
}

template<>
std::unique_ptr<Variant>
Variant::create<float>(float value)
{
    return std::unique_ptr<Variant>(new VariantFloat(value));
}

template<>
std::unique_ptr<Variant>
Variant::create<std::string>(std::string value)
{
    return std::unique_ptr<Variant>(new VariantString(value));
}

std::string
join(const std::vector<std::string> &items, const std::string &sep) {
    std::string result("");
    if (items.size() > 0)
        result = items[0];
    for (size_t i = 1; i < items.size(); i++) {
        result = result + sep + items[i];
    }
    return result;
}

template<>
std::string
fromString<std::string>(const std::string &s) {
    return s;
}

template<>
int
fromString<int>(const std::string &s) {
    // std::stoi is not available on all platforms
    return std::atoi(s.c_str());
}

template<>
float
fromString<float>(const std::string &s) {
    // std::stoi is not available on all platforms
    return static_cast<float>(std::atof(s.c_str()));
}

template<>
std::string
toString<std::string>(std::string value) {
    return value;
};

} // namespace util

/**
 * @brief create a string representation of the sql rule.
 *
 * @remark this is for debugging only and not intended to produce
 * valid sequel. In particular it will not properly quote strings
 * and does not guard against SQL injection in any way.
 */
std::string
Rule::toSqlString() {
    QueryValues vals;
    std::string sql;

    sql = toSql(vals);

    size_t pos = 0;
    for (auto const &v : vals) {
        size_t i = sql.find_first_of("?", pos);
        std::string vs = v->toString();
        sql = sql.substr(0, i) + vs + sql.substr(i + 1, sql.size() - i);
        pos = i + vs.size();
    }
    return sql;
}

//-----------------------------------------------

bool
PartialStringSearchRule::check(const std::map<std::string, std::string> &elem,
                               bool ignoreCase /* = true*/) const {
    (void) ignoreCase;
    // Note: elem.find(m_sColumn)->second allows const access
    // e;em[m_sColumn] does not
    return m_sValue.find(elem.find(m_sColumn)->second) != std::string::npos;
}

std::string
PartialStringSearchRule::toSql(QueryValues &values) const {
    values.push_back( util::Variant::create<std::string>("%" + m_sValue + "%") );
    return m_sColumn + " LIKE ?";
}

bool
InvertedPartialStringSearchRule::check(
    const std::map<std::string, std::string> &elem,
    bool ignoreCase /* = true*/) const {
    (void) ignoreCase;
    return m_sValue.find(elem.find(m_sColumn)->second) == std::string::npos;
}

std::string
InvertedPartialStringSearchRule::toSql(QueryValues &values) const {
    values.push_back( util::Variant::create<std::string>("%"+m_sValue+"%") );
    return m_sColumn + " NOT LIKE ?";
}

//-----------------------------------------------

template<typename T>
bool
RangeSearchRule<T>::check(const std::map<std::string, std::string> &elem,
                          bool ignoreCase /* = true*/) const {
    return util::compare_in<T>(elem.find(m_sColumn)->second, m_tValueLow,
                               m_tValueHigh, ignoreCase);
}

template<typename T>
std::string
RangeSearchRule<T>::toSql(QueryValues &values) const {
    values.push_back(util::Variant::create<T>(m_tValueLow));
    values.push_back(util::Variant::create<T>(m_tValueHigh));
    return m_sColumn + " BETWEEN ? AND ?";
}

template<typename T>
bool
NotRangeSearchRule<T>::check(const std::map<std::string, std::string> &elem,
                             bool ignoreCase /* = true*/) const {
    return !util::compare_in<T>(elem.find(m_sColumn)->second, m_tValueLow,
                                m_tValueHigh, ignoreCase);
}

template<typename T>
std::string
NotRangeSearchRule<T>::toSql(QueryValues &values) const {
    values.push_back(util::Variant::create<T>(m_tValueLow));
    values.push_back(util::Variant::create<T>(m_tValueHigh));
    return m_sColumn + " NOT BETWEEN ? AND ?";
}

//-----------------------------------------------

bool
AndSearchRule::check(const std::map<std::string, std::string> &elem,
                     bool ignoreCase /* = true*/) const {
    for (auto const &rule : m_vRules) {
        if (!rule->check(elem, ignoreCase)) {
            return false;
        }
    }
    return true;
}

std::string
AndSearchRule::toSql(QueryValues &values) const {
    std::vector<std::string> sql;
    QueryValues vals;
    for (auto const &rule : m_vRules) {
        sql.push_back(rule->toSql(vals));
    }

    if (sql.size() == 0) {
        return "";
    }

    for( auto it=vals.begin(); it!=vals.end();)
    {
        values.emplace_back(std::move(*it));
        it = vals.erase(it);
    }


    std::string sqlstr = util::join(sql, " AND ");
    return "(" + sqlstr + ")";
}

bool
OrSearchRule::check(const std::map<std::string, std::string> &elem,
                    bool ignoreCase /* = true*/) const {
    for (auto const &rule : m_vRules) {
        if (rule->check(elem, ignoreCase)) {
            return true;
        }
    }

    return false;
}

std::string
OrSearchRule::toSql(QueryValues &values) const {
    std::vector<std::string> sql;
    QueryValues vals;
    for (auto const &rule : m_vRules) {
        sql.push_back(rule->toSql(vals));
    }

    if (sql.size() == 0) {
        return "";
    }

    for( auto it=vals.begin(); it!=vals.end();)
    {
        values.emplace_back(std::move(*it));
        it = vals.erase(it);
    }

    std::string sqlstr = util::join(sql, " OR ");
    return "(" + sqlstr + ")";
}

bool
NotSearchRule::check(const std::map<std::string, std::string> &elem,
                     bool ignoreCase /* = true*/) const {
    return !m_pRule->check(elem, ignoreCase);
}

std::string
NotSearchRule::toSql(QueryValues &values) const {
    std::string sqlstr = m_pRule->toSql(values);
    return "(NOT " + sqlstr + ")";
}

//-----------------------------------------------

#define TemplateClassDef(X)  \
    template class X<int>;   \
    template class X<float>; \
    template class X<std::string>;

TemplateClassDef(RangeSearchRule);
TemplateClassDef(NotRangeSearchRule);

} /*namespace core*/
} /*namespace yue*/
