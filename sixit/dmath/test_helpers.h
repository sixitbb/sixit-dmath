/*
Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.
This file is licensed under The 3-Clause BSD License, with full text available at the end of the file.
Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin, Victor Istomin
*/

#ifndef sixit_dmath_test_helpers_h_included
#define sixit_dmath_test_helpers_h_included


#include "sixit/core/lwa.h"
#include "sixit/core/testing/test.h"

#include "sixit/geometry/sixit_mathf.h"
#include "sixit/core/units.h"
#include "sixit/dmath/traits.h"
#include "sixit/dmath/fp_approximate_eq.h"

#include "sixit/rw/streams/generic/file_streams.h"
#include "sixit/rw/streams/generic/test/test_buffer_streams.h"
#include "sixit/rw/streams/generic/character_adapter_streams.h"
#include "sixit/rw/comparsers/json_comparser.h"
#include "sixit/gpal/include/bundled_asset_rw_stream.h"

#include "sixit/dmath/gamefloat/ieee_float_soft.h"
#include "sixit/ieee_float_shared_lib/ieee_float_shared_lib.h"
#include "sixit/dmath/gamefloat/ieee_float_inline_asm.h"
#include "sixit/dmath/gamefloat/ieee_float_if_strict_fp.h"
#include "sixit/dmath/gamefloat/ieee_float_if_semicolon_prohibits_reordering.h"
#include "sixit/dmath/gamefloat/ieee_float_static_lib.h"
#include "sixit/dmath/gamefloat/ieee_float_inline_asm.h"
#include "sixit/dmath/gamefloat/ieee_float_soft.h"
#include "sixit/dmath/gamefloat/float_with_sixit.h"

//#include "sixit/rw/memory_buffers/debug_memory_buffer.h"
//#include "sixit/rw/comparsers/cpu_specific_comparser.h"

#include "sixit/profiler/profiler.h"
#include "sixit/profiler/printer.h"

// availability checking
static_assert(sixit::dmath::fp_traits<float>::is_supported);
static_assert(sixit::dmath::fp_traits<sixit::dmath::ieee_float_soft>::is_supported);
#if defined(SIXIT_CPU_X64) || defined(SIXIT_CPU_X86) || (defined(SIXIT_COMPILER_ANY_GCC_OR_CLANG) && defined(SIXIT_CPU_ARM64))
static_assert(sixit::dmath::fp_traits<sixit::dmath::ieee_float_inline_asm>::is_supported);
#endif
#if defined(SIXIT_OS_WINDOWS) || defined(SIXIT_OS_LINUX) || defined(SIXIT_OS_MAC)
static_assert(sixit::dmath::fp_traits<sixit::dmath::ieee_float_shared_lib>::is_supported);
#endif

void report_fp_related_flags()
{
    bool none = true;
#ifdef SIXIT_DMATH_SUPPORT_IEEE_FLOAT_IF_STRICT
    fmt::print("fp-related compiler flag set: {}\n", "SIXIT_DMATH_SUPPORT_IEEE_FLOAT_IF_STRICT");
    none = false;
#endif
#ifdef SIXIT_DMATH_SUPPORT_IEEE_FLOAT_IF_SEMICOLON_PREVENTS_REORDERING
    fmt::print("fp-related compiler flag set: {}\n", "SIXIT_DMATH_SUPPORT_IEEE_FLOAT_IF_SEMICOLON_PREVENTS_REORDERING");
    none = false;
#endif
#ifdef SIXIT_DMATH_SUPPORT_IEEE_FLOAT_STATIC_LIB
    fmt::print("fp-related compiler flag set: {}\n", "SIXIT_DMATH_SUPPORT_IEEE_FLOAT_STATIC_LIB");
    none = false;
#endif
    if (none)
        fmt::print("fp-related compiler flag set: (none)\n");
    fmt::print("\n");
}



#if 0 // [DI] this code is yet underdeveloped and is left for future reworking, see ticket TR-<pending>
namespace sixit::dmath::test_helpers
{

    static_assert(sizeof(char) == sizeof(uint8_t));
    static_assert(alignof(char) == alignof(uint8_t));

