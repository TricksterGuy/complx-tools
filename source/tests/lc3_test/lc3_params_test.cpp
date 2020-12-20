#include <lc3.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <istream>
#include <fstream>
#include <vector>

BOOST_AUTO_TEST_CASE(TestPluginParamsBool)
{
    PluginParams params("foo", "/usr/lib/libfoo.so", {{"bool", "1"}});

    BOOST_CHECK(params.has_key("bool"));
    BOOST_CHECK(!params.has_key("boolean"));

    BOOST_CHECK_EQUAL(params.get_value("bool"), "1");
    BOOST_CHECK_EQUAL(params.get_value("boolean"), "");

    bool var;
    BOOST_CHECK(params.read_bool("bool", var));
    BOOST_CHECK(var);

    BOOST_CHECK(!params.read_bool("boolean", var));

    BOOST_CHECK(params.read_bool_required("bool"));
}

BOOST_AUTO_TEST_CASE(TestPluginParamsBoolValues)
{
    PluginParams params("foo", "/usr/lib/libfoo.so",
                        {{"boola1", "true"}, {"boola2", "1"}, {"boola3", "yes"}, {"boola4", "on"},
                         {"boolb1", "false"}, {"boolb2", "0"}, {"boolb3", "no"}, {"boolb4", "off"}});

    BOOST_CHECK(params.read_bool_required("boola1"));
    BOOST_CHECK(params.read_bool_required("boola2"));
    BOOST_CHECK(params.read_bool_required("boola3"));
    BOOST_CHECK(params.read_bool_required("boola4"));
    BOOST_CHECK(!params.read_bool_required("boolb1"));
    BOOST_CHECK(!params.read_bool_required("boolb2"));
    BOOST_CHECK(!params.read_bool_required("boolb3"));
    BOOST_CHECK(!params.read_bool_required("boolb4"));
}

BOOST_AUTO_TEST_CASE(TestPluginParamsChar)
{
    PluginParams params("foo", "/usr/lib/libfoo.so", {{"char", "123"}, {"uchar", "126"}, {"nchar", "-46"}, {"nuchar", "-46"},
                                                      {"ichar1", "129"}, {"ichar2", "-130"}, {"iuchar", "257"}});

    char var;
    BOOST_CHECK(params.read_char("char", var));
    BOOST_CHECK_EQUAL(var, 123);
    BOOST_CHECK_EQUAL(params.read_char_required("char"), 123);

    BOOST_CHECK(params.read_char("nchar", var));
    BOOST_CHECK_EQUAL(var, -46);
    BOOST_CHECK_EQUAL(params.read_char_required("nchar"), -46);

    unsigned char uvar;
    BOOST_CHECK(params.read_uchar("uchar", uvar));
    BOOST_CHECK_EQUAL(uvar, 126);
    BOOST_CHECK_EQUAL(params.read_uchar_required("uchar"), 126);
}

BOOST_AUTO_TEST_CASE(TestPluginParamsShort)
{
    PluginParams params("foo", "/usr/lib/libfoo.so", {{"short", "513"}, {"ushort", "65534"}, {"nshort", "-8192"}, {"nushort", "-257"},
                                                      {"ishort1", "33000"}, {"ishort2", "-40000"}, {"iushort", "80000"}});

    short var;
    BOOST_CHECK(params.read_short("short", var));
    BOOST_CHECK_EQUAL(var, 513);
    BOOST_CHECK_EQUAL(params.read_short_required("short"), 513);

    BOOST_CHECK(params.read_short("nshort", var));
    BOOST_CHECK_EQUAL(var, -8192);
    BOOST_CHECK_EQUAL(params.read_short_required("nshort"), -8192);

    unsigned short uvar;
    BOOST_CHECK(params.read_ushort("ushort", uvar));
    BOOST_CHECK_EQUAL(uvar, 65534);
    BOOST_CHECK_EQUAL(params.read_ushort_required("ushort"), 65534);
}

