

#ifndef YUE_CORE_SEARCH_RULE_HPP
#define YUE_CORE_SEARCH_RULE_HPP

#include <map>
#include <memory>
#include <set>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib> // atoi and atof over stoi and stof

namespace yue {
namespace core {

//-----------------------------------------------

namespace util {

std::string join(const std::vector<std::string> &items, const std::string &sep);

// convert string to numerical type
template<typename T>
T fromString(const std::string &s){/*compiliation error if used*/}

template<> std::string fromString<std::string>(const std::string &s);
template<> int fromString<int>(const std::string &s);
template<> float fromString<float>(const std::string &s);

template<typename T>
bool
compare_eq(std::string elem, T value, bool ignoreCase) {
    (void) ignoreCase;
    return fromString<T>(elem) == value;
}

template<typename T>
bool
compare_ne(std::string elem, T value, bool ignoreCase) {
    (void) ignoreCase;
    return fromString<T>(elem) != value;
}

template<typename T>
bool
compare_lt(std::string elem, T value, bool ignoreCase) {
    (void) ignoreCase;
    return fromString<T>(elem) < value;
}

template<typename T>
bool
compare_gt(std::string elem, T value, bool ignoreCase) {
    (void) ignoreCase;
    return fromString<T>(elem) > value;
}

template<typename T>
bool
compare_le(std::string elem, T value, bool ignoreCase) {
    (void) ignoreCase;
    return fromString<T>(elem) <= value;
}

template<typename T>
bool
compare_ge(std::string elem, T value, bool ignoreCase) {
    (void) ignoreCase;
    return fromString<T>(elem) >= value;
}

template<typename T>
bool
compare_in(std::string elem, T v1, T v2, bool ignoreCase) {
    (void) ignoreCase;
    T v = fromString<T>(elem);
    return v1 <= v && v <= v2;
}

template<typename T>
std::string
toString(T value) {
    // std::to_string is not available on all platforms
    std::stringstream ss;
    ss << value;
    return ss.str();
};

template<> std::string toString<std::string>(std::string value);


class VariantError : std::runtime_error
{
public:
    VariantError(std::string type1,std::string type2)
        : std::runtime_error(std::string("Invalid Type: ") + type1 +
            " Variant is of type: " + type2)
    {}
};

/**
 * @brief a Variant is a class wrapping a basic type
 *
 * A variant knows what type it is, and an accesor method
 * is provided for every available type.
 *
 * any Variant can be converted to a string, which
 * has the requirement that fromString<T> must be able to
 * recover the original value.
 */
class Variant
{

public:
    enum class Type {
        Unknown,
        Int,
        Float,
        String,
    };
    Variant() {}
    virtual ~Variant() {}

    template<typename T>
    static std::unique_ptr<Variant> create(T value);

    virtual Type type(void)=0;
    virtual int toInt(void)=0;
    virtual float toFloat(void)=0;
    virtual std::string toString(void)=0;
};

template<>
std::unique_ptr<Variant> Variant::create<int>(int value);
template<>
std::unique_ptr<Variant> Variant::create<float>(float value);
template<>
std::unique_ptr<Variant> Variant::create<std::string>(std::string value);

class VariantInt : public Variant
{
private:
    int m_value;

public:
    VariantInt(int value)
        : Variant()
        , m_value(value)
    {}
    ~VariantInt() {}

    virtual Variant::Type type(void) { return Variant::Type::Int; }
    virtual int toInt(void) { return m_value; }
    virtual float toFloat(void) { throw VariantError("Float","Int"); }
    virtual std::string toString(void) { return util::toString<int>(m_value); }
};

class VariantFloat : public Variant
{
private:
    float m_value;

public:
    VariantFloat(float value)
        : Variant()
        , m_value(value)
    {}
    ~VariantFloat() {}

    virtual Variant::Type type(void) { return Variant::Type::Float; }
    virtual int toInt(void) { throw VariantError("Int", "Float"); }
    virtual float toFloat(void) { return m_value; }
    virtual std::string toString(void) { return util::toString<float>(m_value);  }
};

class VariantString : public Variant
{
private:
    std::string m_value;

public:
    VariantString(std::string value)
        : Variant()
        , m_value(value)
    {}
    ~VariantString() {}

    virtual Variant::Type type(void) { return Variant::Type::String; }
    virtual int toInt(void) { throw VariantError("Int", "String"); }
    virtual float toFloat(void) { throw VariantError("Float", "String"); }
    virtual std::string toString(void) { return m_value; }
};

} // namespace util

//-----------------------------------------------

class Rule {

