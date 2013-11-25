// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#include <json/config.h>
#include <json/json.h>
#include "jsontest.h"

// Make numeric limits more convenient to talk about.
// Assumes int type in 32 bits.
#define kint32max Json::Value::maxInt
#define kint32min Json::Value::minInt
#define kuint32max Json::Value::maxUInt
#define kint64max Json::Value::maxInt64
#define kint64min Json::Value::minInt64
#define kuint64max Json::Value::maxUInt64

static const double kdint64max = double(kint64max);
static const float kfint64max = float(kint64max);
static const float kfint32max = float(kint32max);
static const float kfuint32max = float(kuint32max);


// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// Json Library test cases
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
static inline double uint64ToDouble( Json::UInt64 value )
{
    return static_cast<double>( value );
}
#else // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
static inline double uint64ToDouble( Json::UInt64 value )
{
    return static_cast<double>( Json::Int64(value/2) ) * 2.0 + Json::Int64(value & 1);
}
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)

struct ValueTest : JsonTest::TestCase
{
   Json::Value null_;
   Json::Value emptyArray_;
   Json::Value emptyObject_;
   Json::Value integer_;
   Json::Value unsignedInteger_;
   Json::Value smallUnsignedInteger_;
   Json::Value real_;
   Json::Value float_;
   Json::Value array1_;
   Json::Value object1_;
   Json::Value emptyString_;
   Json::Value string1_;
   Json::Value string_;
   Json::Value true_;
   Json::Value false_;


   ValueTest()
      : emptyArray_( Json::arrayValue )
      , emptyObject_( Json::objectValue )
      , integer_( 123456789 )
      , smallUnsignedInteger_( Json::Value::UInt( Json::Value::maxInt ) )
      , unsignedInteger_( 34567890u )
      , real_( 1234.56789 )
      , float_( 0.00390625f )
      , emptyString_( "" )
      , string1_( "a" )
      , string_( "sometext with space" )
      , true_( true )
      , false_( false )
   {
      array1_.append( 1234 );
      object1_["id"] = 1234;
   }

   struct IsCheck
   {
      /// Initialize all checks to \c false by default.
      IsCheck();

      bool isObject_;
      bool isArray_;
      bool isBool_;
      bool isString_;
      bool isNull_;

      bool isInt_;
      bool isInt64_;
      bool isUInt_;
      bool isUInt64_;
      bool isIntegral_;
      bool isDouble_;
      bool isNumeric_;
   };

   void checkConstMemberCount( const Json::Value &value, unsigned int expectedCount );

   void checkMemberCount( Json::Value &value, unsigned int expectedCount );

   void checkIs( const Json::Value &value, const IsCheck &check );

   void checkIsLess( const Json::Value &x, const Json::Value &y );

   void checkIsEqual( const Json::Value &x, const Json::Value &y );

   /// Normalize the representation of floating-point number by stripped leading 0 in exponent.
   static std::string normalizeFloatingPointStr( const std::string &s );
};


std::string 
ValueTest::normalizeFloatingPointStr( const std::string &s )
{
    std::string::size_type index = s.find_last_of( "eE" );
    if ( index != std::string::npos )
    {
        std::string::size_type hasSign = (s[index+1] == '+' || s[index+1] == '-') ? 1 : 0;
        std::string::size_type exponentStartIndex = index + 1 + hasSign;
        std::string normalized = s.substr( 0, exponentStartIndex );
        std::string::size_type indexDigit = s.find_first_not_of( '0', exponentStartIndex );
        std::string exponent = "0";
        if ( indexDigit != std::string::npos ) // There is an exponent different from 0
        {
            exponent = s.substr( indexDigit );
        }
        return normalized + exponent;
    }
    return s;
}


JSONTEST_FIXTURE( ValueTest, checkNormalizeFloatingPointStr )
{
    JSONTEST_ASSERT_STRING_EQUAL( "0.0", normalizeFloatingPointStr("0.0") );
    JSONTEST_ASSERT_STRING_EQUAL( "0e0", normalizeFloatingPointStr("0e0") );
    JSONTEST_ASSERT_STRING_EQUAL( "1234.0", normalizeFloatingPointStr("1234.0") );
    JSONTEST_ASSERT_STRING_EQUAL( "1234.0e0", normalizeFloatingPointStr("1234.0e0") );
    JSONTEST_ASSERT_STRING_EQUAL( "1234.0e+0", normalizeFloatingPointStr("1234.0e+0") );
    JSONTEST_ASSERT_STRING_EQUAL( "1234e-1", normalizeFloatingPointStr("1234e-1") );
    JSONTEST_ASSERT_STRING_EQUAL( "1234e10", normalizeFloatingPointStr("1234e10") );
    JSONTEST_ASSERT_STRING_EQUAL( "1234e10", normalizeFloatingPointStr("1234e010") );
    JSONTEST_ASSERT_STRING_EQUAL( "1234e+10", normalizeFloatingPointStr("1234e+010") );
    JSONTEST_ASSERT_STRING_EQUAL( "1234e-10", normalizeFloatingPointStr("1234e-010") );
    JSONTEST_ASSERT_STRING_EQUAL( "1234e+100", normalizeFloatingPointStr("1234e+100") );
    JSONTEST_ASSERT_STRING_EQUAL( "1234e-100", normalizeFloatingPointStr("1234e-100") );
    JSONTEST_ASSERT_STRING_EQUAL( "1234e+1", normalizeFloatingPointStr("1234e+001") );
}