    template <class T, class CharT>
    constexpr CharT* align_ptr(CharT* p)
    {
        static_assert(std::is_same_v<std::remove_const_t<CharT>, char>);
        static_assert(std::is_trivial<T>::value);
        static_assert(sizeof(char) == 1);
        constexpr size_t al = std::max(alignof(T), sizeof(T)); // sizeof(T) is added to improve compatibility with some MCUs
        static_assert((al % alignof(T)) == 0);                 // as long as al is a multiple of alignof(T) - we're fine
        static_assert(std::popcount(al) == 1);                 //'al is power of 2' check
        constexpr uintptr_t mask = ~(static_cast<uintptr_t>(al) - 1);
        constexpr size_t alm1 = al - 1;
        return reinterpret_cast<CharT*>((reinterpret_cast<uintptr_t>(p) + alm1) & mask);
    }

    class stream_to_struct_comparing_comparser_impl_base
    {
    public:
        static constexpr sixit::rw::comparsers::Proto proto = sixit::rw::comparsers::Proto::CPUSPEC;

        static constexpr std::array<uint8_t, 8> signature()
        {
            std::array<uint8_t, 8> info{
                (std::numeric_limits<float>::is_iec559 && std::numeric_limits<double>::is_iec559 && sizeof(float) == 4 &&
                 sizeof(double) == 8),
                (std::endian::native == std::endian::little ? 0 : (std::endian::native == std::endian::big ? 1 : 2)),
                static_cast<uint8_t>(std::max(alignof(char), sizeof(char))),
                static_cast<uint8_t>(std::max(alignof(uint16_t), sizeof(uint16_t))),
                static_cast<uint8_t>(std::max(alignof(uint32_t), sizeof(uint32_t))),
                static_cast<uint8_t>(std::max(alignof(uint64_t), sizeof(uint64_t))),
                static_cast<uint8_t>(std::max(alignof(float), sizeof(float))),
                static_cast<uint8_t>(std::max(alignof(double), sizeof(double))) };

            return info;
        }

        static constexpr bool is_position_based()
        {
            return true;
        }
        static constexpr bool is_tag_based()
        {
            return false;
        }
        template <sixit::lwa::string_literal_helper structName, class StructT>
        void begin_struct(StructT& ref)
        {
            m_struct_stack.push_back({ stack_entry::EntryType::Struct, reinterpret_cast<uintptr_t>(&ref), (const char*)(structName) });
        }
        void end_struct()
        {
            m_struct_stack.pop_back();
        }

        template <sixit::lwa::string_literal_helper structName, class StructT>
        void begin_nonextensible_struct(StructT& ref)
        {
            begin_struct<structName, StructT>(ref);
        }

        void end_nonextensible_struct()
        {
            end_struct();
        }

        template <sixit::lwa::string_literal_helper structName, class StructT>
        void begin_dynamic_struct(StructT& ref, void*, size_t)
        {
            begin_struct<structName, StructT>(ref);
        }

        void end_dynamic_struct()
        {
            end_struct();
        }

        int conv_depth = 0; // depth of modifying converter calling

    protected:
        struct stack_entry
        {
            enum EntryType { Undefined, Struct, Array, ArrayElem };
            EntryType type = EntryType::Undefined;
            uintptr_t ptr = 0;
            std::string name;
            ptrdiff_t idx = 0; // for elements
        };
        std::vector<stack_entry> m_struct_stack;

        void in_modifying()
        {
            assert(conv_depth >= 0);
            ++conv_depth;
        }

        void out_of_modifying()
        {
            assert(conv_depth > 0);
            --conv_depth;
        }

        bool is_to_be_modified()
        {
            assert(conv_depth >= 0);
            return conv_depth != 0;
        }



        std::string stack2path()
        {
            std::string ret;
            for (auto e : m_struct_stack)
            {
                switch (e.type)
                {
                case stack_entry::EntryType::Struct:
                case stack_entry::EntryType::Array:
                    if (ret.size())
                        ret += ".";
                    ret += e.name;
                    break;
                case stack_entry::EntryType::ArrayElem:
                    ret += fmt::format("[{}]", e.idx);
                    break;
                case stack_entry::EntryType::Undefined:
                default:
                    assert(false);
                }
            }
            return ret;
        }
    };

