import json


def convert_redis_io_commands(path):
    with open(path) as f:
        commands = json.load(f)

        converted = []
        for cmd, info in commands.items():

            arguments = []
            for arg in info.get('arguments', []):
                parts = []
                if 'command' in arg:
                    parts.append(arg["command"])

                    if type == "enum":
                        parts.append("|".join(arg['enum']))
                elif "name" in arg:
                    if isinstance(arg['name'], list):
                        parts.append(" ".join(arg['name']))
                    else:
                        parts.append(arg['name'])

                arg_spec = " ".join(parts)

                if arg.get('optional', False):
                    arguments.append("[%s]" % arg_spec)
                else:
                    arguments.append(arg_spec)

            converted.append({
                "cmd": cmd,
                "summary": info["summary"],
                "arguments": " ".join(arguments),
                "since": info["since"]
            })

        with open("%s.converted" % path, "w") as fres:
            json.dump(converted, fres)


if __name__ == "__main__":
    convert_redis_io_commands("commands.json")
