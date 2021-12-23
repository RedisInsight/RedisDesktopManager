#include "jsonutils.h"

#include <QDebug>
#include <simdjson.h>

// Based on https://github.com/nlohmann/json/blob/ec7a1d834773f9fee90d8ae908a0c9933c5646fc/src/json.hpp#L4604-L4697
// Copyright &copy; 2013-2015 Niels Lohmann.
// The code is licensed under the MIT License
std::size_t extra_space(const std::string_view& s) noexcept
{
    std::size_t result = 0;

    for (const auto& c : s)
    {
        switch (c)
        {
            case '"':
            case '\\':
            case '\b':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
            {
                // from c (1 byte) to \x (2 bytes)
                result += 1;
                break;
            }

            default:
            {
                if (c >= 0x00 and c <= 0x1f)
                {
                    // from c (1 byte) to \uxxxx (6 bytes)
                    result += 5;
                }
                break;
            }
        }
    }

    return result;
}

std::string escape_string(const std::string_view& s) noexcept
{
    const auto space = extra_space(s);
    if (space == 0)
    {
        return std::string(s);
    }

    // create a result string of necessary size
    std::string result(s.size() + space, '\\');
    std::size_t pos = 0;

    for (const auto& c : s)
    {
        switch (c)
        {
            // quotation mark (0x22)
            case '"':
            {
                result[pos + 1] = '"';
                pos += 2;
                break;
            }

            // reverse solidus (0x5c)
            case '\\':
            {
                // nothing to change
                pos += 2;
                break;
            }

            // backspace (0x08)
            case '\b':
            {
                result[pos + 1] = 'b';
                pos += 2;
                break;
            }

            // formfeed (0x0c)
            case '\f':
            {
                result[pos + 1] = 'f';
                pos += 2;
                break;
            }

            // newline (0x0a)
            case '\n':
            {
                result[pos + 1] = 'n';
                pos += 2;
                break;
            }

            // carriage return (0x0d)
            case '\r':
            {
                result[pos + 1] = 'r';
                pos += 2;
                break;
            }

            // horizontal tab (0x09)
            case '\t':
            {
                result[pos + 1] = 't';
                pos += 2;
                break;
            }

            default:
            {
                if (c >= 0x00 and c <= 0x1f)
                {
                    // print character c as \uxxxx
                    std::snprintf(&result[pos + 1], 7, "u%04x", int(c));
                    pos += 6;
                    // overwrite trailing null character
                    result[pos] = '\\';
                }
                else
                {
                    // all other characters are added as-is
                    result[pos++] = c;
                }
                break;
            }
        }
    }

    return result;
}

QByteArray escapeJsonKey(std::string_view key) {
    return QByteArray::fromStdString(escape_string(key));
}

void print_json(QByteArray &result,
                simdjson::ondemand::value element,
                long level, bool objectValue = false) {
  using namespace simdjson::ondemand;

  QByteArray whitespace = QByteArray().fill(' ', level * 2);
  bool add_comma;

  if (!objectValue) result.append(whitespace);

  switch (element.type()) {
    case json_type::array:
      result.append("[\n");
      add_comma = false;
      for (auto child : element.get_array()) {
        if (add_comma) {
          result.append(",\n");
        }
        print_json(result, child.value(), level + 1);
        add_comma = true;
      }

      result.append('\n');
      result.append(whitespace);
      result.append("]");
      break;
    case json_type::object:
      result.append("{\n");
      add_comma = false;
      for (auto field : element.get_object()) {
        if (add_comma) {
          result.append(",\n");
        }

        result.append(whitespace);
        result.append("  ");
        result.append(QString("\"%1\": ")
                          .arg(QString::fromUtf8(escapeJsonKey(field.unescaped_key())))
                          .toUtf8());
        print_json(result, field.value(), level + 1, true);
        add_comma = true;
      }
      result.append('\n');
      result.append(whitespace);
      result.append("}");
      break;
    case json_type::number:
      result.append(QByteArray::fromStdString(
                        std::string(std::string_view(element.raw_json_token())))
                        .trimmed());
      break;
    case json_type::string:
      result.append(QByteArray::fromStdString(
                        std::string(std::string_view(element.raw_json_token())))
                        .trimmed());
      break;
    case json_type::boolean:
      result.append(bool(element) ? "true" : "false");
      break;
    case json_type::null:
      result.append("null");
      break;
  }
}

QByteArray JSONUtils::prettyPrintJSON(QByteArray val)
{
    QByteArray result;
    result.reserve(val.size() * 32);

    val.resize(val.size() + simdjson::SIMDJSON_PADDING);

    simdjson::ondemand::parser p;

    try {
      auto doc = p.iterate(val.data(), val.size());

      if (doc.is_scalar()) {
          return val;
      }

      print_json(result, simdjson::ondemand::value(doc), 0);
    } catch (const std::exception& e) {
      qDebug() << "Cannot parse JSON:" << e.what();
      return QByteArray();
    }

    return result;
}

QByteArray JSONUtils::minifyJSON(const QByteArray &val)
{
    QByteArray minified;
    minified.resize(val.size());

    size_t new_length{};
    auto error = simdjson::minify(val.data(), val.size(), minified.data(), new_length);

    if (error != 0) {
        qDebug() << "Failed to minify JSON with simdjson:" << error;
        return QByteArray();
    }

    minified.resize(new_length);

    return minified;
}

bool JSONUtils::isJSON(QByteArray val)
{
    int originalSize = val.size();
    val.resize(val.size() + simdjson::SIMDJSON_PADDING);

    simdjson::dom::parser parser;
    simdjson::dom::element data;
    auto error = parser.parse(val.data(), originalSize, false).get(data);

    // NOTE(u_glide): Workaround to distinguish invalid JSON and valid JSON with Big Int
    if (error == simdjson::NUMBER_ERROR) {
        simdjson::ondemand::parser p;

        try {
          auto doc = p.iterate(val.data(), val.size());
          return !doc.is_scalar();
        } catch (const std::exception& e) {
          qDebug() << "JSON is not valid:" << e.what();
          return false;
        }
    } else if (error != simdjson::SUCCESS) {
        qDebug() << "JSON is not valid:" << simdjson::error_message(error);
        return false;
    }

    return true;
}