    struct DefaultEqualityChecker
    {
        template<class T>
        static bool is_equal(const T& t1, const T& t2)
        {
            if constexpr (sixit::dmath::fp_traits<T>::is_valid_fp)
                return approximate_eq(t1, t2);
            else
                return t1 == t2;
        }
    };

    template <class BufferT, class EqualityCheckerT = DefaultEqualityChecker, class ErrorHandler = sixit::rw::comparsers::DefaultErrorHandler, class DataT = void>
    SIXIT_LWA_OPTIONAL_REQUIRES2(sixit::rw::memory_buffers::read_memory_buffer_concept, BufferT,
        sixit::rw::comparsers::error_handler, ErrorHandler)
        class stream_to_struct_comparing_parser_impl : public stream_to_struct_comparing_comparser_impl_base,
        public sixit::rw::comparsers::ParserBase<DataT>
    {
    private:
        int inequality_ctr = 0;

    public:
        explicit stream_to_struct_comparing_parser_impl(BufferT& buffer, ErrorHandler err_handler = ErrorHandler())
            : m_buffer(buffer), m_error_handler(err_handler), m_chunk(buffer.initial_buffer()), m_marker(m_chunk.first)
        {
        }

        template<class T>
        void check_is_equal(const T& t1, const T& t2, std::string name = "")
        {
            if (!EqualityCheckerT::is_equal(t1, t2))
            {
                ++inequality_ctr;
                if (name.empty())
                    fmt::print("equality does not stand for: {}\n", stack2path());
                else
                    fmt::print("equality does not stand for: {}\n", stack2path() + "." + name);
            }
        }

        template <typename WireType, auto ptr2member, sixit::lwa::string_literal_helper name_, typename QualifierT>
        void implRWNameValue(QualifierT q)
        {
            static_assert(std::is_member_object_pointer_v<decltype(ptr2member)>);
            using classT = typename ::sixit::rw::impl::pointer_to_member_details<ptr2member>::class_type;
            using memberT = typename ::sixit::rw::impl::pointer_to_member_details<ptr2member>::member_type;
            using wire_type_to_use =
                std::conditional_t<std::is_same_v<WireType, ::sixit::rw::default_wire_type>,
                typename ::sixit::rw::impl::wire_type_from_value_type<memberT>::type, WireType>;
            static_assert(std::is_same_v<WireType, wire_type_to_use>);
            memberT& memptr = (reinterpret_cast<classT*>(m_struct_stack.back().ptr))->*ptr2member;

            using CleanValueT = std::remove_cvref_t<memberT>;
            if constexpr (sixit::rw::member_type_has_alias<CleanValueT>)
            {
                typename sixit::rw::member_type_aliaser<CleanValueT>::type tmp;
                rw_impl<WireType, name_>(tmp, q);
                memptr = sixit::rw::member_type_aliaser<CleanValueT>::alias2value(tmp);
            }
            else
            {
                rw_impl<wire_type_to_use, name_>(memptr, q);
            }
        }

        /*template <typename WireType, typename ValueT, typename QualifierT>
        void implRWNameValue([[maybe_unused]] const std::string_view& name, ValueT& val, QualifierT q)
        {
            using CleanValueT = std::remove_cvref_t<ValueT>;
            if constexpr (sixit::rw::member_type_has_alias<CleanValueT>)
            {
                typename sixit::rw::member_type_aliaser<CleanValueT>::type tmp;
                rw_impl<WireType>(tmp, q);
                val = sixit::rw::member_type_aliaser<CleanValueT>::alias2value(tmp);
            }
            else
            {
                rw_impl<WireType>(val, q);
            }
        }

        template <typename WireType, typename ValueT, typename QualifierT>
        void implRWNameValue([[maybe_unused]] const std::string_view& name, ValueT* val, QualifierT q)
        {
            using CleanValueT = std::remove_cvref_t<ValueT>;
            if constexpr (sixit::rw::member_type_has_alias<CleanValueT>)
            {
                typename sixit::rw::member_type_aliaser<CleanValueT>::type tmp;
                rw_impl<WireType>(tmp, q);
                *val = sixit::rw::member_type_aliaser<CleanValueT>::alias2value(tmp);
            }
            else
            {
                rw_impl<WireType>(*val, q);
            }
        }*/

    private:
        template <typename Type>
        void prepare_marker()
        {
            m_marker = align_ptr<Type>(m_marker);

            if (m_marker + sizeof(Type) > m_chunk.second)
            {
                m_chunk = m_buffer.more_buffer(m_marker);
                m_marker = align_ptr<Type>(m_chunk.first); // we shouldn't assume the returned chunk to be aligned.
            }
        }

        template <typename WireType, sixit::lwa::string_literal_helper name, typename ValueT, typename QualifierT = sixit::rw::qualifiers::no_qualifier_placeholder>
        void rw_impl(ValueT& val, QualifierT q = QualifierT())
        {
            if constexpr (sixit::rw::qualifiers::is_with_filter<QualifierT>)
            {
                if (q.filtered_out())
                {
                    return;
                }
            }

            if constexpr (sixit::rw::qualifiers::is_with_validating_converter<QualifierT>)
            {
                auto temporary = construct_object_of_provided_converted_type(q);
                in_modifying();
                rw_value<WireType, name>(temporary, q);
                out_of_modifying();
                bool status{ false };
                if (is_to_be_modified())
                {
                    std::tie(status, val) = q.value2member(temporary);
                }
                else
                {
                    ValueT val_new;
                    std::tie(status, val_new) = q.value2member(temporary);
                    check_is_equal(val, val_new);
                }

                if (!status)
                {
                    constexpr static const char* errmsg = "Conversion failed";
                    m_error_handler.handleDataError(sixit::rw::comparsers::ComparserErrorCode::PARSED_DATA_CONVERTION_FAILED, errmsg);
                }
            }
            else
            {
                rw_value<WireType, name>(val, q); // checking for equality will be done there
            }
        }

        /*void rw_raw_data(void* data, size_t bytes)
        {
            char* data_marker = reinterpret_cast<char*>(data);
            size_t chunk_space = m_chunk.second - m_marker;
            size_t bytes_remaining = bytes;

            while (bytes_remaining > chunk_space)
            {
                memcpy(data_marker, m_marker, chunk_space);
                bytes_remaining -= chunk_space;
                data_marker += chunk_space;
                m_chunk = m_buffer.more_buffer(m_chunk.second);
                m_marker = m_chunk.first;
                chunk_space = m_chunk.second - m_chunk.first;
            }
            memcpy(data_marker, m_marker, bytes_remaining);
            m_marker += bytes_remaining;
        }*/

        template <typename WireType, sixit::lwa::string_literal_helper name, typename ValueT>
        void rw_scalar(ValueT& val)
        {
            if constexpr (std::is_same_v<WireType, sixit::rw::STRUCT>)
            {
                sixit::rw::comparsers::rw_caller::call(*this, val);
            }
            /*else if constexpr (is_AofBITS<WireType>)
            {
                static_assert(WireType::count == ValueT::bits_count);

                using ChunkType = std::remove_const_t<std::remove_pointer_t<decltype(val.data())>>;
                const size_t chunks = (ValueT::bits_count - 1) / (sizeof(ChunkType) * 8) + 1;
                const size_t bytes = chunks * sizeof(ChunkType);

                prepare_marker<ChunkType>();
                rw_raw_data(val.data(), bytes);
            }
            else if constexpr (std::is_same_v<VofBITS, WireType>)
            {
                uint64_t bits{0};
                rw_scalar<sixit::rw::U64>(bits);
                val.resize(bits);

                if (bits > 0)
                {
                    using ChunkType = std::remove_const_t<std::remove_pointer_t<decltype(val.data())>>;
                    const size_t chunks = (bits - 1) / (sizeof(ChunkType) * 8) + 1;
                    const size_t bytes = chunks * sizeof(ChunkType);

                    prepare_marker<ChunkType>();
                    rw_raw_data(val.data(), bytes);
                }
            }*/
            else // simple type
            {
                using T = std::conditional_t<std::is_same_v<WireType, sixit::rw::ENUM>, ValueT, typename WireType::Type>;
                prepare_marker<T>();

                if (is_to_be_modified())
                    val = static_cast<ValueT>(*reinterpret_cast<const T*>(m_marker));
                else
                    check_is_equal(static_cast<ValueT>(*reinterpret_cast<const T*>(m_marker)), val, (const char*)(name));

                m_marker += sizeof(T);
            }
        }

        template <typename WireType, sixit::lwa::string_literal_helper name, typename VectorLikeObjectT, typename QualifierT>
        void rw_vector(VectorLikeObjectT& vlo, QualifierT q)
        {
            m_struct_stack.push_back({ stack_entry::EntryType::Array, uintptr_t(0), (const char*)(name) });

            uint64_t size{ 0 };
            in_modifying();
            rw_scalar<sixit::rw::U64, "size">(size);
            out_of_modifying();

            if (is_to_be_modified())
                vlo.reserve(size);
            else
                check_is_equal(vlo.size(), size);

            if constexpr (sixit::rw::qualifiers::is_with_element_validating_converter<QualifierT>)
            {
                for (auto i = SIXIT_LWA_Z(0); i < sixit::guidelines::narrow_cast<std::ptrdiff_t>(size); i++)
                {
                    auto temporary = construct_object_of_provided_converted_type(q);
                    in_modifying();
                    rw_value<typename WireType::ValueT, "">(temporary);
                    out_of_modifying();

                    if (is_to_be_modified())
                    {
                        auto [status, value] = q.value2element(temporary);

                        if (status)
                        {
                            vlo.push_back(value);
                        }
                        else
                        {
                            constexpr static const char* errmsg = "Element conversion failed";
                            m_error_handler.handleDataError(sixit::rw::comparsers::ComparserErrorCode::PARSED_DATA_CONVERTION_FAILED, errmsg);
                        }
                    }
                    else
                    {
                        auto [status, value] = q.value2element(temporary);

                        if (status)
                        {
                            check_is_equal(vlo[i], value);
                        }
                        else
                        {
                            constexpr static const char* errmsg = "Element conversion failed";
                            m_error_handler.handleDataError(sixit::rw::comparsers::ComparserErrorCode::PARSED_DATA_CONVERTION_FAILED, errmsg);
                        }
                    }
                }
            }
            else
            {
                using ObjT = typename VectorLikeObjectT::value_type;
                auto construct_object = [] {
                    if constexpr (std::is_constructible_v<ObjT, sixit::rw::constructor_for_rw_tag>)
                        return ObjT(sixit::rw::constructor_for_rw_tag());
                    else
                    {
                        static_assert(std::is_default_constructible_v<ObjT>);
                        return ObjT();
                    }
                    };

                for (auto i = SIXIT_LWA_Z(0); i < sixit::guidelines::narrow_cast<std::ptrdiff_t>(size); i++)
                {
                    ObjT value = construct_object();
                    in_modifying();
                    rw_value<typename WireType::ValueT, "">(value);
                    out_of_modifying();
                    if (is_to_be_modified())
                        vlo.push_back(std::move(value));
                    else
                    {
                        m_struct_stack.push_back({ stack_entry::EntryType::ArrayElem, uintptr_t(0), "", i });
                        check_is_equal(vlo[i], value);
                        m_struct_stack.pop_back();
                    }
                }
            }
            m_struct_stack.pop_back();
        }

        template <typename WireType, sixit::lwa::string_literal_helper name, typename ValueT, typename QualifierT = sixit::rw::qualifiers::no_qualifier_placeholder>
        void rw_value(ValueT& val, QualifierT q = QualifierT())
        {
            if constexpr (WireType::isScalar)
            {
                if constexpr (std::is_same_v<WireType, sixit::rw::STRING>)
                {
                    rw_vector<sixit::rw::VofI8, name>(val, q);
                }
                else
                {
                    rw_scalar<WireType, name>(val);
                }
            }
            else
            {
                rw_vector<WireType, name>(val, q);
            }
        }

    private:
        BufferT& m_buffer;
        ErrorHandler m_error_handler;
        std::pair<const char*, const char*> m_chunk{ nullptr, nullptr };
        const char* m_marker{ nullptr };
    };