BOOST_AUTO_TEST_CASE(TestPluginParamsInt)
{
    PluginParams params("foo", "/usr/lib/libfoo.so", {{"int", "200000"}, {"uint", "2100000"}, {"nint", "-132072"}, {"nuint", "-333333"},
                                                      {"iint1", "2147483648"}, {"iint2", "-2300000000"}, {"iuint", "5000000000"}});

    int var;
    BOOST_CHECK(params.read_int("int", var));
    BOOST_CHECK_EQUAL(var, 200000);
    BOOST_CHECK_EQUAL(params.read_int_required("int"), 200000);

    BOOST_CHECK(params.read_int("nint", var));
    BOOST_CHECK_EQUAL(var, -132072);
    BOOST_CHECK_EQUAL(params.read_int_required("nint"), -132072);

    unsigned int uvar;
    BOOST_CHECK(params.read_uint("uint", uvar));
    BOOST_CHECK_EQUAL(uvar, 2100000);
    BOOST_CHECK_EQUAL(params.read_uint_required("uint"), 2100000);
}

BOOST_AUTO_TEST_CASE(TestPluginParamsLong)
{
    PluginParams params("foo", "/usr/lib/libfoo.so", {{"long", "4294967296"}, {"ulong", "5000000000"}, {"nlong", "-4294967296"}, {"nulong", "-5000000000"},
                                                      {"ilong1", "9223372036854775808"}, {"ilong2", "-9223372036854775809"}, {"iulong", "18446744073709551617"}});

    /*long var;
    BOOST_CHECK(params.read_long("long", var));
    BOOST_CHECK_EQUAL(var, 4294967296);
    BOOST_CHECK_EQUAL(params.read_long_required("long"), 4294967296);

    BOOST_CHECK(params.read_long("nlong", var));
    BOOST_CHECK_EQUAL(var, -4294967296);
    BOOST_CHECK_EQUAL(params.read_long_required("nlong"), -4294967296);

    unsigned long uvar;
    BOOST_CHECK(params.read_ulong("ulong", uvar));
    BOOST_CHECK_EQUAL(uvar, 5000000000);
    BOOST_CHECK_EQUAL(params.read_ulong_required("ulong"), 5000000000);*/
}

BOOST_AUTO_TEST_CASE(TestPluginParamsHex)
{
    PluginParams params("foo", "/usr/lib/libfoo.so", {{"hex1", "xFFFF"}, {"hex2", "x1234"},});

    int var;
    BOOST_CHECK(params.read_int("hex1", var));
    BOOST_CHECK_EQUAL(var, 0xFFFF);
    BOOST_CHECK_EQUAL(params.read_int_required("hex1"), 0xFFFF);

    BOOST_CHECK(params.read_int("hex2", var));
    BOOST_CHECK_EQUAL(var, 0x1234);
    BOOST_CHECK_EQUAL(params.read_int_required("hex2"), 0x1234);

    unsigned short uvar;
    BOOST_CHECK(params.read_ushort("hex1", uvar));
    BOOST_CHECK_EQUAL(uvar, 0xFFFFU);
    BOOST_CHECK_EQUAL(params.read_ushort_required("hex1"), 0xFFFFU);

    BOOST_CHECK(params.read_ushort("hex2", uvar));
    BOOST_CHECK_EQUAL(uvar, 0x1234);
    BOOST_CHECK_EQUAL(params.read_ushort_required("hex2"), 0x1234);
}

BOOST_AUTO_TEST_CASE(TestPluginParamsBoolInvalid)
{
    PluginParams params("foo", "/usr/lib/libfoo.so",
                        {{"boola1", "affirmative"}, {"boola2", "y"}, {"boola3", "2"}, {"boola4", "on-baby"},
                         {"boolb1", "nope"}, {"boolb2", "00"}, {"boolb3", "heckno"}, {"boolb4", "off-baby"}});

    bool var;
    BOOST_CHECK_THROW(params.read_bool("boola1", var), LC3PluginException);
    BOOST_CHECK_THROW(params.read_bool("boola2", var), LC3PluginException);
    BOOST_CHECK_THROW(params.read_bool("boola3", var), LC3PluginException);
    BOOST_CHECK_THROW(params.read_bool("boola4", var), LC3PluginException);
    BOOST_CHECK_THROW(params.read_bool("boolb1", var), LC3PluginException);
    BOOST_CHECK_THROW(params.read_bool("boolb2", var), LC3PluginException);
    BOOST_CHECK_THROW(params.read_bool("boolb3", var), LC3PluginException);
    BOOST_CHECK_THROW(params.read_bool("boolb4", var), LC3PluginException);
    BOOST_CHECK_THROW(params.read_bool_required("nobool"), LC3PluginException);
    BOOST_CHECK_THROW(params.read_bool_required("boola1"), LC3PluginException);
}

