#include "serialize.hpp"

#include <pyc/ast.hpp>

#include <boost/archive/xml_oarchive.hpp>
//#include <boost/archive/xml_iarchive.hpp>
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/detail/common_oarchive.hpp>
#include <boost/archive/impl/archive_serializer_map.ipp>

#include <boost/serialization/export.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/string.hpp>

#include <map>

//namespace {

/*
    class JSONOutputArchive
        : public boost::archive::detail::common_oarchive<JSONOutputArchive>
    {
    public:
        friend class boost::archive::save_access;

    private:
        std::ostream& _out;
        std::string _indent;
        int _class_id;
        std::map<int, std::string> _classes;

    public:
        explicit JSONOutputArchive(std::ostream& out)
            : _out(out)
            , _indent()
            , _class_id(0)
        {}

    public:
        void save(boost::archive::class_id_reference_type const& class_id_reference)
        {
            _out << _indent << "CLASS ID REF: " << class_id_reference << std::endl;
            _class_id = class_id_reference;
        }
        void save(boost::archive::class_name_type const& name)
        {
            _classes[_class_id] = name;
            _out << _indent << "CLASS: " << name << std::endl;
        }
        void save(boost::archive::class_id_type const& class_id)
        { _class_id = class_id; _out << _indent << "CLASS_ID: " << class_id << std::endl; }

        void save(boost::archive::tracking_type const& tracking_type)
        {}//{ _out << _indent << "TRACKING: " << tracking_type << std::endl; }

        void save(boost::archive::object_id_type const& object_id)
        {} //{ _out << _indent << "object_id: " << object_id << std::endl; }

        void save(boost::archive::object_reference_type const&)
        {}

        void save(boost::archive::version_type const&)
        {}

        void save(boost::serialization::collection_size_type const&)
        {}

        void save(int const& value)
        {
            _out << _indent << "INT:" <<  value << std::endl;
        }

        void save(std::string const& value) {
            _out << _indent << "STR:" <<  value << std::endl;
        }

        void save_start(char const* name)
        {
            _out << _indent << "START: " << name << std::endl;
        }

        void save_end(char const* name)
        {
            _out << _indent << "END: " << name << std::endl;
        }

        void end_preamble()
        {
            //_out << "END PREAMBLE" << std::endl;
        }

        template <typename T>
        void save_override(boost::serialization::nvp<T> const& value, int)
        {
//            std::string const* name = nullptr;
//            auto it = _classes.find(_class_id);
//            if (it != _classes.end())
//                name = &it->second;
//            _class_id = 0;
            char const* name = value.name();
            if (name == nullptr)
                name = "WAT";
            _out << _indent << '[' <<name  << ']'<< " ( " << __PRETTY_FUNCTION__ + sizeof("JSONOutputArchive::save_override(T&, int) [with T = const ") << "\n";
            _indent = std::string(_indent.size() + 1, ' ');

//            if (std::is_base_of<pyc::ast::Node, T>::value && name)
//            {
//                _out << _indent << "Starting " << *name << ' ' << __PRETTY_FUNCTION__ + sizeof("JSONOutputArchive::save_override(T&, int) [with T = const ") << "\n";
//                _indent = std::string(_indent.size() + 2, ' ');
//            }
            boost::archive::save(*this, value.const_value());
//            if (std::is_base_of<pyc::ast::Node, T>::value &&  name)
//            {
//                _indent = std::string(_indent.size() - 2, ' ');
//                _out << _indent << "Ending " << *name << ' '<< __PRETTY_FUNCTION__ + sizeof("JSONOutputArchive::save_override(T&, int) [with T = const ") << "\n";
//            }

             _indent = std::string(_indent.size() - 1, ' ');
             _out << _indent<< '[' <<name  << ']'<< ") " << __PRETTY_FUNCTION__ + sizeof("JSONOutputArchive::save_override(T&, int) [with T = const ") << "\n";
        }

        template<typename T>
        void save_override(T const& value, int)
        {
            boost::archive::save(*this, value);
        }
    };

//}

    BOOST_SERIALIZATION_REGISTER_ARCHIVE(JSONOutputArchive);
*/
namespace boost { namespace serialization {

    // std::unique_ptr
    template <typename Archive, typename T>
    static void
    save(Archive& ar, std::unique_ptr<T> const& value, unsigned const)
    {
        T const* ptr = value.get();
        ar & boost::serialization::make_nvp("ptr", ptr);
    }

    template <typename Archive, typename T>
    static void load(Archive& ar, std::unique_ptr<T>& value, unsigned const)
    {
        T* ptr;
        ar & boost::serialization::make_nvp("ptr", ptr);
        value.reset(ptr);
    }

    template <typename Archive, typename T>
    static void
    serialize(Archive& ar, std::unique_ptr<T>& value, unsigned const version)
    {
        boost::serialization::split_free(ar, value, version);
    }

    // std::vector with movable only support
    template <class Archive, class T, class Allocator>
    static void
    save(Archive& ar, const std::vector<T, Allocator>& t, const unsigned int)
    {
        collection_size_type count(t.size());
        ar << BOOST_SERIALIZATION_NVP(count);
        for (auto it = t.begin(), end = t.end(); it != end; ++it)
            ar << boost::serialization::make_nvp("item", (*it));
    }