    template <class BufferT, class EqualityCheckerT = DefaultEqualityChecker, class ErrorHandler = sixit::rw::comparsers::DefaultErrorHandler, class DataT = void>
    SIXIT_LWA_OPTIONAL_REQUIRES2(sixit::rw::memory_buffers::read_memory_buffer_concept, BufferT,
        sixit::rw::comparsers::error_handler, ErrorHandler)
        using stream_to_struct_comparing_parser = stream_to_struct_comparing_parser_impl<BufferT, EqualityCheckerT, ErrorHandler, DataT>;

} // namespace sixit::dmath::test_helpers
#endif // 0


    ////////////////////////////////////////////////////////////////////////////////////////////////////////



namespace sixit::dmath::test_helpers
{
#if 0 // [DI] this code is yet underdeveloped and is left for future reworking, see ticket TR-<pending>
    template<class TestDataT>
    class TestDataProcessor
    {
        //		TestDataT data;
        using BufferT = sixit::rw::memory_buffers::debug_memory_buffer<>;

        BufferT test_data_backup_buffer;

    public:
        TestDataProcessor() {}
        bool init(std::string path)
        {
            using namespace sixit::rw::comparsers;
            using namespace sixit::rw::memory_buffers;
            try
            {
                TestDataT data;
                data = TestDataT::load(path);
                auto composer = cpu_specific_composer<BufferT, std::false_type::value>(test_data_backup_buffer);
                //rw_caller::call(composer, data);
                TestDataT::read_write(data, composer);
            }
            catch (const std::exception& e)
            {
                fmt::print("load failed\n");
                std::cout << e.what();
                return false;
            }
            catch (...)
            {
                return false;
            }
            return true;
        }

