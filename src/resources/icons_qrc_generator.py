import glob
import os

DARK_THEME_PATH = "images/dark_theme"
LIGHT_THEME_PATH = "images/light_theme"


def generate_qrc():
    with open("icons.qrc", "w") as output:
        dark_lines = []
        light_lines = []
        for icon_file in glob.glob("./%s/*.svg" % DARK_THEME_PATH):
            base_name = os.path.basename(icon_file)

            if not os.path.exists("./%s/%s" % (LIGHT_THEME_PATH, base_name)):
                print("Icon %s doesn't exist in light theme" % base_name)
                return

            dark_lines.append(f"<file alias=\"{base_name}\">{DARK_THEME_PATH}/{base_name}</file>")
            light_lines.append(f"<file alias=\"{base_name}\">{LIGHT_THEME_PATH}/{base_name}</file>")

        output.write("<RCC>\n")
        output.write(f"<qresource prefix=\"{DARK_THEME_PATH}\">\n     ")
        output.write("\n    ".join(dark_lines))
        output.write("\n</qresource>\n")
        output.write(f"<qresource prefix=\"{LIGHT_THEME_PATH}\">\n    ")
        output.write("\n    ".join(light_lines))
        output.write("\n</qresource>\n")
        output.write("</RCC>")


if __name__ == "__main__":
    generate_qrc()