    template <class Archive, class T, class Allocator>
    static void
    load(Archive& ar, std::vector<T, Allocator>& t, const unsigned int)
    {
        collection_size_type count;
        ar >> BOOST_SERIALIZATION_NVP(count);
        t.clear();
        t.reserve(count);
        while (count-- > 0) {
            T i;
            ar >> boost::serialization::make_nvp("item", i);
            t.emplace_back(std::move(i));
        }
    }

    template <class Archive, class T, class Allocator>
    static void serialize(Archive& ar, std::vector<T, Allocator>& t,
                          const unsigned int file_version)
    {
        boost::serialization::split_free(ar, t, file_version);
    }

}}

// We implement these here so that the linker keep the symbols
#define EXPORT_SERIALIZATION(Type)                                             \
    BOOST_CLASS_EXPORT(pyc::ast::Type);                                        \
    namespace boost                                                            \
    {                                                                          \
    namespace serialization                                                    \
    {                                                                          \
        template <class Archive>                                               \
        void                                                                   \
        serialize(Archive& ar, pyc::ast::Type& value, unsigned const version); \
    }                                                                          \
    }                                                                          \
    template <class Archive>                                                   \
    void boost::serialization::serialize(                                      \
      Archive& ar, pyc::ast::Type& value, unsigned const)

// For types that are not default constructible
#define LOAD_CONSTRUCT(Type)                                            \
    namespace boost                                                     \
    {                                                                   \
        namespace serialization                                         \
        {                                                               \
            template <class Archive>                                    \
            static void                                                 \
            load_construct_data(Archive& ar, pyc::ast::Type* value,     \
                                const unsigned int /* file_version */); \
        }                                                               \
    }                                                                   \
    template <class Archive>                                            \
    static void boost::serialization::load_construct_data(              \
      Archive& ar, pyc::ast::Type* mem, const unsigned int /* file_version */)

#define SAVE_CONSTRUCT(Type)                                              \
    namespace boost                                                       \
    {                                                                     \
        namespace serialization                                           \
        {                                                                 \
            template <class Archive>                                      \
            static void                                                   \
            save_construct_data(Archive& ar, pyc::ast::Type const* value, \
                                const unsigned int /* file_version */);   \
        }                                                                 \
    }                                                                     \
    template <class Archive>                                              \
    static void boost::serialization::save_construct_data(                \
      Archive& ar, pyc::ast::Type const* value,                           \
      const unsigned int /* file_version */)

EXPORT_SERIALIZATION(Node)
{
    (void) ar;
    (void) value;
}

#define SUPER(Type)  make_nvp(#Type, base_object<pyc::ast::Type>(value))

EXPORT_SERIALIZATION(Block)
{
    ar & SUPER(Node);
    ar & boost::serialization::make_nvp("statements", value.statements);
}

EXPORT_SERIALIZATION(Statement)
{
    ar & SUPER(Node);
}

EXPORT_SERIALIZATION(FunctionDefinition)
{
    ar & SUPER(Statement);
    ar & make_nvp("name", value.name);
    ar & make_nvp("body", value.body);
}

EXPORT_SERIALIZATION(ReturnStatement)
{
    ar & SUPER(Statement);
    ar & make_nvp("value", value.value);
}

EXPORT_SERIALIZATION(Expression)
{
    ar & SUPER(Node);
}

SAVE_CONSTRUCT(Number)
{
    ar << make_nvp("value", value->value);
}

LOAD_CONSTRUCT(Number)
{
    std::string value;
    ar >> make_nvp("value", value);
    ::new (mem) pyc::ast::Number(std::move(value));
}

EXPORT_SERIALIZATION(Number)
{
    ar & SUPER(Expression);
}


SAVE_CONSTRUCT(BinaryExpression)
{
    ar << make_nvp("token", value->token);
    ar << make_nvp("lhs", value->lhs);
    ar << make_nvp("rhs", value->rhs);
}


LOAD_CONSTRUCT(BinaryExpression)
{
    pyc::parser::Token token;
    pyc::Ptr<pyc::ast::Expression> lhs;
    pyc::Ptr<pyc::ast::Expression> rhs;

    ar >> make_nvp("token", token)
       >> make_nvp("lhs", lhs)
       >> make_nvp("rhs", rhs);
    new (mem) pyc::ast::BinaryExpression(token, std::move(lhs), std::move(rhs));
}

EXPORT_SERIALIZATION(BinaryExpression)
{
    ar & SUPER(Expression);
}

namespace pyc { namespace ast { namespace serialize {

    void to_xml(std::ostream& out, Node const& node)
    {
        boost::archive::xml_oarchive ar(out);
        Node const* root = &node;
        ar << BOOST_SERIALIZATION_NVP(root);
    }

    void to_text(std::ostream& out, Node const& node)
    {
        //boost::archive::text_oarchive ar(out);
        //Node const* root = &node;
        //ar << BOOST_SERIALIZATION_NVP(root);
        node.dump(out);
    }

}}}