        void restore_test_data(TestDataT& data)
        {
            using namespace sixit::rw::comparsers;
            using namespace sixit::rw::memory_buffers;
            auto parser = cpu_specific_parser<BufferT, std::false_type::value>(test_data_backup_buffer);
            //rw_caller::call(parser, data);
            TestDataT::read_write(data, parser);
        }

        void compare_test_data(TestDataT& data)
        {
            auto parser = sixit::dmath::test_helpers::stream_to_struct_comparing_parser<BufferT>(test_data_backup_buffer);
            //sixit::rw::comparsers::rw_caller::call(parser, data);
            TestDataT::read_write(data, parser);
        }

        template<class L>
        void calculate(std::string name, L&& l)
        {
            //fmt::print("running test {} ...\n", name);
            TestDataT data;
            restore_test_data(data);
            sixit::test::test(name, [&]() {
                l(data); // calculate
                compare_test_data(data);
                });
            //fmt::print("running test {} done\n", name);
        }
    };
#else

    template<class TestDataT>
    auto load_test_data(std::string path)
    {
        sixit::gpal::bundled_asset_rw_stream stream_reader(path);
        sixit::rw::streams::ICharacterAdapterStreamFromStream8LE<sixit::gpal::bundled_asset_rw_stream> char_stream(stream_reader);
        sixit::rw::comparsers::json_parser<decltype(char_stream)> parser2(char_stream);
        return sixit::rw::make_from_comparser<TestDataT>(parser2);
    }