  public:
    typedef std::vector<std::unique_ptr<util::Variant>> QueryValues;

  public:
    Rule() {}
    virtual ~Rule() {}

    virtual std::string toSql(QueryValues &values) const = 0;
    virtual std::string toFmtString(QueryValues &values) {
        return toSql(values);
    }

    std::string toSqlString();
    // std::string toString();
};

//-----------------------------------------------

class SearchRule : public Rule {
  public:
    typedef std::unique_ptr<SearchRule> SearchRulePtr;
    typedef std::vector<SearchRulePtr> SearchRuleList;

  public:
    SearchRule() {}
    virtual ~SearchRule() {}

    virtual bool check(const std::map<std::string, std::string> &elem,
                       bool ignoreCase = true) const = 0;
};

//-----------------------------------------------

class BlankSearchRule : public SearchRule {
  public:
    BlankSearchRule() {}
    virtual ~BlankSearchRule() {}

    virtual bool check(const std::map<std::string, std::string> &elem,
                       bool ignoreCase = true) const override {
        (void) elem;
        (void) ignoreCase;
        return true;
    }

    virtual std::string toSql(QueryValues &values) const override {
        (void) values;
        return "";
    }
};

//-----------------------------------------------

class ColumnSearchRule : public SearchRule {
  protected:
    std::string m_sColumn;

  public:
    ColumnSearchRule(const std::string &column) : m_sColumn(column) {}
    virtual ~ColumnSearchRule() {}
};

// TODO: RegExpSearchRule

class PartialStringSearchRule : public ColumnSearchRule {
    std::string m_sValue;

  public:
    PartialStringSearchRule(const std::string &column, const std::string &value)
        : ColumnSearchRule(column), m_sValue(value) {}
    virtual ~PartialStringSearchRule(){};

    virtual bool check(const std::map<std::string, std::string> &elem,
                       bool ignoreCase = true) const override;
    virtual std::string toSql(QueryValues &values) const override;
};

class InvertedPartialStringSearchRule : public ColumnSearchRule {
    std::string m_sValue;

  public:
    InvertedPartialStringSearchRule(const std::string &column,
                                    const std::string &value)
        : ColumnSearchRule(column), m_sValue(value) {}
    virtual ~InvertedPartialStringSearchRule(){}

    virtual bool check(const std::map<std::string, std::string> &elem,
                       bool ignoreCase = true) const override;
    virtual std::string toSql(QueryValues &values) const override;
};

#define ColumnSearchRuleDef(cls, fn, repr)                                    \
    template<typename T> class cls : public ColumnSearchRule {                \
        T m_tValue;                                                           \
                                                                              \
      public:                                                                 \
        cls(const std::string &column, T value)                               \
            : ColumnSearchRule(column), m_tValue(value) {}                    \
        virtual ~cls(){};                                                     \
        virtual bool check(const std::map<std::string, std::string> &elem,    \
                           bool ignoreCase = true) const override {           \
            return fn<T>(elem.find(m_sColumn)->second, m_tValue, ignoreCase); \
        }                                                                     \
        virtual std::string toSql(QueryValues &values) const override {       \
            values.push_back(util::Variant::create<T>(m_tValue));                    \
            return m_sColumn + repr;                                          \
        }                                                                     \
    };

ColumnSearchRuleDef(ExactSearchRule, util::compare_eq, " == ?");
ColumnSearchRuleDef(InvertedExactSearchRule, util::compare_ne, " != ?");
ColumnSearchRuleDef(LessThanSearchRule, util::compare_lt, " < ?");
ColumnSearchRuleDef(LessThanEqualSearchRule, util::compare_le, " <= ?");
ColumnSearchRuleDef(GreaterThanSearchRule, util::compare_gt, " > ?");
ColumnSearchRuleDef(GreaterThanEqualSearchRule, util::compare_ge, " >= ?");

#undef ColumnSearchRuleDef

//-----------------------------------------------

template<typename T> class RangeSearchRule : public ColumnSearchRule {
    T m_tValueLow;
    T m_tValueHigh;

  public:
    RangeSearchRule(const std::string &column, T value_low, T value_high)
        : ColumnSearchRule(column)
        , m_tValueLow(value_low)
        , m_tValueHigh(value_high) {}
    virtual ~RangeSearchRule(){};

    virtual bool check(const std::map<std::string, std::string> &elem,
                       bool ignoreCase = true) const override;
    virtual std::string toSql(QueryValues &values) const override;
};

template<typename T> class NotRangeSearchRule : public ColumnSearchRule {
    T m_tValueLow;
    T m_tValueHigh;

  public:
    NotRangeSearchRule(const std::string &column, T value_low, T value_high)
        : ColumnSearchRule(column)
        , m_tValueLow(value_low)
        , m_tValueHigh(value_high) {}
    virtual ~NotRangeSearchRule(){};

    virtual bool check(const std::map<std::string, std::string> &elem,
                       bool ignoreCase = true) const override;
    virtual std::string toSql(QueryValues &values) const override;
};

//-----------------------------------------------

class MetaSearchRule : public SearchRule {
  public:
    MetaSearchRule() {}
    virtual ~MetaSearchRule() {}
};

class AndSearchRule : public MetaSearchRule {
    SearchRule::SearchRuleList m_vRules;