JSONTEST_FIXTURE( ValueTest, memberCount )
{
   JSONTEST_ASSERT_PRED( checkMemberCount(emptyArray_, 0) );
   JSONTEST_ASSERT_PRED( checkMemberCount(emptyObject_, 0) );
   JSONTEST_ASSERT_PRED( checkMemberCount(array1_, 1) );
   JSONTEST_ASSERT_PRED( checkMemberCount(object1_, 1) );
   JSONTEST_ASSERT_PRED( checkMemberCount(null_, 0) );
   JSONTEST_ASSERT_PRED( checkMemberCount(integer_, 0) );
   JSONTEST_ASSERT_PRED( checkMemberCount(unsignedInteger_, 0) );
   JSONTEST_ASSERT_PRED( checkMemberCount(smallUnsignedInteger_, 0) );
   JSONTEST_ASSERT_PRED( checkMemberCount(real_, 0) );
   JSONTEST_ASSERT_PRED( checkMemberCount(emptyString_, 0) );
   JSONTEST_ASSERT_PRED( checkMemberCount(string_, 0) );
   JSONTEST_ASSERT_PRED( checkMemberCount(true_, 0) );
}


JSONTEST_FIXTURE( ValueTest, objects )
{
   // Types
   IsCheck checks;
   checks.isObject_ = true;
   JSONTEST_ASSERT_PRED( checkIs( emptyObject_, checks ) );
   JSONTEST_ASSERT_PRED( checkIs( object1_, checks ) );

   JSONTEST_ASSERT_EQUAL(Json::objectValue, emptyObject_.type());

   // Empty object okay
   JSONTEST_ASSERT(emptyObject_.isConvertibleTo(Json::nullValue));

   // Non-empty object not okay
   JSONTEST_ASSERT(!object1_.isConvertibleTo(Json::nullValue));

   // Always okay
   JSONTEST_ASSERT(emptyObject_.isConvertibleTo(Json::objectValue));

   // Never okay
   JSONTEST_ASSERT(!emptyObject_.isConvertibleTo(Json::arrayValue));
   JSONTEST_ASSERT(!emptyObject_.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!emptyObject_.isConvertibleTo(Json::uintValue));
   JSONTEST_ASSERT(!emptyObject_.isConvertibleTo(Json::realValue));
   JSONTEST_ASSERT(!emptyObject_.isConvertibleTo(Json::booleanValue));
   JSONTEST_ASSERT(!emptyObject_.isConvertibleTo(Json::stringValue));

   // Access through const reference
   const Json::Value &constObject = object1_;

   JSONTEST_ASSERT_EQUAL(Json::Value(1234), constObject["id"]);
   JSONTEST_ASSERT_EQUAL(Json::Value(), constObject["unknown id"]);

   // Access through non-const reference
   JSONTEST_ASSERT_EQUAL(Json::Value(1234), object1_["id"]);
   JSONTEST_ASSERT_EQUAL(Json::Value(), object1_["unknown id"]);

   object1_["some other id"] = "foo";
   JSONTEST_ASSERT_EQUAL(Json::Value("foo"), object1_["some other id"]);
}


JSONTEST_FIXTURE( ValueTest, arrays )
{
   const unsigned int index0 = 0;

   // Types
   IsCheck checks;
   checks.isArray_ = true;
   JSONTEST_ASSERT_PRED( checkIs( emptyArray_, checks ) );
   JSONTEST_ASSERT_PRED( checkIs( array1_, checks ) );

   JSONTEST_ASSERT_EQUAL(Json::arrayValue, array1_.type());

   // Empty array okay
   JSONTEST_ASSERT(emptyArray_.isConvertibleTo(Json::nullValue));

   // Non-empty array not okay
   JSONTEST_ASSERT(!array1_.isConvertibleTo(Json::nullValue));

   // Always okay
   JSONTEST_ASSERT(emptyArray_.isConvertibleTo(Json::arrayValue));

   // Never okay
   JSONTEST_ASSERT(!emptyArray_.isConvertibleTo(Json::objectValue));
   JSONTEST_ASSERT(!emptyArray_.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!emptyArray_.isConvertibleTo(Json::uintValue));
   JSONTEST_ASSERT(!emptyArray_.isConvertibleTo(Json::realValue));
   JSONTEST_ASSERT(!emptyArray_.isConvertibleTo(Json::booleanValue));
   JSONTEST_ASSERT(!emptyArray_.isConvertibleTo(Json::stringValue));

   // Access through const reference
   const Json::Value &constArray = array1_;
   JSONTEST_ASSERT_EQUAL(Json::Value(1234), constArray[index0]);
   JSONTEST_ASSERT_EQUAL(Json::Value(1234), constArray[0]);

   // Access through non-const reference
   JSONTEST_ASSERT_EQUAL(Json::Value(1234), array1_[index0]);
   JSONTEST_ASSERT_EQUAL(Json::Value(1234), array1_[0]);

   array1_[2] = Json::Value(17);
   JSONTEST_ASSERT_EQUAL(Json::Value(), array1_[1]);
   JSONTEST_ASSERT_EQUAL(Json::Value(17), array1_[2]);
}