    template<class TestDataT>
    void save_test_data(const TestDataT& data, std::string path)
    {
        sixit::rw::streams::OFileStream8LE obj_file("intersection_test_data.json");
        sixit::rw::streams::OCharacterAdapterStreamFromStream8LE char_stream(obj_file);
        sixit::rw::comparsers::json_composer<decltype(char_stream), false> writer(char_stream);
        TestDataT::read_write(data, writer);
        char_stream.end();
    }

    template<class FP, class FunctorT, class TestDataT>
    bool run_test_for_one_type(std::string path);

    template<class fp, class TestDataT>
    class TestDataProcessor
    {
        template<class FP, class FunctorT, class TestDataT1>
        friend bool run_test_for_one_type(std::string path);
        TestDataT data;
        int test_counter = 0;
        int test_with_ineq_counter = 0;

        TestDataProcessor() {}
        bool init(std::string path)
        {
            try
            {
                data = load_test_data<TestDataT>(path);
            }
            catch (const std::exception& e)
            {
                fmt::print("load failed: {}\n", e.what());
                return false;
            }
            catch (...)
            {
                return false;
            }
            return true;
        }

    public:
        template<sixit::lwa::string_literal_helper name, class L>
        void calculate(L&& l)
        {
            constexpr auto fullName = name + ", fp: " + fp_traits<fp>::display_name;
            inexact_eq_counter::inexact_eq_ctr() = 0;
            {
                fmt::print("sixit-performance:test: {}\n", (const char*)(fullName));
                //sixit::profile::probe<"sixit-performance:test", 0, sixit::profile::usage::profiling> prof; // create profiler
                sixit::profile::probe<fullName, 1, sixit::profile::usage::profiling> prof; // create profiler
                sixit::test::test((const char*)(fullName), [&]() {
                    l(data); // calculate() with PRESUMEs is called inside lambda
                    });
            }
            //sixit::profile::printer::print_all();
            sixit::profile::printer::print_probe((const char*)(fullName), sixit::loggers::clog(), "sixit-performance: ");
            ++test_counter;
            if (inexact_eq_counter::inexact_eq_ctr())
                ++test_with_ineq_counter;
        }
    };
#endif // 0

