#pragma once
#include <QPalette>
#include <QSettings>

bool isDarkThemeEnabled() {
#if defined(Q_OS_WINDOWS)
  QSettings settings;
  QSettings systemSettings(
      "HKEY_CURRENT_"
      "USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
      QSettings::NativeFormat);

  QString darkMode = settings.value("app/darkMode", "Auto").toString();
  if (darkMode == "Auto") {
      return systemSettings.value("AppsUseLightTheme") == 0;
  } else if (darkMode == "On") {
      return true;
  } else {
      return false;
  }
#elif defined(Q_OS_LINUX)
  QSettings settings;

  return settings.value("app/darkModeOn", false).toBool();
#else
  return false;
#endif
}

QPalette createDarkModePalette() {

  QColor base = QColor(30, 30, 30);
  QColor alt = QColor(50, 50, 50);
  QColor text = QColor(223, 223, 223);
  QColor buttonText = QColor(170, 170, 170);
  QColor disabledColor = QColor(127, 127, 127);

  QPalette p(alt, base);

  p.setColor(QPalette::Light, QColor(76,76,76));
  p.setColor(QPalette::Dark, QColor(235,235,235));
  p.setColor(QPalette::Midlight, QColor(76,76,76));
  p.setColor(QPalette::Mid, QColor(66,66,66));
  p.setColor(QPalette::Window, base);
  p.setColor(QPalette::WindowText, text);
  p.setColor(QPalette::Base, base);
  p.setColor(QPalette::AlternateBase, alt);
  p.setColor(QPalette::ToolTipBase, alt);
  p.setColor(QPalette::ToolTipText, Qt::white);
  p.setColor(QPalette::Text, text);
  p.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
  p.setColor(QPalette::Button, alt);
  p.setColor(QPalette::ButtonText, buttonText);
  p.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
  p.setColor(QPalette::BrightText, text.lighter(80));
  p.setColor(QPalette::Link, QColor(42, 130, 218));
  p.setColor(QPalette::Highlight, QColor(42, 130, 218));
  p.setColor(QPalette::HighlightedText, Qt::black);
  p.setColor(QPalette::Disabled, QPalette::HighlightedText, disabledColor);

  p.setBrush(QPalette::Active, QPalette::Highlight,  QColor(42, 130, 218));
  p.setBrush(QPalette::Inactive, QPalette::Highlight, QColor(42, 130, 218));

  return p;
}