JSONTEST_FIXTURE( ValueTest, null )
{
   JSONTEST_ASSERT_EQUAL(Json::nullValue, null_.type());

   IsCheck checks;
   checks.isNull_ = true;
   JSONTEST_ASSERT_PRED( checkIs( null_, checks ) );

   JSONTEST_ASSERT(null_.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(null_.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(null_.isConvertibleTo(Json::uintValue));
   JSONTEST_ASSERT(null_.isConvertibleTo(Json::realValue));
   JSONTEST_ASSERT(null_.isConvertibleTo(Json::booleanValue));
   JSONTEST_ASSERT(null_.isConvertibleTo(Json::stringValue));
   JSONTEST_ASSERT(null_.isConvertibleTo(Json::arrayValue));
   JSONTEST_ASSERT(null_.isConvertibleTo(Json::objectValue));

   JSONTEST_ASSERT_EQUAL(Json::Int(0), null_.asInt());
   JSONTEST_ASSERT_EQUAL(Json::LargestInt(0), null_.asLargestInt());
   JSONTEST_ASSERT_EQUAL(Json::UInt(0), null_.asUInt());
   JSONTEST_ASSERT_EQUAL(Json::LargestUInt(0), null_.asLargestUInt());
   JSONTEST_ASSERT_EQUAL(0.0, null_.asDouble());
   JSONTEST_ASSERT_EQUAL(0.0, null_.asFloat());
   JSONTEST_ASSERT_STRING_EQUAL("", null_.asString());
}


JSONTEST_FIXTURE( ValueTest, strings )
{
   JSONTEST_ASSERT_EQUAL(Json::stringValue, string1_.type());

   IsCheck checks;
   checks.isString_ = true;
   JSONTEST_ASSERT_PRED( checkIs( emptyString_, checks ) );
   JSONTEST_ASSERT_PRED( checkIs( string_, checks ) );
   JSONTEST_ASSERT_PRED( checkIs( string1_, checks ) );

   // Empty string okay
   JSONTEST_ASSERT(emptyString_.isConvertibleTo(Json::nullValue));

   // Non-empty string not okay
   JSONTEST_ASSERT(!string1_.isConvertibleTo(Json::nullValue));

   // Always okay
   JSONTEST_ASSERT(string1_.isConvertibleTo(Json::stringValue));

   // Never okay
   JSONTEST_ASSERT(!string1_.isConvertibleTo(Json::objectValue));
   JSONTEST_ASSERT(!string1_.isConvertibleTo(Json::arrayValue));
   JSONTEST_ASSERT(!string1_.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!string1_.isConvertibleTo(Json::uintValue));
   JSONTEST_ASSERT(!string1_.isConvertibleTo(Json::realValue));

   JSONTEST_ASSERT_STRING_EQUAL("a", string1_.asString());
   JSONTEST_ASSERT_STRING_EQUAL("a", string1_.asCString());
}


JSONTEST_FIXTURE( ValueTest, bools )
{
   JSONTEST_ASSERT_EQUAL(Json::booleanValue, false_.type());

   IsCheck checks;
   checks.isBool_ = true;
   JSONTEST_ASSERT_PRED( checkIs( false_, checks ) );
   JSONTEST_ASSERT_PRED( checkIs( true_, checks ) );

   // False okay
   JSONTEST_ASSERT(false_.isConvertibleTo(Json::nullValue));

   // True not okay
   JSONTEST_ASSERT(!true_.isConvertibleTo(Json::nullValue));

   // Always okay
   JSONTEST_ASSERT(true_.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(true_.isConvertibleTo(Json::uintValue));
   JSONTEST_ASSERT(true_.isConvertibleTo(Json::realValue));
   JSONTEST_ASSERT(true_.isConvertibleTo(Json::booleanValue));
   JSONTEST_ASSERT(true_.isConvertibleTo(Json::stringValue));

   // Never okay
   JSONTEST_ASSERT(!true_.isConvertibleTo(Json::arrayValue));
   JSONTEST_ASSERT(!true_.isConvertibleTo(Json::objectValue));

   JSONTEST_ASSERT_EQUAL(true, true_.asBool());
   JSONTEST_ASSERT_EQUAL(1, true_.asInt());
   JSONTEST_ASSERT_EQUAL(1, true_.asLargestInt());
   JSONTEST_ASSERT_EQUAL(1, true_.asUInt());
   JSONTEST_ASSERT_EQUAL(1, true_.asLargestUInt());
   JSONTEST_ASSERT_EQUAL(1.0, true_.asDouble());
   JSONTEST_ASSERT_EQUAL(1.0, true_.asFloat());

   JSONTEST_ASSERT_EQUAL(false, false_.asBool());
   JSONTEST_ASSERT_EQUAL(0, false_.asInt());
   JSONTEST_ASSERT_EQUAL(0, false_.asLargestInt());
   JSONTEST_ASSERT_EQUAL(0, false_.asUInt());
   JSONTEST_ASSERT_EQUAL(0, false_.asLargestUInt());
   JSONTEST_ASSERT_EQUAL(0.0, false_.asDouble());
   JSONTEST_ASSERT_EQUAL(0.0, false_.asFloat());
}


JSONTEST_FIXTURE( ValueTest, integers )
{
   IsCheck checks;
   Json::Value val;

   // Conversions that don't depend on the value.
   JSONTEST_ASSERT(Json::Value(17).isConvertibleTo(Json::realValue));
   JSONTEST_ASSERT(Json::Value(17).isConvertibleTo(Json::stringValue));
   JSONTEST_ASSERT(Json::Value(17).isConvertibleTo(Json::booleanValue));
   JSONTEST_ASSERT(!Json::Value(17).isConvertibleTo(Json::arrayValue));
   JSONTEST_ASSERT(!Json::Value(17).isConvertibleTo(Json::objectValue));

   JSONTEST_ASSERT(Json::Value(17U).isConvertibleTo(Json::realValue));
   JSONTEST_ASSERT(Json::Value(17U).isConvertibleTo(Json::stringValue));
   JSONTEST_ASSERT(Json::Value(17U).isConvertibleTo(Json::booleanValue));
   JSONTEST_ASSERT(!Json::Value(17U).isConvertibleTo(Json::arrayValue));
   JSONTEST_ASSERT(!Json::Value(17U).isConvertibleTo(Json::objectValue));

   JSONTEST_ASSERT(Json::Value(17.0).isConvertibleTo(Json::realValue));
   JSONTEST_ASSERT(Json::Value(17.0).isConvertibleTo(Json::stringValue));
   JSONTEST_ASSERT(Json::Value(17.0).isConvertibleTo(Json::booleanValue));
   JSONTEST_ASSERT(!Json::Value(17.0).isConvertibleTo(Json::arrayValue));
   JSONTEST_ASSERT(!Json::Value(17.0).isConvertibleTo(Json::objectValue));

   // Default int
   val = Json::Value(Json::intValue);

   JSONTEST_ASSERT_EQUAL(Json::intValue, val.type());

   checks = IsCheck();
   checks.isInt_ = true;
   checks.isInt64_ = true;
   checks.isUInt_ = true;
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(0, val.asInt());
   JSONTEST_ASSERT_EQUAL(0, val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(0, val.asUInt());
   JSONTEST_ASSERT_EQUAL(0, val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL(0.0, val.asDouble());
   JSONTEST_ASSERT_EQUAL(0.0, val.asFloat());
   JSONTEST_ASSERT_EQUAL(false, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("0", val.asString());

   // Default uint
   val = Json::Value(Json::uintValue);

   JSONTEST_ASSERT_EQUAL(Json::uintValue, val.type());

   checks = IsCheck();
   checks.isInt_ = true;
   checks.isInt64_ = true;
   checks.isUInt_ = true;
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(0, val.asInt());
   JSONTEST_ASSERT_EQUAL(0, val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(0, val.asUInt());
   JSONTEST_ASSERT_EQUAL(0, val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL(0.0, val.asDouble());
   JSONTEST_ASSERT_EQUAL(0.0, val.asFloat());
   JSONTEST_ASSERT_EQUAL(false, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("0", val.asString());

   // Default real
   val = Json::Value(Json::realValue);

   JSONTEST_ASSERT_EQUAL(Json::realValue, val.type());

   JSONTEST_ASSERT(val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::uintValue));

   checks = IsCheck();
   checks.isInt_ = true;
   checks.isInt64_ = true;
   checks.isUInt_ = true;
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT_EQUAL(0, val.asInt());
   JSONTEST_ASSERT_EQUAL(0, val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(0, val.asUInt());
   JSONTEST_ASSERT_EQUAL(0, val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL(0.0, val.asDouble());
   JSONTEST_ASSERT_EQUAL(0.0, val.asFloat());
   JSONTEST_ASSERT_EQUAL(false, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("0.0", val.asString());

   // Zero (signed constructor arg)
   val = Json::Value(0);

   JSONTEST_ASSERT_EQUAL(Json::intValue, val.type());

   checks = IsCheck();
   checks.isInt_ = true;
   checks.isInt64_ = true;
   checks.isUInt_ = true;
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(0, val.asInt());
   JSONTEST_ASSERT_EQUAL(0, val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(0, val.asUInt());
   JSONTEST_ASSERT_EQUAL(0, val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL(0.0, val.asDouble());
   JSONTEST_ASSERT_EQUAL(0.0, val.asFloat());
   JSONTEST_ASSERT_EQUAL(false, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("0", val.asString());

   // Zero (unsigned constructor arg)
   val = Json::Value(0u);

   JSONTEST_ASSERT_EQUAL(Json::uintValue, val.type());

   checks = IsCheck();
   checks.isInt_ = true;
   checks.isInt64_ = true;
   checks.isUInt_ = true;
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(0, val.asInt());
   JSONTEST_ASSERT_EQUAL(0, val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(0, val.asUInt());
   JSONTEST_ASSERT_EQUAL(0, val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL(0.0, val.asDouble());
   JSONTEST_ASSERT_EQUAL(0.0, val.asFloat());
   JSONTEST_ASSERT_EQUAL(false, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("0", val.asString());

   // Zero (floating-point constructor arg)
   val = Json::Value(0.0);

   JSONTEST_ASSERT_EQUAL(Json::realValue, val.type());

   checks = IsCheck();
   checks.isInt_ = true;
   checks.isInt64_ = true;
   checks.isUInt_ = true;
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(0, val.asInt());
   JSONTEST_ASSERT_EQUAL(0, val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(0, val.asUInt());
   JSONTEST_ASSERT_EQUAL(0, val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL(0.0, val.asDouble());
   JSONTEST_ASSERT_EQUAL(0.0, val.asFloat());
   JSONTEST_ASSERT_EQUAL(false, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("0.0", val.asString());

   // 2^20 (signed constructor arg)
   val = Json::Value(1 << 20);

   JSONTEST_ASSERT_EQUAL(Json::intValue, val.type());
   checks = IsCheck();
   checks.isInt_ = true;
   checks.isInt64_ = true;
   checks.isUInt_ = true;
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL((1 << 20), val.asInt());
   JSONTEST_ASSERT_EQUAL((1 << 20), val.asLargestInt());
   JSONTEST_ASSERT_EQUAL((1 << 20), val.asUInt());
   JSONTEST_ASSERT_EQUAL((1 << 20), val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL((1 << 20), val.asDouble());
   JSONTEST_ASSERT_EQUAL((1 << 20), val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("1048576", val.asString());

   // 2^20 (unsigned constructor arg)
   val = Json::Value(Json::UInt(1 << 20));

   JSONTEST_ASSERT_EQUAL(Json::uintValue, val.type());

   checks = IsCheck();
   checks.isInt_ = true;
   checks.isInt64_ = true;
   checks.isUInt_ = true;
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL((1 << 20), val.asInt());
   JSONTEST_ASSERT_EQUAL((1 << 20), val.asLargestInt());
   JSONTEST_ASSERT_EQUAL((1 << 20), val.asUInt());
   JSONTEST_ASSERT_EQUAL((1 << 20), val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL((1 << 20), val.asDouble());
   JSONTEST_ASSERT_EQUAL((1 << 20), val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("1048576", val.asString());

   // 2^20 (floating-point constructor arg)
   val = Json::Value((1 << 20) / 1.0);

   JSONTEST_ASSERT_EQUAL(Json::realValue, val.type());

   checks = IsCheck();
   checks.isInt_ = true;
   checks.isInt64_ = true;
   checks.isUInt_ = true;
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL((1 << 20), val.asInt());
   JSONTEST_ASSERT_EQUAL((1 << 20), val.asLargestInt());
   JSONTEST_ASSERT_EQUAL((1 << 20), val.asUInt());
   JSONTEST_ASSERT_EQUAL((1 << 20), val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL((1 << 20), val.asDouble());
   JSONTEST_ASSERT_EQUAL((1 << 20), val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("1048576.0", normalizeFloatingPointStr(val.asString()));

   // -2^20
   val = Json::Value(-(1 << 20));

   JSONTEST_ASSERT_EQUAL(Json::intValue, val.type());

   checks = IsCheck();
   checks.isInt_ = true;
   checks.isInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(-(1 << 20), val.asInt());
   JSONTEST_ASSERT_EQUAL(-(1 << 20), val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(-(1 << 20), val.asDouble());
   JSONTEST_ASSERT_EQUAL(-(1 << 20), val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("-1048576", val.asString());

   // int32 max
   val = Json::Value(kint32max);

   JSONTEST_ASSERT_EQUAL(Json::intValue, val.type());

   checks = IsCheck();
   checks.isInt_ = true;
   checks.isInt64_ = true;
   checks.isUInt_ = true;
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(kint32max, val.asInt());
   JSONTEST_ASSERT_EQUAL(kint32max, val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(kint32max, val.asUInt());
   JSONTEST_ASSERT_EQUAL(kint32max, val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL(kint32max, val.asDouble());
   JSONTEST_ASSERT_EQUAL(kfint32max, val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("2147483647", val.asString());

   // int32 min
   val = Json::Value(kint32min);

   JSONTEST_ASSERT_EQUAL(Json::intValue, val.type());

   checks = IsCheck();
   checks.isInt_ = true;
   checks.isInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(kint32min, val.asInt());
   JSONTEST_ASSERT_EQUAL(kint32min, val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(kint32min, val.asDouble());
   JSONTEST_ASSERT_EQUAL(kint32min, val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("-2147483648", val.asString());

   // uint32 max
   val = Json::Value(kuint32max);

   JSONTEST_ASSERT_EQUAL(Json::uintValue, val.type());

   checks = IsCheck();
   checks.isInt64_ = true;
   checks.isUInt_ = true;
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::uintValue));

#ifndef JSON_NO_INT64
   JSONTEST_ASSERT_EQUAL(kuint32max, val.asLargestInt());
#endif
   JSONTEST_ASSERT_EQUAL(kuint32max, val.asUInt());
   JSONTEST_ASSERT_EQUAL(kuint32max, val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL(kuint32max, val.asDouble());
   JSONTEST_ASSERT_EQUAL(kfuint32max, val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("4294967295", val.asString());

#ifdef JSON_NO_INT64
   // int64 max
   val = Json::Value(double(kint64max));

   JSONTEST_ASSERT_EQUAL(Json::realValue, val.type());

   checks = IsCheck();
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(double(kint64max), val.asDouble());
   JSONTEST_ASSERT_EQUAL(float(kint64max), val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("9.22337e+18", val.asString());

   // int64 min
   val = Json::Value(double(kint64min));

   JSONTEST_ASSERT_EQUAL(Json::realValue, val.type());

   checks = IsCheck();
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(double(kint64min), val.asDouble());
   JSONTEST_ASSERT_EQUAL(float(kint64min), val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("-9.22337e+18", val.asString());

   // uint64 max
   val = Json::Value(double(kuint64max));

   JSONTEST_ASSERT_EQUAL(Json::realValue, val.type());

   checks = IsCheck();
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(double(kuint64max), val.asDouble());
   JSONTEST_ASSERT_EQUAL(float(kuint64max), val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("1.84467e+19", val.asString());
#else  // ifdef JSON_NO_INT64
   // 2^40 (signed constructor arg)
   val = Json::Value(Json::Int64(1) << 40);

   JSONTEST_ASSERT_EQUAL(Json::intValue, val.type());

   checks = IsCheck();
   checks.isInt64_ = true;
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asInt64());
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asLargestInt());
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asUInt64());
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asDouble());
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("1099511627776", val.asString());

   // 2^40 (unsigned constructor arg)
   val = Json::Value(Json::UInt64(1) << 40);

   JSONTEST_ASSERT_EQUAL(Json::uintValue, val.type());

   checks = IsCheck();
   checks.isInt64_ = true;
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asInt64());
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asLargestInt());
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asUInt64());
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asDouble());
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("1099511627776", val.asString());

   // 2^40 (floating-point constructor arg)
   val = Json::Value((Json::Int64(1) << 40) / 1.0);

   JSONTEST_ASSERT_EQUAL(Json::realValue, val.type());

   checks = IsCheck();
   checks.isInt64_ = true;
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asInt64());
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asLargestInt());
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asUInt64());
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asDouble());
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 40), val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("1099511627776.0", normalizeFloatingPointStr(val.asString()));

   // -2^40
   val = Json::Value(-(Json::Int64(1) << 40));

   JSONTEST_ASSERT_EQUAL(Json::intValue, val.type());

   checks = IsCheck();
   checks.isInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(-(Json::Int64(1) << 40), val.asInt64());
   JSONTEST_ASSERT_EQUAL(-(Json::Int64(1) << 40), val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(-(Json::Int64(1) << 40), val.asDouble());
   JSONTEST_ASSERT_EQUAL(-(Json::Int64(1) << 40), val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("-1099511627776", val.asString());

   // int64 max
   val = Json::Value(Json::Int64(kint64max));

   JSONTEST_ASSERT_EQUAL(Json::intValue, val.type());

   checks = IsCheck();
   checks.isInt64_ = true;
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(kint64max, val.asInt64());
   JSONTEST_ASSERT_EQUAL(kint64max, val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(kint64max, val.asUInt64());
   JSONTEST_ASSERT_EQUAL(kint64max, val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL(double(kint64max), val.asDouble());
   JSONTEST_ASSERT_EQUAL(float(kint64max), val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("9223372036854775807", val.asString());

   // int64 max (floating point constructor). Note that kint64max is not exactly
   // representable as a double, and will be rounded up to be higher.
   val = Json::Value(double(kint64max));

   JSONTEST_ASSERT_EQUAL(Json::realValue, val.type());

   checks = IsCheck();
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(Json::UInt64(1) << 63, val.asUInt64());
   JSONTEST_ASSERT_EQUAL(Json::UInt64(1) << 63, val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL(uint64ToDouble(Json::UInt64(1) << 63), val.asDouble());
   JSONTEST_ASSERT_EQUAL(float(uint64ToDouble(Json::UInt64(1) << 63)), val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("9.223372036854776e+18", normalizeFloatingPointStr(val.asString()));

   // int64 min
   val = Json::Value(Json::Int64(kint64min));

   JSONTEST_ASSERT_EQUAL(Json::intValue, val.type());

   checks = IsCheck();
   checks.isInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(kint64min, val.asInt64());
   JSONTEST_ASSERT_EQUAL(kint64min, val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(double(kint64min), val.asDouble());
   JSONTEST_ASSERT_EQUAL(float(kint64min), val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("-9223372036854775808", val.asString());

   // int64 min (floating point constructor). Note that kint64min *is* exactly
   // representable as a double.
   val = Json::Value(double(kint64min));

   JSONTEST_ASSERT_EQUAL(Json::realValue, val.type());

   checks = IsCheck();
   checks.isInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(kint64min, val.asInt64());
   JSONTEST_ASSERT_EQUAL(kint64min, val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(-9223372036854775808.0, val.asDouble());
   JSONTEST_ASSERT_EQUAL(-9223372036854775808.0, val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("-9.223372036854776e+18", normalizeFloatingPointStr(val.asString()));

   // uint64 max
   val = Json::Value(Json::UInt64(kuint64max));

   JSONTEST_ASSERT_EQUAL(Json::uintValue, val.type());

   checks = IsCheck();
   checks.isUInt64_ = true;
   checks.isIntegral_ = true;
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(kuint64max, val.asUInt64());
   JSONTEST_ASSERT_EQUAL(kuint64max, val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL(uint64ToDouble(kuint64max), val.asDouble());
   JSONTEST_ASSERT_EQUAL(float(uint64ToDouble(kuint64max)), val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("18446744073709551615", val.asString());

   // uint64 max (floating point constructor). Note that kuint64max is not
   // exactly representable as a double, and will be rounded up to be higher.
   val = Json::Value(uint64ToDouble(kuint64max));

   JSONTEST_ASSERT_EQUAL(Json::realValue, val.type());

   checks = IsCheck();
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));

   JSONTEST_ASSERT_EQUAL(18446744073709551616.0, val.asDouble());
   JSONTEST_ASSERT_EQUAL(18446744073709551616.0, val.asFloat());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_STRING_EQUAL("1.844674407370955e+19", normalizeFloatingPointStr(val.asString()));
#endif
}


JSONTEST_FIXTURE( ValueTest, nonIntegers )
{
   IsCheck checks;
   Json::Value val;

   // Small positive number
   val = Json::Value(1.5);

   JSONTEST_ASSERT_EQUAL(Json::realValue, val.type());

   checks = IsCheck();
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::uintValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::realValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::booleanValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::stringValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::arrayValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::objectValue));

   JSONTEST_ASSERT_EQUAL(1.5, val.asDouble());
   JSONTEST_ASSERT_EQUAL(1.5, val.asFloat());
   JSONTEST_ASSERT_EQUAL(1, val.asInt());
   JSONTEST_ASSERT_EQUAL(1, val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(1, val.asUInt());
   JSONTEST_ASSERT_EQUAL(1, val.asLargestUInt());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_EQUAL("1.50", val.asString());

   // Small negative number
   val = Json::Value(-1.5);

   JSONTEST_ASSERT_EQUAL(Json::realValue, val.type());

   checks = IsCheck();
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::realValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::booleanValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::stringValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::arrayValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::objectValue));

   JSONTEST_ASSERT_EQUAL(-1.5, val.asDouble());
   JSONTEST_ASSERT_EQUAL(-1.5, val.asFloat());
   JSONTEST_ASSERT_EQUAL(-1, val.asInt());
   JSONTEST_ASSERT_EQUAL(-1, val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_EQUAL("-1.50", val.asString());

   // A bit over int32 max
   val = Json::Value(kint32max + 0.5);

   JSONTEST_ASSERT_EQUAL(Json::realValue, val.type());

   checks = IsCheck();
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(val.isConvertibleTo(Json::uintValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::realValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::booleanValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::stringValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::arrayValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::objectValue));

   JSONTEST_ASSERT_EQUAL(2147483647.5, val.asDouble());
   JSONTEST_ASSERT_EQUAL(float(2147483647.5), val.asFloat());
   JSONTEST_ASSERT_EQUAL(2147483647U, val.asUInt());
#ifdef JSON_HAS_INT64
   JSONTEST_ASSERT_EQUAL(2147483647L, val.asLargestInt());
   JSONTEST_ASSERT_EQUAL(2147483647U, val.asLargestUInt());
#endif
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_EQUAL("2147483647.50", normalizeFloatingPointStr(val.asString()));

   // A bit under int32 min
   val = Json::Value(kint32min - 0.5);

   JSONTEST_ASSERT_EQUAL(Json::realValue, val.type());

   checks = IsCheck();
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(val.isConvertibleTo(Json::realValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::booleanValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::stringValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::arrayValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::objectValue));

   JSONTEST_ASSERT_EQUAL(-2147483648.5, val.asDouble());
   JSONTEST_ASSERT_EQUAL(float(-2147483648.5), val.asFloat());
#ifdef JSON_HAS_INT64
   JSONTEST_ASSERT_EQUAL(-Json::Int64(1)<< 31, val.asLargestInt());
#endif
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_EQUAL("-2147483648.50", normalizeFloatingPointStr(val.asString()));

   // A bit over uint32 max
   val = Json::Value(kuint32max + 0.5);

   JSONTEST_ASSERT_EQUAL(Json::realValue, val.type());

   checks = IsCheck();
   checks.isDouble_ = true;
   checks.isNumeric_ = true;
   JSONTEST_ASSERT_PRED( checkIs( val, checks ) );

   JSONTEST_ASSERT(val.isConvertibleTo(Json::realValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::booleanValue));
   JSONTEST_ASSERT(val.isConvertibleTo(Json::stringValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::nullValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::intValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::uintValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::arrayValue));
   JSONTEST_ASSERT(!val.isConvertibleTo(Json::objectValue));

   JSONTEST_ASSERT_EQUAL(4294967295.5, val.asDouble());
   JSONTEST_ASSERT_EQUAL(float(4294967295.5), val.asFloat());
#ifdef JSON_HAS_INT64
   JSONTEST_ASSERT_EQUAL((Json::Int64(1) << 32)-1, val.asLargestInt());
   JSONTEST_ASSERT_EQUAL((Json::UInt64(1) << 32)-Json::UInt64(1), val.asLargestUInt());
#endif
   JSONTEST_ASSERT_EQUAL(true, val.asBool());
   JSONTEST_ASSERT_EQUAL("4294967295.50", normalizeFloatingPointStr(val.asString()));

   val = Json::Value(1.2345678901234);
   JSONTEST_ASSERT_STRING_EQUAL( "1.23456789012340", normalizeFloatingPointStr(val.asString()));
}


void
ValueTest::checkConstMemberCount( const Json::Value &value, unsigned int expectedCount )
{
   unsigned int count = 0;
   Json::Value::const_iterator itEnd = value.end();
   for ( Json::Value::const_iterator it = value.begin(); it != itEnd; ++it )
   {
      ++count;
   }
   JSONTEST_ASSERT_EQUAL( expectedCount, count ) << "Json::Value::const_iterator";
}

void
ValueTest::checkMemberCount( Json::Value &value, unsigned int expectedCount )
{
   JSONTEST_ASSERT_EQUAL(expectedCount, value.size() );

   unsigned int count = 0;
   Json::Value::iterator itEnd = value.end();
   for ( Json::Value::iterator it = value.begin(); it != itEnd; ++it )
   {
      ++count;
   }
   JSONTEST_ASSERT_EQUAL( expectedCount, count ) << "Json::Value::iterator";

   JSONTEST_ASSERT_PRED( checkConstMemberCount(value, expectedCount) );
}


ValueTest::IsCheck::IsCheck()
   : isObject_( false )
   , isArray_( false )
   , isBool_( false )
   , isString_( false )
   , isNull_( false )
   , isInt_( false )
   , isInt64_( false )
   , isUInt_( false )
   , isUInt64_( false )
   , isIntegral_( false )
   , isDouble_( false )
   , isNumeric_( false )
{
}


void 
ValueTest::checkIs( const Json::Value &value, const IsCheck &check )
{
   JSONTEST_ASSERT_EQUAL(check.isObject_, value.isObject() );
   JSONTEST_ASSERT_EQUAL(check.isArray_, value.isArray() );
   JSONTEST_ASSERT_EQUAL(check.isBool_, value.isBool() );
   JSONTEST_ASSERT_EQUAL(check.isDouble_, value.isDouble() );
   JSONTEST_ASSERT_EQUAL(check.isInt_, value.isInt() );
   JSONTEST_ASSERT_EQUAL(check.isUInt_, value.isUInt() );
   JSONTEST_ASSERT_EQUAL(check.isIntegral_, value.isIntegral() );
   JSONTEST_ASSERT_EQUAL(check.isNumeric_, value.isNumeric() );
   JSONTEST_ASSERT_EQUAL(check.isString_, value.isString() );
   JSONTEST_ASSERT_EQUAL(check.isNull_, value.isNull() );

#ifdef JSON_HAS_INT64
   JSONTEST_ASSERT_EQUAL(check.isInt64_, value.isInt64() );
   JSONTEST_ASSERT_EQUAL(check.isUInt64_, value.isUInt64() );
#else
   JSONTEST_ASSERT_EQUAL(false, value.isInt64() );
   JSONTEST_ASSERT_EQUAL(false, value.isUInt64() );
#endif
}

JSONTEST_FIXTURE( ValueTest, compareNull )
{
    JSONTEST_ASSERT_PRED( checkIsEqual( Json::Value(), Json::Value() ) );
}


JSONTEST_FIXTURE( ValueTest, compareInt )
{
    JSONTEST_ASSERT_PRED( checkIsLess( 0, 10 ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( 10, 10 ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( -10, -10 ) );
    JSONTEST_ASSERT_PRED( checkIsLess( -10, 0 ) );
}


JSONTEST_FIXTURE( ValueTest, compareUInt )
{
    JSONTEST_ASSERT_PRED( checkIsLess( 0u, 10u ) );
    JSONTEST_ASSERT_PRED( checkIsLess( 0u, Json::Value::maxUInt ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( 10u, 10u ) );
}


JSONTEST_FIXTURE( ValueTest, compareDouble )
{
    JSONTEST_ASSERT_PRED( checkIsLess( 0.0, 10.0 ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( 10.0, 10.0 ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( -10.0, -10.0 ) );
    JSONTEST_ASSERT_PRED( checkIsLess( -10.0, 0.0 ) );
}


JSONTEST_FIXTURE( ValueTest, compareString )
{
    JSONTEST_ASSERT_PRED( checkIsLess( "", " " ) );
    JSONTEST_ASSERT_PRED( checkIsLess( "", "a" ) );
    JSONTEST_ASSERT_PRED( checkIsLess( "abcd", "zyui" ) );
    JSONTEST_ASSERT_PRED( checkIsLess( "abc", "abcd" ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( "abcd", "abcd" ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( " ", " " ) );
    JSONTEST_ASSERT_PRED( checkIsLess( "ABCD", "abcd" ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( "ABCD", "ABCD" ) );
}


JSONTEST_FIXTURE( ValueTest, compareBoolean )
{
    JSONTEST_ASSERT_PRED( checkIsLess( false, true ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( false, false ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( true, true ) );
}


JSONTEST_FIXTURE( ValueTest, compareArray )
{
    // array compare size then content
    Json::Value emptyArray(Json::arrayValue);
    Json::Value l1aArray;
    l1aArray.append( 0 );
    Json::Value l1bArray;
    l1bArray.append( 10 );
    Json::Value l2aArray;
    l2aArray.append( 0 );
    l2aArray.append( 0 );
    Json::Value l2bArray;
    l2bArray.append( 0 );
    l2bArray.append( 10 );
    JSONTEST_ASSERT_PRED( checkIsLess( emptyArray, l1aArray ) );
    JSONTEST_ASSERT_PRED( checkIsLess( emptyArray, l2aArray ) );
    JSONTEST_ASSERT_PRED( checkIsLess( l1aArray, l2aArray ) );
    JSONTEST_ASSERT_PRED( checkIsLess( l2aArray, l2bArray ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( emptyArray, Json::Value( emptyArray ) ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( l1aArray, Json::Value( l1aArray) ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( l2bArray, Json::Value( l2bArray) ) );
}


JSONTEST_FIXTURE( ValueTest, compareObject )
{
    // object compare size then content
    Json::Value emptyObject(Json::objectValue);
    Json::Value l1aObject;
    l1aObject["key1"] = 0;
    Json::Value l1bObject;
    l1aObject["key1"] = 10;
    Json::Value l2aObject;
    l2aObject["key1"] = 0;
    l2aObject["key2"] = 0;
    JSONTEST_ASSERT_PRED( checkIsLess( emptyObject, l1aObject ) );
    JSONTEST_ASSERT_PRED( checkIsLess( emptyObject, l2aObject ) );
    JSONTEST_ASSERT_PRED( checkIsLess( l1aObject, l2aObject ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( emptyObject, Json::Value( emptyObject ) ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( l1aObject, Json::Value( l1aObject ) ) );
    JSONTEST_ASSERT_PRED( checkIsEqual( l2aObject, Json::Value( l2aObject ) ) );
}


JSONTEST_FIXTURE( ValueTest, compareType )
{
    // object of different type are ordered according to their type
    JSONTEST_ASSERT_PRED( checkIsLess( Json::Value(), Json::Value(1) ) );
    JSONTEST_ASSERT_PRED( checkIsLess( Json::Value(1), Json::Value(1u) ) );
    JSONTEST_ASSERT_PRED( checkIsLess( Json::Value(1u), Json::Value(1.0) ) );
    JSONTEST_ASSERT_PRED( checkIsLess( Json::Value(1.0), Json::Value("a") ) );
    JSONTEST_ASSERT_PRED( checkIsLess( Json::Value("a"), Json::Value(true) ) );
    JSONTEST_ASSERT_PRED( checkIsLess( Json::Value(true), Json::Value(Json::arrayValue) ) );
    JSONTEST_ASSERT_PRED( checkIsLess( Json::Value(Json::arrayValue), Json::Value(Json::objectValue) ) );
}


void 
ValueTest::checkIsLess( const Json::Value &x, const Json::Value &y )
{
    JSONTEST_ASSERT( x < y );
    JSONTEST_ASSERT( y > x );
    JSONTEST_ASSERT( x <= y );
    JSONTEST_ASSERT( y >= x );
    JSONTEST_ASSERT( !(x == y) );
    JSONTEST_ASSERT( !(y == x) );
    JSONTEST_ASSERT( !(x >= y) );
    JSONTEST_ASSERT( !(y <= x) );
    JSONTEST_ASSERT( !(x > y) );
    JSONTEST_ASSERT( !(y < x) );
    JSONTEST_ASSERT( x.compare( y ) < 0 );
    JSONTEST_ASSERT( y.compare( x ) >= 0 );
}


void 
ValueTest::checkIsEqual( const Json::Value &x, const Json::Value &y )
{
    JSONTEST_ASSERT( x == y );
    JSONTEST_ASSERT( y == x );
    JSONTEST_ASSERT( x <= y );
    JSONTEST_ASSERT( y <= x );
    JSONTEST_ASSERT( x >= y );
    JSONTEST_ASSERT( y >= x );
    JSONTEST_ASSERT( !(x < y) );
    JSONTEST_ASSERT( !(y < x) );
    JSONTEST_ASSERT( !(x > y) );
    JSONTEST_ASSERT( !(y > x) );
    JSONTEST_ASSERT( x.compare( y ) == 0 );
    JSONTEST_ASSERT( y.compare( x ) == 0 );
}


struct WriterTest : JsonTest::TestCase
{
};


JSONTEST_FIXTURE( WriterTest, dropNullPlaceholders )
{
    Json::FastWriter writer;
    Json::Value nullValue;
    JSONTEST_ASSERT( writer.write(nullValue) == "null\n" );

    writer.dropNullPlaceholders();
    JSONTEST_ASSERT( writer.write(nullValue) == "\n" );
}


int main( int argc, const char *argv[] )
{
   JsonTest::Runner runner;
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, checkNormalizeFloatingPointStr );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, memberCount );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, objects );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, arrays );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, null );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, strings );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, bools );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, integers );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, nonIntegers );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, compareNull );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, compareInt );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, compareUInt );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, compareDouble );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, compareString );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, compareBoolean );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, compareArray );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, compareObject );
   JSONTEST_REGISTER_FIXTURE( runner, ValueTest, compareType );
   JSONTEST_REGISTER_FIXTURE( runner, WriterTest, dropNullPlaceholders );
   return runner.runCommandLine( argc, argv );
}