    template<class fp, class FunctorT>
    bool run_test_for_one_type();

    template<class fp>
    class NoTestDataProcessor
    {
        template<class FP, class FunctorT>
        friend bool run_test_for_one_type();
        int test_counter = 0;
        int test_with_ineq_counter = 0;

        NoTestDataProcessor() {}

    public:
        template<sixit::lwa::string_literal_helper name, class L>
        void calculate(L&& l)
        {
            constexpr auto fullName = name + ", fp: " + fp_traits<fp>::display_name;
            inexact_eq_counter::inexact_eq_ctr() = 0;
            {
                fmt::print("sixit-performance:test: {}\n", (const char*)(fullName));
                sixit::profile::probe<fullName, 1, sixit::profile::usage::profiling> prof; // create profiler
                sixit::test::test((const char*)(fullName), [&]() {l(); /* calculate() with PRESUMEs is called inside lambda*/});
            }
            //sixit::profile::printer::print_all();
            sixit::profile::printer::print_probe((const char*)(fullName), sixit::loggers::clog(), "sixit-performance: ");
            ++test_counter;
            if (inexact_eq_counter::inexact_eq_ctr())
                ++test_with_ineq_counter;
        }
    };

    template<class fp, class FunctorT, class TestDataT>
    bool run_test_for_one_type(std::string path)
    {
        TestDataProcessor<fp, TestDataT> bt;
        if (!bt.init(path))
            return false;
        bool ret = FunctorT::run(bt);
        static_assert(fp_traits<fp>::is_valid_fp);
        if constexpr (!fp_traits<fp>::is_deterministic)
        {
            fmt::print("sixit-fp-exactness: fp={}, {} test with no exact equality out of {} tests\n", sixit::guidelines::type_display_name<fp>(), bt.test_with_ineq_counter, bt.test_counter);
        }
        return ret;
    }

    template<class fp, class FunctorT>
    bool run_test_for_one_type()
    {
        NoTestDataProcessor<fp> nbt;
        bool ret = FunctorT::run(nbt);
        static_assert(fp_traits<fp>::is_valid_fp);
        if constexpr (!fp_traits<fp>::is_deterministic)
        {
            fmt::print("sixit-fp-exactness: fp={}, {} test with no exact equality out of {} tests\n", sixit::guidelines::type_display_name<fp>(), nbt.test_with_ineq_counter, nbt.test_counter);
        }
        return ret;
    }

    template <typename fp>
    struct no_storable_test_data {};

