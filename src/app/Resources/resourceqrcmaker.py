import os
from collections import namedtuple

#Define a named tuple to represent our files
FileStruct = namedtuple("File", "file_name file_ext file_path dir")

def XMLifyFile(file_struct):
    return '\t\t<file alias="' + file_struct.file_name + '">' + file_struct.file_path + "</file>\n"


valid_exts = [".graphml", ".png", ".gif", ".jpg" , ".png", ".ico"]

directories = {}

for root, dirs, files in os.walk("."):
    for file_name in files:
        # Trim off the ./
        dir_name = root[2:]
        alias_name = dir_name
        dir_name = "Resources/" + dir_name
        # Append Resource Path
        file_prefix = os.path.splitext(file_name)[0]
        file_ext = os.path.splitext(file_name)[1].lower()
        file_path = os.path.join(dir_name, file_name)
        if file_ext in valid_exts:
            s = FileStruct(file_name = file_prefix, file_ext = file_ext, file_path = file_path, dir = dir_name)
            if alias_name not in directories:
                directories[alias_name] = []
            directories[alias_name].append(s)

resource_file = open("../resources.qrc", 'w')


resource_file.write("<RCC>\n")
for dir_name in directories:
    open_qresource = '\t<qresource prefix="/'+ dir_name + '">\n'
    close_qresource = '\t</qresource>\n'
    resource_file.write(open_qresource)
    for file_struct in directories[dir_name]:
        resource_file.write(XMLifyFile(file_struct))
    resource_file.write(close_qresource)
resource_file.write("</RCC>\n")
resource_file.flush()
resource_file.close()