  public:
    AndSearchRule(SearchRule::SearchRuleList rules)
        : MetaSearchRule(), m_vRules(std::move(rules)){};
    virtual ~AndSearchRule() {}

    virtual bool check(const std::map<std::string, std::string> &elem,
                       bool ignoreCase = true) const override;
    virtual std::string toSql(QueryValues &values) const override;
};

class OrSearchRule : public MetaSearchRule {
    SearchRule::SearchRuleList m_vRules;

  public:
    OrSearchRule(SearchRule::SearchRuleList rules)
        : MetaSearchRule(), m_vRules(std::move(rules)){};
    virtual ~OrSearchRule() {}

    virtual bool check(const std::map<std::string, std::string> &elem,
                       bool ignoreCase = true) const override;
    virtual std::string toSql(QueryValues &values) const override;
};

class NotSearchRule : public MetaSearchRule {
    SearchRulePtr m_pRule = nullptr;

  public:
    NotSearchRule(SearchRulePtr rule)
        : MetaSearchRule(), m_pRule(std::move(rule)) {}
    virtual ~NotSearchRule() {}

    virtual bool check(const std::map<std::string, std::string> &elem,
                       bool ignoreCase = true) const override;
    virtual std::string toSql(QueryValues &values) const override;
};

//-----------------------------------------------

/**
 * @brief template specialization for joining rules together
 *
 * the static function match_any returns true for when there are
 * multiple rules, and any one in the set can match true for the
 * whole expression to be considered true.
 *
 * the struct is used to apply De Morgan's Law to an expression
 *      !(a | b | c) === (!a & !b & !c)
 */
template<typename R, typename T> struct _MultiColumnSearchRuleMetaBehavior {
    static bool match_any(void) {
        return true;
    }
};

template<typename T>
struct _MultiColumnSearchRuleMetaBehavior<InvertedExactSearchRule<T>, T> {
    static bool match_any(void) {
        return false;
    }
};

template<typename T>
struct _MultiColumnSearchRuleMetaBehavior<InvertedPartialStringSearchRule, T> {
    static bool match_any(void) {
        return false;
    }
};

/**
 * @brief A Search Rule for applying the same query to multiple columns.
 *
 * The class is parameterized by a column rule (R) and the type (T).
 * R can be any column search rule, parameterized by T.
 *
 * This is a convenience function mostly used for debugging purposes.
 */
template<typename R, typename T>
class MultiColumnSearchRule : public SearchRule {
    std::set<std::string> m_vColumns;
    SearchRulePtr m_pRule;
    std::string m_sColumnId;

  public:
    MultiColumnSearchRule(std::set<std::string> columns,
                          T value,
                          const std::string &colid = "multi")
        : SearchRule(), m_vColumns(columns), m_sColumnId(colid) {
        SearchRuleList rules;
        for (auto const &col : columns) {
            rules.push_back(std::unique_ptr<SearchRule>(new R(col, value)));
        }

        typedef _MultiColumnSearchRuleMetaBehavior<R, T> SearchBehavior;

        if (SearchBehavior::match_any()) {
            m_pRule =
                std::unique_ptr<SearchRule>(new OrSearchRule(std::move(rules)));
        } else {
            m_pRule = std::unique_ptr<SearchRule>(
                new AndSearchRule(std::move(rules)));
        }
    };
    ~MultiColumnSearchRule(){};

    virtual bool check(const std::map<std::string, std::string> &elem,
                       bool ignoreCase = true) const override {
        return m_pRule->check(elem, ignoreCase);
    }
    virtual std::string toSql(QueryValues &values) const override {
        return m_pRule->toSql(values);
    }
};

} /*namespace core*/
} /*namespace yue*/

#endif /*YUE_CORE_SEARCH_RULE_HPP*/