BOOST_AUTO_TEST_CASE(TestPluginParamsInvalidChar)
{
    PluginParams params("foo", "/usr/lib/libfoo.so", {{"char", "123"}, {"uchar", "126"}, {"nchar", "-46"}, {"nuchar", "-46"},
                                                      {"ichar1", "129"}, {"ichar2", "-130"}, {"iuchar", "257"}});

    char var;
    unsigned char uvar;
    BOOST_CHECK_THROW(params.read_char("ichar1", var), LC3PluginException);
    BOOST_CHECK_THROW(params.read_char_required("ichar2"), LC3PluginException);
    BOOST_CHECK_THROW(params.read_uchar("iuchar", uvar), LC3PluginException);
    BOOST_CHECK_THROW(params.read_uchar_required("iuchar"), LC3PluginException);
    BOOST_CHECK_THROW(params.read_uchar_required("nuchar"), LC3PluginException);
}

BOOST_AUTO_TEST_CASE(TestPluginParamsInvalidShort)
{
    PluginParams params("foo", "/usr/lib/libfoo.so", {{"short", "513"}, {"ushort", "65534"}, {"nshort", "-8192"}, {"nushort", "-257"},
                                                      {"ishort1", "33000"}, {"ishort2", "-40000"}, {"iushort", "80000"}});

    short var;
    unsigned short uvar;
    BOOST_CHECK_THROW(params.read_short("ishort1", var), LC3PluginException);
    BOOST_CHECK_THROW(params.read_short_required("ishort2"), LC3PluginException);
    BOOST_CHECK_THROW(params.read_ushort("iushort", uvar), LC3PluginException);
    BOOST_CHECK_THROW(params.read_ushort_required("iushort"), LC3PluginException);
    BOOST_CHECK_THROW(params.read_ushort_required("nushort"), LC3PluginException);
}

BOOST_AUTO_TEST_CASE(TestPluginParamsInvalidInt)
{
    PluginParams params("foo", "/usr/lib/libfoo.so", {{"int", "200000"}, {"uint", "2100000"}, {"nint", "-132072"}, {"nuint", "-333333"},
                                                      {"iint1", "2147483648"}, {"iint2", "-2300000000"}, {"iuint", "5000000000"}});

    int var;
    unsigned int uvar;
    BOOST_CHECK_THROW(params.read_int("iint1", var), LC3PluginException);
    BOOST_CHECK_THROW(params.read_int_required("iint2"), LC3PluginException);
    BOOST_CHECK_THROW(params.read_uint("iuint", uvar), LC3PluginException);
    BOOST_CHECK_THROW(params.read_uint_required("iuint"), LC3PluginException);
    //BOOST_CHECK_THROW(params.read_uint_required("nuint"), LC3PluginException);
}

BOOST_AUTO_TEST_CASE(TestPluginParamsInvalidLong)
{
    PluginParams params("foo", "/usr/lib/libfoo.so", {{"long", "4294967296"}, {"ulong", "5000000000"}, {"nlong", "-4294967296"}, {"nulong", "-5000000000"},
                                                      {"ilong1", "9223372036854775808"}, {"ilong2", "-9223372036854775809"}, {"iulong", "18446744073709551617"}});

    long var;
    unsigned long uvar;
    BOOST_CHECK_THROW(params.read_long("ilong1", var), LC3PluginException);
    BOOST_CHECK_THROW(params.read_long_required("ilong2"), LC3PluginException);
    BOOST_CHECK_THROW(params.read_ulong("iulong", uvar), LC3PluginException);
    BOOST_CHECK_THROW(params.read_ulong_required("iulong"), LC3PluginException);
    BOOST_CHECK_THROW(params.read_ulong_required("nulong"), LC3PluginException);
}
