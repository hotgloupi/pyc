#include "serialize.hpp"

#include <pyc/ast.hpp>

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/export.hpp>
#include <boost/serialization/split_free.hpp>

namespace boost { namespace serialization {

    // std::unique_ptr
    template <typename Archive, typename T>
    void
    save(Archive& ar, std::unique_ptr<T> const& value, unsigned const)
    {
        T const* ptr = value.get();
        ar & boost::serialization::make_nvp("ptr", ptr);
    }

    template <typename Archive, typename T>
    void load(Archive& ar, std::unique_ptr<T>& value, unsigned const)
    {
        T* ptr;
        ar & boost::serialization::make_nvp("ptr", ptr);
        value.reset(ptr);
    }


    template <typename Archive, typename T>
    void serialize(Archive& ar, std::unique_ptr<T>& value, unsigned const version)
    {
        boost::serialization::split_free(ar, value, version);
    }

    // std::vector with movable only support
    template <class Archive, class T, class Allocator>
    inline void
    save(Archive& ar, const std::vector<T, Allocator>& t, const unsigned int)
    {
        collection_size_type count(t.size());
        ar << BOOST_SERIALIZATION_NVP(count);
        for (auto it = t.begin(), end = t.end(); it != end; ++it)
            ar << boost::serialization::make_nvp("item", (*it));
    }

    template <class Archive, class T, class Allocator>
    inline void
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
    inline void serialize(Archive& ar, std::vector<T, Allocator>& t,
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
        boost::archive::text_oarchive ar(out);
        Node const* root = &node;
        ar << BOOST_SERIALIZATION_NVP(root);
    }

}}}