    template< template<typename Ty> class FunctorT, template<typename Ty> class TestDataT>
    int run_test_for_all_types(std::string path_to_data)
    {
        fmt::print("test set begin\n");
        report_fp_related_flags();

        bool ok = true;

        ok = run_test_for_one_type<float, FunctorT<float>, TestDataT<float>>(path_to_data) && ok;

        if constexpr (sixit::dmath::fp_traits<sixit::dmath::ieee_float_static_lib>::is_supported)
            ok = run_test_for_one_type<sixit::dmath::ieee_float_static_lib, FunctorT<sixit::dmath::ieee_float_static_lib>, TestDataT<sixit::dmath::ieee_float_static_lib>>(path_to_data) && ok;

        ok = run_test_for_one_type<sixit::dmath::ieee_float_soft, FunctorT<sixit::dmath::ieee_float_soft>, TestDataT<sixit::dmath::ieee_float_soft>>(path_to_data) && ok;

        if constexpr (sixit::dmath::fp_traits<sixit::dmath::ieee_float_if_strict_fp>::is_supported)
            ok = run_test_for_one_type<sixit::dmath::ieee_float_if_strict_fp, FunctorT<sixit::dmath::ieee_float_if_strict_fp>, TestDataT<sixit::dmath::ieee_float_if_strict_fp>>(path_to_data) && ok;

        if constexpr (sixit::dmath::fp_traits<sixit::dmath::ieee_float_if_semicolon_prohibits_reordering>::is_supported)
            ok = run_test_for_one_type<ieee_float_if_semicolon_prohibits_reordering, FunctorT<ieee_float_if_semicolon_prohibits_reordering>, TestDataT<ieee_float_if_semicolon_prohibits_reordering>>(path_to_data) && ok;

        if constexpr (sixit::dmath::fp_traits<sixit::dmath::ieee_float_inline_asm>::is_supported)
            ok = run_test_for_one_type<sixit::dmath::ieee_float_inline_asm, FunctorT<sixit::dmath::ieee_float_inline_asm>, TestDataT<sixit::dmath::ieee_float_inline_asm>>(path_to_data) && ok;

        if constexpr (sixit::dmath::fp_traits<sixit::dmath::ieee_float_shared_lib>::is_supported)
            ok = run_test_for_one_type<sixit::dmath::ieee_float_shared_lib, FunctorT<sixit::dmath::ieee_float_shared_lib>, TestDataT<sixit::dmath::ieee_float_shared_lib>>(path_to_data) && ok;

        // if constexpr (sixit::dmath::fp_traits<sixit::dmath::float_with_sixit>::is_supported)
        //     ok = run_test_for_one_type<sixit::dmath::float_with_sixit, FunctorT<sixit::dmath::float_with_sixit>, TestDataT<sixit::dmath::float_with_sixit>>(path_to_data) && ok;

        fmt::print("test set end\n\n");
        return ok;
    }

    template< template<typename Ty> class FunctorT>
    int run_test_for_all_types()
    {
        fmt::print("test set begin\n");
        report_fp_related_flags();

        bool ok = true;

        ok = run_test_for_one_type<float, FunctorT<float>>() && ok;
        if constexpr (sixit::dmath::fp_traits<sixit::dmath::ieee_float_static_lib>::is_supported)
            ok = run_test_for_one_type<sixit::dmath::ieee_float_static_lib, FunctorT<sixit::dmath::ieee_float_static_lib>>() && ok;
        ok = run_test_for_one_type<sixit::dmath::ieee_float_soft, FunctorT<sixit::dmath::ieee_float_soft>>() && ok;

        if constexpr (sixit::dmath::fp_traits<sixit::dmath::ieee_float_if_strict_fp>::is_supported)
            ok = run_test_for_one_type<sixit::dmath::ieee_float_if_strict_fp, FunctorT<sixit::dmath::ieee_float_if_strict_fp>>() && ok;

        if constexpr (sixit::dmath::fp_traits<sixit::dmath::ieee_float_if_semicolon_prohibits_reordering>::is_supported)
            ok = run_test_for_one_type<ieee_float_if_semicolon_prohibits_reordering, FunctorT<ieee_float_if_semicolon_prohibits_reordering>>() && ok;

        if constexpr (sixit::dmath::fp_traits<sixit::dmath::ieee_float_inline_asm>::is_supported)
            ok = run_test_for_one_type<sixit::dmath::ieee_float_inline_asm, FunctorT<sixit::dmath::ieee_float_inline_asm>>() && ok;

        if constexpr (sixit::dmath::fp_traits<sixit::dmath::ieee_float_shared_lib>::is_supported)
            ok = run_test_for_one_type<sixit::dmath::ieee_float_shared_lib, FunctorT<sixit::dmath::ieee_float_shared_lib>>() && ok;

        // if constexpr (sixit::dmath::fp_traits<sixit::dmath::float_with_sixit>::is_supported)
        //     ok = run_test_for_one_type<sixit::dmath::float_with_sixit, FunctorT<sixit::dmath::float_with_sixit>>() && ok;

        fmt::print("test set end\n\n");
        return ok;
    }

} // sixit::dmath::test_helpers

#endif //sixit_dmath_test_helpers_h_included

/*
The 3-Clause BSD License

Copyright (C) 2023-2024 Six Impossible Things Before Breakfast Limited.

Contributors: Sherry Ignatchenko, Dmytro Ivanchykhin, Victor Istomin

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
