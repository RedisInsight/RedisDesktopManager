// Copyright 2007-2010 Baptiste Lepilleur
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#ifndef JSON_WRITER_H_INCLUDED
# define JSON_WRITER_H_INCLUDED

#if !defined(JSON_IS_AMALGAMATION)
# include "value.h"
#endif // if !defined(JSON_IS_AMALGAMATION)
# include <vector>
# include <string>

namespace Json {

   class Value;

   /** \brief Abstract class for writers.
    */
   class JSON_API Writer
   {
   public:
      virtual ~Writer();

      virtual std::string write( const Value &root ) = 0;
   };

   /** \brief Outputs a Value in <a HREF="http://www.json.org">JSON</a> format without formatting (not human friendly).
    *
    * The JSON document is written in a single line. It is not intended for 'human' consumption,
    * but may be usefull to support feature such as RPC where bandwith is limited.
    * \sa Reader, Value
    */
   class JSON_API FastWriter : public Writer
   {
   public:
      FastWriter();
      virtual ~FastWriter(){}

      void enableYAMLCompatibility();

      /** \brief Drop the "null" string from the writer's output for nullValues.
       * Strictly speaking, this is not valid JSON. But when the output is being
       * fed to a browser's Javascript, it makes for smaller output and the
       * browser can handle the output just fine.
       */
      void dropNullPlaceholders();

   public: // overridden from Writer
      virtual std::string write( const Value &root );

   private:
      void writeValue( const Value &value );

      std::string document_;
      bool yamlCompatiblityEnabled_;
      bool dropNullPlaceholders_;
   };

   /** \brief Writes a Value in <a HREF="http://www.json.org">JSON</a> format in a human friendly way.
    *
    * The rules for line break and indent are as follow:
    * - Object value:
    *     - if empty then print {} without indent and line break
    *     - if not empty the print '{', line break & indent, print one value per line
    *       and then unindent and line break and print '}'.
    * - Array value:
    *     - if empty then print [] without indent and line break
    *     - if the array contains no object value, empty array or some other value types,
    *       and all the values fit on one lines, then print the array on a single line.
    *     - otherwise, it the values do not fit on one line, or the array contains
    *       object or non empty array, then print one value per line.
    *
    * If the Value have comments then they are outputed according to their #CommentPlacement.
    *
    * \sa Reader, Value, Value::setComment()
    */
   class JSON_API StyledWriter: public Writer
   {
   public:
      StyledWriter();
      virtual ~StyledWriter(){}

   public: // overridden from Writer
      /** \brief Serialize a Value in <a HREF="http://www.json.org">JSON</a> format.
       * \param root Value to serialize.
       * \return String containing the JSON document that represents the root value.
       */
      virtual std::string write( const Value &root );

   private:
      void writeValue( const Value &value );
      void writeArrayValue( const Value &value );
      bool isMultineArray( const Value &value );
      void pushValue( const std::string &value );
      void writeIndent();
      void writeWithIndent( const std::string &value );
      void indent();
      void unindent();
      void writeCommentBeforeValue( const Value &root );
      void writeCommentAfterValueOnSameLine( const Value &root );
      bool hasCommentForValue( const Value &value );
      static std::string normalizeEOL( const std::string &text );

      typedef std::vector<std::string> ChildValues;

      ChildValues childValues_;
      std::string document_;
      std::string indentString_;
      int rightMargin_;
      int indentSize_;
      bool addChildValues_;
   };

   /** \brief Writes a Value in <a HREF="http://www.json.org">JSON</a> format in a human friendly way,
        to a stream rather than to a string.
    *
    * The rules for line break and indent are as follow:
    * - Object value:
    *     - if empty then print {} without indent and line break
    *     - if not empty the print '{', line break & indent, print one value per line
    *       and then unindent and line break and print '}'.
    * - Array value:
    *     - if empty then print [] without indent and line break
    *     - if the array contains no object value, empty array or some other value types,
    *       and all the values fit on one lines, then print the array on a single line.
    *     - otherwise, it the values do not fit on one line, or the array contains
    *       object or non empty array, then print one value per line.
    *
    * If the Value have comments then they are outputed according to their #CommentPlacement.
    *
    * \param indentation Each level will be indented by this amount extra.
    * \sa Reader, Value, Value::setComment()
    */
   class JSON_API StyledStreamWriter
   {
   public:
      StyledStreamWriter( std::string indentation="\t" );
      ~StyledStreamWriter(){}

   public:
      /** \brief Serialize a Value in <a HREF="http://www.json.org">JSON</a> format.
       * \param out Stream to write to. (Can be ostringstream, e.g.)
       * \param root Value to serialize.
       * \note There is no point in deriving from Writer, since write() should not return a value.
       */
      void write( std::ostream &out, const Value &root );

   private:
      void writeValue( const Value &value );
      void writeArrayValue( const Value &value );
      bool isMultineArray( const Value &value );
      void pushValue( const std::string &value );
      void writeIndent();
      void writeWithIndent( const std::string &value );
      void indent();
      void unindent();
      void writeCommentBeforeValue( const Value &root );
      void writeCommentAfterValueOnSameLine( const Value &root );
      bool hasCommentForValue( const Value &value );
      static std::string normalizeEOL( const std::string &text );

      typedef std::vector<std::string> ChildValues;

      ChildValues childValues_;
      std::ostream* document_;
      std::string indentString_;
      int rightMargin_;
      std::string indentation_;
      bool addChildValues_;
   };

# if defined(JSON_HAS_INT64)
   std::string JSON_API valueToString( Int value );
   std::string JSON_API valueToString( UInt value );
# endif // if defined(JSON_HAS_INT64)
   std::string JSON_API valueToString( LargestInt value );
   std::string JSON_API valueToString( LargestUInt value );
   std::string JSON_API valueToString( double value );
   std::string JSON_API valueToString( bool value );
   std::string JSON_API valueToQuotedString( const char *value );

   /// \brief Output using the StyledStreamWriter.
   /// \see Json::operator>>()
   std::ostream& operator<<( std::ostream&, const Value &root );

} // namespace Json



#endif // JSON_WRITER_H_INCLUDED
